#include <stdio.h>
#include <stdlib.h>
#include<dirent.h>      // directory structure
#include<errno.h>       // for knowing which error has occurred
#include<unistd.h>      // directory related functions
#include<string.h>      // string functions
#include <sys/stat.h>   // for file status
#include <sys/types.h>  // for user id type
#include <time.h>       // for file last modified time
#include <pwd.h>        // for user information
#include <grp.h>        // for group information
#include "dirHelper.c"	// contains filter sorting function for scandir


// Note: All colors are not used in this code
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define BOLDBLACK   "\033[1m\033[30m" 
#define BOLDRED     "\033[1m\033[31m" 
#define BOLDGREEN   "\033[1m\033[32m" 
#define BOLDYELLOW  "\033[1m\033[33m" 
#define BOLDBLUE    "\033[1m\033[34m" 
#define BOLDMAGENTA "\033[1m\033[35m"
#define BOLDCYAN    "\033[1m\033[36m"
#define BOLDWHITE   "\033[1m\033[37m"




char* getFilePermissions(char *file);
char* getFormattedTime(time_t t);
void ls_l(char *path, struct dirent **filesList, int nof);
void ls(char* path, struct dirent **filesList, int nof);
void lsWithArgument(char *argv, int flag);

/****************************************************************************
****************************  Main Function *********************************
*****************************************************************************/
int main(int argc,char **argv)
{
    char **args;
    int tokens = 0;

    if(argc == 2){
    tokens = split(argv[1], ' ',&args);
    
    }

    int noOfFiles;
    char * pathBuffer = malloc(512);
    getcwd(pathBuffer, 512);
    if( pathBuffer == NULL)
    {

        printf("Something went wrong with get_curret_dir_name()\n");

    }
    else
    {

        
        struct dirent **filesList;      // for storing file names
        int i;                          // loop variable
        noOfFiles = scandir(pathBuffer, &filesList, filter, mySorter);


        // if no arguments are given 
        if(tokens == 0)
            ls(pathBuffer, filesList, noOfFiles);

        // if only one one arguments are given
        else if(tokens == 1)
        {
	
		
        	// check for -l tag and show for current directory
            if(strcmp(args[0], "-l") == 0)
            {
                ls_l(pathBuffer, filesList, noOfFiles);
            }
            else
            {
                lsWithArgument(args[0], 0);

            }

        }
        // if two arguments are given 
        else if(tokens == 2)
        {

            if(strcmp(args[0], "-l")==0)
            {
     
                lsWithArgument(args[i], 1);
                	
            }
            else
            {
                for(i = 0; i < 2; i++)
                {
                    printf("%s:\n", args[i]);
                    lsWithArgument(args[i], 0);
                }
            }
        }
        // if multiple arguments are given
        else if( tokens > 2)
        {

            if(strcmp(args[0], "-l")==0)
            {
            
                for(i = 1; i < tokens; i++)
                {
                    printf("%s:\n", args[i]);
                	lsWithArgument(args[i], 1);
                }

            }
            else
            {
                for(i = 0; i < tokens; i++)
                {
                    printf("%s:\n", args[i]);
                	lsWithArgument(args[i], 0);
                }
                

            }

        }

        free(pathBuffer);

    }
}

/* This Function will  return formatted file permissions and the first index will denote whether given name is dir or file */
char* getFilePermissions(char *file)
{

    struct stat st;
    char *modeval = malloc(sizeof(char) * 11);
    if(stat(file, &st) == 0)
    {
        mode_t perm = st.st_mode;
        modeval[0] = (isDir(file)==1) ? 'd' : '-';
        modeval[1] = (perm & S_IRUSR) ? 'r' : '-';
        modeval[2] = (perm & S_IWUSR) ? 'w' : '-';
        modeval[3] = (perm & S_IXUSR) ? 'x' : '-';
        modeval[4] = (perm & S_IRGRP) ? 'r' : '-';
        modeval[5] = (perm & S_IWGRP) ? 'w' : '-';
        modeval[6] = (perm & S_IXGRP) ? 'x' : '-';
        modeval[7] = (perm & S_IROTH) ? 'r' : '-';
        modeval[8] = (perm & S_IWOTH) ? 'w' : '-';
        modeval[9] = (perm & S_IXOTH) ? 'x' : '-';
        modeval[10] = '\0';
        return modeval;
    }
    else
    {
        return strerror(errno);
    }
}


/* This function will return the formatted time as displayed in ls -l command */
char* getFormattedTime(time_t t)
{
    struct tm *lt;
    char * fTime = malloc(sizeof(char) * 80);

    lt = localtime(&t);

    strftime(fTime, 80, "%b %d %H:%M",lt);

    return fTime;

}

