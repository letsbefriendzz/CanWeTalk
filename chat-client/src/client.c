#include "../inc/chat-client.h"
#include "../../common/inc/common.h"

#include <netinet/in.h>
#include <arpa/inet.h>

#include <netdb.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    int                 my_server_socket, len, done;
    struct sockaddr_in  server_addr;
    struct hostent*     host;

    char* userName;
    char* ip;

    if(argc != 3)
    {
        printf("bad args\n");
        return -1;
    }
    else
    {
        userName    = argv[1];
        ip          = argv[2];
    }

    if ((host = gethostbyname (ip)) == NULL) 
    {
        printf ("[CLIENT] : Host Info Search - FAILED\n");
        return 2;
    }

    memset (&server_addr, 0, sizeof (server_addr));
    server_addr.sin_family = AF_INET;
    memcpy (&server_addr.sin_addr, host->h_addr, host->h_length); // copy the host's internal IP addr into the server_addr struct
    server_addr.sin_port = htons (PORT);

    /*
    * get a socket for communications
    */
    printf ("[CLIENT] : Getting STREAM Socket to talk to SERVER\n");
    fflush(stdout);
    if ((my_server_socket = socket (AF_INET, SOCK_STREAM, 0)) < 0) 
    {
        printf ("[CLIENT] : Getting Client Socket - FAILED\n");
        return 3;
    }

    /*
    * attempt a connection to server
    */
    printf ("[CLIENT] : Connecting to SERVER\n");
    fflush(stdout);
    if (connect (my_server_socket, (struct sockaddr *)&server_addr,sizeof (server_addr)) < 0) 
    {
        printf ("[CLIENT] : Connect to Server - FAILED\n");
        close (my_server_socket);
        return 4;
    }
}