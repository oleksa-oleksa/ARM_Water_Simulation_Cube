/*******************************************************************************
    This module implements a linux character device driver for the QVGA chip.
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
*******************************************************************************/
#include "LPC23xx.h"				/* LPC23xx/24xx definitions */
#include "type.h"
#include "irq.h"
#include "target.h"
#include "lcd_hw.h"

/******************************************************************************
** Function name:		mdelay
**
** Descriptions:		
**
** parameters:			delay length
** Returned value:		None
** 
******************************************************************************/
void mdelay( DWORD delay )
{
  DWORD i;
  for( i = 0; i < delay * 40; i++);
  return;
}

/******************************************************************************
** Function name:		lcd_hw_init
**
** Descriptions:		Initialize hardware for LCD controller, it includes
**						both EMC and I/O initialization. External CS2 is used 
**						for LCD controller.		
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void lcd_hw_init(void)
{
  DWORD regVal;

  /****************************************************************
   * Initialize EMC for CS2
   ****************************************************************/
  regVal = PINSEL4;
  regVal &= 0x0FFFFFFF;
  regVal |= 0x50000000;
  PINSEL4 = regVal;
 
  regVal = PINSEL5;
  regVal &= 0xF0F0F000;
  regVal |= 0x05050555;
  PINSEL5 = regVal;

  PINSEL6 = 0x55555555;
  PINSEL8 = 0x55555555;

  regVal = PINSEL9;
  regVal &= 0x0F000000;
  regVal |= 0x50555555;
  PINSEL9 = regVal;

  // Initialize EMC for CS2
  EMC_STA_CFG2 = 0x00000081;	/* 16 bit, byte lane state, BLSn[3:0] are low. */ 
  EMC_STA_WAITWEN2 = 0x1;		/* WE delay 2(n+1)CCLK */
  EMC_STA_WAITOEN2 = 0x2;		/* OE delay, 2(n)CCLK */
  EMC_STA_WAITRD2 = 0x10;		/* RD delay, 17(n+1)CCLK */ 
  EMC_STA_WAITPAGE2 = 0x1F;		/* Page mode read delay, 32CCLK(default) */
  EMC_STA_WAITWR2 = 0x8;		/* Write delay, 10(n+2)CCLK */
  EMC_STA_WAITTURN2 = 0x5;		/* Turn arounc delay, 5(n+1)CCLK */

  EMC_STA_EXT_WAIT = 0x0;		/* Extended wait time, 16CCLK */

  /****************************************************************
   * Setup control of backlight
   ****************************************************************/
  FIO3DIR = BACKLIGHT_PIN;
#if 0
  FIO3SET = BACKLIGHT_PIN;		/* in V1.0 board, set pins high = turn light on */
#else
  FIO3CLR = BACKLIGHT_PIN;		/* in V1.1 board, set pins low = turn light on */
#endif
  return;
}