/* Function that prints files in ls -l format */
void ls_l(char* path, struct dirent **filesList, int nof)
{

	long totalBlocks = 0;	// Total in ls -l command represents no of 1024 Bytes blocks used by user files and folders in that directory
	//int totalFiles;
	//struct dirent **tempFilesList;

	//totalFiles = scandir(path, &tempFilesList, NULL, mySorter);

    int i;                         // loop variable
    struct stat fileStat;         // file stat structure


    struct group *grp;      // for group information of file
    struct passwd *pwd;     // for user owner of file
    char* permissions;   // permissions

    __off_t maxFileSize = 0;
    __nlink_t maxNLink = 0;


    // getting max_hard_links and max_file_size for dynamic format specifying

    for(i = 0; i < nof; i++)
    {

        if(stat(filesList[i]->d_name, &fileStat) == 0)
        {
            if(maxFileSize < fileStat.st_size)
                maxFileSize = fileStat.st_size;

            if(maxNLink < fileStat.st_nlink)
                maxNLink = fileStat.st_nlink;
        }
    }

    int f1 = 0;             // store padding for maxNLinks
    long t = (long)maxNLink;

    // getting no. of digits in maxNLinks
    while(t>0)
    {
        f1++;
        t /= 10;
    }
    


    int f2 = 0;         // store padding for file size
    t = (long)maxFileSize;

    // getting no. of digits in maxFileSize
    while(t>0)
    {
        f2++;
        t /= 10;
    }
    


    char *fileNameWithPath;
    fileNameWithPath = malloc(sizeof(char) * PATH_MAX);

    // calculating total blocks
    for(i = 0; i < nof; i++)
   {

            strcpy(fileNameWithPath, path);
            strcat(fileNameWithPath, "/");
            strcat(fileNameWithPath, filesList[i]->d_name);

            if(stat(fileNameWithPath, &fileStat) == 0)
            {

                totalBlocks += fileStat.st_blocks;

            }
            else
            {
                printf("File status obtaining failed...  %s %d \n", fileNameWithPath, errno);
            }

    }


    if(nof > 0)
    {
     	printf("Total %ld\n", totalBlocks/2);   
        for(i = 0; i < nof; i++)
        {



            strcpy(fileNameWithPath, path);
            strcat(fileNameWithPath, "/");
            strcat(fileNameWithPath, filesList[i]->d_name);

            if(stat(fileNameWithPath, &fileStat) == 0)
            {

                // printing permissions
                permissions = getFilePermissions(fileNameWithPath);  //getting permissions of file
                printf("%s",permissions);

                // printing number of hard links
                printf(" %*ld", f1, fileStat.st_nlink);

                //printing username
                pwd = getpwuid(fileStat.st_uid);       // getting user name and details
                printf(" %s", pwd->pw_name);

                //printing group name
                grp = getgrgid(fileStat.st_gid);    // getting group name and details
                printf(" %s", grp->gr_name);


                //printing file size
                printf(" %*ld", f2, fileStat.st_size);

                //printing modified time
                printf(" %s", getFormattedTime(fileStat.st_mtim.tv_sec));



            }
            else
            {
                printf("File status obtaining failed...  %s %d \n", fileNameWithPath, errno);
            }

            // printing file names changing directory color if it is dir
            if(permissions[0]=='d')
                printf(ANSI_COLOR_BLUE " %s\n" ANSI_COLOR_RESET, filesList[i]->d_name);
            else if(permissions[3] == 'x')
        		printf(ANSI_COLOR_GREEN " %s\n" ANSI_COLOR_RESET, filesList[i]->d_name);
            else
                printf(" %s\n", filesList[i]->d_name);


        }

        free(fileNameWithPath);
        //printf("\n");
    }

}

/* Function will print files as displayed with ls command */
void ls(char* path, struct dirent **filesList, int nof)
{

    int i = 0;
    char *fileNameWithPath;
    fileNameWithPath = malloc(sizeof(char) * PATH_MAX);

    
    for(i = 0; i < nof; i++)
    {



        strcpy(fileNameWithPath, path);
        strcat(fileNameWithPath, "/");
        strcat(fileNameWithPath, filesList[i]->d_name);

        char *permissions = getFilePermissions(fileNameWithPath);

        if(permissions[0] == 'd')
            printf( ANSI_COLOR_BLUE "%s\n" ANSI_COLOR_RESET, filesList[i]->d_name);
        else if(permissions[3] == 'x')
        	printf(ANSI_COLOR_GREEN "%s\n" ANSI_COLOR_RESET, filesList[i]->d_name);
        else
            printf("%s\n", filesList[i]->d_name);



    }
    free(fileNameWithPath);

    //printf("\n");


}


/* This function will will take arguments and call ls or ls-l functions to print according to flag value
	If flag = 0 then ls to be execute
	Else flag = 1 then ls -l to be execute 
*/
void lsWithArgument(char *argv, int flag)
{
    //flag define whether which flag is given ex. -l

    char* path = argv;
    DIR * dir = opendir(path);
    int nof;

    if(dir)
    {
        struct dirent **files;
        nof = scandir(argv, &files, filter, mySorter);

        switch(flag)
        {

        case 0:
            ls(path, files, nof);
            break;

        case 1:
            ls_l(path, files, nof);
            break;

        default:
            printf("Invalid flag given");
        }

        closedir(dir);
    }
    else if(errno == ENOENT)
    {
        printf("myls: no access \'%s\' : No Such File or Directory.\n", path);
    }
}


