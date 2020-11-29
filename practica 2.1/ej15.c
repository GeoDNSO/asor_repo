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
 
int main(int argc, char *argv[]){

    setlocale(LC_ALL, "spanish");

    char outstr[200];
    time_t t;
    struct tm *tmp;

    t = time(NULL);
    tmp = localtime(&t);
    if (tmp == NULL) {
        perror("localtime");
        exit(EXIT_FAILURE);
    }

    char *str = "%A, %d de %B de %Y, %H:%M";
    if (strftime(outstr, sizeof(outstr), str, tmp) == 0) {
        fprintf(stderr, "strftime returned 0");
        exit(EXIT_FAILURE);
    }

    printf("%s\n", outstr);
    exit(EXIT_SUCCESS);
}