#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>	// malloc functions
#include <dirent.h>	// dirent structure
#include <sys/stat.h> // mkdir
#include <errno.h>	// to know which errors are raised
#include <string.h>	// string functions
#include <unistd.h> // dir functions
#include <sys/types.h>     // for pwd functions
#include "dirHelper.c"  // split and trim functions


char *path;

void mkDirCommandHandler(char *arg);
char isAlreadyExists(char *path, char *dirName);
mode_t getumask(void);

int main(int argc,char **argv)
{

    path = malloc(256);
    //printf("%d\n", argc);
    char **args;



    /** Checking argument is given or not **/
    if(argc < 2)
    {
        printf("\nmymkdir: missing operand\n\n");
    }
    else if(argc == 2)
    {
        int tokens = split(argv[1], ' ', &args);
        //printf("%d\n", tokens);
        if(tokens == 1)
        {
            mkDirCommandHandler(args[0]);
        }
        else
        {
            int i;

            for(i = 0; i < tokens; i++)
                mkDirCommandHandler(args[i]);
        }
    }
    // This is used because our main function is using execvp() we are passing only one string with all arguments given to main function
    // So valid argc will be only one or two  
    else
    {
        printf("something went wrong args are not 2 but %d\n", argc);
    }


    free(path);

    return 0;
}

/* This function will create directory */
void mkDirCommandHandler(char *arg)
{
    

    char *dirName = arg;
    //printf("%s\n", dirName);
    mode_t umask = getumask();
    mode_t mask = 0777 - umask;
    getcwd(path, 255);

    /** No path given only Directory Name is given **/
    if(strchr(dirName, '/')==0)
    {

        strcat(path, "/");
        strcat(path, arg);
 
        DIR * dir = opendir(path);

        // checking if directory exist or not
        if(dir)
        {

            printf("mymkdir: cannot create directory ‘%s’: File exists\n", arg);

        }
        else if(errno == ENOENT)
        {
            if(mkdir(path, mask) != 0)
                printf("Error occurred during mkdir() Error : %s\n", strerror(errno));

        }

    }
    /** Relative Path given **/
    else if(arg[0] == '.')
    {

        arg += 1;
        strcat(path, arg);

        char *temp = strchr(dirName,'/');
        // finding last /
        while(temp)
        {
            //printf("%s\n",temp);
            dirName = temp + 1;
            temp = strchr(dirName,'/');
        }


        DIR * dir = opendir(path);

        // checking if directory exist or not
        if(dir)
        {
            printf("mymkdir: cannot create directory ‘%s’: File exists\n", arg-1);

        }
        else if(errno == ENOENT)
        {
            if(mkdir(path, mask) != 0)
                printf("Error occurred during mkdir() Error : %s\n", strerror(errno));

        }
    }
    /** Absolute Path given **/
    else
    {

        strcpy(path, arg);

        DIR * dir = opendir(path);

        // checking if directory exist or not
        if(dir)
        {
            printf("mymkdir: cannot create directory ‘%s’: File exists\n", arg);

        }
        else if(errno == ENOENT)
        {
            if(mkdir(path, mask) != 0)
                printf("Error occurred during mkdir() Error : %s\n", strerror(errno));

        }

 
    }

}

/* This function will return file creation mask and we will invert this mask to create files */
mode_t getumask(void)
{
    mode_t mask = umask( 0 );
    umask(mask);
    return mask;
}
