/*****************************************************************************
 *   wdttest.c:  main C entry file for NXP LPC23xx/24xx Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.07.19  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#include "LPC23xx.h"                        /* LPC23xx/24xx definitions */
#include "type.h"
#include "target.h"
#include "irq.h"
#include "timer.h"
#include "wdt.h"

extern volatile DWORD timer0_counter;
volatile DWORD feed_counter = 0;

/*****************************************************************************
**   Main Function  main()
******************************************************************************/
int main (void)
{ 	    		
  /*** The main Function is an endless loop ****/
  init_timer( 0, TIME_INTERVAL );
    
  WDTInit();
  enable_timer( 0 );

  while( 1 ) 
  {
	if ( timer0_counter > 0 )
	{
	  /* Feed the watchdog timer */
	  feed_counter++;
	  WDTFeed();
	  timer0_counter = 0;
	}
  }
  return 0;
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
