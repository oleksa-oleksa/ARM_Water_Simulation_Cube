/*
    This module implements a linux character device driver for the XXX chip.
    Copyright (C) 2006  Embedded Artists AB (www.embeddedartists.com)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef _LCD_HW_
#define _LCD_HW_

/******************************************************************************
 * Typedefs and defines
 *****************************************************************************/
#define LCD_COMMAND  (*((volatile WORD *) 0x82000000)) 
#define LCD_DATA     (*((volatile WORD *) 0x82000002))

#define BACKLIGHT_PIN 0x10000000  //P3.28

extern void mdelay( DWORD delay );
extern void lcd_hw_init( void );
extern DWORD lcd_init(void);

#endif /* _LCD_HW_ */
