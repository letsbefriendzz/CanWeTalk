#ifndef CHAT_CLIENT_H
#define CHAT_CLIENT_H

#include <netdb.h>

#define SERVER_PREFIX "-server"
#define USER_PREFIX "-user"

int window_loop(int socket, struct sockaddr_in server, struct hostent* host);

#endif