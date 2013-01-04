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



pthread_cond_t iWarProcDoWork;
pthread_mutex_t iWarProcWorkMutex;

int iwar_work;
//char number[64]; 

void iWar_Mother_Forker ( void ) {

for (;;) {

        pthread_mutex_lock(&iWarProcWorkMutex);
	//while ( iwar_work == 0 ) pthread_cond_wait(&iWarProcDoWork, &iWarProcWorkMutex);	
	pthread_cond_wait(&iWarProcDoWork, &iWarProcWorkMutex);
//	printf("got work %s\n", number);
	pthread_mutex_unlock(&iWarProcWorkMutex);
	

}
} /* End of iWar_Mother_Forker */
