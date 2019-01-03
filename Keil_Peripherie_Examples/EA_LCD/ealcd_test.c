/*****************************************************************************
 *   ealcd_test.c:  Test Embedded Artists QVGA LCD controller main C entry 
 *   file for NXP LPC23xx/24xx Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.07.14  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#include "LPC23xx.h"				/* LPC23xx/24xx definitions */
#include "type.h"
#include "irq.h"
#include "target.h"
#include "lcd_hw.h"
#include "lcd_grph.h"
#include "font5x7.h"

/*****************************************************************************
**   Main Function  main()
******************************************************************************/
int main (void)
{
#if EA_BOARD_LPC24XX
  lcd_hw_init();
  if ( lcd_init() != TRUE )
  {
    while( 1 );		/* Display fatal error */
  }

  while ( 1 )
  {
    lcd_fillScreen(RED);
    mdelay( 300000 );
    lcd_putString(0, 0, (BYTE *)"1234567891011121314151617181920\n");
    mdelay( 300000 );
	
    lcd_fillScreen(GREEN);
    mdelay( 300000 );
    lcd_putString(0, 40, (BYTE *)"1234567891011121314151617181920\n");
    mdelay( 300000 );
	
    lcd_fillScreen(YELLOW);
    mdelay( 300000 );
    lcd_putString(0, 80, (BYTE *)"1234567891011121314151617181920\n");
    mdelay( 300000 );
	
    lcd_fillScreen(BLUE);
    mdelay( 300000 );
    lcd_putString(0, 120, (BYTE *)"1234567891011121314151617181920\n");
    mdelay( 300000 );
  }
#endif
  return 0;
}

/*****************************************************************************
**                            End Of File
*****************************************************************************/

