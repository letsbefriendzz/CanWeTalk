/*
NAME        : chat-client.h
PROJECT     : Can We Talk
AUTHOR      : Ryan Enns
DESC        :
    chat-client.h defines macros, objects, and function prototypes for
    the chat-client.c file.
*/

#ifndef CHAT_CLIENT_H
#define CHAT_CLIENT_H

#include <ncurses.h>

#define SERVER_PREFIX "-server"
#define USER_PREFIX "-user"
#define MAX_MSG 40

typedef struct threadParameters
{
    WINDOW* window;
    int socket;
    const char* userName;
} threadParameters;

int window_loop(int, const char*);

#endif