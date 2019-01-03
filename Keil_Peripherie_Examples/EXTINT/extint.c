/*****************************************************************************
 *   extint.c:  External interrupt API C file for NXP LPC23xx/24xx 
 *   Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.07.13  ver 1.00    Prelimnary version, first Release
 *
*****************************************************************************/
#include "LPC23xx.h"                        /* LPC23xx/24xx definitions */
#include "type.h"
#include "irq.h"
#include "extint.h"

volatile DWORD eint0_counter;

/*****************************************************************************
** Function name:		EINT0_Handler
**
** Descriptions:		external INT handler
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void EINT0_Handler (void) __irq 
{
  EXTINT = EINT0;		/* clear interrupt */
		
  IENABLE;				/* handles nested interrupt */
  eint0_counter++;
  if ( eint0_counter & 0x01 )	/* alternate the LED display */
  {
	FIO2SET = 0x0000000F;	/* turn off P2.0~3 */	
	FIO2CLR = 0x000000F0;	/* turn on P2.4~7 */
  }
  else
  {
	FIO2SET = 0x000000F0;	/* turn on P2.0~3 */	
	FIO2CLR = 0x0000000F;	/* turn off P2.4~7 */
  }
  IDISABLE;
  VICVectAddr = 0;		/* Acknowledge Interrupt */
}

/*****************************************************************************
** Function name:		EINTInit
**
** Descriptions:		Initialize external interrupt pin and
**						install interrupt handler
**
** parameters:			None
** Returned value:		true or false, return false if the interrupt
**						handler can't be installed to the VIC table.
** 
*****************************************************************************/
DWORD EINTInit( void )
{
  PINSEL4 = 0x00100000;	/* set P2.10 as EINT0 and
							P2.0~7 GPIO output */
  FIO2DIR = 0x000000FF;	/* port 2, bit 0~7 only */
  FIO2CLR = 0x000000FF;	/* turn off LEDs */

  IO2_INT_EN_F = 0x200;	/* Port2.10 is falling edge. */
  EXTMODE = EINT0_EDGE;	/* INT0 edge trigger */
  EXTPOLAR = 0;			/* INT0 is falling edge by default */
     
  if ( install_irq(EINT0_INT,(void *)EINT0_Handler,HIGHEST_PRIORITY )==FALSE)
  {
	return (FALSE);
  }
  return( TRUE );
}

/******************************************************************************
**                            End Of File
******************************************************************************/

