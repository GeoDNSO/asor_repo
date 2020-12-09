#include <dirent.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>


/*
Escribir un programa que cumpla las siguientes especificaciones:
    El programa tiene un único argumento que es la ruta a un directorio. El programa debe comprobar la corrección del argumento.

    El programa recorrerá las entradas del directorio de forma que:
        Si es un fichero normal, escribirá el nombre.
        Si es un directorio, escribirá el nombre seguido del carácter ‘/’.
        Si es un enlace simbólico, escribirá su nombre seguido de ‘->’ y el nombre del fichero enlazado. Usar readlink(2) y dimensionar adecuadamente el buffer.
        Si el fichero es ejecutable, escribirá el nombre seguido del carácter ‘*’.

    Al final de la lista el programa escribirá el tamaño total que ocupan los ficheros (no directorios) en kilobytes.

*/

long long sizeOfFile(struct stat sb);  
char *typeOfFile(struct stat sb);           //returns a string of chars representantion of type of the file/directory stat given
void showInfo(struct stat sb, char *name);  //show the information about the file/directory like ls -l command --> "Permisos" | "Tamaño" | "i-Nodo" | "Tipo" | "FecMod" | "Nombre"

int main(int argc, char *argv[]) {
    DIR *directory = NULL;
    struct stat sb;
    struct dirent *cDirect;
    char cwd[256];

    //No arguments --> show information about the current directory
    if (argc != 2) {
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            perror("getcwd() error");
            exit(EXIT_FAILURE);
        }

        else {
            printf("current working directory is: %s\n", cwd);
            directory = opendir(cwd);  //Assign to directory the current directory
            argv[1] = cwd;
        }

    }
    //If not, show the information about the file/directory given...
    else {
        directory = opendir(argv[1]);  //Controlar errores
    }
    //Load the stat structure
    if (lstat(argv[1], &sb) == -1) {
        perror("lstat error");
        closedir(directory);
        exit(EXIT_FAILURE);
    }

    long long totalSize = 0;
    //If its not a directory, show information and exit
    if ((sb.st_mode & S_IFMT) != S_IFDIR) {
        showInfo(sb, basename(argv[1]));
        printf("Total size: %lu KiB", sizeOfFile(sb));
        closedir(directory);
        exit(EXIT_SUCCESS);
    }

    while ((cDirect = readdir(directory)) != NULL) {
        totalSize += sizeOfFile(sb);
        lstat(cDirect->d_name, &sb);  //Stat of current file/directory
        showInfo(sb, cDirect->d_name);
    }
    printf("Total size: %lu KiB\n", (totalSize/1024));

    closedir(directory);
    exit(EXIT_SUCCESS);
}

void showInfo(struct stat sb, char *name) {
    char *s = typeOfFile(sb);

    if(strcmp ("->", s) == 0){

        int bufsiz = sb.st_size + 1;

        /* Some magic symlinks under (for example) /proc and /sys
        report 'st_size' as zero. In that case, take PATH_MAX as
        a "good enough" estimate. */

        if (sb.st_size == 0)
            bufsiz = PATH_MAX;

        char *buf = malloc(bufsiz);
        int nbytes = readlink(name, buf, bufsiz);
            if (nbytes == -1) {
                free(buf);
                perror("readlink");
                exit(EXIT_FAILURE);
            }
        printf("%s -> %s\n", name, buf);
        free(buf);
    }
    else{
        printf("%s%s\n", typeOfFile(sb), name);
    }
}

long long sizeOfFile(struct stat sb){
    switch (sb.st_mode & S_IFMT) {
        case S_IFDIR: //Directorio
           return 0;
        default:
            return sb.st_size;
    }
}

char *typeOfFile(struct stat sb) {
    char *tipo = "";

    if ((sb.st_mode & S_IEXEC) != 0){
        tipo = "*";
    }

    switch (sb.st_mode & S_IFMT) {
        case S_IFBLK:
            tipo = "block device";
            break;
        case S_IFCHR:
            tipo = "character device";
            break;
        case S_IFDIR: //Directorio
            tipo = "/";
            break;
        case S_IFIFO:
            tipo = "FIFO/pipe";
            break;
        case S_IFLNK: //Enlace Simbolico
            tipo = "->";
            break;
        case S_IFREG: //Regular file
            //Si es un fichero normal no hacemos nada
            break;
        case S_IFSOCK:
            tipo = "socket";
            break;
        default:
            tipo = "unknown?";
            break;
    }
    return tipo;
}