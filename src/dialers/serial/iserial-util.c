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

/* iserial-util.c
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "sysdep.h"
/*
void sendmodem(const char *sendstring)
{

char *dest=NULL;
char dialstring[128];
int len;
*/
/* modemqueue is used for things like volume changes,  speaker off, etc */

   /* No commands should start with 13 (return). If the sendstring is
      only 13,  then we are probably hanging up the line.  As in,
      you've sent ATDT5551212,  waited and nothing happened.  So,  to
      hang up the line you send 13.   If thats the case,  we send the
      13 _first_ then send whatever is in the modemqueue.  */

/*
   len=sendstring[0];
   if (len == 13 )
   {
   write(portfd, "\r", 1);
   sleep(1);                  /* Let the modem return NO CARRIER */
 //  }

/*
   if (strcmp(modemqueue, ""))
      {
      nstatus("Sending Queued Commands...");
      write(portfd, modemqueue, strlen(modemqueue));
      strncpy(modemqueue, "", sizeof(modemqueue));      /* Command was sent,
                                                           clear the queue */
//      sleep(1);
 //     }

   /* If it's not just a return,  do whatever you where originally
      instructed to do .... */
  
  // if (len != 13)  write(portfd, sendstring, strlen(sendstring));
//}

