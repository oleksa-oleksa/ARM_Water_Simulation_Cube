/*****************************************************************************
 *   pwmtest.c:  main C entry file for NXP LPC23xx/24xx Family Microprocessors
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
#include "pwm.h"

extern volatile DWORD match_counter1;

/******************************************************************************
**   Main Function  main()
******************************************************************************/
int main (void)
{	    
  DWORD cycle = PWM_CYCLE, offset = 0;

  /* The test is done on the Keil MCB2300 board, that all the PWM1 pins are
  laid out to the connector for probing. PWM0 is not not tested. */ 
  if ( PWM_Init( 1, 0 ) != TRUE )
  {
	while( 1 );			/* fatal error */
  }

  PWM_Set( 1, cycle, offset );
  PWM_Start( 1 );

  while ( 1 )
  {
	if ( match_counter1 != 0 )
	{
	  match_counter1 = 0;
	  if( offset <= PWM_CYCLE )
		offset += PWM_OFFSET;
	  else
		offset = 0;
	  PWM_Set( 1, cycle, offset );
	}
  }
  PWM_Stop( 1 );
  return 0;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
