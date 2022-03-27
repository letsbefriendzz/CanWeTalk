#ifndef CHAT_SERVER_H
#define CHAT_SERVER_H

#define MAX_CLIENTS 10

typedef struct client
{
    int ip;
    const char* name;
} client;

typedef struct masterList
{
    client clients[MAX_CLIENTS];
} masterList;

void logger(const char*);

void* handleClient(void* clientSocket);

#endif