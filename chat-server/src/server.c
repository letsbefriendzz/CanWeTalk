#include "../../common/inc/common.h"
#include "../inc/chat-server.h"

#include <sys/socket.h>
#include <sys/types.h>

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <pthread.h>

#define NAME "SERVER"

volatile masterList ml;

void* handleClient( void* clientSocket );
void cleanup( int server_sock );
void displayMasterList();
int broadcastMessage(int socket, const char* msg);
char* stripMessage(char* msg);

int main()
{
    /////////////////////////////////////////////////////////////////

    int                 server_sock, client_sock, client_len;
    struct sockaddr_in  client_addr, server_addr;
    pthread_t           threads[MAX_CLIENTS];

    #pragma region init server

    initMasterList( &ml );
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

    #pragma endregion

    #pragma region main listening loop

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

        #pragma region populate listenThreadParameters instance
        
        char ipbuffer[32];
        inet_ntop(AF_INET, &client_addr.sin_addr, ipbuffer, sizeof(ipbuffer));
        listenThreadParameters ltp;
        strcpy(ltp.ip, ipbuffer);
        ltp.client_sock = client_sock;

        #pragma endregion

        if (pthread_create(  &(threads[(ml.activeClients-1)])  , NULL , handleClient, (void *)&ltp))
        {
            logger (NAME, "pthread_create() FAILED\n");
            fflush(stdout);
            return 5;
        }
        else
        {
            logger(NAME, "Created new thread");
            ml.clients[ml.activeClients].ip = client_sock;
        }

        displayMasterList();
        printf("THREADS RUNNING:\t%d\n", ml.activeClients);

    } while( ml.activeClients > 0 );

    #pragma endregion
    
    cleanup(server_sock);

    return 0;
}

void displayMasterList()
{
    for(int i = 0; i < MAX_CLIENTS; i++)
    {
        printf("[%d] - IP:\t%d\n", i, ml.clients[i].ip);
    }

    printf("==============================\n");
}

void* handleClient(void* clientData)
{
    char buffer[BUFSIZ];
    char message[BUFSIZ];
    //int client_sock = *((int*)clientSocket);
    listenThreadParameters ltp = *( (listenThreadParameters*)clientData );
    int threadIndex = ml.activeClients;
    
    ml.activeClients++;

    while( 1 )
    {
        // clear out and get the next command and process
        memset(buffer,0,BUFSIZ);
        memset(message,0,BUFSIZ);
        
        // attempt to read from the socket
        int numBytesRead = read (ltp.client_sock, buffer, BUFSIZ);
        
        sprintf (message, "%-15s %s", ltp.ip, buffer);
        replace(message, '|', ' ');

        char* stripped_message = stripMessage(buffer);
        if(strcmp(stripped_message, ">>bye<<") == 0) break;
        free(stripped_message);

        if(numBytesRead > 0)
        {
            for(int i = 0; i < ml.activeClients; i++)
                broadcastMessage(ml.clients[i].ip, message);
        }
    }
    removeFromMasterList( &ml, threadIndex );
    ml.activeClients--;
    pthread_exit( (void *) (0) );
}

int broadcastMessage(int socket, const char* msg)
{
    write(socket, msg, strlen(msg));
    printf("writing to socket %d :\t%s\n\n", socket, msg);
}

//free() THIS FUNCTION'S RETURN VALUE
char* stripMessage( char* msg)
{
    char* ss = subString( msg, getIndexOf(msg, '|', 1), getIndexOf(msg, '|', 2) );
    // LMAO THIS IS SO BoTCHED BUT IT WORKS
    replace(ss, ' ',  '\0');
    return ss;
}

void cleanup(int server_sock)
{
    close(server_sock);
}