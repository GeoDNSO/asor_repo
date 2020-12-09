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

#define TIME_SUSPENDED 10
/*
 struct flock {
    ...
    short l_type;    /* Type of lock: F_RDLCK,
                        F_WRLCK, F_UNLCK 
    short l_whence;  /* How to interpret l_start:
                        SEEK_SET, SEEK_CUR, SEEK_END 
    off_t l_start;   /* Starting offset for lock 
    off_t l_len;     /* Number of bytes to lock 
    pid_t l_pid;     /* PID of process blocking our lock
                        (F_GETLK only) 
    ...
};
*/

char *actTime();

int main(int argc, char *argv[]){

    if (argc != 2) {
		fprintf(stderr, "Usage: %s <pathname>\n", argv[1]);
		exit(EXIT_FAILURE);
	}

	int fd = open(argv[1], O_RDWR);

	if(fd == -1){
		perror("Error");
	}

    struct flock lock;

    lock.l_type = F_UNLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    /*
    Specifying 0 for  l_len  has the special meaning: lock all bytes starting at the location specified by l_whence and l_start
    through to the end of file, no matter how large the file grows.
    */
    lock.l_len = 0; 
    lock.l_pid = getpid();


    int a = fcntl(fd, F_GETLK, &lock);
    printf("Lock antes: %i\n", lock.l_type);
	
    if(lock.l_type == F_UNLCK){

        //Si fcntl devuelve F_UNLOCK, los campos se manteienen igual
        
        printf("File %s was unlocked\n", argv[1]);
        
        lock.l_type = F_WRLCK;

        if(fcntl(fd, F_SETLK, &lock) == -1){ //No se bloquea para otro proceso??
            perror("fcntl 1 error");
            return -1;
        }
		
        printf("Lock despues: %i\n", lock.l_type);
		
        //Escribir en el fichero la hora actual
        char* s = actTime();
        write(fd, s, strlen(s));
        free(s);

        sleep(TIME_SUSPENDED);

        printf("%i s has passed suspended\n", TIME_SUSPENDED);

        
        lock.l_type = F_UNLCK;

        if(fcntl(fd, F_SETLK, &lock) == -1){
            perror("fcntl 2 error");
            return -1;
        }

        printf("File unlocked again\n");

        close(fd); //No olvidar cerrar fichero
    }
    else{
        printf("File %s is locked\n", argv[1]);
    }

    return 0;
}


 
char *actTime(){


    char outstr[200];
    time_t t;
    struct tm *tmp;

    t = time(NULL);
    tmp = localtime(&t);
    if (tmp == NULL) {
        perror("localtime");
        exit(EXIT_FAILURE);
    }

    char *str = "%H:%M:%S\n";
    if (strftime(outstr, sizeof(outstr), str, tmp) == 0) {
        fprintf(stderr, "strftime returned 0");
        exit(EXIT_FAILURE);
    }
    char *s = malloc(sizeof(outstr));
    strcpy(s, outstr);
    return s;
}