#include <stdio.h>
#include <stdio_ext.h>
#include <stdlib.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <pwd.h>      // for pwd functions
#include "dirHelper.c"

//for coloring user name
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_RESET   "\x1b[0m"



void splitCommandAndArguments(char  **input, char **command, char **args);
void runCommand(char *command, char *args);
void callMYPWD(char * args);
void callMYCD(char *args);
void callMYLS(char *args);
void callMYMKDIR(char *args);
void callMYRM(char *args);
void callMYCAT(char *args);
void callMYTAIL(char *args);
void callMYMV(char *args);
void callMYPS();
char* displayName();
char* getFormattedPathName();
void mycd(char *arg);

/**************************************************************
*********************** Main Function *************************
***************************************************************/
int main()
{


    char *userName;     // name with machine
    char *path;         // display formatted path as in terminal
    char *input = malloc(256);    // assuming max 256 characters command with arguments
    char *args = 0;        // it will point from where arguments start
    char *command = 0;    // it will store the first word of input i.e. actual command
    int pd[2];          // store the pipe descriptors


    userName = displayName();   // will store the userName@HostName as displayed by terminal
    while(1)
    {

        path = getFormattedPathName(); // will store the path as displayed in terminal



        printf( ANSI_COLOR_GREEN "%s:" ANSI_COLOR_BLUE "%s"  ANSI_COLOR_RESET "$ ", userName,path);
        // scanning input until enter has been pressed
        scanf("%[^\n]s", input);

        // below code clears \n from buffer put it into char c
        char c;
        scanf("%c",&c);


        char* pipePosition = 0; // points to pipe character in string
        pipePosition = strchr(input, '|');  // searching for pipe

        // input contains pipe operation
        if(pipePosition)
        {
            int len = strlen(input);

            // This variable will store strings seperated by pipe |
            char **pipeArgs = (char **)malloc(sizeof(char*) * 10);

            int k = 0;
            int count = 1;

            // putting null character in place of | starting next string from next next character
            *pipePosition = 0;
            pipePosition += 1;
            pipeArgs[k] = (char *)malloc(pipePosition - input);

            // storing trimmed string removed white spaces before and after the string
            strcpy(pipeArgs[k++], trim(input));


            char *prev = pipePosition; // will store the previous pointer to pipe position

            while(pipePosition)
            {
                count++;
                pipePosition = strchr(pipePosition, '|');


                if(pipePosition == 0)   //no more pipe there
                {

                    pipeArgs[k] = malloc(input + len + 1 - prev);
                    strcpy(pipeArgs[k++], trim(prev));
                    break;
                }
                else
                {
                    *pipePosition = 0;
                    pipePosition += 1;
                }

            }

            if(count > 2)   // checking if only one pipe is there
            {
                printf("Only One Pipe is supported.\n");
                continue;
            }

            // separating commands and arguments
            splitCommandAndArguments(&pipeArgs[0], &command, &args);
            //printf("%s -- *%s*\n", command, args);

            // creating pipe
            pipe(pd);

            if(fork() == 0)            //first fork for first command in input
            {

                dup2(pd[1], STDOUT_FILENO);        //replacing stdout with pipe write
                close(pd[0]);       //closing pipe read
                close(pd[1]);       //closing pipe write
                runCommand(command, args);  // running command
                exit(1);
            }


             // separating commands and arguments
            splitCommandAndArguments(&pipeArgs[1], &command, &args);

            //printf("%s -- *%s*\n", command, args);

            if(strcmp(command, "mycat") == 0)
            {

                if(fork() == 0)
                {
                    close(STDIN_FILENO);        //closing stdin
                    dup2(pd[0], STDIN_FILENO);    //replacing stdin with pipe read
                    close(pd[1]);       //closing pipe write and read
                    close(pd[0]);

                    // If arguments are given with mycat then ignore the previous process output
                    if(args != NULL)
                    {
                        runCommand("mycat", args);
                    }
                    // passing 2 arguments to differentiate between mycat without pipe
                    else
                    {
                        execlp("/home/akshay/IIT KGP/Computing Lab 1/Assignment 7/mycat", "/home/akshay/IIT KGP/Computing Lab 1/Assignment 7/mycat", "1", NULL);
                    }
                    exit(1);
                }
                else
                {
                    wait(0);
                }

                close(pd[0]);
                close(pd[1]);
            }
            else if(strcmp(command, "mytail") == 0)
            {

                if(fork() == 0)
                {
                    close(STDIN_FILENO);   //closing stdin
                    dup2(pd[0], STDIN_FILENO);         //replacing stdin with pipe read
                    close(pd[1]);       //closing pipe write and read
                    close(pd[0]);
                    if(args != NULL)
                    {
                        char **t;
                        int tokens = split(args, ' ', &t);

                        // checking if arguments are given with mytail -n if given then ignore pipe print file
                        if(tokens >= 2)
                            runCommand("mytail", args);
                        else
                            execlp("/home/akshay/IIT KGP/Computing Lab 1/Assignment 7/mytail", "/home/akshay/IIT KGP/Computing Lab 1/Assignment 7/mytail", args, NULL);
                    }
                    // default printing last 10 lines
                    else
                        execlp("/home/akshay/IIT KGP/Computing Lab 1/Assignment 7/mytail", "/home/akshay/IIT KGP/Computing Lab 1/Assignment 7/mytail", "-10", NULL);

                    exit(1);
                }
                else
                {
                    wait(0);
                }

                close(pd[0]);
                close(pd[1]);

            }
            else
                printf("Invalid Pipe command : At last position only mycat and mytail are supported.\n");

            wait(0);

        }

        else
        {

            splitCommandAndArguments(&input, &command, &args);
           // printf("%s , %s  --  %s\n",input, command, args);
            runCommand(command, args);

        }

    }

    // deallocating memory created by malloc
    free(userName);
    free(path);
    free(input);
    return 0;
}

