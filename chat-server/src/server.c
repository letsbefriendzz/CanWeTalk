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
char* stripMessage(char* msg);

int main()
{
    /////////////////////////////////////////////////////////////////

    int                 server_sock, client_sock, client_len;
    struct sockaddr_in  client_addr, server_addr;
    pthread_t           threads[MAX_CLIENTS];

    #pragma region init server & masterList

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

        displayMasterList( &ml );
        printf("THREADS RUNNING:\t%d\n", ml.activeClients);

    } while( ml.activeClients > 0 );

    #pragma endregion

    cleanup(server_sock);

    return 0;
}

/*
NAME    : handleClient
DESC    :
    This function is executed by however many client threads are running.
    The function is responsible for accepting incoming messages from clients
    and then sending them out to all clients; including the client that sent
    the message.

    This function also uses the static volatile masterList that's declared
    at the top of this file. It uses this masterList to access all available
    node information. When the thread is finished executing, it removes the
    masterList client instance it is responsible for from the masterList.

    WHY SEND THE MESSAGE BACK THE SOURCE CLIENT?

    Great question! From a user's perspective, they may be confused if the
    message they send shows up in their chat window but is not received by
    other users. Sending the message back to the client and having the client
    only display messages received from the server eliminates ambiguity; you
    only see messages that are trasnferred over the network.
RTRN    : void*
PARM    : void*
    This function expects a listThreadParameters struct instance; this allows
    me to send multiple parameters, being the client socket integer needed to
    read and write with the socket, as well as the IP char* for message writing.
*/
void* handleClient(void* clientData)
{
    char buffer[PACKET_WIDTH];
    char message[BUFSIZ];
    //int client_sock = *((int*)clientSocket);
    listenThreadParameters ltp = *( (listenThreadParameters*)clientData );
    int clientIndex = ml.activeClients;
    
    ml.activeClients++;

    while( 1 )
    {
        // clear out and get the next command and process
        memset(buffer,0,PACKET_WIDTH);
        memset(message,0,BUFSIZ);
        
        // attempt to read from the socket
        int numBytesRead = read (ltp.client_sock, buffer, BUFSIZ);
        
        // format the message with 15 characters dedicated for the ltp.ip field
        // then poop the remaining buffer out after it.
        sprintf (message, "%-15s %s", ltp.ip, buffer);
        // replace the bars with spaces because standards
        replace(message, '|', ' ');

        // strip the message and check if the first word in it is >>bye<<
        // if it is, we terminate execution.
        char* stripped_message = stripMessage(buffer);
        if(strcmp(stripped_message, ">>bye<<") == 0)
        {
            free(stripped_message);
            break;
        }
        free(stripped_message);

        // Broadcast our message to all clients, if we have any bytes that we read
        if(numBytesRead > 0)
        {
            for(int i = 0; i < ml.activeClients; i++)
            {
                write(ml.clients[i].ip, message, strlen(message));
                // printf("writing to socket %d :\t%s\n\n", socket, message);
            }
        }
    }

    // when the loop terminates, remove the current instance from our masterList
    removeFromMasterList( &ml, clientIndex );
    // decrement the activeClients member of the masterList
    ml.activeClients--;
    // return 0; not that we're
    pthread_exit( (void *) (0) );
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