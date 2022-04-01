/*
NAME        : chat-server.h
PROJECT     : Can We Talk
AUTHOR      : Ryan Enns
DESC        :
    chat-server.h defines macros, objects, and function prototypes
    relevant to chat-server.c and server.c's execution.
*/

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
    volatile int activeClients;
    client clients[MAX_CLIENTS];
} masterList;

typedef struct listenThreadParameters
{
    char        ip[32];
    int         client_sock;
    int         index;
} listenThreadParameters;

int removeFromMasterList( volatile masterList* list, int index );
void initMasterList( volatile masterList* list );
void displayMasterList( volatile masterList* list );

void countdown(int s);

#endif