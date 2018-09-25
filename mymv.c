#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <dirent.h>
#include <sys/stat.h> // mkdir
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>     // for pwd functions
#include "dirHelper.c"


char *path;


char* getFileName(char *filePath);
void mvCommandHandler(char *arg, char *dest);

int main(int argc,char **argv)
{

    path = malloc(256);

    //printf("%d\n", argc);
    char **args;



    /** Checking argument is given or not **/
    if(argc < 2)
    {
        printf("\nmymv: missing file operand\n\n");
    }
    else if(argc == 2)
    {
        
        int tokens = split(argv[1], ' ', &args);
        //printf("%d\n", tokens);
        if(tokens == 1)
        {
            printf("mv: missing destination file operand after '%s'", args[0]);
        }
        else
        {
            int i,limit;
            limit = tokens - 1;

            for(i = 0; i < limit; i++)
            {
                mvCommandHandler(args[i], args[limit]);
            }
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

void mvCommandHandler(char *arg, char *dest)
{

    //printf("%s\n%s\n%c\n", arg, dest, dest[0]);
    // printf("%c\n", dest[0]);
    char *newPath = malloc(256);
    

    // given file to move reside in current dir 
    if(strchr(arg, '/') == 0)
    {

        getcwd(path, 255);
        strcat(path, "/");
        strcat(path, arg);
        //printf("%s\n", path);

    }
    else if(arg[0] == '.' )
    {
    	// relative path with .. given 
    	if(strlen(arg) >= 2 && arg[1] == '.')
    	{
    		getcwd(path, 255);

            char *lastSlash = 0;
            char *temp = strchr(path, '/');

            while(temp)
            {
                lastSlash = temp;
                temp = temp+1;
                temp = strchr(temp, '/');
            }

            // truncating path to prev directory
            if(lastSlash != 0 && lastSlash != path)
                *lastSlash = 0;

            strcat(path, arg + 2);
    	}
    	else
    	{
    		getcwd(path, 255);
    		strcat(path, arg + 1);
    	}
    }
    else
    	strcpy(path, arg);


    //printf("%c\n", dest[0]);
    if(dest[0] == '.')
    {
        // for .. dir
        if(strlen(dest) >= 2 && dest[1] == '.')
        {
            getcwd(newPath, 255);

            char *lastSlash = 0;
            char *temp = strchr(newPath, '/');

            while(temp)
            {
                lastSlash = temp;
                temp = temp+1;
                temp = strchr(temp, '/');
            }

            // truncating path to prev directory
            if(lastSlash != 0 && lastSlash != newPath)
                *lastSlash = 0;

            //strcat(newPath, dest + 2);
            
            strcat(newPath, "/");

            // only given ..
            if(strlen(dest) == 2)
            {
            	// relative path given 
            	if(arg[0] == '.')
            	{
            		strcat(newPath, getFileName(arg));

            	}
            	// absolute path given
            	else if(arg[0] == '/')
            	{
            		strcat(newPath, getFileName(arg));
            	}
            	// only file Name given
      			else
            	strcat(newPath, arg);
            }
            // ../ given in dest
            else if(strlen(dest) == 3 && dest[2] == '/')
            {
            	
            	// relative path given 
            	if(arg[0] == '.')
            	{
            		strcat(newPath, getFileName(arg));

            	}
            	// absolute path given
            	else if(arg[0] == '/')
            	{
            		strcat(newPath, getFileName(arg));
            	}
            	// only file Name given
      			else
            	strcat(newPath, arg);

            }
            else
            {

            	strcat(newPath, dest + 3);
            }

        }

        // for current dir
        else
        {
        	getcwd(newPath, 255);

        	strcat(newPath, "/");

            // only given .
            if(strlen(dest) == 1)
            {
            
            	printf("mv: '%s' and './%s' are the same file\n", arg, arg);
         		return;   	
            }
             else if(strlen(dest) == 2 && dest[1] == '/')
            {
            	
            	printf("mv: '%s' and '%s' are the same file\n", arg, arg);
         		return;

            }
            else if(isDir(newPath) == 1)
    		{
    			strcat(newPath, dest + 2);
    			strcat(newPath, "/");
    			strcat(newPath, getFileName(path));

    		}
            else
            {

            	strcat(newPath, dest + 2);
            }

        }
    	
    }
    // dest absolute path given
    else
    {
    	strcpy(newPath, dest);
    	
    	
    	if(isDir(newPath) == 1)
    	{
    		if(newPath[strlen(newPath) - 1] != '/')
    			strcat(newPath, "/");

    		strcat(newPath, getFileName(path));

    	}

    	
    	
    }

    
    if(rename(path, newPath) != 0)
        	printf("mymv: cannot move '%s' to '%s': %s\n", arg, dest, strerror(errno));

    free(newPath);

}



char* getFileName(char *filePath)
{
	char *temp2 = filePath;
	char *temp = temp2;

	
	while((temp2 = strchr(temp, '/')) != 0)
	{
	
		temp = temp2 + 1;
	}

	//printf("%s\n", temp);
	return temp;

}