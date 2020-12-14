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



/*
Ej7 -->
¿Qué sucede si el proceso padre termina antes que el hijo (observar el PPID del proceso hijo)? 
Que otro proceso lo "adopta", en este caso, el proceso con PID 1

¿Y si el proceso que termina antes es el hijo (observar el estado del proceso hijo con ps)?
Que el hijo tiene el ppid el pid del proceso padre
*/

int processInfo(char *process);

int main(int argc, char *argv[]){

    pid_t pid;
    pid = fork();

    switch (pid) {
        case -1:
            perror("Error");
            exit(1);
        case 0:{
            //sleep(3);
            pid_t sidHijo;
            sidHijo = setsid(); //Crea una nueva sesión y grupo de procesos
            if(sidHijo == -1){
                perror("Error");
                return -1;
            }
            int ok =  chdir("/tmp");
            if(ok == -1){
                perror("Error");
                return -1;
            }
            return processInfo("Hijo");
        }
        default:
            return processInfo("Padre");
        break;
    }
}

int processInfo(char *process){
   pid_t pid = getpid();
   pid_t ppid = getppid();
   pid_t pgid = getpgid(0); //0 --> Proceso actual
   if(pgid == -1){
       perror("Error");
       return -1;
   }

   pid_t sid = getsid(0);
   if(sid == -1){
       perror("Error");
       return -1;
   }

   char *wd=(char *)get_current_dir_name();
   if(wd == NULL){
       perror("Error");
       return -1;
   }

   struct rlimit rlim;
   int ok = getrlimit(RLIMIT_NOFILE, &rlim);

    if(ok == -1){
        perror("Error");
        return -1;
    }

   printf("---PROCESO: %s---\n", process);
   printf("Ruta Absoluta del Directorio de Trabajo: %s\n", wd);
   free(wd);
   printf("PID: %ld | PPID: %ld  | PGID: %ld  | SID: %ld\n", pid, ppid, pgid, sid);
   
   printf("Número Máximo de Ficheros que puede Abrir el Proceso Actual: %ld\n", rlim.rlim_cur);

   return 0;
}
