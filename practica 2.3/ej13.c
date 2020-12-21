#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h> //library for perror(*str)
#include <sys/utsname.h> //library for uname
#include <unistd.h> //library for sysconf and pathconf
//libraries for getpwnam() and getpwuid()
#include <sys/types.h>
#include <pwd.h>
#include <sys/time.h>
#include <time.h>
#include <locale.h>
//libraries for open(), stat()
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sched.h>
#include <sys/resource.h>
#include <signal.h>
#include <limits.h>

void signal_delete_handler(int signnum);

volatile static int delete = 1;


int main(int argc, char *argv[]){

    if (argc < 2) {
        fprintf(stderr, "Usage: ./ej13.c seconds\n");
        exit(EXIT_FAILURE);
    }

    long int seconds = strtol(argv[1], NULL, 10);

    if(seconds == LONG_MIN || seconds == LONG_MAX){
        perror("strtol error");
        return -1;
    }

    struct sigaction sact;
    sact.sa_handler = signal_delete_handler;
    sact.sa_flags = SA_RESTART;

    int r;
    r = sigaction(SIGUSR1, &sact, NULL);
    if(r == -1){
        perror("sigaction error");
        return -1;
    }

    sleep(seconds); //al recibir la señal, no se ignora
    
    if(delete){
        printf("Han pasado %is\n", seconds);
        printf("No se recibio SIGUSR1, el ejecutable se borrará\n");
        remove(argv[0]);
        return 2;
    }
        
    printf("Se recebió la señal SIGUSR1\n")
;    return 0;
}

void signal_delete_handler(int signnum){
    if(signnum == SIGUSR1)
        delete = 0;
}