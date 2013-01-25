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
#include <stdint.h>
#include <inttypes.h>

#include "iwar2-defs.h"
#include "iwar2.h"


struct _iWarCounters *counters;
struct _iWar_Dialer_Number *iWarDialerNumber;
struct _iWarConfig *config;
struct _iWarS *serial;

pthread_cond_t iWarProcDoWork;
pthread_mutex_t iWarProcWorkMutex;

uint64_t number_to_dial=0;
int iwar_msgslot=0;
int thread_num;

//char number[64]; 

void iWar_Mother_Forker ( void ) {

int rc;
int int_thread_num;

pthread_mutex_lock(&iWarProcWorkMutex);
int_thread_num = thread_num;
thread_num++; 
pthread_mutex_unlock(&iWarProcWorkMutex);

char tmp_command[512];
char tmp_fifo[512];

for (;;) {

        pthread_mutex_lock(&iWarProcWorkMutex);
	while ( iwar_msgslot == 0 ) pthread_cond_wait(&iWarProcDoWork, &iWarProcWorkMutex);	
	pthread_cond_wait(&iWarProcDoWork, &iWarProcWorkMutex);
        iwar_msgslot--;
        pthread_mutex_unlock(&iWarProcWorkMutex);

	snprintf(tmp_command, sizeof(tmp_command), "%s --dial %" PRIu64 "", serial[int_thread_num].command, number_to_dial);
	rc = system(tmp_command);
        }
} /* End of iWar_Mother_Forker */

void iWar_Master ( void ) { 

iWarDialerNumber = malloc(counters->serial_count * sizeof(struct _iWar_Dialer_Number));

char tmp[64] = { 0 };

number_to_dial = config->iwar_start - 1;            // If seq. then do this,  otherwise, we'll go random 

for (;;) { 

	iwar_msgslot++;
	
	if ( iwar_msgslot <= counters->serial_count )  { 

          sleep(1); 
 	   iWar_Send_FIFO(config->iwar_fifo, "DEBUG|DEBUG|HERE\n");

	   } else { 
	   
	   iWar_Get_Next_Number(0);
	   iWar_Send_FIFO(config->iwar_fifo, "DEBUG|DEBUG| %" PRIu64 "\n", number_to_dial);

	   pthread_mutex_lock(&iWarProcWorkMutex);
	   snprintf(iWarDialerNumber[iwar_msgslot].number, sizeof(iWarDialerNumber[iwar_msgslot].number), "%" PRIu64 "", number_to_dial);
	   iwar_msgslot++; 
	   pthread_cond_signal(&iWarProcDoWork);
	   pthread_mutex_unlock(&iWarProcWorkMutex);
	   sleep(1);
	}
    }
}
