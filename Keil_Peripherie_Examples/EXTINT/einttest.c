/*****************************************************************************
 *   einttest.c:  main C entry file for NXP LPC23xx/24xx Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.07.13  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#include "LPC23xx.h"				/* LPC23xx/24xx definitions */
#include "type.h"
#include "irq.h"
#include "extint.h"

/*****************************************************************************
**   Main Function  main()
******************************************************************************/
int main (void)
{	    		
  /* initialize GPIO pins as external interrupts */        
  EINTInit();

  /****** It's an endless loop waiting for external interrupt ******/    
  /* On Keil MCB2300 board, EINT0 can be used to test the external interrupt */   
  while( 1 );
  return 0;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
