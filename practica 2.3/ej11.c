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


/*
Ejercicio 10. En un terminal, arrancar un proceso de larga duración (ej. sleep 600). 
En otra terminal, enviar diferentes señales al proceso, comprobar el comportamiento. 
Observar el código de salida del proceso. ¿Qué relación hay con la señal enviada?

El código de salida está asociado a la señal enviada, si uso el comando
kill -s SIGINT 999

el proceso con PID 999, devolverá 130, lo que quiere decir que se ejecuto ctrl + c
*/

#define SLEEP_SECS 10

int main(int argc, char *argv[]){

    sigset_t blk_set, wait_set;

    sigemptyset(&blk_set);
    sigaddset(&blk_set, SIGINT);
    sigaddset(&blk_set, SIGTSTP);

    //Bloqueo del programa
    sigprocmask(SIG_BLOCK, &blk_set, NULL);


    printf("Durmiendo %is...\n", SLEEP_SECS);

    sleep(SLEEP_SECS);

    //Miramos la señal pendiente
    sigpending(&wait_set);

    if(sigismember(&wait_set, SIGINT) == 1){
        printf("Se envio una señal de SIGINT\n");
    }

    if(sigismember(&wait_set, SIGTSTP) == 1){
        printf("Se envio una señal de SIGTSTP\n");
         /* Código protegido */
        sigprocmask(SIG_UNBLOCK, &blk_set, NULL);
        // COn kill -CONT PID reanudamos
    }

    printf("Fin del Programa\n");
   
    return 0;
}