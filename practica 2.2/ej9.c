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
struct stat {
    dev_t     st_dev;      ID of device containing file 
    ino_t     st_ino;      inode number 
    mode_t    st_mode;     file type and mode 
    nlink_t   st_nlink;    number of hard links 
    uid_t     st_uid;      user ID of owner 
    gid_t     st_gid;      group ID of owner 
    dev_t     st_rdev;     device ID (if special file) 
    off_t     st_size;     total size, in bytes 
    blksize_t st_blksize;  blocksize for filesystem I/O 
    blkcnt_t  st_blocks;   number of 512B blocks allocated 

   /* Since Linux 2.6, the kernel supports nanosecond
                  precision for the following timestamp fields.
                  For the details before Linux 2.6, see NOTES. 

    struct timespec st_atim;  /* time of last access 
    struct timespec st_mtim;  /* time of last modification 
    struct timespec st_ctim;  /* time of last status change 

*/

/*
 st_mtime
    This  is  the file's last modification timestamp.  It is changed by file modifications,
    for example, by mknod(2), truncate(2),  utime(2),  and  write(2)  (of  more  than  zero
    bytes).   Moreover,  st_mtime  of a directory is changed by the creation or deletion of
    files in that directory.  The st_mtime field is  not  changed  for  changes  in  owner,
    group, hard link count, or mode.


st_ctime
    This  is  the file's last status change timestamp.  It is changed by writing or by set‐
    ting inode information (i.e., owner, group, link count, mode, etc.).
*/

int main(int argc, char *argv[]){

    if(argc < 2){
        fprintf( stderr, "Error: Path/File required as argument\n", 35);
        return -1;
    }

    char * file = argv[1];
    struct stat sb;

    if (stat(file, &sb) == -1) {
        perror("stat");
        exit(EXIT_FAILURE);
    }

    printf("ID of containing device[major, minor]:  [%lu,%lu]\n",
    (long) major(sb.st_dev), (long) minor(sb.st_dev));
   
   printf("I-Node: %lu\n", sb.st_ino);

    printf("Type of File: ");
    //Informacion sobre la mascara en el manual
    switch (sb.st_mode & S_IFMT) {
        case S_IFBLK:  printf("block device\n");            break;
        case S_IFCHR:  printf("character device\n");        break;
        case S_IFDIR:  printf("directory\n");               break;
        case S_IFIFO:  printf("FIFO/pipe\n");               break;
        case S_IFLNK:  printf("symlink\n");                 break;
        case S_IFREG:  printf("regular file\n");            break;
        case S_IFSOCK: printf("socket\n");                  break;
        default:       printf("unknown?\n");                break;
    }

    printf("Last file access:  %s", ctime(&sb.st_atime));

   return 0;
}
