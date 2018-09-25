#include<stdio.h>
#include<stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
//#include <getopt.h>
#include<errno.h>
#include<dirent.h>
#include<string.h>
#include "dirHelper.c"

#define BUFF_SIZE 4096

void printLines(FILE *file, off_t startline);
void getNLines(FILE *file, long nlines);

int main(int argc, char *argv[])
{
    FILE *file;
    long n;
    char **args;

    if(argc == 1)
    {
       printf("\nmytail: missing operand\n\n");
        exit(1);
    }
    else if(argc == 2)
    {
        int len = 10, i = 0;

        if(atol(argv[1]) != 0)
            len = -atol(argv[1]);


        file = fopen("tempBuff.txt", "w+");
        if(file == NULL)
        {
                printf("Error opening tempBuff file \n");
                return -1;
        }

        //printf("Inside mytail : \n");

        char buff[5001] = {0};
        char c;

        if(( c = getchar())!=EOF)
        {
                fputc(c,file);
                 while((c=getchar())!=EOF)
                    fputc(c, file);
        }


        getNLines(file, len);

        if(remove("tempBuff.txt") != 0)
        	perror("tempBuff.txt ");

    }
    else if(argc == 3)
    {
        int tokens = split(argv[2], ' ', &args);

        if(tokens == 1)
        {

            //printf("%s", argv[1]);
            if(atol(args[0]) == 0)
            {
                file = fopen(args[0],"r");  //default 10 lines are printed in tail command
                if(file == NULL)
                {
                    printf("Error opening file: %s\n",args[0]);
                    return -1;
                }
                getNLines(file, 10);
            }
            else
            {
                printf("Please pass the file as argument after -N\n");
            }
        }
        else if(tokens == 2)
        {
            int i;
            n = -atol(args[0]);
            if(n == 0)
            {
                for(i = 0; i < 2; i++)
                {
                    file = fopen(args[i], "r");
                    getNLines(file, 10);
                }
            }
            else
            {
                file = fopen(args[1], "r");
                getNLines(file, n);
            }
        }
        else
        {
            int  i;
            n = -atol(args[0]);
            if(n == 0)
            {
                n = 10;
                i = 0;
            }
            else
                i = 1;

            for(; i < tokens; i ++)
            {
                printf("==> %s <==\n", args[i]);
                file = fopen(args[i], "r");
                getNLines(file, n);
            }


        }
    }
    else
    {
         printf("something went wrong args are not 2 but %d\n", argc);
    }

    return 0;
}

/* This function will print lines from given offset to end of file*/
void printLines(FILE *file, off_t startline)
{
    char c;

    fseek(file, startline + 1, SEEK_SET);
    while( (c =fgetc(file)) != EOF)
    {
        printf("%c", c);
    }
}

/* This function will set offset before n lines from end and call printLines to print them */
void getNLines(FILE *file, long nlines)
{
    off_t fposition;
    fseek(file,0,SEEK_END);
    fposition= ftell(file);
    off_t index= fposition;
    off_t end= fposition;
    long countlines= 0;
    char cbyte;

    for(; index >= 0; index --)
    {
        cbyte= fgetc(file);
        if (cbyte == '\n' && (end - index) > 1)
        {
            countlines ++;
            if(countlines == nlines)
            {
                break;
            }
        }
        fposition--;
        fseek(file,fposition,SEEK_SET);
    }
    printLines(file, fposition);
    printf("\n");
    fclose(file);
}

