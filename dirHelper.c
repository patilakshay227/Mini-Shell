#include <ctype.h>


/* This Function will filter out dir names starting with . and two default dir . and .. 
    Return 0 for such Directories
    Return 1 for others 
*/ 
int filter(const struct dirent *d)
{
    if ((strcmp(d->d_name, ".") == 0) ||(strcmp(d->d_name, "..") == 0))
        return 0;
    else if(d->d_name[0] == '.')
        return 0;
    else
        return 1;
}


/* This Function will alphabetically sort the files  
    Return 0 if two file names are equal
    Return <0 if first file name comes before second file name in alphabetic order
    Return >0 if first file name comes after second file name in alphabetic order
    
*/
int mySorter(const struct dirent **a, const struct dirent **b)
{


    int len1 = strlen(a[0]->d_name);
    int len2 = strlen(b[0]->d_name);
    int i;

    char *arr1 =  malloc(sizeof(char) * len1);
    char *arr2 =  malloc(sizeof(char) * len2);

    strcpy( arr1, a[0]->d_name);
    strcpy( arr2, b[0]->d_name);

    int limit = len1<len2?len1:len2;

    for(i = 0; i <  limit; i++)
    {
        // checking if capital letter turning it into small to sort alphabetically
        if('A' <= arr1[i] && arr1[i] <= 'Z')
            arr1[i] += 32;

        if('A' <= arr2[i] && arr2[i] <= 'Z')
            arr2[i] += 32;
    }

    return strcmp(arr1, arr2);
}

/* This Function will check for valid Directory for given path 
    Return  1 if path points to Directory
    Return  0 if it is file or invalid path 
*/
int isDir(const char* name)
{
    DIR* directory = opendir(name);

    if(directory != NULL)
    {
        closedir(directory);
        return 1;
    }

    if(errno == ENOTDIR)
    {
        return 0;
    }

    return -1;
}

/* This function will split given string by given deliminator 
   Return number of splitted words
   Also update the pointers to each word
*/
int split(char *str, char c, char ***arr)
{
    int count = 1;
    int token_len = 1;
    int i = 0;
    char *p;
    char *t;

    p = str;
    while (*p != '\0')
    {
        if (*p == c)
            count++;
        p++;
    }

    *arr = (char**) malloc(sizeof(char*) * count);
    if (*arr == NULL)
        exit(1);

    p = str;
    while (*p != '\0')
    {
        if (*p == c)
        {
            (*arr)[i] = (char*) malloc( sizeof(char) * token_len );
            if ((*arr)[i] == NULL)
                exit(1);

            token_len = 0;
            i++;
        }
        p++;
        token_len++;
    }
    (*arr)[i] = (char*) malloc( sizeof(char) * token_len );
    if ((*arr)[i] == NULL)
        exit(1);

    i = 0;
    p = str;
    t = ((*arr)[i]);
    while (*p != '\0')
    {
        if (*p != c && *p != '\0')
        {
            *t = *p;
            t++;
        }
        else
        {
            *t = '\0';
            i++;
            t = ((*arr)[i]);
        }
        p++;
    }

    return count;
}

/* This function will remove whitespaces before and after given string
    Return pointer to the start of trimmed string
    NULL if all are white spaces
*/
char *trim(char *string)
{
    char *start;
    int len = strlen(string);
    int i;

    // Find the first non whitespace char
    for (i = 0; i < len; i++) {
        if (string[i] != ' ') {
            break;
        }
    }

    if (i == len) {
        // string is all whitespace 
        return "\0";
    }

    start = &string[i];

    // Remove trailing white space 
    for (i = len - 1; i > 0; i--) {
        
        if (string[i] == ' ') {
            string[i] = '\0';
        } else {
            break;
        }
    }
    return start;
}