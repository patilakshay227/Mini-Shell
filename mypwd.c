#include <stdio.h>
#include <stdlib.h>
#include<dirent.h>	// dirent structure
#include<errno.h>	// to know which errors are raised
#include<unistd.h>	// dir functions

int main()
{
	// getting current dir
    char * pathBuffer = malloc(512);
    getcwd(pathBuffer, 512);
    if( pathBuffer == NULL)
    {

        printf("Something went wrong with get_curret_dir_name()\n");

    }
    else
    {
        printf("%s\n", pathBuffer);
        free(pathBuffer);

    }

    return 0;
}
