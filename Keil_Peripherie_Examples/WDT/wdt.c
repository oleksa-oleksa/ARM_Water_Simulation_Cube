/*****************************************************************************
 *   wdt.c:  Watchdog C file for NXP LPC23xx/24xx Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.07.20  ver 1.00    Prelimnary version, first Release
 *
*****************************************************************************/
#include "LPC23xx.h"                        /* LPC23xx/24xx definitions */
#include "type.h"
#include "irq.h"
#include "timer.h"
#include "wdt.h"

volatile DWORD wdt_counter;

/*****************************************************************************
** Function name:		WDTHandler
**
** Descriptions:		Watchdog timer interrupt handler
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void WDTHandler(void) __irq 
{
  WDMOD &= ~WDTOF;		/* clear the time-out terrupt flag */		  
  IENABLE;			/* handles nested interrupt */
  wdt_counter++;
  IDISABLE;
  VICVectAddr = 0;		/* Acknowledge Interrupt */
}

/*****************************************************************************
** Function name:		WDTInit
**
** Descriptions:		Initialize watchdog timer, install the
**				watchdog timer interrupt handler
**
** parameters:			None
** Returned value:		true or false, return false if the VIC table
**				is full and WDT interrupt handler can be
**				installed.
** 
*****************************************************************************/
DWORD WDTInit( void )
{
  wdt_counter = 0;
  if ( install_irq( WDT_INT, (void *)WDTHandler, HIGHEST_PRIORITY ) == FALSE )
  {
	return (FALSE);
  }

  WDTC = WDT_FEED_VALUE;	/* once WDEN is set, the WDT will start after feeding */
//  WDMOD = WDEN | WDRESET;
  WDMOD = WDEN;

  WDFEED = 0xAA;		/* Feeding sequence */
  WDFEED = 0x55;    
  return( TRUE );
}

/*****************************************************************************
** Function name:		WDTFeed
**
** Descriptions:		Feed watchdog timer to prevent it from timeout
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void WDTFeed( void )
{
  WDFEED = 0xAA;		/* Feeding sequence */
  WDFEED = 0x55;
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
