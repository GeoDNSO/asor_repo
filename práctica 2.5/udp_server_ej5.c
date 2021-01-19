#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/utsname.h> 
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <sys/time.h>
#include <time.h>
#include <locale.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sched.h>
#include <sys/resource.h>
#include <signal.h>
#include <limits.h>
#include <sys/socket.h>
#include <netdb.h>

#define BUF_SIZE 512
#define MAX_CHILDS 5

#define TIMEOUT_SECONDS 100

#define SERVER_ORIGIN 10

#define ERROR -1
#define CURRENT_TIME_HOUR 1
#define CURRENT_DATE 2
#define FINISH 3
#define OTHER_MESSAGE 4

char *actTime(char *str);
int createSocket(char *host, char *port);
int processCommand(char* message, char **messageToSend);
int processRequest(int fd, int pid);
int runCommand(int code, char* messageToSend);
int serverMultiplexChildProcess(int sfd, int pid);

int main(int argc, char *argv[]){

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <host> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int sfd = createSocket(argv[1], argv[2]);//Socket File Descriptor

    int i = 0;
    for(; i < MAX_CHILDS; ++i){
        pid_t pid = fork();
        switch(pid){
        case -1:
            perror("fork");
            exit(1);
        case 0: //Hijo
            printf("Child %i (Father %i)\n", getpid(), getppid());
            serverMultiplexChildProcess(sfd, getpid());
            printf("Child %i recieved a FINISH signal...\n");
            break;
        default: //Padre
            if(i == (MAX_CHILDS-1)){
                printf("Father %i\n", getpid());
                printf("Father: waiting all child for finishing...\n");
                wait(NULL);
            }
        }
    }

    return 0;
}

int serverMultiplexChildProcess(int sfd, int pid){
    //Parte Multiplexacion
    fd_set rfds;//Set de lectura
    struct timeval timeout; 
    int changes = -1;

    do{
        timeout.tv_sec = TIMEOUT_SECONDS;
        timeout.tv_usec = 0;
        FD_ZERO(&rfds);
        FD_SET(sfd, &rfds);
        FD_SET(STDOUT_FILENO, &rfds);

        int nfds = (sfd < STDOUT_FILENO) ? STDOUT_FILENO : sfd; //max fd + 1 para el select
        ++nfds;//+1 para select

        //No usamos los sets de escritura o excepciones
        changes = select(nfds, &rfds, NULL, NULL, &timeout);

        if(changes == -1){
            perror("Select Error");
            return -1;
        }
        else if(changes == 0){
            printf("Child %i --> server select(2): %is has passed\n", pid, TIMEOUT_SECONDS);
        }
        else{
            if(FD_ISSET(sfd, &rfds)){
                changes = processRequest(sfd, pid);
            }
            else if(FD_ISSET(STDOUT_FILENO, &rfds)){
                changes = processRequest(STDOUT_FILENO, pid);
            }            
        }
    }while(changes != FINISH);

    return 0;
}

