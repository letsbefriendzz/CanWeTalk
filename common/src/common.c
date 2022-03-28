#include <stdio.h>
#include <string.h>

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
    int count;
    for(int i = 0; i < strlen(str); i++)
        if(str[i] == c) count++;
    return count;
}