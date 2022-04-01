/*
NAME        : client.c
PROJECT     : Can We Talk
AUTHOR      : Ryan Enns
DESC        :
    client.c defines the main function for the chat client program. It is primarily responsible
    for establishing a connection to the server, where it then passes control to widow_loop().
*/

#define __REENTRANT
#include "../inc/chat-client.h"
#include "../../common/inc/common.h"

#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#define NAME "CLIENT"

int main(int argc, char* argv[])
{
    ///////////////////////////////////////////////////////////////////

    char buffer[80];
    int                 server_socket, len, done;
    struct sockaddr_in  server_addr;
    struct hostent*     host;
    pthread_t           listener;

    #pragma region parsing cmd arguments

    char* userName;
    char* ip;

    if(argc != 3)
    {
        logger(NAME, "Expects THREE ARGS");
        return -1;
    }
    else
    {
        userName    = argv[1];
        ip          = argv[2];

        if( strlen(userName) > 5 )
        {
            logger (NAME, "Username provided is longer than max character length (5)");
            return -2;
        }

        // purify username
        replace(userName, '|', ';');
    }

    #pragma endregion

    #pragma region establishing server connection

    if ((host = gethostbyname (ip)) == NULL) 
    {
        logger (NAME, "[CLIENT] : Host Info Search - FAILED");
        return 2;
    }

    memset (&server_addr, 0, sizeof (server_addr));
    server_addr.sin_family = AF_INET;
    memcpy (&server_addr.sin_addr, host->h_addr, host->h_length); // copy the host's internal IP addr into the server_addr struct
    server_addr.sin_port = htons (PORT);

    // get a stream socket for server communication
    fflush(stdout);
    if ((server_socket = socket (AF_INET, SOCK_STREAM, 0)) < 0) 
    {
        logger (NAME, "Getting Client Socket - FAILED");
        return 3;
    }

    // attempt a connection
    fflush(stdout);
    if (connect (server_socket, (struct sockaddr *)&server_addr,sizeof (server_addr)) < 0) 
    {
        logger (NAME, "Connect to Server - FAILED");
        close (server_socket);
        return 4;
    }

    #pragma endregion

    window_loop(server_socket, userName);
    if( close(server_socket) < 0)
        logger(NAME, "SOCKET CLOSE FAIL");
    else
        logger(NAME, "QUITTING...");
    return -1;

    ///////////////////////////////////////////////////////////////////
}