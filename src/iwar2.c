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
#include <stdint.h>
#include <inttypes.h>
#include <fcntl.h>

#include "version.h"
#include "iwar2-defs.h"
#include "iwar2.h"

struct _iWarVar *var;
struct _iWarCounters *counters;
struct _iWarConfig *config;
struct _iWarAlertConfig *alertconfig;
struct _iWar_Screen_Info *screen_info;

pthread_cond_t iWarProcDoWork=PTHREAD_COND_INITIALIZER;
pthread_mutex_t iWarProcWorkMutex=PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char **argv) {

int fd=0;

const struct option long_options[] = {
        { "help",         no_argument,          NULL,   'h' },
        { "daemon",       no_argument,          NULL,   'D' },
        { "config",       required_argument,    NULL,   'c' },
	{ "mask", 	  required_argument,    NULL,   'm' },
	{ "predial", 	  required_argument,    NULL, 	'p' }, 
	{ "postdial",     required_argument,    NULL,   'P' },
        {0, 0, 0, 0}
};

static const char *short_options = "P:p:m:c:hD";
int option_index = 0;

int i=0;
int rc=0;

char iwar_buffer[IWAR_FIFO_BUFFER];

char *ptmp=NULL;
char *tok=NULL;

char tmp[2];
char tmp2[64];
char c;

char dialed_number[30] = { 0 }; 
char status[30] = { 0 }; 
char human_message[256] = { 0 };

/* Allocate memroy for global struct _SaganConfig */
config = malloc(sizeof(_iWarConfig));
memset(config, 0, sizeof(_iWarConfig));

counters = malloc(sizeof(_iWarCounters));
memset(counters, 0, sizeof(_iWarCounters));

screen_info = malloc(sizeof(_iWar_Screen_Info));
memset(counters, 0, sizeof(_iWarCounters));

/* Default startup screen settings */

screen_info->row = 10;
screen_info->col = 2; 

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

	   snprintf(tmp2, sizeof(tmp2), "%s", iWar_Mask_Replace(optarg, "0"));
	   config->iwar_start = strtoull(tmp2, NULL, 10);

	   snprintf(tmp2, sizeof(tmp2), "%s", iWar_Mask_Replace(optarg, "9"));
	   config->iwar_end = strtoull(tmp2, NULL, 10);
	   break;

	   case 'p':
	   snprintf(config->iwar_predial, sizeof(config->iwar_predial), "%s", iWar_Remove_Return(optarg));
	   break;

           case 'P':
           snprintf(config->iwar_postdial, sizeof(config->iwar_postdial), "%s", iWar_Remove_Return(optarg));
           break;

	    
	    }
}

if (config->iwar_start == 0) iWar_Log(1, "No MASK speicifed! [start]");
if (config->iwar_end == 0) iWar_Log(1, "No MASK speicifed! [end]");
if (config->iwar_start == config->iwar_end) iWar_Log(1, "Nothing to dial!");


counters->numbers_left = config->iwar_end - config->iwar_start + 1; 

printf("  %" PRIu64 " - %" PRIu64 "\n", config->iwar_start, config->iwar_end);
sleep(1);

iWar_Load_Config(); 

pthread_attr_t dialer_thread_attr;
pthread_attr_init(&dialer_thread_attr);
pthread_attr_setdetachstate(&dialer_thread_attr,  PTHREAD_CREATE_DETACHED);

/* Master thread that feeds numbers to children */

pthread_attr_t master_thread_attr;
pthread_attr_init(&master_thread_attr);
pthread_attr_setdetachstate(&master_thread_attr,  PTHREAD_CREATE_DETACHED);

pthread_t master_thread_id;

iWar_Initscreen();
iWar_Mainscreen();

/* Set right hand side counters to zero */
iWar_Update_Right(CONNECT, 0);
iWar_Update_Right(NO_CARRIER, 0);
iWar_Update_Right(BUSY, 0);
iWar_Update_Right(TONE, 0);
iWar_Update_Right(TIMEOUT, 0);
iWar_Update_Right(VOICE, 0);
iWar_Update_Right(TOTAL_LEFT, counters->numbers_left);

//iWar_Intro();

