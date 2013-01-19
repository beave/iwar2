/*
** Copyright (C) 2009-2012 Quadrant Information Security <quadrantsec.com>
** Copyright (C) 2009-2012 Champ Clark III <cclark@quadrantsec.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License Version 2 as
** published by the Free Software Foundation.  You may not use, modify or
** distribute this program under any other version of the GNU General
** Public License.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

/* iwar-ncurses.c
 *
 * Screen drawing functions.  Largely taken from the orginal iWar. 
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <curses.h>
#include <string.h>
#include <form.h>
#include <unistd.h>
#include <stdlib.h>
#include "version.h"
#include "iwar2.h"

struct _iWarCounters *counters;
struct _iWarConfig *config;
struct _iWar_Screen_Info *screen_info;

void iWar_Initscreen() { 

initscr();
noecho();
cbreak();
timeout(0);                       /* Always process user input,  don't wait */
start_color();
clear();
getmaxyx(stdscr, screen_info->row, screen_info->col);

}


void iWar_Testscreen()
{

 getmaxyx(stdscr, screen_info->row, screen_info->col); /* Current screen attributes */

 if (screen_info->row < 24 || screen_info->col < 80)
    {
    endwin();
    fprintf(stderr, "This program requires a screen size of 80x24\n");
    exit(1);
    }

}


void iWar_Mainscreen()
{
        int b;
        int c;
        int maxrow;
        int maxcol;
        int tmp;


        getmaxyx(stdscr,maxrow,maxcol); /* Current screen attributes */

        init_pair(1, COLOR_WHITE, COLOR_BLUE);
        init_pair(2, COLOR_RED,   COLOR_BLUE);

        init_pair(3, COLOR_WHITE, COLOR_RED);     /* Error messages */
        init_pair(4, COLOR_BLACK, COLOR_YELLOW);  /* Warn message   */
        init_pair(5, COLOR_BLACK, COLOR_WHITE);   /* Intro message  */
        init_pair(6, COLOR_RED,   COLOR_WHITE);   /* Intro border   */


        init_pair(10, COLOR_WHITE, COLOR_BLACK);  /* NO CARRIER */
        init_pair(11, COLOR_GREEN, COLOR_BLACK);  /* CONNECT    */
        init_pair(12, COLOR_YELLOW, COLOR_BLACK); /* BUSY       */
        init_pair(13, COLOR_BLUE,  COLOR_BLACK);  /* VOICE      */

        init_pair(14, COLOR_MAGENTA,  COLOR_BLACK);  /* Already scanned */
        init_pair(15, COLOR_CYAN, COLOR_BLACK);      /* Blacklist number */
        init_pair(16, COLOR_RED, COLOR_BLACK);       /* Skipped number */

        init_pair(30, COLOR_BLUE, COLOR_BLACK);    /* voip */

        attron(COLOR_PAIR(1));
        move(0,0); addch(ACS_ULCORNER);

        for (b = 0; b < maxcol-2; b++) { addch(ACS_HLINE); }
        addch(ACS_URCORNER);

        /* Probably a better way to do this .... */
       for (c=1; c < 9;  c++)
        {
        move(c,0);
        addch(ACS_VLINE);
                for (b = 0; b < maxcol-2; b++) { addch(' '); }
        addch(ACS_VLINE);
        }

        attron(COLOR_PAIR(10));

        move(maxrow-8,0); addch(ACS_ULCORNER);
        for (b = 0; b < maxcol-2; b++) { addch(ACS_HLINE); }
        addch(ACS_URCORNER);

        //move(maxrow-8, maxcol-35); printw("[Thread Status Window]");
	move(maxrow-8, 5); printw("[Thread Status Window]");

        /* For online information */
	
	tmp=maxrow-1;
        for (c=maxrow-7; c < tmp;  c++)
                {
                move(c,0);
                addch(ACS_VLINE);
                move(c,maxcol-1);
                addch(ACS_VLINE);
                }

         move(tmp,0); addch(ACS_LLCORNER);

         for (b = 0; b < maxcol-2; b++) { addch(ACS_HLINE); }
         addch(ACS_LRCORNER);

        attron(COLOR_PAIR(1));

        move(1,0); addch(ACS_VLINE); printw(" Dialer Command  :");
        move(2,0); addch(ACS_VLINE); printw(" Start/End Scan  :");
        move(3,0); addch(ACS_VLINE); printw(" Pre/Post Dial   : %s %s", config->iwar_predial, config->iwar_postdial);
        move(4,0); addch(ACS_VLINE); printw(" Log File        :");
        move(5,0); addch(ACS_VLINE); printw(" Status          :");
        move(6,0); addch(ACS_VLINE); //printw(" Idle            :");	/* Multi-threaded,  no longer a single timera /*
        move(7,0); addch(ACS_VLINE); printw(" CNAM Lookup     :");

        move(8,0);
        addch(ACS_LLCORNER);
        for (b = 0; b < maxcol-2; b++) { addch(ACS_HLINE); }
        addch(ACS_LRCORNER);

        attron(COLOR_PAIR(1));
        move(1, maxcol-23);
        printw("MODEM/FAX/TDD:");
        move(2, maxcol-23);
        printw("NO CARRIER   :");
        move(3, maxcol-23);
        printw("BUSY         :");
        move(4, maxcol-23);
        printw("VOICE        :");
        move(5, maxcol-23);
        printw("TONE/SILENCE :");
        move(6, maxcol-23);
        printw("TIMEOUT/SKIP :");
        move(7,maxcol-23);
        printw("Numbers Left : ");


        refresh();   /* Draw! BANG! BANG! */
}


