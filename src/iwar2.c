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
#include <curses.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <pthread.h>

#include "version.h"
#include "iwar2-defs.h"
#include "iwar2.h"

struct _iWarConfig *config;
struct _iWarVar *var;
struct _iWarCounters *counters;

pthread_cond_t iWarProcDoWork=PTHREAD_COND_INITIALIZER;
pthread_mutex_t iWarProcWorkMutex=PTHREAD_MUTEX_INITIALIZER;

int iwar_work=0;

int main(int argc, char **argv) {

const struct option long_options[] = {
        { "help",         no_argument,          NULL,   'h' },
        { "daemon",       no_argument,          NULL,   'D' },
        { "config",       required_argument,    NULL,   'c' },
	{ "mask", 	  required_argument,    NULL,   'm' },
	{ "predial", 	  required_argument,    NULL, 	'p' }, 
        {0, 0, 0, 0}
};

static const char *short_options = "m:c:hD";
int option_index = 0;

int i=0;
int rc=0;

/* Allocate memroy for global struct _SaganConfig */
config = malloc(sizeof(_iWarConfig));
memset(config, 0, sizeof(_iWarConfig));

counters = malloc(sizeof(_iWarCounters));
memset(counters, 0, sizeof(_iWarCounters));


char c; 

WINDOW *terminalwin;

/* Set some default values */

snprintf(config->iwar_config_file, sizeof(config->iwar_config_file), "%s", IWAR_CONFIG_FILE);

while ((c = getopt_long(argc, argv, short_options, long_options, &option_index)) != -1) {

   switch(c) {

           if (c == -1) break;

           case 'h':
           iWar_Usage();
           exit(0);
           break;

           case 'm':
	   snprintf(config->iwar_mask, sizeof(config->iwar_mask), "%s", iWar_Remove_Return(optarg));
	   config->iwar_start = atol(iWar_Mask_Replace(optarg, "0")); 
	   config->iwar_end   = atol(iWar_Mask_Replace(optarg, "9")); 
	   break;

	   case 'p':
	   snprintf(config->iwar_predial, sizeof(config->iwar_predial), "%s", iWar_Remove_Return(optarg));
	   break;
	    
	    }
}

if (config->iwar_start == 0) iWar_Log(1, "No MASK speicifed! [start]");
if (config->iwar_end == 0) iWar_Log(1, "No MASK speicifed! [end]");
if (config->iwar_start == config->iwar_end) iWar_Log(1, "Nothing to dial!");

iWar_Load_Config(); 

pthread_attr_t thread_attr;
pthread_attr_init(&thread_attr);
pthread_attr_setdetachstate(&thread_attr,  PTHREAD_CREATE_DETACHED);

iWar_Log(0, "Dialing mask: %s [%lu - %lu]", config->iwar_mask, config->iwar_start, config->iwar_end);

/* Spin up serial threads */

if ( config->serial_flag ) { 

iWar_Log(0, "Spinning up %d serial threads!", counters->serial_count);
sleep(2);

pthread_t thread_id[counters->serial_count];

for (i = 0; i < counters->serial_count; i++) {
     rc = pthread_create ( &thread_id[i], &thread_attr, (void *)iWar_Mother_Forker, NULL ); 
     if ( rc != 0 ) iWar_Log(1, "Could not pthread_create() for I/O processors [Error code: %d]", rc);
     }
}

iWar_Initscreen();
iWar_Mainscreen();
iWar_Intro();
sleep(1);

sleep(5);
terminalwin = newwin(6, counters->max_col-5, counters->max_row-7,2);
scrollok(terminalwin, TRUE);
wrefresh(terminalwin);
sleep(60);

}
