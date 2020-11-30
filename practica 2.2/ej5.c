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

//libraries for open()
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

 
int main(int argc, char *argv[]){


    int fileDesc = open("hola", O_TRUNC | O_CREAT, 0645); //rw-r--r-x

    if(fileDesc == -1){
        perror("Error");
        return -1;
    }

   return 0;
}