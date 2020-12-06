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

//libraries for open(), stat()
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


/*
https://ostechnix.com/explaining-soft-link-and-hard-link-in-linux-with-examples/
ls -l fichero link --> Enlace fisico, mismo inodo
ls -i fichero link   --> Enlace simbolico, inodo diferente

SI el fichero referenciado se borra, 
    el enlace fisico no desaparece y mantiene el contenido del fichero borrado
    el enlace simbolico desaparece


[cursoredes@localhost pr2.2]$ ls
ej10y11.c  ej1.c  ej5  ej5.c  ej7  ej7.c  ej9  ej9.c  hardLink  statMan.txt  symbLink
[cursoredes@localhost pr2.2]$ stat hardLink 
  File: ‘hardLink’
  Size: 23452     	Blocks: 48         IO Block: 4096   regular file
Device: fd00h/64768d	Inode: 51384621    Links: 2
Access: (0664/-rw-rw-r--)  Uid: ( 1000/cursoredes)   Gid: ( 1000/cursoredes)
Access: 2020-12-06 16:10:53.991463316 +0100
Modify: 2020-12-06 16:10:54.090468949 +0100
Change: 2020-12-06 16:40:53.540837289 +0100
 Birth: -
[cursoredes@localhost pr2.2]$ stat symbLink 
  File: ‘symbLink’ -> ‘statMan.txt’
  Size: 11        	Blocks: 0          IO Block: 4096   symbolic link
Device: fd00h/64768d	Inode: 51384658    Links: 1
Access: (0777/lrwxrwxrwx)  Uid: ( 1000/cursoredes)   Gid: ( 1000/cursoredes)
Access: 2020-12-06 16:41:11.533860886 +0100
Modify: 2020-12-06 16:41:11.533860886 +0100
Change: 2020-12-06 16:41:11.533860886 +0100
 Birth: -

*/

int main(int argc, char *argv[]){

    if(argc < 2){
        fprintf( stderr, "Error: Path/File required as argument\n", 35);
        exit(EXIT_FAILURE);
    }

    char * file = argv[1];
    struct stat sb;

    if (stat(file, &sb) == -1) {
        perror("stat");
        exit(EXIT_FAILURE);
    }

    if(sb.st_mode & S_IFMT != S_IFREG){
        fprintf( stderr, "Error: Argument must be a file\n", 30);
        exit(EXIT_FAILURE);
    }

    char* symName = malloc(sizeof(file));
    char* hardName = malloc(sizeof(file));

    strncpy(symName, file, sizeof(file));
    strncpy(hardName, file, sizeof(file));

    strncat(hardName, ".hard", 5);
    strncat(symName, ".sym", 4);
    
    if(link(file, hardName) == -1){
        free(symName);
        free(hardName);
        perror("Hard Link Error");
        return -1;
    }

    if(symlink(file, symName) == -1){
        free(symName);
        free(hardName);
        perror("Sym Link Error");
        return -1;
    }

    free(symName);
    free(hardName);

   return 0;
}
