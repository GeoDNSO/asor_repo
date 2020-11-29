#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h> //library for perror(*str)

int main(){

    if(setuid(0) == -1){ //Cambiar el ID del proceso
        char buff[60];
        char *msg = "Error code: %i --> %s";
        snprintf(buff, sizeof(buff), msg, errno, strerror(errno));
        printf("%s\n", buff);
        
        return -1;
    }
    
    return 0;

}