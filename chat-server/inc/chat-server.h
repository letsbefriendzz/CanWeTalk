#ifndef CHAT_SERVER_H
#define CHAT_SERVER_H

#define MAX_CLIENTS 10
#include <netdb.h>

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
    int                 client_sock;
    struct sockaddr_in  client_addr;
} listenThreadParameters;

#endif