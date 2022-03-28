#ifndef COMMON_H
#define COMMON_H

#define PORT 5000

typedef struct message
{
    const char* ip;
    const char* username;
    const char* message;
} message;

void helloWorld();
void logger(const char* user, const char* msg);
const char* constructMessage(message* m);

#endif