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

/*
 * iwar2-threads.c
 *
 * This becomes the pthreads to execute our dialer(s)
 *
 */


#ifdef HAVE_CONFIG_H
#include "config.h"             /* From autoconf */
#endif

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#include "iwar2-defs.h"
#include "iwar2.h"


struct _iWarCounters *counters;
struct _iWar_Dialer_Number *iWarDialerNumber;
struct _iWarConfig *config;
struct _iWarS *serial;

pthread_cond_t iWarProcDoWork;
pthread_mutex_t iWarProcWorkMutex;

int iwar_msgslot=0;
//char number[64]; 

void iWar_Mother_Forker ( void ) {

int rc;

char tmp_command[512];
char tmp_fifo[512];

for (;;) {

        pthread_mutex_lock(&iWarProcWorkMutex);
//	while ( iwar_msgslot == 0 ) pthread_cond_wait(&iWarProcDoWork, &iWarProcWorkMutex);	
	pthread_cond_wait(&iWarProcDoWork, &iWarProcWorkMutex);
	
	snprintf(tmp_command, sizeof(tmp_command), "%s --dial 12345", serial->command);
	snprintf(tmp_fifo, sizeof(tmp_fifo), "-|DIALING|%s\n", tmp_command); 
	iWar_Send_FIFO(config->iwar_fifo, tmp_fifo);
//	pthread_cond_wait(&iWarProcDoWork, &iWarProcWorkMutex);
//	printf("got work %s\n", WarDialerNumber[iwar_msgslot].number);
//	printf("\nGOT WORK\n");
//	rc = execl(serial->command, serial->command, NULL, (char *)NULL);
	rc = system(tmp_command);
	
//	iWar_Send_FIFO(config->iwar_fifo, "-|-|DONE\n");
	iwar_msgslot--;
	pthread_mutex_unlock(&iWarProcWorkMutex);
}
} /* End of iWar_Mother_Forker */

void iWar_Master ( void ) { 

iWarDialerNumber = malloc(counters->serial_count * sizeof(struct _iWar_Dialer_Number));

char *number="12345";
char tmp[64];


for (;;) { 

	iwar_msgslot++;
	
	if ( iwar_msgslot <= counters->serial_count )  { 

//	  snprintf(tmp, sizeof(tmp), "Counters: %d| msgslot: %d|Idle\n", counters->serial_count, iwar_msgslot);
//	  iWar_Send_FIFO(config->iwar_fifo, tmp);
          sleep(1); 

	   } else { 
	   
//	   iWar_Send_FIFO(config->iwar_fifo, "-|-|Got work!\n");
           
	   pthread_mutex_lock(&iWarProcWorkMutex);
	   snprintf(iWarDialerNumber[iwar_msgslot].number, sizeof(iWarDialerNumber[iwar_msgslot].number), "1234");
	   iwar_msgslot++; 
	   pthread_cond_signal(&iWarProcDoWork);
	   pthread_mutex_unlock(&iWarProcWorkMutex);
	   sleep(1);
	}
    }
}