// This function will split input into command and arguments
void splitCommandAndArguments(char  **input, char **command, char **args)
{
    // splitting the input with first space
    *args = strchr(*input, ' ');
    //printf("%s, %u %u\n", *input, *input, *args);
    // storing actual command first word of input
    *command = *input;

    if(*args != NULL)
    {
        // putting null character at place of first space
        **args = 0;
        // args will point to arguments after command
        *(args) = *(args) + 1;
    }

}

// This function will call for respective functions according to command
void runCommand(char *command, char *args)
{
    if(command == NULL)
    {
        return;
    }
    else if(strcmp(command, "myexit")==0)
    {
        exit(EXIT_SUCCESS);
    }
    else if(strcmp(command, "mypwd")==0)
    {
        callMYPWD(args);
    }
    else if(strcmp(command, "myls") == 0)
    {
        callMYLS(args);
    }
    else if(strcmp(command, "mymkdir") == 0)
    {
        callMYMKDIR(args);
    }
    else if(strcmp(command, "mycd") == 0)
    {
        callMYCD(args);
    }
    else if(strcmp(command, "myrm") == 0)
    {
        callMYRM(args);
    }
    else if(strcmp(command, "mycat") == 0)
    {
        callMYCAT(args);
    }
    else if(strcmp(command, "mytail") == 0)
    {
        callMYTAIL(args);
    }
    else if(strcmp(command, "mymv") == 0)
    {
        callMYMV(args);
    }
    else if(strcmp(command, "myps") == 0)
    {
        callMYPS();
    }
    else
    {
        printf("Command Not Found : %s\n", command);
    }
}

