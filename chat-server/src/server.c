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

void logger(const char* l)
{
    printf("[SERVER] : %s\n", l);
}

int main()
{
    int                 server_sock, client_sock, client_len;
    struct sockaddr_in  client_addr, server_addr;
    masterList* ml = NULL;

    if((server_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        logger("Failed to create socket");
        return -1;
    }
    else
    {
        logger("Created socket");
    }

    // init server_addr to empty
    memset (&server_addr, 0, sizeof (server_addr));
    // set family, address, and port
    server_addr.sin_family      = AF_INET;
    server_addr.sin_addr.s_addr = htonl (INADDR_ANY);
    server_addr.sin_port        = htons (PORT);

    if (bind (server_sock, (struct sockaddr *)&server_addr, sizeof (server_addr)) < 0) 
    {
        logger ("bind() FAILED");
        close (server_sock);
        return 2;
    }
    logger ("bind() successful");

    if (listen (server_sock, 5) < 0) 
    {
        logger ("listen() - FAILED.\n");
        close (server_sock);
        return 3;
    }
    logger ("listen() successful\n");

    int i=0;
    while(i == 0)
    {
        logger("Ready to accept()");
        fflush(stdout);	
        client_len = sizeof (client_addr);
        if ((client_sock = accept (server_sock,(struct sockaddr *)&client_addr, &client_len)) < 0) 
        {
            logger("accept() FAILED");
            fflush(stdout);	
            return 4;
        }

        logger ("received a packet from CLIENT");
        fflush(stdout);	

        i++;
    }

    close(server_sock);

    return 0;
}