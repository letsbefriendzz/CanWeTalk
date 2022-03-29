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
    int activeClients;
    client clients[MAX_CLIENTS];
} masterList;

typedef struct listenThreadParameters
{
    char        ip[32];
    int         client_sock;
} listenThreadParameters;

int removeFromMasterList( volatile masterList* list, int index );
void initMasterList( volatile masterList* list );

#endif