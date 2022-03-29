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

typedef struct listenThreadParameters
{
    char        ip[32];
    int         client_sock;
} listenThreadParameters;

#endif