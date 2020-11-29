#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h> //library for perror(*str)
#include <sys/utsname.h> //library for uname
#include <unistd.h> //library for sysconf and pathconf

//libraries for getpwnam() and getpwuid()
#include <sys/types.h>
#include <pwd.h>

 
int main(){

    struct passwd *pw = getpwuid(getuid());
    //struct passwd *pw = getpwnam("root");

    printf("User ID: %i \n", getuid());
	printf("Effective ID: %i \n", geteuid());

    printf("Username: %s\n", pw->pw_name);
    printf("Home Directory: %s\n", pw->pw_dir);
    printf("Username Information: %s\n", pw->pw_gecos);

    return 0;
}