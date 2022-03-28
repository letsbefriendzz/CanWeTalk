#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <ncurses.h>
#include <pthread.h>
#include <netdb.h>

static volatile int exit = 1;
static WINDOW *msg_win;
static int i = 0;
static int shouldBlank = 0;

void* listener_func(void* l);
int broadcastMessage(int socket, const char* msg);

WINDOW *create_newwin(int, int, int, int);
void destroy_win(WINDOW *);
void input_win(WINDOW *, char *);
void display_win(WINDOW *, const char *, int, int);
void destroy_win(WINDOW *win);
void blankWin(WINDOW *win);

void threadWindowTest(const char* msg)
{
  display_win(msg_win, msg, i, 0);
}

int window_loop(int socket)
{
  pthread_t listener;
  //pthread_create( &listener, NULL , listener_func, (void *)&socket);
  sleep(1);
  printf("EXIT:\t%d\n", exit);
  //return -1;
  // so from about here

  WINDOW *chat_win;
  int chat_startx, chat_starty, chat_width, chat_height;
  int msg_startx, msg_starty, msg_width, msg_height, i;
  char buf[BUFSIZ];

  initscr();                      /* Start curses mode*/
  cbreak();
  noecho();
  refresh();

  chat_height = 5;
  chat_width  = COLS - 2;
  chat_startx = 1;
  chat_starty = LINES - chat_height;

  msg_height = LINES - chat_height - 1;
  msg_width  = COLS;
  msg_startx = 0;
  msg_starty = 0;

  // to here, I have no clue what the hell is happening
  
  /* create the input window */
  msg_win = create_newwin(msg_height, msg_width, msg_starty, msg_startx);
  scrollok(msg_win, TRUE);

  /* create the output window */
  chat_win = create_newwin(chat_height, chat_width, chat_starty, chat_startx);
  scrollok(chat_win, TRUE);

  /* allow the user to input 5 messages for display */ 
  while(strcmp( buf, ">>bye<<" ) != 0)
  {
    input_win(chat_win, buf);
    broadcastMessage(socket, buf);
    //display_win(msg_win, buf, i, shouldBlank);
    //printf("\n%s\n", buf);
    threadWindowTest((const char*)buf);
    i++;
  }

  /* tell the user that the 5 messages are done ... */
  shouldBlank = 1;
  sprintf(buf,"Messaging is complete ... destroying window in 5 seconds");
  display_win(msg_win, buf, 0, shouldBlank);
  
  sleep(1);
     
  destroy_win(chat_win);
  destroy_win(msg_win);
  endwin();
}
     
WINDOW *create_newwin(int height, int width, int starty, int startx)
{       
  WINDOW *local_win;
     
  local_win = newwin(height, width, starty, startx);
  box(local_win, 0, 0);               /* draw a box */
  wmove(local_win, 1, 1);             /* position cursor at top */
  wrefresh(local_win);
  return local_win;
}
     
/*
This function takes characters from the user and sends them to the word
pointer that was provided as a parameter.
*/
void input_win(WINDOW *win, char *word)
{
  int ch;
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
     
void display_win(WINDOW *win, const char *word, int whichRow, int shouldBlank)
{
  // if shouldBlank flag is set, clear the window
  if(shouldBlank == 1)
  blankWin(win);
  wmove(win, (whichRow+1), 1); // position cusor at approp row
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

void* listener_func(void* s)
{
  int server_socket = *(int*)s;
  char buffer[BUFSIZ];
  while( 1 )
  {
    // clear out and get the next command and process
    memset(buffer,0,BUFSIZ);
    int numBytesRead = read (server_socket, buffer, BUFSIZ);

    if(strcmp(buffer, ">>bye<<") == 0) break;

    display_win(msg_win, buffer, i, 0);
    i++;
  }
  pthread_exit((void*) 0);
}

int broadcastMessage(int socket, const char* msg)
{
    return write(socket, msg, strlen(msg));
}