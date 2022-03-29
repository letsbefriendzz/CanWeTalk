#include "../inc/chat-server.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

/*
NAME    : removeFromMasterList
DESC    :
    The masterList struct contains an array of ten client instances.
    This function compresses the list nicely when a node is removed.
    This is copied from my hoochamacallit solution.
RTRN    : int
PARM    : voaltile masterList*, int
*/
int removeFromMasterList( volatile masterList* list, int index )
{
    // if we're removing the end of the list, just make the values invalid
    if ( index + 1 == list->activeClients )
    {
        list->clients[index].ip   = -1;
        list->clients[index].name = NULL;
    }
    else // otherwise, iterate over the list and compress it, overwriting our list->clients[index].
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
NAME    : initMasterList
DESC    :
    Sets all values of a masterList instance to empty values.
RTRN    : //
PARM    : volatile masterList8
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

/*
NAME    : displayMasterList
DESC    :
    Displays the contents of a volatile masterList pointer.
    Used for debugging.
RTRN    : //
PARM    : volatile masterList*
*/
void displayMasterList( volatile masterList* list )
{
    for(int i = 0; i < MAX_CLIENTS; i++)
    {
        printf("[%d] - IP:\t%d\n", i, list->clients[i].ip);
    }

    printf("==============================\n");
}

void countdown(int s)
{
    printf("COUNTDOWN FROM %d STARTING\n\n", s);
    for(int i = 0; i < s; i++)
    {
        printf("%d\n", (s - i) );
        sleep(1);
    }
}