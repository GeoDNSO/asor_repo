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

    if (argc != 4) {
        fprintf(stderr, "Usage: %s <host> <port> <command>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    //Se podrian devolver el valor de rp obtenido en la funcion para la funcion sendto(...)
    int sfd = createSocket(argv[1], argv[2]);//Socket File Descriptor

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    //Enviar texto recibido por parametro
    size_t messageToSendLen = strlen(argv[3]);
    if (sendto(sfd, argv[3], messageToSendLen, 0, 
                (struct sockaddr *) &servaddr, sizeof(servaddr)) != messageToSendLen){
        fprintf(stderr, "Error sending response\n");
    }

    struct sockaddr_storage peer_addr;
    socklen_t peer_addr_len = sizeof(struct sockaddr_storage);
    char buf[BUF_SIZE];
    buf[0] = '\0';
    ssize_t nread = recvfrom(sfd, buf, BUF_SIZE, 0, (struct sockaddr *) &peer_addr, &peer_addr_len);
    if (nread == -1)
        return 0;   /* Ignore failed request */

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

    return 0;
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

        if (connect(sfd, rp->ai_addr, rp->ai_addrlen) == 0) //USAMOS CONNECT E VEZ DE BIND
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