int processRequest(int fd, int pid){
    char buf[BUF_SIZE];

    struct sockaddr_storage peer_addr;
    socklen_t peer_addr_len = sizeof(struct sockaddr_storage);

    int s = SERVER_ORIGIN; //Usado para recoger el resultado de getnameinfo, si se envia por STDOUT conserva su valor inicial
    char host[NI_MAXHOST], service[NI_MAXSERV]; //Buffers para guardar el nombre del host y el servicio
    ssize_t nread = -1;
    if(fd == STDOUT_FILENO){ //Leer de terminal
        nread = read(STDOUT_FILENO, buf, BUF_SIZE);
    }
    else{ //Leer mensaje recibido en la red
        nread = recvfrom(fd, buf, BUF_SIZE, 0, (struct sockaddr *) &peer_addr, &peer_addr_len);
        s = getnameinfo((struct sockaddr *) &peer_addr, peer_addr_len, host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICSERV);
    }

    if (nread == -1)
        return 0; //continue;   /* Ignore failed request */

    if(buf[nread-1] == '\n') //Evitar dobles saltos de linea al imprimir/Para recibir con saltos de lineas --> eliminar este if
        buf[nread-1] = '\0';

    if (s == 0){
        printf("Child %i --> Received %ld bytes from %s:%s\n", pid, (long) nread, host, service);
        printf("Child %i --> Received message: %s\n", pid, buf); 
    }
    else if(s == SERVER_ORIGIN){
        printf("Child %i --> Server message recieved: %s\n", pid, buf);
    }
    else
        fprintf(stderr, "getnameinfo: %s\n", gai_strerror(s));
    
    //Enviar respuesta al cliente
    char *messageToSend = "";
    int code = processCommand(buf, &messageToSend);
    int result = runCommand(code, messageToSend); //Lee el comando recibido y lo ejecuta. Ej: devolver FINISH para terminar, // o imprimir mensajes de informacion extra
    
    if(result == FINISH) //enviamos la señal de FINISH
        return result;

    //Enviar mensaje por terminal
    if(fd == STDOUT_FILENO){
        printf("%s\n", messageToSend);
        free(messageToSend);
        return 0;
    }

    //Enviar mensaje al cliente
    size_t messageToSendLen = strlen(messageToSend);
    if (sendto(fd, messageToSend, messageToSendLen, 0, 
                (struct sockaddr *) &peer_addr, peer_addr_len) != messageToSendLen){
        free(messageToSend);
        fprintf(stderr, "Error sending response\n");
    }

    free(messageToSend);
    memset(buf,'\0',BUF_SIZE);//Si no reseteamos el array, el tamaño del buffer se lee incorrectamente

    printf("----------\n");
    return result;
}

int runCommand(int code, char *messageToSend){
    if(code == FINISH){
        printf("Exit recieved...\n");
        return FINISH;
    }
    else if(code == OTHER_MESSAGE){
        printf("This won't be sent: %s\n", messageToSend);
        messageToSend[0] = '\r';
        messageToSend[1] = '\0';
    }
    return 0;
}


int processCommand(char* message, char** messageToSend){
    if(message == NULL)
        return -1;
    
    if((strlen(message) == 2 && (message[1] == '\0' || message[1] == '\n')) || strlen(message) == 1){ //char + "\0"
        char a = message[0];
        switch(a){
            case 't':
                *messageToSend = actTime("%H:%M:%S\n");
                return CURRENT_TIME_HOUR;
            case 'd':
                *messageToSend = actTime("%d/%m/%y\n");
                return CURRENT_DATE;
            case 'q':
                *messageToSend = "Connection Finished\n"; //No se enviará pero puede usarse para pruebas
                return FINISH;
        }
    }

    *messageToSend = malloc(sizeof("Command not Found:") + sizeof(message) + 1);
    *messageToSend[0] = '\0'; //Para evitar los caracteres basura al hacer strcat
    strcat(*messageToSend, "Command not Found:");
    strcat(*messageToSend, message);
    strcat(*messageToSend, "\n");
    return OTHER_MESSAGE;
}

int createSocket(char *host, char *port){
    struct addrinfo hints;
    struct addrinfo *result;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_DGRAM; //IMPORTANTE , ES UDP
    hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
    hints.ai_protocol = 0;          /* Any protocol */
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    int s = getaddrinfo(host, port, &hints, &result); //result = posibles hosts...
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }

    struct addrinfo *rp;
    int sfd = -1;
    for (rp = result; rp != NULL; rp = rp->ai_next) {

        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol); //socket file descriptor
        if (sfd == -1)
            continue;

        if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
            break; /* Success */

        close(sfd);
    }

    if(rp == NULL){
        fprintf(stderr, "Could not bind\n");
        exit(EXIT_FAILURE);
    }
    freeaddrinfo(result); /* No longer needed */
    return sfd;
}

//str is a formated string like str = "%H:%M:%S\n"
char *actTime(char *str){
    char outstr[200];
    time_t t;
    struct tm *tmp;

    t = time(NULL);
    tmp = localtime(&t);
    if (tmp == NULL) {
        perror("localtime");
        exit(EXIT_FAILURE);
    }
    if (strftime(outstr, sizeof(outstr), str, tmp) == 0) {
        fprintf(stderr, "strftime returned 0");
        exit(EXIT_FAILURE);
    }
    char *s = malloc(sizeof(outstr));
    strcpy(s, outstr);
    return s;
}