void iWar_Intro()
{
WINDOW *info;

int i; int b;
//int maxrow;  int maxcol;
char msg[50];

getmaxyx(stdscr,screen_info->row, screen_info->col);
info = newwin(15,50, (screen_info->row - 7) / 2, (screen_info->col - 50) / 2);
wattrset(info, COLOR_PAIR(6));
box(info, ACS_VLINE, ACS_HLINE);

wattrset(info, COLOR_PAIR(5));

for (i=1; i<14; i++)
{ for (b=1; b<49; b++) { mvwaddch(info,i,b, ' '); } }

strncpy(msg, "Intelligent War Dialer 2 [iWar2]", sizeof(msg)); mvwprintw(info, 1, (50-strlen(msg)) / 2, "%s", msg);
strncpy(msg, "https://quadrantsec.com/iwar2", sizeof(msg));  mvwprintw(info, 2, (50-strlen(msg)) / 2, "%s", msg);
snprintf(msg, sizeof(msg), "Version: %s", VERSION); mvwprintw(info, 3, (50-strlen(msg)) / 2, "%s", msg);
strncpy(msg, "\"Dial Hard 2 - Dial Harder\"", sizeof(msg)); mvwprintw(info, 4, (50-strlen(msg)) / 2, "%s", msg);
wattrset(info, COLOR_PAIR(6));
strncpy(msg, "Written By Da Beave", sizeof(msg)); mvwprintw(info, 6, (50-strlen(msg)) / 2, "%s", msg);
wattrset(info, COLOR_PAIR(5));
strncpy(msg, "Released XXX. XXth 2013", sizeof(msg)); mvwprintw(info, 8, (50-strlen(msg)) / 2, "%s", msg);
strncpy(msg, "Contact Information:", sizeof(msg)); mvwprintw(info, 10, (50-strlen(msg)) / 2, "%s", msg);
strncpy(msg, "Email: cclark@quadrantsec.com", sizeof(msg)); mvwprintw(info, 11, (50-strlen(msg)) / 2, "%s", msg);
strncpy(msg, "IRC: irc.2600.net #telephreak", sizeof(msg)); mvwprintw(info, 12, (50-strlen(msg)) / 2, "%s", msg);

wrefresh(info);
sleep(3);
delwin(info);
touchwin(stdscr);
refresh();
}

void iWar_Display_Info(const char *message, int seconds, int message_type)
{

WINDOW *info;

int i; int b;
int maxrow;  int maxcol;
getmaxyx(stdscr, screen_info->row, screen_info->col);

info = newwin(7,50, (screen_info->row - 7) / 2, (screen_info->col - 50) / 2);

if (message_type == 1) { wattrset(info, COLOR_PAIR(3)); }	/* Red */
if (message_type == 2) { wattrset(info, COLOR_PAIR(4)); }	/* Yellow */
if (message_type == 3) { wattrset(info, COLOR_PAIR(5)); }	/* White */

box(info, ACS_VLINE, ACS_HLINE);

/* Fill in box */

for (i=1; i<6; i++)
 { for (b=1; b<49; b++) { mvwaddch(info,i,b, ' '); } }

mvwprintw(info, 3, (50-strlen(message)) / 2, "%s",  message);
wrefresh(info);
sleep(seconds);
delwin(info);
touchwin(stdscr);
refresh();
}



void iWar_Update_Status (const char *nstring,... ) {

char buf[256];

va_list ap;
va_start(ap, nstring);
vsnprintf(buf, sizeof(buf), nstring, ap);

init_pair(1, COLOR_WHITE, COLOR_BLUE);
attron(COLOR_PAIR(1));
move(5,20);
printw("                                     ");
move(5,20);
printw("%s", buf);
attroff(COLOR_PAIR(1));
refresh();
}

/* DEBUG -  screen_info shouldnt be a global */

void iWar_Plot(char *dialnum, int color, int type ) {

if ( type == IWAR_NORMAL) attron( COLOR_PAIR(color) | A_NORMAL );
if ( type == IWAR_BLINK) attron( COLOR_PAIR(color) | A_BLINK );
if ( type == IWAR_REVERSE) attron( COLOR_PAIR(color) | A_REVERSE );
if ( type == IWAR_UNDERLINE) attron( COLOR_PAIR(color) | A_UNDERLINE );
if ( type == IWAR_STANDOUT) attron( COLOR_PAIR(color) | A_STANDOUT );
if ( type == IWAR_BOLD) attron( COLOR_PAIR(color) | A_BOLD );
	
move(screen_info->row,screen_info->col);
printw("%s", dialnum);

if ( type == IWAR_NORMAL) attroff( COLOR_PAIR(color) | A_NORMAL );
if ( type == IWAR_BLINK) attroff( COLOR_PAIR(color) | A_BLINK );
if ( type == IWAR_REVERSE) attroff( COLOR_PAIR(color) | A_REVERSE );
if ( type == IWAR_UNDERLINE) attroff( COLOR_PAIR(color) | A_UNDERLINE );
if ( type == IWAR_STANDOUT) attroff( COLOR_PAIR(color) | A_STANDOUT );
if ( type == IWAR_BOLD) attroff( COLOR_PAIR(color) | A_BOLD );

refresh();
}

void iWar_Update_Right(int type, int value) { 

int right_row=0; 
int row, col;

getmaxyx(stdscr, row, col);

if ( type == CONNECT ) right_row = 1; 
if ( type == NO_CARRIER ) right_row = 2;
if ( type == BUSY ) right_row = 3;
if ( type == VOICE ) right_row = 4; 
if ( type == TONE ) right_row = 5;
if ( type == TIMEOUT ) right_row = 6;
if ( type == TOTAL_LEFT ) right_row = 7;

attron( COLOR_PAIR(1) | A_NORMAL ); 
move(right_row, col - 8); 
printw("%d ", value); 
attroff( COLOR_PAIR(1) | A_NORMAL );
refresh();

}


