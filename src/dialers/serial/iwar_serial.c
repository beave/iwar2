#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include "sysdep.h"

int main() { 

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
m_setparms(portfd,baudrate,parity,bits,hwhandshake,swhandshake);
m_nohang(portfd);       /* Do we need to do this? */
m_hupcl(portfd, 1);
m_flush(portfd);        /* Flush any old data out before we start */


}
