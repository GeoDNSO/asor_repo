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


int main(int argc, char *argv[]){

    if (argc != 2) {
        fprintf(stderr, "Usage: %s host\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    struct addrinfo hints;
    struct addrinfo *result;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = 0;
    hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
    hints.ai_protocol = 0;          /* Any protocol */
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    

    int s = getaddrinfo(argv[1], NULL , &hints, &result); //result = posibles hosts...
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }

    /* getaddrinfo() returns a list of address structures. */
    //Iteramos la lista hasta que ai_next sea NULL


    /*
    Cda dirección se imprimirá en su valor numérico, usando getnameinfo(3) con el flag NI_NUMERICHOST, 
    así como la familia de direcciones y el tipo de socket.
    */
    printf("Posibles direcciones para crear un socket asociado a: %s\n", argv[1]);
    printf("--------------------------------------------\n");
    struct addrinfo *rp;
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        printf("Familia: %i \n", rp->ai_family); //???
        printf("Tipo de Socket: %i \n", rp->ai_socktype);
        printf("Protocolo: %i \n", rp->ai_protocol);

        char host[NI_MAXHOST], service[NI_MAXSERV]; //Estos parametrros pueden ser nulos en getnameinfo()
        s = getnameinfo(rp->ai_addr, rp->ai_addrlen, host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICHOST);
        if (s != 0) {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
            exit(EXIT_FAILURE);
        }

        printf("Host: %s\n", host);
        printf("Servicio: %s\n", service); //???

        printf("--------------------------------------------\n");
    }

    freeaddrinfo(result);           /* No longer needed */

    return 0;
}