terminalwin = newwin(6, screen_info->col-5, screen_info->row-7,2);
scrollok(terminalwin, TRUE);
wrefresh(terminalwin);

rc = pthread_create ( &master_thread_id, &master_thread_attr, (void *)iWar_Master, NULL );
if ( rc != 0 ) iWar_Log(1, "Could not pthread_create() master thread [Error code: %d]", rc);


if ( config->serial_flag ) {

//iWar_Display_Info("Spinning serial up threads", 1, 2);

pthread_t dialer_thread_id[counters->serial_count];
for (i = 0; i < counters->serial_count; i++) {
     rc = pthread_create ( &dialer_thread_id[i], &dialer_thread_attr, (void *)iWar_Mother_Forker, NULL );
     if ( rc != 0 ) iWar_Log(1, "Could not pthread_create() dialer threads [Error code: %d]", rc);
     }
}

//iWar_Display_Info("Open FIFO/Waiting on communications", 1, 2);
fd = open(config->iwar_fifo, O_RDONLY);

if(fd < 0) iWar_Display_Info("Error opening FIFO!", 1, 1); 

/* Start thread that'll feed the other threads here (See the iwar-fifo) */

while(1) {


                i = read(fd, &c, 1);
                if (i < 0) iWar_Update_Status("Error reading FIFO [read]");

		if ((int)c != 0 ) {
		   snprintf(tmp, sizeof(tmp), "%c", c);
		   strncat(iwar_buffer, tmp, 1);
		   }

		if ((int)c == 10 ) {
	    	   touchwin(terminalwin);
    		   wprintw(terminalwin, "%s", iwar_buffer);
    		   wrefresh(terminalwin);

		   ptmp = strtok_r(iwar_buffer, "|", &tok); 
		   strlcpy(dialed_number, ptmp, sizeof(dialed_number));

		   ptmp = strtok_r(NULL, "|", &tok);
		   strlcpy(status, ptmp, sizeof(status));
		   
		   ptmp = strtok_r(NULL, "|", &tok);
		   strlcpy(human_message, iWar_Remove_Return(ptmp), sizeof(human_message)); 

		if (!strcmp(status, "ATDT")) iWar_Update_Status("Calling %s....", dialed_number);
		
		if (!strcmp(status, "BUSY")) {
		   counters->busy++; 
		   counters->numbers_left--; 
		   iWar_Update_Status("%s is busy.", dialed_number); 
		   iWar_Row_Col_Check(dialed_number);
                   iWar_Plot(dialed_number, BUSY, IWAR_BOLD);
		   iWar_Update_Right(BUSY, counters->busy);
		   iWar_Update_Right(TOTAL_LEFT, counters->numbers_left);
                   }

		if (!strcmp(status, "NO CARRIER")) { 
		   counters->no_carrier++; 
		   counters->numbers_left--;
		   iWar_Update_Status("Nothing was found at %s", dialed_number);
		   iWar_Row_Col_Check(dialed_number);
		   iWar_Plot(dialed_number, NO_CARRIER, IWAR_NORMAL);
		   iWar_Update_Right(BUSY, counters->no_carrier);
                   iWar_Update_Right(TOTAL_LEFT, counters->numbers_left);
		   }		

		if (!strcmp(status, "CONNECT")) { 
		   counters->connect++;
		   counters->numbers_left--;
		   iWar_Update_Status("CONNECTED at %s!", dialed_number);
		   iWar_Row_Col_Check(dialed_number);
		   iWar_Plot(dialed_number, CONNECT, IWAR_BLINK);
		   iWar_Update_Right(CONNECT, counters->connect);
                   iWar_Update_Right(TOTAL_LEFT, counters->numbers_left);
		   }
		   
		   /* Search for result from dialer that we're interested in */

		   for (i=0; i<counters->alert_config_count; i++) { 
		       if (!strcmp(status, alertconfig[i].alert)) { 
		          iWar_Update_Status("%s at %s", alertconfig[i].alert, dialed_number); 
			  // Record to log here
			  }
	           }

    		   strncpy(iwar_buffer, "", sizeof(iwar_buffer));
    		   c=0;
                   }
         }

} /* End of main() */
