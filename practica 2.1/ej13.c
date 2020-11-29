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

#define CICLES 1000000
 
int main(){

    struct timeval tvi, tvf;

    int v =  gettimeofday(&tvi, NULL);
    if(v == -1){
        perror("Error");
        return -1;
    }

    int i = 0;
    for(; i < CICLES; i++){
    }

    v =  gettimeofday(&tvf, NULL);
    if(v == -1){
        perror("Error");
        return -1;
    }

    printf("Time Elapsed: %ld us\n", (tvf.tv_usec - tvi.tv_usec));
    return 0;
}