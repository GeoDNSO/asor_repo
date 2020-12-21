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

volatile static int contSIGINT = 0;
volatile static int contSIGTSTP = 0;

void signal_cont_handler(int signnum);

int main(int argc, char *argv[]){
 
    struct sigaction sact;

    sact.sa_handler = signal_cont_handler;
    sact.sa_flags = SA_RESTART;

    int r;
    r = sigaction(SIGINT, &sact, NULL);

    if(r == -1){
        perror("sigaction error");
        return -1;
    }
    r = sigaction(SIGTSTP, &sact, NULL);
    if(r == -1){
        perror("sigaction error");
        return -1;
    }

    sigset_t wait_set;
    sigemptyset(&wait_set);

    while((contSIGINT + contSIGTSTP) < 10){
        sigsuspend(&wait_set);
    }

    printf("Señales SIGINT recibidas: %i\n", contSIGINT);
    printf("Señales SIGTSTP recibidas: %i\n", contSIGTSTP);

    return 0;
}

void signal_cont_handler(int signsum){

    if(signsum == SIGINT) 
        contSIGINT++;
    if(signsum == SIGTSTP)   
        contSIGTSTP++;
}