/******************************************************************************
** Function name:		lcd_init
**
** Descriptions:		Read LCD controller R49 and R50 to make sure
**						the controller has been initialized correctly,
**						then, turn on system, set display, adjust GAMMA,
**						finally, display.		
**
** parameters:			None
** Returned value:		TRUE or FALSE
** 
******************************************************************************/
DWORD lcd_init(void)
{
  DWORD result;

  /****************************************************************
   * Check if contact with Lcd controller (read register R49 & R50)
   ****************************************************************/
  /* read register R49, should be 0x10 */
  LCD_COMMAND = 0x3100;
  result = LCD_COMMAND;
  if (result != 0x10)
  {
    return( FALSE );
  }

  /* read register R50, should be 0x02 */
  LCD_COMMAND  = 0x3200;
  result = LCD_COMMAND;
  if (result != 0x02)
  {
    return( FALSE );
  }
  
  /****************************************************************
   * Initialize Lcd controller (long sequence) 
   ****************************************************************/
  /* system power on */
  LCD_COMMAND = 0x0301;
  mdelay(20);
  LCD_COMMAND = 0x0111;
  mdelay(10);
  LCD_COMMAND = 0x0301;
  mdelay(20);
  LCD_COMMAND = 0x0028;
  mdelay(10);
  LCD_COMMAND = 0x2201;
  mdelay(10);
  LCD_COMMAND = 0x0020;
  mdelay(10);

  /* set Display Window */
  LCD_COMMAND = 0x0110;
  LCD_COMMAND = 0x0500;
  LCD_COMMAND = 0x4200;
  LCD_COMMAND = 0x4300;
  LCD_COMMAND = 0x4400;
  LCD_COMMAND = 0x4500;
  mdelay(10);
  LCD_COMMAND = 0x46EF;
  LCD_COMMAND = 0x4700;
  LCD_COMMAND = 0x4800;
  LCD_COMMAND = 0x4901;
  LCD_COMMAND = 0x4A3F;
  LCD_COMMAND = 0x0200;
  LCD_COMMAND = 0x0D00;
  LCD_COMMAND = 0x0E00;
  LCD_COMMAND = 0x0F00;

  LCD_COMMAND = 0x1000;
  LCD_COMMAND = 0x1100;
  LCD_COMMAND = 0x1200;
  LCD_COMMAND = 0x1300;
  LCD_COMMAND = 0x1400;
  LCD_COMMAND = 0x1500;
  LCD_COMMAND = 0x1600;
  LCD_COMMAND = 0x1700;
  LCD_COMMAND = 0x1D08;
  mdelay(10);
  LCD_COMMAND = 0x2300;
  mdelay(10);
  LCD_COMMAND = 0x2D01;
  LCD_COMMAND = 0x3301;
  LCD_COMMAND = 0x3401;
  LCD_COMMAND = 0x3500;
  LCD_COMMAND = 0x3701;
  mdelay(10);
  LCD_COMMAND = 0x3E01;
  LCD_COMMAND = 0x3F3F;
  LCD_COMMAND = 0x4008;
  LCD_COMMAND = 0x410A;
  LCD_COMMAND = 0x4C00;
  LCD_COMMAND = 0x4D01;
  LCD_COMMAND = 0x4E3F;
  LCD_COMMAND = 0x4F00;
  LCD_COMMAND = 0x5000;
  LCD_COMMAND = 0x7600;
  LCD_COMMAND = 0x8600;
  LCD_COMMAND = 0x8736;
  LCD_COMMAND = 0x8806;
  LCD_COMMAND = 0x8904;
  LCD_COMMAND = 0x8B3F;
  LCD_COMMAND = 0x8D01;

  /* adjust GAMMA */
  LCD_COMMAND = 0x8F00;
  LCD_COMMAND = 0x9022;
  LCD_COMMAND = 0x9167;
  LCD_COMMAND = 0x9240;
  LCD_COMMAND = 0x9307;
  LCD_COMMAND = 0x9412;
  LCD_COMMAND = 0x9522;
  LCD_COMMAND = 0x9600;
  LCD_COMMAND = 0x9707;
  LCD_COMMAND = 0x9873;
  LCD_COMMAND = 0x9901;
  LCD_COMMAND = 0x9A21;
  LCD_COMMAND = 0x9B24;
  LCD_COMMAND = 0x9C42;
  LCD_COMMAND = 0x9D01;
  mdelay(100);
  LCD_COMMAND = 0x2494;
  mdelay(10);
  LCD_COMMAND = 0x256F;
  mdelay(110);

  /* power on sequence, step 2 */
  LCD_COMMAND = 0x2812;
  mdelay(10);
  LCD_COMMAND = 0x1900;
  mdelay(10);
  LCD_COMMAND = 0x2110;
  mdelay(10);
  LCD_COMMAND = 0x1e00;
  mdelay(50);
  LCD_COMMAND = 0x18f7;
  mdelay(100);
  LCD_COMMAND = 0x2100;
  mdelay(10);
  LCD_COMMAND = 0x2812;
  mdelay(10);
  LCD_COMMAND = 0x1a00;
  mdelay(10);
  LCD_COMMAND = 0x197c;
  mdelay(10);
  LCD_COMMAND = 0x1f51;
  LCD_COMMAND = 0x2060;

  mdelay(10);
  LCD_COMMAND = 0x1e80;
  mdelay(10);
  LCD_COMMAND = 0x1b0b;
  mdelay(10);

  /* start display */
  LCD_COMMAND = 0x0020;
  mdelay(10);
  LCD_COMMAND = 0x3b01;
  return( TRUE );
}

/******************************************************************************
**                            End Of File
******************************************************************************/