// This function will call mypwd executable
void callMYPWD(char *args)
{
    //checking if parameters are given with mypwd then error
    if(args != NULL)
    {
        printf("mypwd %s : Invalid option\n", args);
        return;
    }

    int pid = fork();

    if(pid == 0)
    {
        char *const argv[] = {"/home/akshay/IIT KGP/Computing Lab 1/Assignment 7/mypwd",NULL};
        execvp("/home/akshay/IIT KGP/Computing Lab 1/Assignment 7/mypwd", argv);
    }
    else if(pid > 0)
    {
        wait(NULL);
    }
    else
    {
        printf("Fork not working in mypwd.");
        exit(EXIT_FAILURE);
    }
}


// This function will call mycd function
void callMYCD(char *args)
{
    mycd(args);
}

// This function will call myls executable
void callMYLS(char *args)
{
    int pid = fork();

    if(pid == 0)
    {

        char *const argv[] = {"/home/akshay/IIT KGP/Computing Lab 1/Assignment 7/myls",args,NULL};

        execvp("/home/akshay/IIT KGP/Computing Lab 1/Assignment 7/myls", argv);
    }
    else if(pid > 0)
    {
        wait(NULL);
    }
    else
    {
        printf("Fork not working in myls.");
        exit(EXIT_FAILURE);
    }
}

// This function will call mymkdir executable
void callMYMKDIR(char *args)
{
    int pid = fork();
    if(pid == 0)
    {

        char *const argv[] = {"/home/akshay/IIT KGP/Computing Lab 1/Assignment 7/mymkdir",args,NULL};

        execvp("/home/akshay/IIT KGP/Computing Lab 1/Assignment 7/mymkdir", argv);
    }
    else if(pid > 0)
    {
        wait(NULL);
    }
    else
    {
        printf("Fork not working in mymkdir.");
        exit(EXIT_FAILURE);
    }


}


// This function will call myrm executable
void callMYRM(char *args)
{
    int pid = fork();
    if(pid == 0)
    {

        char *const argv[] = {"/home/akshay/IIT KGP/Computing Lab 1/Assignment 7/myrm",args,NULL};
        execvp("/home/akshay/IIT KGP/Computing Lab 1/Assignment 7/myrm", argv);
    }
    else if(pid > 0)
    {
        wait(NULL);
    }
    else
    {
        printf("Fork not working in myrm.");
        exit(EXIT_FAILURE);
    }


}

// This function will call mycat executable
void callMYCAT(char *args)
{
    int pid = fork();
    if(pid == 0)
    {
        // Passing 0 as extra argument for differentiating via no. of arguments in mycat program  
        char *const argv[] = {"/home/akshay/IIT KGP/Computing Lab 1/Assignment 7/mycat", "0", args,NULL};
        execvp("/home/akshay/IIT KGP/Computing Lab 1/Assignment 7/mycat", argv);
    }
    else if(pid > 0)
    {
        wait(NULL);
    }
    else
    {
        printf("Fork not working in mycat.");
        exit(EXIT_FAILURE);
    }
}

// This function will call mytail executable
void callMYTAIL(char *args)
{
    int pid = fork();
    if(pid == 0)
    {

        char *const argv[] = {"/home/akshay/IIT KGP/Computing Lab 1/Assignment 7/mytail", "0", args,NULL};

        execvp("/home/akshay/IIT KGP/Computing Lab 1/Assignment 7/mytail", argv);
    }
    else if(pid > 0)
    {
        wait(NULL);
    }
    else
    {
        printf("Fork not working in mytail.");
        exit(EXIT_FAILURE);
    }


}

// This function will call mymv executable
void callMYMV(char *args)
{
    int pid = fork();

    if(pid == 0)
    {

        char *const argv[] = {"/home/akshay/IIT KGP/Computing Lab 1/Assignment 7/mymv",args,NULL};

        execvp("/home/akshay/IIT KGP/Computing Lab 1/Assignment 7/mymv", argv);
    }
    else if(pid > 0)
    {
        wait(NULL);
    }
    else
    {
        printf("Fork not working in mymv.");
        exit(EXIT_FAILURE);
    }
}

