#include "../../common/inc/common.h"
#include "../inc/chat-server.h"

#include <sys/socket.h>
#include <sys/types.h>

#include <netdb.h>
#include <netinet/in.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <pthread.h>

#define NAME "SERVER"

volatile masterList ml;
static volatile int activeThreads = 0;

void* handleClient( void* clientSocket );
void cleanup( int server_sock );
void displayMasterList();
int removeFromMasterList( int index );
void initMasterList();

int main()
{
    int                 server_sock, client_sock, client_len;
    struct sockaddr_in  client_addr, server_addr;
    pthread_t           threads[MAX_CLIENTS];

    initMasterList();

    if((server_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        logger(NAME, "Failed to create socket");
        return -1;
    }
    else
        logger(NAME, "Created socket");

    // init server_addr to empty
    memset (&server_addr, 0, sizeof (server_addr));
    // set family, address, and port
    server_addr.sin_family      = AF_INET;
    server_addr.sin_addr.s_addr = htonl (INADDR_ANY);
    server_addr.sin_port        = htons (PORT);

    // attempt to bind
    if (bind (server_sock, (struct sockaddr *)&server_addr, sizeof (server_addr)) < 0) 
    {
        // log a fail
        logger (NAME, "bind() call in server failed");
        cleanup(server_sock);
        return -2;
    } // log a success
    logger (NAME, "bind() call in server succesful");

    // attempt to init listener
    if (listen (server_sock, 5) < 0) 
    {
        // log a fail
        logger (NAME, "listen() call in server failed");
        cleanup(server_sock);
        return -3;
    }// log a success
    logger (NAME, "listen() call in server successful");

    int i = 0;
    do
    {
        // flush the toilet
        fflush(stdout);
        client_len = sizeof (client_addr);
        // if accept() returns < 0, an error has occured
        if ( ( client_sock = accept (server_sock,(struct sockaddr *)&client_addr, &client_len ) ) < 0)
        {
            logger(NAME, "accept() call in server failed");
            fflush(stdout);	
            return -4;
        }

        fflush(stdout);
        logger (NAME, "received a packet from NEW CLIENT");
        if (pthread_create(  &(threads[(activeThreads-1)])  , NULL , handleClient, (void *)&client_sock))
        {
            logger (NAME, "pthread_create() FAILED\n");
            fflush(stdout);
            return 5;
        }
        else
        {
            logger(NAME, "Created new thread");
            ml.clients[activeThreads].ip = client_sock;
            // activeThreads is iterated within the threads themselves.
        }

        displayMasterList();

    } while( activeThreads > 0 );
    printf("%d\n", activeThreads);

    cleanup(server_sock);

    return 0;
}

void displayMasterList()
{
    for(int i = 0; i < activeThreads; i++)
        printf("[%d] - IP:\t%d\n", i, ml.clients[i].ip);
}

int removeFromMasterList( int index )
{
    // if i + 1 == the number of DCs connected, we're just removing the last one
    // ez pz
    if ( index + 1 == activeThreads )
    {
        ml.clients[index].ip   = 0;
        ml.clients[index].name = NULL;
    }
    else
    {
        for(int i = index; i < activeThreads; i++)
        {
            ml.clients[i].ip = ml.clients[i+1].ip;
            ml.clients[i].name = ml.clients[i+1].name;
        }
    }
    return 0;
}

void initMasterList()
{
    for(int i = 0; i < MAX_CLIENTS; i++)
    {
        ml.clients[i].ip   = 0;
        ml.clients[i].name = NULL;
    }
}

void* handleClient(void* clientSocket)
{
    char buffer[BUFSIZ];
    char message[BUFSIZ];
    int client_sock = *((int*)clientSocket);
    int threadIndex = activeThreads;
    activeThreads++;

    while( strcmp(buffer, "asdf") != 0 )
    {
        // clear out and get the next command and process
        memset(buffer,0,BUFSIZ);
        int numBytesRead = read (client_sock, buffer, BUFSIZ);

        sprintf (message, "COMMAND - %s\n", buffer);
        write (client_sock, message, strlen(message)); 
    }

    close(client_sock);
    pthread_exit( (void *) (0) );
    
    activeThreads--;
    return 0;
}

void cleanup(int server_sock)
{
    close(server_sock);
}