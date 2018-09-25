#define _XOPEN_SOURCE 500       // FTW flags
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>	// malloc functions
#include <dirent.h>	// dirent structure
#include <sys/stat.h> // mkdir
#include <errno.h>	// to know which errors are raised
#include <string.h>	// string functions
#include <ftw.h>    //nftw dir traversal
#include <unistd.h>	// dir functions
#include <sys/types.h>     // for pwd functions
#include "dirHelper.c"	// split and trim functions


char *path;

void deleteIt(char *path, char flag);
int removeFunction(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf);
int recursiveRemove(char *path);

int main(int argc,char **argv)
{

    path = malloc(256);

    char **args;


    /** Checking argument is given or not **/
    if(argc < 2)
    {
        printf("\nmyrm: missing operand\n\n");
    }
    else if(argc == 2)
    {
        int tokens = split(argv[1], ' ', &args);
        //printf("%d\n", tokens);
        if(tokens == 1)
        {
            if(strcmp(args[0], "-r") == 0)
                printf("myrm: missing operand\n");
            else
                deleteIt(args[0], 0);
        }
        else
        {
            int i;

            if(strcmp(args[0], "-r") == 0)
            {
                for(i = 1; i < tokens; i++)
                    deleteIt(args[i], 1);
            }
            else
            {
                for(i = 0; i < tokens; i++)
                    deleteIt(args[i], 0);
            }


        }
    }
    // This is used because our main function is using execvp() we are passing only one string with all arguments given to main function
    // So valid argc will be only one (if no arguments passed to main) or two
    else
    {
        printf("something went wrong args are not 2 %d\n",argc);
    }


    free(path);

    return 0;
}


/* 	This Function will remove files or directories
	if flag = 1 then remove recursive
	else flag = 0 then remove only 
*/
void deleteIt(char *path, char flag)
{
	// if dir then rmdir() is to be used
    if(isDir(path))
    {

        DIR * dir = opendir(path);

        // checking if directory exist or not
        if(dir)
        {
            if( rmdir(path) != 0)
            {
                if(!flag)
                    printf("myrm: cannot remove ‘%s’: %s\n", path, strerror(errno));
                else if(errno == EEXIST || errno == ENOTEMPTY)
                {
                    recursiveRemove(path);
                }

            }
        }
        else
        {
            printf("myrm: cannot remove '%s' : %s\n", path, strerror(errno));

        }
    }
    // if file unlink() to be used
    else
    {
        // unlink is used to remove files
        if( unlink(path) != 0)
            printf("myrm: cannot remove ‘%s’: %s\n", path, strerror(errno));

    }
}

/* Below functions are used to remove directory recursively if it is not empty */

/* Remove Recursive function */
int recursiveRemove(char *path)
{
    /*
    nftw() walks through the directory tree that is located under the directory dirpath, and calls fn() once for each entry in the tree.
    It uses flags.
    FTW_DEPTH
    If set, do a post-order traversal, that is, call fn() for the directory itself 
    after handling the contents of the directory and its subdirectories. 
    (By default, each directory is handled before its contents.) 
    */

    return nftw(path, removeFunction, 64, FTW_DEPTH | FTW_PHYS);
}

// This function will be passed as argument to nftw
int removeFunction(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    int status = remove(fpath);

    if (status)
        printf("Unable to remove %s\n", fpath);

    return status;
}



