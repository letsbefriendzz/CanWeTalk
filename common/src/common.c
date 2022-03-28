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
    int count = 0;
    for(int i = 0; i < strlen(str); i++)
        if(str[i] == c) count++;
    return count;
}

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