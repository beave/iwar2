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

/* iwar-engine.c
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <stdarg.h>
#include <stdlib.h>
#include <fcntl.h>
#include <curses.h>
#include "iwar2.h"

struct _iWarConfig *config;
struct _iWarVar *var;
struct _iWarCounters *counters;
struct _iWar_Screen_Info *screen_info;

pthread_mutex_t iWarProcWorkMutex;

uint64_t number_to_dial;


char *iWar_Replace_Str(char *str, char *orig, char *rep)
{

  static char buffer[4096] = { 0 };
  char *p = NULL;

  if(!(p = strstr(str, orig)))  return str;

  strlcpy(buffer, str, p-str);
  buffer[p-str] = '\0';
  sprintf(buffer+(p-str), "%s%s", rep, p+strlen(orig));
  return(buffer);
}

char *iWar_Mask_Replace(char *number, char *replace) {

int i=0;
char newstring[30] = { 0 };
char modstring[30] = { 0 };
char *retbuf = NULL;
char a[2] = { 0 };

snprintf(newstring, sizeof(newstring), "%s", number);

for (i=0; i < strlen(newstring); i++) {

    if ( newstring[i] == 'X' ) {
       strcat(modstring, replace);
       } else {
       snprintf(a, sizeof(a), "%c", newstring[i]);
       strcat(modstring, a);
       }
}

retbuf = modstring;
return(retbuf);
}

char  *iWar_Remove_Return(char *s) {
       char *s1, *s2;
       for(s1 = s2 = s;*s1;*s1++ = *s2++ )
       while( *s2 == '\n' )s2++;
       return s;
}


void iWar_Log (int type, const char *format,... ) {

   char buf[1024];
   va_list ap;
   va_start(ap, format);
   char *chr="*";
   char curtime[64];
   time_t t;
   struct tm *now;
   t = time(NULL);
   now=localtime(&t);
   strftime(curtime, sizeof(curtime), "%m/%d/%Y %H:%M:%S",  now);

   if ( type == 1 ) chr="E";
   if ( type == 2 ) chr="W";

   vsnprintf(buf, sizeof(buf), format, ap);
//   fprintf(config->sagan_log_stream, "[%s] [%s] - %s\n", chr, curtime, buf);
//   fflush(config->sagan_log_stream);

     printf("[%s] %s\n", chr, buf);
//     if ( config->daemonize == 0) printf("[%s] %s\n", chr, buf);
     if ( type == 1 ) exit(1);
}

char *iWar_Between_Quotes(char *instring) {

sbool flag=0;
int i;
char tmp1[2];

/* quick and dirty fix added by drforbin....this function really should be reworked 
fix added to make tmp2 presistent (non-automatic) so once the function returns it is presistent */

static char tmp2[512];
memset(tmp2,0,sizeof(tmp2));
char *ret;

for ( i=0; i<strlen(instring); i++) {

if ( flag == 1 && instring[i] == '\"' ) flag = 0;
if ( flag == 1 ) {
   snprintf(tmp1, sizeof(tmp1), "%c", instring[i]);
   strlcat(tmp2, tmp1, sizeof(tmp2));
   }

if ( instring[i] == '\"' ) flag++;

}

ret=tmp2;
return(ret);
}

char *iWar_Var_To_Value(char *instring) { 

char *ptmp = NULL; 
char *tok = NULL; 
char *retbuf = NULL;
char tmp[256] = { 0 };
char tmp2[256] = { 0 };
char tmp3[254] = { 0 }; 
char tmp_result[256] = { 0 }; 
char *tmpbuf = NULL;
int i=0;

snprintf(tmp, sizeof(tmp), "%s", instring);
tmpbuf = tmp;

for (i=0; i<counters->var_count; i++) { 
    
    ptmp = strtok_r(tmp, " ", &tok);

	while (ptmp != NULL ) {
	     strlcpy(tmp2, iWar_Replace_Str(ptmp, var[i].var_name, var[i].var_value), sizeof(tmp2));
	     snprintf(tmp3, sizeof(tmp3), "%s ", tmp2); 
	     strlcat(tmp_result, tmp3, sizeof(tmp_result));
//	     printf("= %s\n", tmp_result);
             ptmp = strtok_r(NULL, " ", &tok);
             }

snprintf(tmp, sizeof(tmp), "%s", tmp_result);
tmpbuf = tmp;
strlcpy(tmp_result, "", sizeof(tmp_result));
}


return(tmpbuf);
}

void iWar_Send_FIFO(char *fifo, const char *format,...) { 

char buf[1024];
va_list ap;
va_start(ap, format);
char *chr="*";
int fd = 0; 
int rc = 0; 

vsnprintf(buf, sizeof(buf), format, ap);

fd = open(fifo, O_RDWR);
if (fd == -1) iWar_Log(1, "Can't open %s fifo for write", fifo);

rc = write(fd, buf, strlen(buf));
close(fd);
}

uint64_t iWar_Get_Next_Number( sbool dialtype ) { 

/* Sequential */

if ( dialtype == 0 ) { 
pthread_mutex_lock(&iWarProcWorkMutex);
number_to_dial++; 
pthread_mutex_unlock(&iWarProcWorkMutex);
}

return(number_to_dial);


}


/* DEBUG - screen_info shouldnt be a global */

void iWar_Row_Col_Check( char *dialnum ) { 

	int i=0;
	int b=0;
        int maxrow;
        int maxcol;
	char tmp[64] = { 0 };

        getmaxyx(stdscr,maxrow,maxcol);  /* Current screen attributes */
        screen_info->row++;

        snprintf(tmp, sizeof(tmp), "%s", dialnum);

        if ( screen_info->row > maxrow-10)
                {
                screen_info->row=10; screen_info->col = screen_info->col + strlen(tmp) + 2;
                }

        /* If we're outside our range,   wipe the screen.   There's
           probably a better way to do this */

        if (screen_info->col + strlen(tmp) >= maxcol)
               {
//               iWar_Update_Status("Screen full: Clearing...");
               screen_info->col=2; attron(COLOR_PAIR(10) | A_NORMAL);
               for (i=10; i < maxrow-9; i++)
                 {
                 move(i,0); for (b=0; b < maxcol-1; b++)
                   {
                   addch(' ');          /* wipe it away.... */
                   }
                 }
               attroff(COLOR_PAIR(10) | A_NORMAL);
               }

}



