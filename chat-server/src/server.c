#include "../../common/inc/common.h"
#include "../inc/chat-server.h"

#include <sys/socket.h>
#include <sys/types.h>

#include <netdb.h>
#include <netinet/in.h>

#include <stdio.h>

void logger(const char* l)
{
    printf("[SERVER] : %s\n", l);
}

int main()
{
    int                 server_sock, client_sock, client_len;
    struct sockaddr_in  client_addr, server_addr;

    if((server_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        logger("Failed to create socket");
    else
        logger("Created socket");

    return 0;
}