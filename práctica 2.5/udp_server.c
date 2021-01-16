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

#define ERROR -1
#define CURRENT_TIME_HOUR 1
#define CURRENT_DATE 2
#define FINISH 3
#define OTHER_MESSAGE 4

int createSocket(char *host, char *port);
int processCommand(char* message, char **messageToSend);
char *actTime(char *str);

int main(int argc, char *argv[]){

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <host> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int sfd = createSocket(argv[1], argv[2]);//Socket File Descriptor

    /* Read datagrams and echo them back to sender */

    for (;;) {
        struct sockaddr_storage peer_addr;
        char buf[BUF_SIZE];

        socklen_t peer_addr_len = sizeof(struct sockaddr_storage);
        ssize_t nread = recvfrom(sfd, buf, BUF_SIZE, 0, (struct sockaddr *) &peer_addr, &peer_addr_len);
        if (nread == -1)
            continue;   /* Ignore failed request */

        char host[NI_MAXHOST], service[NI_MAXSERV];
        int s = getnameinfo((struct sockaddr *) &peer_addr, peer_addr_len, host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICSERV);
        if (s == 0){
            printf("Received %ld bytes from %s:%s\n", (long) nread, host, service);

            //Para evitar un doble salto de linea, si tocamos directamente buf, la aplicacion falla
            if(buf[nread-1] == '\n'){
                printf("Received message: %s", buf);
            }else{
                printf("Received message: %s\n", buf);
            }
            
        }
        else
            fprintf(stderr, "getnameinfo: %s\n", gai_strerror(s));

        //Enviar respuesta al cliente
        char *messageToSend = "";
        int code = processCommand(buf, &messageToSend);

        if(code == FINISH){
            printf("Exit recieved...\n");
            break;
        }
        //Comentar if para enviar errores al cliente
        if(code == OTHER_MESSAGE){
            printf("This won't be sent: %s\n", messageToSend);
            messageToSend[0] = '\r';
            messageToSend[1] = '\0';
        }
            

        size_t messageToSendLen = strlen(messageToSend);
        

        if (sendto(sfd, messageToSend, messageToSendLen, 0, 
                    (struct sockaddr *) &peer_addr, peer_addr_len) != messageToSendLen){

            free(messageToSend);
            fprintf(stderr, "Error sending response\n");
        }
        free(messageToSend);
        memset(buf,'\0',BUF_SIZE);//Si no reseteamos el array, el tamaño del buffer se lee incorrectamente
        printf("----------\n");
            
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