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

#define BUFF_SIZE 256
#define TIMEOUT_SECONDS 5

int main(int argc, char *argv[]){

    int fd[2] = {-1, -1};
    char *pipeName[2];

    //No se han dado tuberias sobre la que trabajar --> las creamos
    if (argc < 3) {
        pipeName[0] = "ex_pipe1";
        pipeName[1] = "ex_pipe2";
        int i = 0;
        for(; i < 2; ++i){
            int ok = mkfifo(pipeName[i], 00777);
            if(ok == -1 && errno != EEXIST){ //Si el pipe existia podemos ejecutar el programa
                perror("mkfifo Error");
                return -1;
            }
        }
    }
    else{ //Abrimos el pipe que se ha pasado por parametro
        pipeName[0] = argv[1];
        pipeName[1] = argv[2];
    }
    
    fd[0] = open(pipeName[0], O_RDONLY | O_NONBLOCK);
    fd[1] = open(pipeName[1], O_RDONLY | O_NONBLOCK);
    if(fd[0] == -1 || fd[1] == -1){
        perror("Open Error");
        return -1;
    }

    char buffer[BUFF_SIZE];

    fd_set rfds;//Set de lectura
    struct timeval timeout;

    int changes = -1;

    do{
        int currentPipe = 0;

        timeout.tv_sec = TIMEOUT_SECONDS;
        timeout.tv_usec = 0;

        FD_ZERO(&rfds);
        FD_SET(fd[0], &rfds);
        FD_SET(fd[1], &rfds);

        //max fd + 1 para el select
        int nfds = (fd[0] < fd[1]) ? fd[1] : fd[0];
        ++nfds;//+1 para select

        //No usamos los sets de escritura o excepciones
        changes = select(nfds, &rfds, NULL, NULL, &timeout);

        if(changes == -1){
            perror("Select Error");
            return -1;
        }
        else if(changes == 0){
            printf("Timeout[%is], trying again...\n", TIMEOUT_SECONDS);
        }
        else{

            if(FD_ISSET(fd[0], &rfds)){
                currentPipe = 0;
            }
            else if(FD_ISSET(fd[1], &rfds)){
                currentPipe = 1;
            }
            
            int bytesRead = read(fd[currentPipe], buffer, 256);
            if (buffer[bytesRead-1] == '\n')
                buffer[bytesRead-1] = '\0';
            else
                buffer[bytesRead] = '\0';

            if(bytesRead == -1){
                perror("Read error");
                return -1;
            }
            else if(bytesRead == 0){
            
                close(fd[currentPipe]);
                fd[currentPipe] = open(pipeName[currentPipe], O_RDONLY | O_NONBLOCK);
                
                if(fd[currentPipe] == -1){
                    close(fd[0]);
                    close(fd[1]);
                    perror("Open Error");
                    return -1;
                }
            }
            else{
                printf("Tuberia %i[%s]: %s\n", currentPipe, pipeName[currentPipe], buffer);
            }
        }

    }while(changes < 1);

    close(fd[0]);
    close(fd[1]);

    return 0;
}