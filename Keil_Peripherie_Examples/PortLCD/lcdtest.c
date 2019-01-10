/*****************************************************************************
 *   lcdtest.c:  main C entry file for NXP LPC23xx/24xx Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.07.13  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#include "LPC23xx.h"                        /* LPC23xx/24xx definitions */
#include "type.h"
#include "irq.h"
#include "portlcd.h"

BYTE lcd_text[2][16+1] = {"   NXP SEMI.    ",      /* Buffer for LCD text      */
                          "  LPC23/24xx    " };

/*****************************************************************************
**   Main Function  main()
******************************************************************************/
int main (void)
{
#if KEIL_BOARD_LPC23XX
  /* LCD Module.2x16 init */
  LCD_init();
  LCD_cur_off();

  /* Update LCD Module display text. */
  LCD_cls();
  LCD_puts( lcd_text[0] );
  LCD_gotoxy (1,2);
  LCD_puts( lcd_text[1] );
#endif		
  return 0;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
