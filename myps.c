#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <ctype.h>
#include <pwd.h>
#include "dirHelper.c"

int getCommandName(char *arg, char **commandName);

// global variable
char *path;
int main(int argc, char *argv[])
{

    path = malloc(512);
	char *commandName = malloc(128);

	// getting current user id
	uid_t userId = getuid();

	// vairable to store the process stat
    struct stat pStat;

	DIR* dir  = opendir("/proc");
	struct dirent* temp;
	
	printf("\nUser\tPID\tProcess Name\n\n");

	while ((temp = readdir(dir)) != NULL)
	{
	    // ignoring . and ..
		if( strcmp(temp->d_name, ".") == 0 || strcmp(temp->d_name, "..") == 0)
		{
			continue;
		}
		// ignoring if not valid process id
        else if( atoi(temp->d_name) == 0)
        {
            continue;
        }

        //putting dir name in path
		sprintf(path, "/proc/%s", temp->d_name);

		if (isDir(path) == 1)
		{
			if(access(path, F_OK ) == 0)
			{
				stat(path, &pStat);

				if(userId == pStat.st_uid)
				{

                    if(getCommandName(temp->d_name, &commandName))
					printf("%s\t%d\t%s\n", (getpwuid(getuid()))->pw_name, atoi(temp->d_name), commandName);
				}
			}
			else
            {

                //printf("Error accessing path : %s\n", strerror(errno));
            }

		}
		else
        {
            //printf("Error No such directory %s\n", path);
        }
	}

    free(path);
	free(commandName);
	closedir(dir);

	return 0;
}

// This function will retrieve the command name form given process name store it in commandName variable
// and return the status whether retrieval succeed or not
int getCommandName(char *arg, char **commandName)
{
	int retValue = 1;

	sprintf(path, "/proc/%s/status",arg);

	FILE* fp = fopen(path,"r");

	if(fp)
	{
		size_t n;
		char temp[128];
		if(getline(commandName, &n, fp) != -1)
		{
            sscanf(*commandName, "Name:	%s", temp);
            strcpy(*commandName, temp);
		}
        fclose(fp);
	}
	else
    {
        retValue = 0;
    }

	return retValue;
}
