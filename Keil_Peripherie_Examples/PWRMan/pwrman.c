/*****************************************************************************
 *   pwrman.c:  Power Management module file for NXP LPC23xx/24xx Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.07.20  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#include "LPC23xx.h"                        /* LPC23xx/24xx definitions */
#include "type.h"
#include "irq.h"
#include "target.h"
#include "pwrman.h"

volatile DWORD eint0_counter;
volatile DWORD eint3_counter;
/*****************************************************************************
** Function name:	EINT0_Handler
**
** Descriptions:	external INT0 handler
**
** parameters:		None
** Returned value:	None
** 
*****************************************************************************/
void EINT0_Handler (void) __irq 
{
  EXTINT = EINT0;		/* clear interrupt */
		
  IENABLE;			/* handles nested interrupt */
  eint0_counter++;
  IDISABLE;
  VICVectAddr = 0;		/* Acknowledge Interrupt */
  return;
}

/*****************************************************************************
** Function name:	EINT3_Handler
**
** Descriptions:	external INT3 handler, shared with GPIO0 and GPIO2 port
**					pins as interrupt source.
**
** parameters:		None
** Returned value:	None
** 
*****************************************************************************/
void EINT3_Handler (void) __irq 
{
  EXTINT = EINT3;		/* clear interrupt */
		
  IENABLE;			/* handles nested interrupt */
  eint3_counter++;
  if ( IO0_INT_STAT_F & (1 << 8))
  {
	IO0_INT_CLR |= (1 << 8); 
  }
  if ( IO2_INT_STAT_F & (1 << 8))
  {
	IO2_INT_CLR |= (1 << 8);
  }
  IDISABLE;
  VICVectAddr = 0;		/* Acknowledge Interrupt */
  return;
}

/*****************************************************************************
** Function name:	EINT0Init
**
** Descriptions:	Initialize external interrupt pin and
**					install interrupt handler
**
** parameters:		None
** Returned value:	true or false, return false if the interrupt
**					handler can't be installed to the VIC table.
** 
*****************************************************************************/
DWORD EINT0Init( void )
{
  PINSEL4 |= 0x00100000;	/* set P2.10 as EINT0 */
							
  IO2_INT_EN_F = 0x200;	/* Port2.10 is falling edge. */
  EXTMODE = EINT0_EDGE;	/* INT0 edge trigger */
  EXTPOLAR = 0;			/* INT0 is falling edge by default */
     
  if ( install_irq( EINT0_INT, (void *)EINT0_Handler, HIGHEST_PRIORITY ) == FALSE )
  {
	return (FALSE);
  }      
  return( TRUE );
}

/*****************************************************************************
** Function name:	EINT3Init
**
** Descriptions:	Initialize GPIO0 and GPIO2 pins as 
**					interrupt and used as GPIO wakeup source.
**
** parameters:		None
** Returned value:	true or false, return false if the interrupt
**					handler can't be installed to the VIC table.
** 
*****************************************************************************/
DWORD EINT3Init( void )
{
  IO2_INT_EN_F = 1 << 8;	/* Port2.8 is falling edge using EINT3. */
  IO0_INT_EN_F = 1 << 8;	/* Port0.8 is falling edge using EINT3. */
     
  if ( install_irq( EINT3_INT, (void *)EINT3_Handler, HIGHEST_PRIORITY ) == FALSE )
  {
	return (FALSE);
  }
  return( TRUE );
}

/******************************************************************************
** Function name:		PM_Init
**
** Descriptions:		Set up wakeup source before powering down
**						Use EXTINT0 as wakeup source		
**
** parameters:			
** Returned value:		true or fase, if VIC table is full, return false
** 
******************************************************************************/
DWORD PM_Init( void )
{
  FIO2DIR = 0x000000FF;	/* port 2, bit 0~7 only, GPIO output */
  FIO2CLR = 0x000000FF;	/* turn off LEDs */

  if ( EINT0Init() != TRUE )	/* Enable external interrupt 0 */
  {
	return ( FALSE );
  }
  if ( EINT3Init() != TRUE )	/* Enable external interrupt 0 */
  {
	return ( FALSE );
  }

  /* Set EXTINT0 and GPIO0 and GPIO2 as wakeup source */
  /* Note: there is a documentation error, that INTWAKE bit 7
  GPIOWAKE should be GPIO0WAKE, bit 8 should be GPIO2WAKE. */
  INTWAKE |= ((0x01<<0)|(0x01<<7)|(0x01<<8));	
  return ( TRUE );    
}

/******************************************************************************
**                            End Of File
******************************************************************************/
