#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void helloWorld()
{
    printf("Hello World!\n");
}

void logger(const char* user, const char* msg)
{
    printf("[%s] : %s\n", user, msg);
}

int occursIn(const char* str, char c)
{
    int count = 0;
    for(int i = 0; i < strlen(str); i++)
        if(str[i] == c) count++;
    return count;
}

// i love writing string manipulation functions that just work
// otherwise, C strings are insufferable to work with

int getIndexOf(const char* str, char c, int ins)
{
    if(occursIn(str, c) < ins)
        return -1;

    int count = 0;
    for(int i = 0; i < strlen(str); i++)
    {
        if(str[i] == c)
            count++;

        if(count == ins)
            return i;
    }

    return -2;
}

char* subString(char* str, int s, int e)
{
    if(( e-s ) < 1 || s < 0 || e > strlen(str) || strlen(str) < 1)
        return NULL;

    char* rtrn = malloc(sizeof(char) * ((e-s)) );
    for(int i = s+1; i < e; i++)
    {   
        rtrn[i-(s+1)] = str[i];
    }
    return rtrn;
}

void replace(char* str, char o, char r)
{
    int len = strlen(str);
    for(int i = 0; i < len; i++)
        if(str[i] == o) str[i] = r;
}