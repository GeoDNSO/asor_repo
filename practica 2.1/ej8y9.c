#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h> //library for perror(*str)
#include <sys/utsname.h> //library for uname
#include <unistd.h> //library for sysconf and pathconf


/*
id - print real and effective user and group IDs

setuid --> https://es.wikipedia.org/wiki/Setuid
*/

/*
Cuando el uid y el euid sean distintos podemos asegurar que 
el but setuid está activado
*/

int main(){

    printf("User ID: %i \n", getuid());
	printf("Effective ID: %i \n", geteuid());


    return 0;
}