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

    int p_h_fd[2];
    int h_p_fd[2];

    int r = pipe(p_h_fd);
    if(r == -1){
        perror("Pipe error");
        return -1;
    }

    r = pipe(h_p_fd);
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
            close(p_h_fd[1]);
            close(h_p_fd[0]);
           
            char c;
            int count = 0;
            while(count < 10){
                if(read(p_h_fd[0], &c, 1)==-1){
                    perror("Son Reading Father Error");
                    return -1;
                }
                
                printf("El caracter enviado por el padre fue %c\n", c);
                c = (count == 10) ? 'q' : 'l';
				sleep(1);
                if(write(h_p_fd[1], &c, 1) == -1){
                    perror("Son Writing Father Error");
                    return -1;
                }
                ++count;    
            }
            
            close(p_h_fd[0]);
            close(h_p_fd[1]);
        }
        default: //Padre
            close(p_h_fd[0]);
            close(h_p_fd[1]);

            char c;
            do{
                if((c = fgetc(stdin))==EOF){
                    perror("Father Reading Console Error");
                    return -1;
                }

                if(write(p_h_fd[1], &c, 1) ==-1){
                    perror("Father Writting Son Error");
                    return -1;
                }

                if(read(h_p_fd[0], &c, 1)==-1){
                    perror("Father Reading Son Error");
                    return -1;
                }
                printf("Padre recibió %c\n", c);
                
            }while(c != 'q');
            
            close(p_h_fd[1]);
            close(h_p_fd[0]);
        break;
    };   

    return 0;
}