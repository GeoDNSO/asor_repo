#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h> //library for perror(*str)
#include <sys/utsname.h> //library for uname
#include <unistd.h> //library for sysconf and pathconf


int main(){
    int macros[] = {_PC_LINK_MAX, _PC_PATH_MAX, _PC_NAME_MAX};
    
    //Text used in each pathconf(macros)
    char* msg[] = {"Max Number of Links: %ld\n", 
        "Max Path Length: %ld\n", 
        "Max File Name Length: %ld\n"
        };

    int arr_length = sizeof(macros)/sizeof(int);
    int i = 0;

    for(; i < arr_length; i++){
        int v = pathconf(".", macros[i]);

        if(v == -1){
            perror("Error");
            return -1;
        }

        printf(msg[i], v);
    }

    return 0;
}