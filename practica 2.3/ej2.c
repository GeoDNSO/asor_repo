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

/*
Ej 3 -->
[root@localhost pr2.3]# chrt -f -p 1 4677
[root@localhost pr2.3]# ./ej2 
Política de Planificación: FIFO
Máxima Prioridad: 99, Mínima Prioridad: 1
Prioridad del Proceso Actual: 1

*/

int main(int argc, char *argv[]){

    int policy = sched_getscheduler(0);//0 --> Proceso actual

    if(policy == -1){
        perror("Error");
        return -1;
    }
    printf("Política de Planificación: ");
    switch(policy){
        case SCHED_RR:
            printf("Round Robin\n");
        break;
        case SCHED_OTHER:
            printf("Round Robin\n");
        break;
       
        case SCHED_FIFO:
            printf("FIFO\n");
        break;
        default:
            printf("Planificador Desconocido\n");
        break; 
    }
        /*
        //ERROR
     case SCHED_BATCH:
            printf("Planificador Batch\n");
        break;
        case SCHED_IDLE:
            printf("Planificador Ocioso\n");
        break;
    */

    int max = sched_get_priority_max(policy);
    if(max == -1){
        perror("Error");
        return -1;
    }
    int min = sched_get_priority_min(policy);
    if(min == -1){
        perror("Error");
        return -1;
    }

    
    printf("Máxima Prioridad: %i, Mínima Prioridad: %i\n", max, min);

    struct sched_param scheduler;

    int ok = sched_getparam(0, &scheduler);

    if(ok == -1){
        perror("Error");
        return -1;
    }

    printf("Prioridad del Proceso Actual: %i\n", scheduler.sched_priority);


    return 0;
}
