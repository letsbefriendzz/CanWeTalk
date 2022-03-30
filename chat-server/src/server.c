#define __REENTRANT
#define NAME "SERVER"

#include "../../common/inc/common.h"
#include "../inc/chat-server.h"

#include <sys/socket.h>
#include <fcntl.h>
#include <sys/types.h>

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <pthread.h>

volatile masterList ml;

void* handleClient( void* clientSocket );
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
        close(server_sock);
        return -2;
    } // log a success
    logger (NAME, "bind() call in server succesful");

    // attempt to init listener
    if (listen (server_sock, 5) < 0) 
    {
        // log a fail
        logger (NAME, "listen() call in server failed");
        close(server_sock);
        return -3;
    }// log a success
    logger (NAME, "listen() call in server successful");

    // magic that results in our socket being nonblocking
    if( fcntl(server_sock, F_SETFL, fcntl(server_sock, F_GETFL, 0) | O_NONBLOCK) < 0 )
    {
        logger (NAME, "attempt to make socket nonblocking failed");
        close(server_sock);
        return -4;
    }

    #pragma endregion

    #pragma region main listening loop

    countdown(15);
    do
    {
        // flush the toilet
        fflush(stdout);
        client_len = sizeof (client_addr);
        // if accept() returns < 0, an error has occured
        if ( ( client_sock = accept (server_sock,(struct sockaddr *)&client_addr, &client_len ) ) < 0)
        {
            //logger(NAME, "accept() call in server failed");
            // fflush(stdout);	
            //return -4;
        }
        else
        {
            // flush stdout
            fflush(stdout);
            // log that we've received a new client packet
            logger (NAME, "received a packet from NEW CLIENT");

            #pragma region populate listenThreadParameters instance
            
            // create a buffer to store the ip address of this client
            char ipbuffer[32];
            // call inet_ntop to get ip
            inet_ntop(AF_INET, &client_addr.sin_addr, ipbuffer, sizeof(ipbuffer));
            // create ltp, populate the ip and client socket members

            listenThreadParameters ltp;
            strcpy(ltp.ip, ipbuffer);
            ltp.client_sock = client_sock;

            #pragma endregion

            // if we have space for a new thread
            if(ml.activeClients <= 10)
            {
                // try and open one
                if (pthread_create(  &(threads[(ml.activeClients)])  , NULL , handleClient, (void *)&ltp))
                {
                    // log a failure and flush stdout, should it fail
                    logger (NAME, "pthread_create() FAILED\n");
                    fflush(stdout);
                    return 5;
                }
                else
                {
                    // otherwise, log that a new thread has been created
                    logger(NAME, "Created new thread");
                    // inform the operating sys that we don't care about the return value of the thread
                    pthread_detach( threads[ml.activeClients] );
                    // copy the ip field to the master list, iterate activeClients counter
                    ml.clients[ml.activeClients].ip = client_sock;
                    ml.activeClients++;
                }
            }

            // dump the ml contents for debugging
            displayMasterList( &ml );
            printf("THREADS RUNNING:\t%d\n", ml.activeClients);
            // LEAVE THE SLEEP CALL
            sleep(1);
        }
    } while( ml.activeClients > 0 );

    #pragma endregion

    printf("Waiting for clients to close...");
    sleep(10);

    if(close(server_sock) < 0)
        printf("FAILED TO EXIT\n");
    else
        printf("EXITED SUCCESSFULLY\n");
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
    int clientIndex = ml.activeClients - 1;

    int run = 0;
    while( run == 0 )
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
            printf("DETECTED >>bye<<\n");
            run = 1;
        }
        free(stripped_message);

        // Broadcast our message to all clients, if we have any bytes that we read
        if(numBytesRead > 0)
        {
            for(int i = 0; i < ml.activeClients; i++)
            {
                write(ml.clients[i].ip, message, strlen(message));
                printf("writing to socket %d :\t%s\n", ml.clients[i].ip, message);
            }
        }
    }

    // when the loop terminates, remove the current instance from our masterList
    removeFromMasterList( &ml, clientIndex );
    // decrement the activeClients member of the masterList
    ml.activeClients--;
    // return 0; not that we're
    printf("Thread responsible for ML %d DONE\n", clientIndex);
    pthread_exit( (void *) (0) );
}

//free() THIS FUNCTION'S RETURN VALUE
char* stripMessage( char* msg)
{
    char* ss = NULL;
    ss = subString( msg, getIndexOf(msg, '|', 1), getIndexOf(msg, '|', 2) );
    // LMAO THIS IS SO BoTCHED BUT IT WORKS
    replace(ss, ' ',  '\0');
    return ss;
}