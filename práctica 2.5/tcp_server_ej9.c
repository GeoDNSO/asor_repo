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

#define MAX_BACKLOG 5 //Numero maximo de conexiones en la cola

#define MAX_CHILDS 5

#define ERROR -1
#define CURRENT_TIME_HOUR 1
#define CURRENT_DATE 2
#define FINISH 3
#define OTHER_MESSAGE 4

void signal_sighld_handler(int signnum);
char *actTime(char *str);
int createSocket(char *host, char *port);
int child(int clisd, struct sockaddr_storage peer_addr, socklen_t peer_addr_len);
int processCommand(char* message, char** messageToSend);

int main(int argc, char *argv[]){

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <host> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int sfd = createSocket(argv[1], argv[2]);//Socket File Descriptor

    int err = listen(sfd, MAX_BACKLOG);
    if(err == -1){
        perror("Listen");
        return -1;
    }

    struct sigaction sact;
    sact.sa_handler = signal_sighld_handler;
    sact.sa_flags = SA_RESTART;
    int r;
    r = sigaction(SIGCHLD, &sact, NULL);
    if(r == -1){
        perror("sigaction error");
        return -1;
    }


    for (;;) {
        struct sockaddr_storage peer_addr;
        socklen_t peer_addr_len = sizeof(struct sockaddr_storage);

        pid_t pid;

        int i = 0;
        int clisd = -1;
 
        for(;i < MAX_CHILDS; ++i){
            clisd = accept(sfd, (struct sockaddr *) &peer_addr, &peer_addr_len);
            if(clisd == -1){
                perror("Accept");
                exit(EXIT_FAILURE);
            }
            
            pid = fork();

            //Los hijos no generan más procesos...
            if(pid == -1 || pid == 0){
                break;
            }
        }

        switch(pid){
            case -1:
                perror("fork");
                exit(1);
            case 0: //Hijo
                printf("Child %i (Father %i)\n", getpid(), getppid());
                child(clisd, peer_addr, peer_addr_len);
                printf("Child %i recieved a FINISH signal...\n", getpid());
                exit(EXIT_SUCCESS);
                break;
            default: //Padre
                printf("Father %i\n", getpid());
                printf("Father: waiting all child for finishing\n");
                wait(NULL);
                printf("No more childs...\n");
                exit(EXIT_SUCCESS);
        }
    }

    return 0;
}

void signal_sighld_handler(int signnum){
    int status = -1;
    wait(&status); // Necesario??? COn bucle????
    printf("Status: %i\n");
}

int child(int clisd, struct sockaddr_storage peer_addr, socklen_t peer_addr_len){
    char host[NI_MAXHOST], service[NI_MAXSERV];

    int s = getnameinfo((struct sockaddr *) &peer_addr, peer_addr_len, host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICSERV|NI_NUMERICHOST);

    if (s == 0){
        printf("Conection from %s:%s\n", host, service);
    }
    else
        fprintf(stderr, "getnameinfo: %s\n", gai_strerror(s));

    while(1){
        char buf[BUF_SIZE];
        ssize_t nread = recv(clisd, buf, BUF_SIZE, 0);
        if(nread == 0){ //El cliente ha cerrado la conexion
            break;
        }
        if (nread == -1)
            continue;   /* Ignore failed request */
        //Enviar respuesta al cliente
        char *messageToSend = "";
        int code = processCommand(buf, &messageToSend);
        if(code == FINISH){
            printf("Exit recieved...\n");
            close(clisd);
            break;
        }
        //Comentar if para enviar errores al cliente
        printf("Recieved: %s", messageToSend);    
        size_t messageToSendLen = strlen(messageToSend);
        if (send(clisd, messageToSend, messageToSendLen, 0) != messageToSendLen){
            free(messageToSend);
            fprintf(stderr, "Error sending response\n");
        }
        
        free(messageToSend);
        memset(buf, '\0', BUF_SIZE);//Si no reseteamos el array, el tamaño del buffer se lee incorrectamente
        printf("----------\n");
    }
    printf("-----Client has left...-----\n");
    close(clisd);
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
            case 'Q':
                *messageToSend = "Connection Finished\n"; //No se enviará pero puede usarse para pruebas
                return FINISH;
        }
    }

    *messageToSend = malloc(sizeof(message) + 1);
    *messageToSend[0] = '\0'; //Para evitar los caracteres basura al hacer strcat
    strcat(*messageToSend, message);
    //strcat(*messageToSend, "\n");
    return OTHER_MESSAGE;
}

int createSocket(char *host, char *port){

    struct addrinfo hints;
    struct addrinfo *result;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; //IMPORTANTE , ES UDP
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

//str is a formated string like "%H:%M:%S\n"
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

    //char *str = "%H:%M:%S\n";
    if (strftime(outstr, sizeof(outstr), str, tmp) == 0) {
        fprintf(stderr, "strftime returned 0");
        exit(EXIT_FAILURE);
    }
    char *s = malloc(sizeof(outstr));
    strcpy(s, outstr);
    return s;
}