#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "../inc/chat-server.h"

/*
Removes and flattens the masterList given an index to delete.
*/
int removeFromMasterList( volatile masterList* list, int index )
{
    if ( index + 1 == list->activeClients )
    {
        list->clients[index].ip   = -1;
        list->clients[index].name = NULL;
    }
    else
    {
        for(int i = index; i < list->activeClients; i++)
        {
            list->clients[i].ip    = list->clients[i+1].ip;
            list->clients[i].name  = list->clients[i+1].name;
        }
    }
    return 0;
}

/*
Sets all values of the global masterList instance to empty values.
*/
void initMasterList( volatile masterList* list )
{
    list->activeClients = 0;
    for(int i = 0; i < MAX_CLIENTS; i++)
    {
        list->clients[i].ip   = -1;
        list->clients[i].name = NULL;
    }
}

void displayMasterList( volatile masterList* list )
{
    for(int i = 0; i < MAX_CLIENTS; i++)
    {
        printf("[%d] - IP:\t%d\n", i, list->clients[i].ip);
    }

    printf("==============================\n");
}