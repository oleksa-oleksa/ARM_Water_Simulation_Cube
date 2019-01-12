/*****************************************************************************
 *   pmtest.c:  main C entry file for NXP LPC23xx/24xx Family Microprocessors
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
#include "timer.h"
#include "pwrman.h"


extern volatile DWORD timer0_counter;

/******************************************************************************
**   Main Function  main()
******************************************************************************/
int main (void)
{
  DWORD counter = 0;

  /* The power management routine is tested on the Keil MCB23xx board */
  /* It has be to tested when the code is running in the flash. In the
  debugging mode, the debugger link may be broken when the MCU is in the
  power down mode. The link can't be reestablished when EINT0 button is
  pressed. */
  FIO2DIR = 0x000000FF;		/* P2.0..7 defined as Outputs */
  FIO2CLR = 0x000000FF;		/* turn off all the LEDs */
    
  init_timer( 0, TIME_INTERVAL );
  enable_timer( 0 );
	
  /* There are two tests conducted here, one is using EINT0 push button
  to test the wakeup; the other is using GPIO0.8 and GPIO2.8 to test the
  wakeup. The easiest way to test GPIO wakeup is to ground either pin
  momentarily, then, the current consumption of the board should go
  up significantly once it is waken up. */    
  if ( PM_Init( ) != TRUE )
  {
	while( 1 );			/* fatal error */
  }

  /* After setting up the wake up source, go to power down mode, 
  when the push button on EXTINT0 is pressed or p0.8 or p2.8 is grounded, 
  the system will wake up, Reset_MainClock() will be the first routine to 
  be executed, the PLL will be reconfigured, finally, LEDs are blinking 
  happily. */
  PCON = 0x2;	

  /* CPU is in power down mode here until a external wakeup source is 
  kicked in. Once waken up, the first thing is to reconfigure the PLL. */
  ConfigurePLL();

  counter = 0;
  timer0_counter = 0;
  while ( 1 )
  {
	if ( timer0_counter >= (0x20 * counter) )
	{
	  FIO2SET = 1 << counter;
	  counter++;
	  if ( counter > 8 )
	  {
		counter = 0;	
		timer0_counter = 0;
		FIO2CLR = 0x000000FF;
	  }
	}
  }
  return 0;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
