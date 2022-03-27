#include <stdio.h>

void helloWorld()
{
    printf("Hello World!\n");
}

void logger(const char* user, const char* msg)
{
    printf("[%s] : %s\n", user, msg);
}