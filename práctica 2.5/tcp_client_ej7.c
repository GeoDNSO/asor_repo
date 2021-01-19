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

int createSocket(char *host, char *port);

int main(int argc, char *argv[]){

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <host> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int sfd = createSocket(argv[1], argv[2]);//Socket File Descriptor

    while(1){
        char buf[BUF_SIZE];

        //Leer de terminal
        read(STDIN_FILENO, buf, BUF_SIZE);
        size_t messageToSendLen = strlen(buf);
        if (send(sfd, buf, messageToSendLen, 0) != messageToSendLen){
            fprintf(stderr, "Error sending response\n");
        }

        //Fin
        if(!strcmp(buf, "Q\n")){
            break;
        }
            
        ssize_t nread = recv(sfd, buf, BUF_SIZE, 0);
        if(nread == 0){ //El servidor ha cerrado la conexion
            break;
        }
        if (nread == -1)
            continue;   /* Ignore failed request */
        
        printf("%s", buf); //Mensaje del server
        memset(buf, '\0', BUF_SIZE);//Si no reseteamos el array, el tamaño del buffer se lee incorrectamente
    }


    return 0;
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

        if (connect(sfd, rp->ai_addr, rp->ai_addrlen) == 0) //Conect para cliente
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