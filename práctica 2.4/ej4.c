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

//Ej3 --> https://stackoverflow.com/questions/4113986/example-of-using-named-pipes-in-linux-bash

int main(int argc, char *argv[]){

     if (argc < 2) {
        fprintf(stderr, "Usage: %s <named pipe>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int fd = open(argv[1], O_WRONLY);

    if(fd == -1){
        perror("Write Error");
        return -1;
    }

    int ok = write(fd, argv[1], strlen(argv[1]));
    if(ok == -1){
        perror("Write Error");
        return -1;
    }
	close(fd);

    return 0;
}