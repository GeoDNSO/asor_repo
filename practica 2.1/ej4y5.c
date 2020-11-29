#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h> //library for perror(*str)
#include <sys/utsname.h> //library for uname


/*
----Info del Sistema a partir de linea de comandos-----
[cursoredes@localhost practicas_asor]$ uname -a
Linux localhost.localdomain 3.10.0-862.11.6.el7.x86_64 #1 SMP Tue Aug 14 21:49:04 UTC 2018 x86_64 x86_64 x86_64 GNU/Linux
*/

/*
 struct utsname {
    char sysname[];     Operating system name (e.g., "Linux") 
    char nodename[];    Name within "some implementation-defined network" 
    char release[];     Operating system release (e.g., "2.6.28") 
    char version[];    Operating system version
    char machine[];    Hardware identifier 
    #ifdef _GNU_SOURCE
        char domainname[];  NIS or YP domain name 
    #endif
};
*/

int main(){

    struct utsname buf;

    int v = uname(&buf);

    if(v == -1){
        printf("Error: %i", errno);
        perror("");
        return -1;
    }

    printf("---System Info---\n");
    printf("Operating System Name: %s\n", buf.sysname);
    printf("Node Name: %s\n", buf.nodename);
    printf("Release: %s\n", buf.release);
    printf("Version: %s\n", buf.version);
    printf("Hardware Identifier: %s\n", buf.machine);

    return 0;
}