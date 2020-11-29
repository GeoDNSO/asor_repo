#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h> //library for perror(*str)
#include <sys/utsname.h> //library for uname
#include <unistd.h> //library for sysconf and pathconf

//libraries for getpwnam() and getpwuid()
#include <sys/types.h>
#include <pwd.h>

#include <time.h>


 
int main(){

    time_t t = time(NULL);
   
   /* Same as above
   time_t t;
   time(&t);
   */
    printf("Seconds since Epoch: %ld s\n", t);
    return 0;
}