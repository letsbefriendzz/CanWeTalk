#ifndef CHAT_CLIENT_H
#define CHAT_CLIENT_H

#include <ncurses.h>

#define SERVER_PREFIX "-server"
#define USER_PREFIX "-user"
#define MAX_MSG 40

typedef struct listenerParameters
{
    WINDOW* window;
} listenerParameters;

int window_loop(int, const char*);

#endif