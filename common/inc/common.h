/*
NAME        :
PROJECT     : Can We Talk
AUTHOR      : Ryan Enns
DESC        :
*/

#ifndef COMMON_H
#define COMMON_H

#define PORT 5000
#define PACKET_WIDTH 78

#define IP_START    0
#define USER_START  15
#define MSG_START   26
#define TIME_START  67

typedef struct message
{
    const char* ip;
    const char* username;
    const char* message;
} message;

void helloWorld();
void logger(const char* user, const char* msg);
const char* constructMessage(message* m);
int occursIn(const char*, char);
int getIndexOf(const char* str, char c, int ins);
char* subString(const char* str, int s, int e);
void replace(const char* str, char o, char r);

#endif