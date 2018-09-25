#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<errno.h>
#include<string.h>
#include<dirent.h>
#include"dirHelper.c"


void catCommandHandler(char *arg);

int main( int argc,char **argv)
{
    int i;
    
    if(argc == 1)
    {
        printf("\nmycat: missing operand\n\n");
        exit(1);
    }
    else if(argc == 2)
    {
            //printf("Inside mycat : \n");
            char c;

            if((c=getchar())!=EOF)
            {
                putchar(c);
            
                while((c=getchar())!=EOF)
                    putchar(c);
            }
    }
    else if(argc == 3)
    {
        
            char **args;

            int tokens = split(argv[2], ' ', &args);

            if(args[0][0] == 0)
            {
                printf("\nmycat: missing operand\n");
                exit(1);
            }
            if(tokens == 1)
            {
                catCommandHandler(args[0]);
            }
            else
            {

                for(i = 0; i < tokens; i++)
                {

                    catCommandHandler(args[i]);
                }

            }
    }
    // This is used because our main function is using execvp() we are passing only one string with all arguments given to main function
    // So valid argc will be only one or two
    else
    {
        printf("something went wrong args are not 3 but %d\n", argc);
    }

    return 0;
}

/* This function will read files one character at time and print them */
void catCommandHandler(char *arg)
{
    int i,fd;
    char buf[2];

    fd = open(arg,O_RDONLY,0777);

    if(fd == -1)
    {
        printf("cat: %s: %s\n",arg, strerror(errno));
    }
    else
    {

        while((i=read(fd,buf,1))>0)
        {
            printf("%c",buf[0]);
        }
        printf("\n");
        close(fd);
    }
}
