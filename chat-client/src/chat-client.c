/* splitWin.c is a simple example to show how to deal with split screens.
   Due to the limited time, this program is not finished yet.

   To compile:   gcc splitWin.c -lncurses

		Sam Hsu (11/17/10)
*/ // bruh i'm literally gonna clone this code and not understand a bit of it

#define __REENTRANT

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <ncurses.h>

////////////////////////////////////////////
#include <pthread.h>
#include "../inc/chat-client.h"
#include "../../common/inc/common.h"

static int MASTER_ROW = 0;
static char* TEST_VALS[5] = { "test1", "test2", "test3", "test4", "test5" };
static int exec = 0;

void listenerThread(void* param);
void* writerThead(void* param);
////////////////////////////////////////////     
WINDOW *create_newwin(int, int, int, int);
void destroy_win(WINDOW *);
void input_win(WINDOW *, char *);
void display_win(WINDOW *, char *, int, int);
void destroy_win(WINDOW *win);
void blankWin(WINDOW *win);
     
void appendToWindow( WINDOW* win, char* word, int shouldBlank )
{
  display_win(win, word, MASTER_ROW, shouldBlank);
  MASTER_ROW++;
}

int window_loop(int server_socket, const char* userName)
{
  pthread_t thread;

  #pragma region window setup
  WINDOW *chat_win, *msg_win;
  int chat_startx, chat_starty, chat_width, chat_height;
  int msg_startx, msg_starty, msg_width, msg_height, i;
  int shouldBlank;
  char buf[BUFSIZ];

  initscr();                      /* Start curses mode            */
  cbreak();
  noecho();
  refresh();
  
  shouldBlank = 0;

  chat_height = 5;
  chat_width  = COLS - 2;
  chat_startx = 1;        
  chat_starty = LINES - chat_height;        
     
  msg_height = LINES - chat_height - 1;
  msg_width  = COLS;
  msg_startx = 0;        
  msg_starty = 0;        
  
  /* create the input window */
  msg_win = create_newwin(msg_height, msg_width, msg_starty, msg_startx);
  scrollok(msg_win, TRUE);

  /* create the output window */
  chat_win = create_newwin(chat_height, chat_width, chat_starty, chat_startx);
  scrollok(chat_win, TRUE);

  #pragma endregion

  // setup threadParameters for the writing thread
  threadParameters w;
  // copy server_socket value
  w.socket    = server_socket;
  // point to the correct window
  w.window    = chat_win;
  // copy the local username
  w.userName  = userName;
  // inform the compiler that we don't care about the return value of this thread
  //pthread_detach(thread);
  // aaaaaaand attempt to make the thread
  if (pthread_create( &thread, NULL , writerThead, (void *)&w))
  {
    fflush(stdout);
    return 5;
  }

  // create threadParameters for the listener thread
  // which is actually the main thread; i'm just acting like
  // it's another thread because I've bounced between which
  // task should be on the main thread and which should be on its own.
  threadParameters l;
  // copy the server socket
  l.socket    = server_socket;
  // point to the message window
  l.window    = msg_win;
  // set username to null, because we don't need it here
  l.userName  = NULL;
  // call the listenerThread function with the above parameter
  listenerThread( (void*)&l );

  void** status;
  pthread_join( thread, status );

  // once the listenerThread 
  shouldBlank = 1;
  sprintf(buf,"Messaging is complete! Terminating in 3 seconds...");
  display_win(msg_win, buf, 0, shouldBlank);
  sleep(3);
  
  #pragma region window teardown
  destroy_win(chat_win);
  destroy_win(msg_win);
  endwin();
  #pragma endregion

  printf("COMPLETE\n");
}

void listenerThread(void* param)
{
  threadParameters lp = *((threadParameters*)param);
  char b[PACKET_WIDTH + 1];

  while( exec == 0 )
  {
      // our local buffer, just in case
      memset(b,0,PACKET_WIDTH);
      int numBytesRead = read (lp.socket, b, sizeof(b));

      if( numBytesRead > 0 )
      {
        b[78] = '\0';
        appendToWindow( lp.window, b, 0 );
      }
    sleep(1);
  }
  printf("fin\n");
  return;
}

