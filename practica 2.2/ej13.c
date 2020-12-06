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

// https://unix.stackexchange.com/questions/89386/what-is-symbol-and-in-unix-linux


int main(int argc, char *argv[]){

struct stat sb;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <pathname>\n", argv[1]);
		exit(EXIT_FAILURE);
	}

	int fd = open(argv[1], O_CREAT | O_RDWR);

	if(fd == -1){
		perror("Error");
	}

	dup2(fd, STDOUT_FILENO);
    printf("...\n");
    printf("Escrito para salida estandar...\n");
    printf("...\n");

    dup2(fd, STDERR_FILENO);
    perror("Error");
    perror("Error2");

   return 0;
}