// This function will call myps executable
void callMYPS()
{
    int pid = fork();

    if(pid == 0)
    {
        char *const argv[] = {"/home/akshay/IIT KGP/Computing Lab 1/Assignment 7/myps",NULL};
        execvp("/home/akshay/IIT KGP/Computing Lab 1/Assignment 7/myps", argv);
    }
    else if(pid > 0)
    {
        wait(NULL);
    }
    else
    {
        printf("Fork not working in myps.");
        exit(EXIT_FAILURE);
    }
}

// This function will return string in user@machine format
char* displayName()
{

    char* dispName = malloc(sizeof(char)*80);       // store the current name with machine to display
    struct passwd *pwd;         // for getting current user logged in
    char machineName[30];       // assuming max 20 characters name

    //getting current logged in user name
    pwd = getpwuid(getuid());

    strcpy(dispName,pwd->pw_name);


    if(gethostname(machineName, 29)!=0)
        printf("Machine name retrieving failed.\n");
    else
    {
        strcat(dispName,"@");
        strcat(dispName, machineName);
    }


    return dispName;
}


// This function will return pathName which replace the home dir with ~
char* getFormattedPathName()
{
    char *dispPath = malloc(sizeof(char) * 256);
    getcwd(dispPath,256);

    const char * t = strstr(dispPath, "/home");
    if(t != NULL)
    {
    	
        int i = 0;
        int slashCount = 0;
        while(t[i] != 0 && slashCount != 3)
        {
           
            if(t[i] == '/')
                slashCount++;

            i++;
        }
    	
        if(slashCount == 3)
        {
            
            sprintf(dispPath,"~%s", t+i-1);
            
        }
        else if(slashCount == 2)
        {
            strcpy(dispPath,"~");
            strcat(dispPath, t+i);
        }

    }

    return dispPath;
}

// This function will change dir to given path
void mycd(char *arg)
{

    char * pathBuffer = malloc(512);
    getcwd(pathBuffer,512);

   // printf("inside my cd : %s\n", arg);

    if( pathBuffer == NULL)
    {
        printf("Something went wrong with getcwd()\n");
        free(pathBuffer);
        exit(EXIT_FAILURE);

    }

    // if  no path given then move to home directory
    if(arg == 0)
    {

        struct passwd *p;

        //getting current logged in user name
        p = getpwuid(getuid());
        

        strcpy(pathBuffer, "/home/");
        strcat(pathBuffer, p->pw_name);
       
        if(chdir(pathBuffer) != 0)
            printf("error occurred during chdir() error : %s", strerror(errno));

    }
    else
    {

        // if .. given then change to one dir backward
        if(strcmp(arg, "..") == 0)
        {
            //printf("%s\n", pathBuffer);
            char *lastSlash;
            char *temp = strchr(pathBuffer, '/');

            while(temp)
            {
                lastSlash = temp;
                temp = temp+1;
                temp = strchr(temp, '/');
            }


            if(lastSlash != NULL && lastSlash != pathBuffer)
                *lastSlash = 0;

            else if(lastSlash == pathBuffer)
                strcpy(pathBuffer, "/");

            if(chdir(pathBuffer) != 0)
                printf("mycd: %s: %s\n",arg, strerror(errno));


        }
        // checking if all spaces are given then also go to home directory
        else if(strcmp(trim(arg), "") == 0)
        {
            struct passwd *p;

            //getting current logged in user name
            p = getpwuid(getuid());

            strcpy(pathBuffer, "/home/");
            strcat(pathBuffer, p->pw_name);

            if(chdir(pathBuffer) != 0)
                printf("mycd: %s: %s\n",arg, strerror(errno));

        }
        else
        {
            if(chdir(arg) != 0)
                printf("mycd: %s: %s\n",arg, strerror(errno));

        }

    }

    free(pathBuffer);

}