void* writerThead(void* param)
{
  threadParameters p = *((threadParameters*)param);
  char buf[MAX_MSG];
  while(exec == 0)
  {
    // reset buffer to nill
    memset(buf,0,MAX_MSG);
    // get input from the user
    sleep(1);
    input_win(p.window, buf);
    replace(buf, '|', ';');
    if( strlen(buf) < 80 )
    {
      char message[BUFSIZ];

      // strip a newline from the input, if it is present
      if (buf[strlen (buf) - 1] == '\n')
        buf[strlen (buf) - 1] = '\0';

      // get time
      time_t s = time(0);
      struct tm* local = localtime(&s);

      // format the message -- ONLY the username, msg and time()
      sprintf(message, "[%-5s] >>|%-40s|(%02d:%02d:%02d)", p.userName, buf, local->tm_hour, local->tm_min, local->tm_sec);

      // if the user inputs >>bye<<, we can set the done flag to 0
      if(strcmp(buf,">>bye<<") == 0)
        exec = 1;

      // done or not, we write to the server
      write (p.socket, message, strlen (message));
    }
  }

  appendToWindow( p.window, "exiting...", 0 );
  pthread_exit((void*) 0);
}

#pragma region window functions

WINDOW *create_newwin(int height, int width, int starty, int startx)
{
  WINDOW *local_win;
     
  local_win = newwin(height, width, starty, startx);
  box(local_win, 0, 0);               /* draw a box */
  wmove(local_win, 1, 1);             /* position cursor at top */
  wrefresh(local_win);     
  return local_win;
}
     
/* This function is for taking input chars from the user */
void input_win(WINDOW *win, char *word)
{
  int i, ch;
  int maxrow, maxcol, row = 1, col = 0;
     
  blankWin(win);                          /* make it a clean window */
  getmaxyx(win, maxrow, maxcol);          /* get window size */
  bzero(word, BUFSIZ);
  wmove(win, 1, 1);                       /* position cusor at top */
  for (i = 0; (ch = wgetch(win)) != '\n'; i++) 
  {
    word[i] = ch;                       /* '\n' not copied */
    if (col++ < maxcol-2)               /* if within window */
    {
      wprintw(win, "%c", word[i]);      /* display the char recv'd */
    }
    else                                /* last char pos reached */
    {
      col = 1;
      if (row == maxrow-2)              /* last line in the window */
      {
        scroll(win);                    /* go up one line */
        row = maxrow-2;                 /* stay at the last line */
        wmove(win, row, col);           /* move cursor to the beginning */
        wclrtoeol(win);                 /* clear from cursor to eol */
        box(win, 0, 0);                 /* draw the box again */
      } 
      else
      {
        row++;
        wmove(win, row, col);           /* move cursor to the beginning */
        wrefresh(win);
        wprintw(win, "%c", word[i]);    /* display the char recv'd */
      }
    }
  }
}  /* input_win */
     
void display_win(WINDOW *win, char *word, int whichRow, int shouldBlank)
{
  if(shouldBlank == 1) blankWin(win);                /* make it a clean window */
  wmove(win, (whichRow+1), 1);                       /* position cusor at approp row */
  wprintw(win, word);
  wrefresh(win);
} /* display_win */
     
void destroy_win(WINDOW *win)
{
  delwin(win);
}  /* destory_win */
     
void blankWin(WINDOW *win)
{
  int i;
  int maxrow, maxcol;
     
  getmaxyx(win, maxrow, maxcol);
  for (i = 1; i < maxcol-2; i++)  
  {
    wmove(win, i, 1);
    refresh();
    wclrtoeol(win);
    wrefresh(win);
  }
  box(win, 0, 0);             /* draw the box again */
  wrefresh(win);
}  /* blankWin */

#pragma endregion