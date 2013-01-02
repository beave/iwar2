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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <getopt.h>

#include "iwar2-defs.h"
#include "iwar2-serial.h"
#include "sysdep.h"

int main(int argc, char **argv) {

const struct option long_options[] = {
        { "help",         no_argument,          NULL,   'h' },
        { "port",         required_argument,    NULL,   'p' },
	{ "speed",	  required_argument, 	NULL, 	's' }, 
        { "config",       required_argument,    NULL,   'c' },
        { "fifo",         required_argument,    NULL,   'f' },
	{ "dial", 	  required_argument, 	NULL, 	'd' },
        {0, 0, 0, 0}
};

static const char *short_options = "p:s:c:f:d:h";
int option_index = 0;

char c; 

struct _iWarSerialConfig *serialconfig;
serialconfig = malloc(sizeof(_iWarSerialConfig));
memset(serialconfig, 0, sizeof(_iWarSerialConfig));

while ((c = getopt_long(argc, argv, short_options, long_options, &option_index)) != -1) {

   switch(c) {

           if (c == -1) break;

           case 'h':
           iWar_Serial_Usage();
           exit(0);
           break;

/*
           case 'm':
           snprintf(config->iwar_mask, sizeof(config->iwar_mask), "%s", iWar_Remove_Return(optarg));
           config->iwar_start = atol(iWar_Mask_Replace(optarg, "0"));
           config->iwar_end   = atol(iWar_Mask_Replace(optarg, "9"));
           break;

           case 'p':
           snprintf(config->iwar_predial, sizeof(config->iwar_predial), "%s", iWar_Remove_Return(optarg));
           break;
*/
            }
}



/*
int portfd = 0; 
char tty[20]            = "/dev/ttyS0";
char baudrate[7]        = "1200";
char parity[2]          = "N";
char bits[2]            = "8";
int hwhandshake=1;
int swhandshake=0;

portfd = open(tty, O_RDWR);
if  (portfd == -1)
                {
                fprintf(stderr, "ERROR: Can't open %s.\n", tty);
                exit(1);
                }

m_savestate(portfd);    /* Save current state & setup the port */
//m_setparms(portfd,baudrate,parity,bits,hwhandshake,swhandshake);
//m_nohang(portfd);       /* Do we need to do this? */
//m_hupcl(portfd, 1);
//m_flush(portfd);        /* Flush any old data out before we start */


}
