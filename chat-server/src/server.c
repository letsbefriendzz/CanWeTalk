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
volatile int activeThreads = 0;
void* handleClient(void* clientSocket);
void cleanup(int server_sock);

int main()
{
    int                 server_sock, client_sock, client_len;
    struct sockaddr_in  client_addr, server_addr;
    pthread_t           threads[MAX_CLIENTS];

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
        activeThreads++;
        if (pthread_create(  &(threads[(activeThreads-1)])  , NULL , handleClient, (void *)&client_sock))
        {
            logger (NAME, "pthread_create() FAILED\n");
            fflush(stdout);
            return 5;
        }
        else
            logger(NAME, "Created new thread");
        fflush(stdout);	
        printf("%d\n", activeThreads);
    } while( activeThreads > 0 );
    printf("%d\n", activeThreads);

    cleanup(server_sock);

    return 0;
}

void* handleClient(void* clientSocket)
{
    char buffer[BUFSIZ];
    char message[BUFSIZ];
    int client_sock = *((int*)clientSocket);
    do
    {
        int numBytesRead = read (client_sock, buffer, BUFSIZ);

        if(strcmp(buffer,"asdf") == 0)
            break;
        /* we're actually not going to execute the command - but we could if we wanted */
        sprintf (message, "COMMAND - %s\n", buffer);
        write (client_sock, message, strlen(message)); 

        // clear out and get the next command and process
        memset(buffer,0,BUFSIZ);
    } while( strcmp(buffer, "asdf") != 0 );
    
    write(client_sock, "CLOSING SOCK", strlen("CLOSING SOCK"));
    close(client_sock);
    pthread_exit( (void *) (0) );
    //activeThreads--;
    return 0;
}

void cleanup(int server_sock)
{
    close(server_sock);
}