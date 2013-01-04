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
 * iwar-config.c = Loads configuration file
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "version.h"
#include "iwar2-defs.h"
#include "iwar2.h"

struct _iWarConfig *config;
struct _iWarAlertConfig *alertconfig;
struct _iWarVar *var;
struct _iWarCounters *counters;
struct _iWarS *serial; 


void iWar_Load_Config () { 

FILE *iwar_cfg;
char tmpbuf[1024] = { 0 };

char *iwar_option = NULL;
char *iwar_var1 = NULL; 
char *iwar_var2 = NULL;
char *tok;

char s_tmp[256];
char tmp[256];

int i=0;

counters->var_count=0; 

/* Some defaults */

snprintf(config->iwar_fifo, sizeof(config->iwar_fifo), "%s", IWAR_FIFO);
snprintf(config->iwar_alert_config, sizeof(config->iwar_alert_config), "%s", IWAR_ALERT_CONFIG);


if ((iwar_cfg = fopen(config->iwar_config_file, "r")) == NULL) 
    iWar_Log(1, "[%s, line %d] Cannot open configuration file (%s)", __FILE__,  __LINE__, config->iwar_config_file);


while(fgets(tmpbuf, sizeof(tmpbuf), iwar_cfg) != NULL) {
     if (tmpbuf[0] == '#') continue;
     if (tmpbuf[0] == ';') continue;
     if (tmpbuf[0] == 10 ) continue;
     if (tmpbuf[0] == 32 ) continue;

     iwar_option = strtok_r(iWar_Remove_Return(tmpbuf), " ", &tok);

     /* Load 'var' values */

     if (!strcmp(iwar_option, "var")) {
         iwar_var1 = strtok_r(NULL, " ", &tok);	/* Store the name of the var */
	 var = (_iWarVar *) realloc(var, (counters->var_count+1) * sizeof(_iWarVar));	/* Allocate memory */
	 snprintf(var[counters->var_count].var_name, sizeof(var[counters->var_count].var_name), "$%s", iwar_var1);
	 iwar_var2 = strtok_r(NULL, " ", &tok); /* Move to position of value of var */
	 snprintf(var[counters->var_count].var_value, sizeof(var[counters->var_count].var_value), "%s", iwar_var2);
	 counters->var_count++; 

	 if (!strcmp(iwar_var1, "FIFO")) snprintf(config->iwar_fifo, sizeof(config->iwar_fifo), "%s", iwar_var2);
	 if (!strcmp(iwar_var1, "ALERT_CONFIG")) snprintf(config->iwar_alert_config, sizeof(config->iwar_alert_config), "%s", iwar_var2);
	 }

     /* Load sequential options */

     if (!strcmp(iwar_option, "S:")) { 
        config->serial_flag=1; 
        iwar_var1 = strtok_r(NULL, ":", &tok);  /* Store the name of the var */
	snprintf(s_tmp, sizeof(s_tmp), "%s", iWar_Between_Quotes(iwar_var1)); 
	strlcpy(tmp, iWar_Var_To_Value(s_tmp), sizeof(tmp));
	serial = (_iWarS *) realloc(var, (counters->serial_count+1) * sizeof(_iWarS));   /* Allocate memory */
	snprintf(serial[counters->serial_count].command, sizeof(serial[counters->serial_count].command), "%s", tmp);
	counters->serial_count++;
	}
  }
fclose(iwar_cfg);

/* Load the alert file */

if ((iwar_cfg = fopen(config->iwar_alert_config, "r")) == NULL)
    iWar_Log(1, "[%s, line %d] Cannot open alert configuration file (%s)", __FILE__,  __LINE__, config->iwar_alert_config);

while(fgets(tmpbuf, sizeof(tmpbuf), iwar_cfg) != NULL) {

     if (tmpbuf[0] == '#') continue;
     if (tmpbuf[0] == ';') continue;
     if (tmpbuf[0] == 10 ) continue;
     if (tmpbuf[0] == 32 ) continue;

     alertconfig = (_iWarAlertConfig *) realloc(alertconfig, (counters->alert_config_count+1) * sizeof(_iWarAlertConfig));   /* Allocate memory */
     snprintf(alertconfig[counters->alert_config_count].alert, sizeof(alertconfig[counters->alert_config_count].alert), "%s", iWar_Remove_Return(tmpbuf));
     counters->alert_config_count++; 
}


} /* end of iWar_Load_Config */

