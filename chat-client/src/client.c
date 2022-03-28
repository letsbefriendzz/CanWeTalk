#include "../inc/chat-client.h"
#include "../../common/inc/common.h"
#include <netinet/in.h>
#include <arpa/inet.h>

#include <netdb.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#define NAME "CLIENT"

char buffer[BUFSIZ];

void* listen_thread(void* s);

int main(int argc, char* argv[])
{
    int                 my_server_socket, len, done;
    struct sockaddr_in  server_addr;
    struct hostent*     host;
    pthread_t           listener;

    char* userName;
    char* ip;

    if(argc != 3)
    {
        logger(NAME, "bad args");
        return -1;
    }
    else
    {
        userName    = argv[1];
        ip          = argv[2];
    }

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
    if ((my_server_socket = socket (AF_INET, SOCK_STREAM, 0)) < 0) 
    {
        logger (NAME, "Getting Client Socket - FAILED");
        return 3;
    }

    /*
    * attempt a connection to server
    */
    logger (NAME, "Connecting to SERVER");
    fflush(stdout);
    if (connect (my_server_socket, (struct sockaddr *)&server_addr,sizeof (server_addr)) < 0) 
    {
        logger (NAME, "Connect to Server - FAILED");
        close (my_server_socket);
        return 4;
    }

/*
    window_loop(my_server_socket);
    return -1;
*/

    if (pthread_create(  &listener, NULL, listen_thread, (void *)&my_server_socket))
    {
        logger (NAME, "pthread_create() FAILED\n");
        fflush(stdout);
        return 5;
    }

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

        if (buffer[strlen (buffer) - 1] == '\n')
            buffer[strlen (buffer) - 1] = '\0';

        /* check if the user wants to quit */
        if(strcmp(buffer,">>bye<<") == 0)
        {
            // send the command to the SERVER
            write (my_server_socket, buffer, strlen (buffer));
            done = 0;
        }
        else
        {
            write (my_server_socket, buffer, strlen (buffer));
        }
    }

    close(my_server_socket);
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

    int lti = 0;
    while( 1 )
    {
        // clear out and get the next command and process
        memset(b,0,BUFSIZ);
        int numBytesRead = read (server_socket, b, sizeof(b));

        if(strcmp(b, ">>bye<<") == 0) break;

        if(numBytesRead > 0)
        {
            printf("\"%s\"\n", b);
            fflush(stdout);
        }

        lti++;
    }
    pthread_exit((void*) 0);
}