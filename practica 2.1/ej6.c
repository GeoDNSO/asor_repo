#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h> //library for perror(*str)
#include <sys/utsname.h> //library for uname
#include <unistd.h> //library for sysconf




int main(){

    int macros[] = {_SC_ARG_MAX, _SC_CHILD_MAX, _SC_OPEN_MAX};
    
    //Text used in each sysconf(macros)
    char* msg[] = {"Max Arguments %ld\n", 
        "Max Numer of Simultaneous Processes per user ID %ld\n", 
        "Max number of files that a process can have open %ld\n"
        };

    int arr_length = sizeof(macros)/sizeof(int);
    int i = 0;

    for(; i < arr_length; i++){
        int v = sysconf(macros[i]);

        if(v == -1){
            perror("Invalid Name, Error");
            return -1;
        }

        printf(msg[i], v);
    }

    return 0;
}