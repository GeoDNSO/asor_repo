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

int main(int argc, char *argv[]){

    if (argc < 4) {
        fprintf(stderr, "Usage: %s <command1> <param1> <command2> <param2>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int pipefd[2];

    int r = pipe(pipefd); //Sincroniza automatica en procesos de escritura/lectura --> no es necesario wait

    if(r == -1){
        perror("Pipe error");
        return -1;
    }

    pid_t pid;
    pid = fork();

    switch (pid) {
        case -1:
            perror("Error");
            exit(1);
        case 0:{ //Hijo
            //cualquier proceso de lectura usará el extremo de lectura de la tuberia y no el de la terminal
            dup2(pipefd[0], STDIN_FILENO); 

            //No necesitamos más los descriptores
            close(pipefd[0]);
            close(pipefd[1]);

            execlp(argv[3], argv[3], argv[4], NULL);
            
        }
        default: //Padre

            //lo que se escriba por printf se enviara al extremo de escritura del pipe
            dup2(pipefd[1], STDOUT_FILENO);

            //No necesitamos más los descriptores
            close(pipefd[0]);
            close(pipefd[1]);

            execlp(argv[1], argv[1], argv[2], NULL);
           
        break;
    };   

    return 0;
}

