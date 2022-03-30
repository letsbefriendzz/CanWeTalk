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

void* listen_thread(void* s);

int main(int argc, char* argv[])
{
    ///////////////////////////////////////////////////////////////////

    char buffer[40];
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

    #pragma region creating listener thread

    if (pthread_create(  &listener, NULL, listen_thread, (void *)&server_socket))
    {
        logger (NAME, "pthread_create() FAILED\n");
        fflush(stdout);
        return 5;
    }

    #pragma endregion

    #pragma region main msg writing loop

    done = 1;
    while(done == 1)
    {
        // reset buffer to nill
        memset(buffer,0,MAX_MSG);

        // flush the toilet
        fflush (stdout);

        // get input from the user
        fgets (buffer, sizeof (buffer), stdin);
        replace(buffer, '|', ';');

        if(strlen(buffer) < 40)
        {
            char message[BUFSIZ];

            // strip a newline from the input, if it is present
            if (buffer[strlen (buffer) - 1] == '\n')
                buffer[strlen (buffer) - 1] = '\0';

            // format the message -- ONLY the username, msg and time()
            sprintf(message, "[%-5s] >>|%-40s|(HH:MM:SS)", userName, buffer);

            // if the user inputs >>bye<<, we can set the done flag to 0
            if(strcmp(buffer,">>bye<<") == 0)
                done = 0;

            // done or not, we write to the server
            write (server_socket, message, strlen (message));
        }
        else
            printf("LONG STRING MEME\n\n");
    }

    #pragma endregion
}


// i become nothing more than a robot when i write code like this
// i never knew flow until i started programming
/*
NAME    : listen_thread
DESC    :
    The function called by our client thread; responsible for listening for
    incoming messages from the server and writing them to the screen.
RTRN    : void*
PARM    : void*
*/
void* listen_thread(void* s)
{
    int server_socket = *(int*)s;
    char b[BUFSIZ];
    while( 1 )
    {
        // our local buffer, just in case
        memset(b,0,BUFSIZ);
        int numBytesRead = read (server_socket, b, sizeof(b));

        if(strcmp(b, ">>bye<<") == 0) break;

        if(numBytesRead > 0)
        {
            printf("%s\n", b);
            fflush(stdout);
        }
    }
    pthread_exit((void*) 0);
}