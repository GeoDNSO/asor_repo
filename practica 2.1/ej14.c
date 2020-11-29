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

#define BEGINNING_YEAR 1900
 
int main(){

    time_t t = time(NULL);
    
    struct tm *c_time;
    c_time = localtime(&t);

    if(c_time == NULL){
        perror("Error");
        return -1;
    }

    printf("Current year: %i\n", (BEGINNING_YEAR + c_time->tm_year)); //Years since 1900

    return 0;
}