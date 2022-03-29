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

char buffer[BUFSIZ];

void* listen_thread(void* s);

int main(int argc, char* argv[])
{
    ///////////////////////////////////////////////////////////////////

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

    /*
    * get a socket for communications
    */
    logger (NAME, "Getting STREAM Socket to talk to SERVER");
    fflush(stdout);
    if ((server_socket = socket (AF_INET, SOCK_STREAM, 0)) < 0) 
    {
        logger (NAME, "Getting Client Socket - FAILED");
        return 3;
    }

    /*
    * attempt a connection to server
    */
    logger (NAME, "Connecting to SERVER");
    fflush(stdout);
    if (connect (server_socket, (struct sockaddr *)&server_addr,sizeof (server_addr)) < 0) 
    {
        logger (NAME, "Connect to Server - FAILED");
        close (server_socket);
        return 4;
    }

    #pragma endregion

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
        /* clear out the contents of buffer (if any) */
        memset(buffer,0,BUFSIZ);

        /*
        * now that we have a connection, get a commandline from
        * the user, and fire it off to the server */
        //printf ("[%s] >>> ", userName);
        fflush (stdout);
        fgets (buffer, sizeof (buffer), stdin);
        char message[BUFSIZ];

        if (buffer[strlen (buffer) - 1] == '\n')
            buffer[strlen (buffer) - 1] = '\0';

        sprintf(message, "xxx.xxx.xxx.xxx|[%5s]|>>|%s|(HH:MM:SS)", userName, buffer);

        /* check if the user wants to quit */
        if(strcmp(buffer,">>bye<<") == 0)
        {
            // send the command to the SERVER
            write (server_socket, message, strlen (message));
            done = 0;
        }
        else
        {
            write (server_socket, message, strlen (message));
        }
    }

    #pragma endregion

    close(server_socket);
    logger(NAME, "QUITTING...");
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
        // clear out and get the next command and process
        memset(b,0,BUFSIZ);
        int numBytesRead = read (server_socket, b, sizeof(b));

        if(strcmp(b, ">>bye<<") == 0) break;

        if(numBytesRead > 0)
        {
            replace(b, '|', ' ');
            printf("%s\n", b);
            fflush(stdout);
        }
    }
    pthread_exit((void*) 0);
}