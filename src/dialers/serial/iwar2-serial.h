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

/* iwar_serial.h
 */

#define	DEFAULT_PORT "/dev/ttyUSB0"
#define DEFAULT_PARITY "N"
#define DEFAULT_BAUD "1200"
#define DEFAULT_CONFIG "/usr/local/etc/iwar_serial.conf"
#define DEFAULT_FIFO "/var/run/iwar.fifo"
#define DEFAULT_TIMEOUT 30

typedef struct _iWarSerialConfig _iWarSerialConfig;
struct _iWarSerialConfig {

    char	 port[64]; 
    char	 parity[2];
    char	 baud[8];  
    char	 databits[2];
    char	 config[MAX_PATH];
    char	 fifo[MAX_PATH];
    int	 	 hwhandshake;
    int		 swhandshake;
    int		 portfd;
    int		 modem_timeout;


    char	 dial[256];

};

void iWar_Serial_Usage( void );
void iWar_Send_Modem(int, const char *);
