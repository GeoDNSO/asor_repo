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

char* pointerCharArrayToString(int argc, char* argv[]);
int processInfo(char *process);
int execvp_daemon(int argc, char *argv[]); //ej 7

int main(int argc, char *argv[]){

    pid_t pid;
    pid = fork();

    switch (pid) {
        case -1:
            perror("Error");
            exit(1);
        case 0:{
            pid_t sidHijo;
            sidHijo = setsid(); //Crea una nueva sesión y grupo de procesos
            if(sidHijo == -1){
                perror("Error");
                return -1;
            }
            int ok =  chdir("/tmp"); //Cambiar directorio
            if(ok == -1){
                perror("Error");
                return -1;
            }

            //Creamos los archivos/directorios
            int fd_err = open("daemon.err", O_CREAT | O_RDWR, 00777);
            int fd_out = open("daemon.out", O_CREAT | O_RDWR, 00777);
            int fd_in = open("/dev/null", O_CREAT | O_RDWR, 00777);
            if(fd_err == -1 || fd_in == -1 || fd_out == -1){
                perror("Error OPEN");
                return -1;
            }

            //Redirijir los flujos asociados al terminal
            if(dup2(fd_in, STDIN_FILENO) == -1){
                perror("Error IN");
                return -1;
            }
            if(dup2(fd_out, STDOUT_FILENO) == -1){
                perror("Error OUT");
                return -1;
            }
            if(dup2(fd_err, STDERR_FILENO) == -1){
                perror("Error ERR");
                return -1;
            }
            //ejecutar comando
            processInfo("Hijo"); //info del proceso hijo
            printf("El comando del daemon se ejecutará en 3s\n");
            sleep(3);//Esperamos a ejecutar el comando

            execvp_daemon(argc, argv);
        }
        default:
            return processInfo("Padre");
        break;
    }
}


int execvp_daemon(int argc, char *argv[]){
    if(argc == 1){
		fprintf(stderr, "Usage: ./ej7.c <command>\n");
		exit(EXIT_FAILURE);
    }
    char *command;
    if(argc == 2){
        command = argv[1];
    }else{
        command = pointerCharArrayToString(argc, argv);
    }

    //Comentar y descomentar según se necesite...
    //int ok = system(command);
    int ok = execvp(argv[1], argv+1);
    free(command);
    printf("El comando terminó de ejecutarse\n");
    return ok;
}

char* pointerCharArrayToString(int argc, char* argv[]){
    char *buff = malloc(sizeof(argv[1]) + 1);
    strcpy(buff, argv[1]);
    strcat(buff, " ");
    int i = 2;
    for(; i < argc; ++i){
        buff = (char *)realloc(buff, strlen(argv[i])+1);
        strcat(buff, argv[i]);
        strcat(buff, " ");
    }
    return buff;
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