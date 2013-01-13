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

/* iwar_serial.c
 *
 */


/* hwhandshake / swhandshake opts */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>

#include "iwar2-defs.h"
#include "iwar2.h"
#include "iwar2-serial.h"
#include "sysdep.h"

int main(int argc, char **argv) {

/* See old iwar for more flags Beave */

const struct option long_options[] = {
        { "help",         no_argument,          NULL,   'h' },
        { "port",         required_argument,    NULL,   'p' },
	{ "parity",	  required_argument,  	NULL,  	'P' }, 
	{ "databits",	  required_argument, 	NULL,   'D' },
	{ "baud",	  required_argument, 	NULL, 	'b' }, 
        { "config",       required_argument,    NULL,   'c' },
        { "fifo",         required_argument,    NULL,   'f' },
	{ "dial", 	  required_argument, 	NULL, 	'd' },
        {0, 0, 0, 0}
};

static const char *short_options = "d:P:p:s:c:f:d:h";
int option_index = 0;

char c; 
char ch;
char buf[2] = { 0 };
char tmpbuf[2] = { 0 }; 
char tmpdial[64] = { 0 }; 
char modem_in[1024] = { 0 };  

int rc=0;
int i=0;
int timer=0;
int modem_timer=0;

sbool connect_flag=0;

fd_set fds;
struct timeval tv;

struct _iWarSerialConfig *serialconfig;
serialconfig = malloc(sizeof(_iWarSerialConfig));
memset(serialconfig, 0, sizeof(_iWarSerialConfig));

/* Defaults */

snprintf(serialconfig->port, sizeof(serialconfig->port), "%s", DEFAULT_PORT);
snprintf(serialconfig->parity, sizeof(serialconfig->parity), "%s", DEFAULT_PARITY);
snprintf(serialconfig->baud, sizeof(serialconfig->baud), "%s", DEFAULT_BAUD); 
snprintf(serialconfig->config, sizeof(serialconfig->config), "%s", DEFAULT_CONFIG);
snprintf(serialconfig->fifo, sizeof(serialconfig->fifo), "%s", DEFAULT_FIFO);

serialconfig->swhandshake=0;
serialconfig->hwhandshake=1;
serialconfig->portfd=0;
serialconfig->modem_timeout = DEFAULT_TIMEOUT;


while ((c = getopt_long(argc, argv, short_options, long_options, &option_index)) != -1) {

   switch(c) {

           if (c == -1) break;

           case 'h':
           iWar_Serial_Usage();
           exit(0);
           break;

	   case 'p':
	   snprintf(serialconfig->port, sizeof(serialconfig->port), "%s", iWar_Remove_Return(optarg)); 
	   break; 

	   case 'P':
	   snprintf(serialconfig->parity, sizeof(serialconfig->parity), "%s", iWar_Remove_Return(optarg));
	   if (strcmp(serialconfig->parity, "N") && strcmp(serialconfig->parity, "E") && strcmp(serialconfig->parity, "O")) iWar_Log(1, "Invalid parity!");
	   break;

           case 'D':

           /*  Databits - Valid are 5,6,7,8 */

           snprintf(serialconfig->databits, sizeof(serialconfig->databits), "%s", iWar_Remove_Return(optarg));

           if (strcmp(serialconfig->parity, "5") && 
               strcmp(serialconfig->parity, "6") && 
               strcmp(serialconfig->parity, "7") && 
               strcmp(serialconfig->parity, "8"))  iWar_Log(1, "Invalid databits!");
           break;

	   case 'b':

	   snprintf(serialconfig->baud, sizeof(serialconfig->baud), "%s", iWar_Remove_Return(optarg));

           if (strcmp(serialconfig->baud, "110")   && strcmp(serialconfig->baud,"300")     &&
               strcmp(serialconfig->baud, "1200")  && strcmp(serialconfig->baud,"2400")    &&
               strcmp(serialconfig->baud, "4800")  && strcmp(serialconfig->baud,"9600")    &&
               strcmp(serialconfig->baud, "19200") && strcmp(serialconfig->baud,"38400")   &&
               strcmp(serialconfig->baud, "57600") && strcmp(serialconfig->baud,"115200")  && 
 	       strcmp(serialconfig->baud, "230400")) 
	       iWar_Log(1, "Invalid speed!");
	   break;

	   case 'c':

	   snprintf(serialconfig->config, sizeof(serialconfig->config), "%s", iWar_Remove_Return(optarg));
	   break; 
	
	   case 'f': 

           snprintf(serialconfig->fifo, sizeof(serialconfig->fifo), "%s", iWar_Remove_Return(optarg));
           break;

	   case 'd':

	   snprintf(serialconfig->dial, sizeof(serialconfig->dial), "%s", iWar_Remove_Return(optarg));
	   break;

           }
}

if (!strcmp(serialconfig->dial, "")) iWar_Send_FIFO(serialconfig->fifo, "-|ERROR|No number specified.\n");

serialconfig->portfd = open(serialconfig->port, O_RDWR); 
if (serialconfig->portfd == -1)  iWar_Send_FIFO(serialconfig->fifo, "-|ERROR|Can't open serial port.\n");

m_savestate(serialconfig->portfd);
m_setparms(serialconfig->portfd,serialconfig->baud,serialconfig->parity,serialconfig->databits,serialconfig->hwhandshake,serialconfig->swhandshake);
m_nohang(serialconfig->portfd);       /* Do we need to do this? */
m_hupcl(serialconfig->portfd, 1);
m_flush(serialconfig->portfd);        /* Flush any old data out before we start */

snprintf(tmpdial, sizeof(tmpdial), "ATDT%s\r", serialconfig->dial);
iWar_Send_FIFO(serialconfig->fifo, "%s|DIALING|Calling %s....\n", serialconfig->dial, serialconfig->dial);

iWar_Send_Modem(serialconfig->portfd, tmpdial);

while( modem_timer < serialconfig->modem_timeout + 5) { 

	
        tv.tv_sec=1;                    /* 1 second */
        tv.tv_usec=0;
        FD_ZERO(&fds);
        FD_SET(serialconfig->portfd, &fds);
	
	if (select(serialconfig->portfd+1, &fds, NULL, NULL, &tv)) { 
           rc = read(serialconfig->portfd, buf, 1);
	   if ( rc == -1 ) iWar_Send_FIFO(serialconfig->fifo, "%s|ERROR|Error reading from serial port.\n", serialconfig->dial);
	   snprintf(tmpbuf, sizeof(tmpbuf), "%c", buf[0]);
	   strlcat(modem_in, tmpbuf, sizeof(modem_in));
	   
	   //printf("%s\n", modem_in);
	   
	   if ( (int)buf[0] == 13 || (int)buf[0] == 10 ) strlcpy(modem_in, "", sizeof(modem_in));

	   /*CONNECT FLAG */
	   if (strstr(modem_in, "NO DIALTONE")  || strstr(modem_in, "NO DIAL TONE")) { 
	      iWar_Send_FIFO(serialconfig->fifo, "%s|NO DIALTONE|NO DIALTONE\n", serialconfig->dial);
	      m_restorestate(serialconfig->portfd);        /* Restore state from "savestate" */
	      close(serialconfig->portfd);                 /* Close serial/output file */
	      exit(0);
	      }

	   if (!strcmp(modem_in, "NO CARRIER") && connect_flag == 0 ) { 
	      iWar_Send_FIFO(serialconfig->fifo, "%s|NO CARRIER|NO CARRIER\n", serialconfig->dial);
	      exit(0);
	      }

	   if (!strcmp(modem_in, "BUSY") && connect_flag == 0) {
	      iWar_Send_FIFO(serialconfig->fifo, "%s|BUSY|The number is busy\n", serialconfig->dial);
	      exit(0);
	      }

	   modem_timer=0;
	}

if ( connect_flag == 0 && modem_timer == serialconfig->modem_timeout  ) iWar_Send_Modem(serialconfig->portfd, "\r"); 		/* Nudge for "NO CARRIER".  If this doesn't happen the modem might be hung */

modem_timer++;
}

m_restorestate(serialconfig->portfd);        /* Restore state from "savestate" */
close(serialconfig->portfd);                 /* Close serial/output file */
exit(0);
}
