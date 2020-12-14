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

/*
¿En qué casos se imprime la cadena? ¿Por qué? 
Se imprime en el caso de la llamada a system, porque a diferencia de execvp()
no sustiye la imagen del proceso actual por el del comand
*/

int main(int argc, char *argv[]){
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