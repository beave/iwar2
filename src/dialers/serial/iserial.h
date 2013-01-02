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

/* iserial.h
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/* Serial prototypes for iWar */

extern void m_savestate( int);
extern void m_restorestate( int);
extern void m_setparms( int, char *, char *, char *, int, int);
extern void m_nohang( int);
extern void m_hupcl( int, int);
extern void m_flush( int);
extern void m_break( int); 
extern void m_dtrtoggle( int, int ); 
