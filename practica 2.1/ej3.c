#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h> //library for perror(*str)

#define N_ERROR 255

int main(){
    int i = 0;
    for(; i < N_ERROR; i++){
        printf("Code Error: %i - %s\n", i, strerror(i)); //134 tipos de errores
    }
    
    return 0;

}