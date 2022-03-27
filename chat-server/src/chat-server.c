#include <stdio.h>
#include <string.h>
#include <unistd.h>

void* handleClient(void* clientSocket)
{
    char buffer[BUFSIZ];
    char message[BUFSIZ];
    int client_sock = *((int*)clientSocket);
    do
    {
        int numBytesRead = read (client_sock, buffer, BUFSIZ);
        /* we're actually not going to execute the command - but we could if we wanted */
        sprintf (message, "[SERVER (Thread-%02d)] : Received %d bytes - command - %s\n", 0, numBytesRead, buffer);
        write (client_sock, message, strlen(message)); 

        // clear out and get the next command and process
        memset(buffer,0,BUFSIZ);
    } while( strcmp(message, "<<bye>>") != 0 );

    close(client_sock);
}