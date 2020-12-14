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


//Info sobre ps https://kb.iu.edu/d/afnv

/*
struct rlimit {
               rlim_t rlim_cur;  /* Soft limit 
               rlim_t rlim_max;  /* Hard limit (ceiling for rlim_cur) 
           };
*/

int main(int argc, char *argv[]){

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
   printf("Ruta Absoluta del Directorio de Trabajo: %s\n", wd);
   free(wd);
   printf("PID: %ld | PPID: %ld  | PGID: %ld  | SID: %ld\n", pid, ppid, pgid, sid);


    struct rlimit rlim;
    int ok = getrlimit(RLIMIT_NOFILE, &rlim);

    if(ok == -1){
        perror("Error");
        return -1;
    }
    printf("Número Máximo de Ficheros que puede Abrir el Proceso Actual: %ld\n", rlim.rlim_cur);

    return 0;
}
