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

int createSocket(char *host, char *port);
int child(int clisd, struct sockaddr_storage peer_addr, socklen_t peer_addr_len, int pid);

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

    //Manejadores de la señal SIGCHLD
    struct sigaction sact;
    sact.sa_handler = SIG_IGN; //No es necesario asignarle una funcion, la ignoramos
    sact.sa_flags = SA_RESTART;
    int r;

    
    r = sigaction(SIGCHLD, &sact, NULL);
    if(r == -1){
        perror("sigaction error");
        return -1;
    }    
    
    printf("-----------Father Server PID:%i------------\n", getpid());

    while(1){  //Aceptamos conexiones siempre que sea necesario...
        struct sockaddr_storage peer_addr;
        socklen_t peer_addr_len = sizeof(struct sockaddr_storage);

        int clisd = accept(sfd, (struct sockaddr *) &peer_addr, &peer_addr_len);
        
        if(clisd == -1){
            perror("Accept");
            exit(EXIT_FAILURE);
        }
        
        pid_t pid = fork();
        switch(pid){
            case -1:
                perror("fork");
                exit(1);
            case 0: //Hijo
                printf("Child %i (Father %i)\n", getpid(), getppid());
                child(clisd, peer_addr, peer_addr_len, getpid());
                printf("Child %i recieved a FINISH signal...\n", getpid());
                printf("---------------\n");
                return 0;
            default: //Padre
                close(clisd); //Cerramos el descriptor en el padre
                printf("Server Father:%i, new connection recieved...\n", getpid());
        }
    }
    return 0;
}

int child(int clisd, struct sockaddr_storage peer_addr, socklen_t peer_addr_len, int pid){
    char host[NI_MAXHOST], service[NI_MAXSERV];

    int s = getnameinfo((struct sockaddr *) &peer_addr, peer_addr_len, host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICSERV|NI_NUMERICHOST);

    if (s == 0){
        printf("Child Server with PID:%i r --> Conection from %s:%s\n", pid, host, service);
        printf("-------------------\n");
    }
    else
        fprintf(stderr, "getnameinfo: %s\n", gai_strerror(s));

    //Mantenemos la conexion hasta que se reciba una 'Q'
    while(1){
        char buf[BUF_SIZE];
        ssize_t nread = recv(clisd, buf, BUF_SIZE, 0);
        if(nread == 0){ //El cliente ha cerrado la conexion
            break;
        }
        if (nread == -1)
            continue;   /* Ignore failed request */

        //Si reicibimos Q terminamos
        if(strcmp(buf, "Q\n") == 0)
            break;

        printf("Child Server with PID:%i recieved: %s", pid, buf);    
        size_t messageToSendLen = strlen(buf);
        if (send(clisd, buf, messageToSendLen, 0) != messageToSendLen){
            fprintf(stderr, "Error sending response\n");
        }
        
        memset(buf, '\0', BUF_SIZE);//Si no reseteamos el array, el tamaño del buffer se lee incorrectamente
        printf("----------\n");
    }
    close(clisd);
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