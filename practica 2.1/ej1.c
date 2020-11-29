#include <stdio.h>
#include <stdlib.h>
#include <errno.h> //library for perror(*str)

int main(){

    if(setuid(0) == -1){
        perror("Error");
        return -1;
    }
    
    return 0;

}