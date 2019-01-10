/*****************************************************************************
 *   pwm.c:  PWM module file for NXP LPC23xx/24xx Family Microprocessors
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
#include "timer.h"
#include "pwm.h"

volatile DWORD match_counter0, match_counter1;

/******************************************************************************
** Function name:		PWM0Handler
**
** Descriptions:		PWM0 interrupt handler
**						For now, it only deals with PWM1 match 0
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void PWMHandler (void) __irq 
{
  DWORD regVal;

  IENABLE;				/* handles nested interrupt */
  regVal = PWM0IR;
  if ( regVal & MR0_INT )
  {
	match_counter0++;	
  }
  PWM0IR |= regVal;		/* clear interrupt flag on match 0 */

  regVal = PWM1IR;
  if ( regVal & MR0_INT )
  {
	match_counter1++;	
  }
  PWM1IR |= regVal;		/* clear interrupt flag on match 0 */
  IDISABLE;
  VICVectAddr = 0;		/* Acknowledge Interrupt */
  return;
}

/******************************************************************************
** Function name:		PWM_Init
**
** Descriptions:		PWM initialization, setup all GPIOs to PWM0~6,
**				reset counter, all latches are enabled, interrupt
**				on PWMMR0, install PWM interrupt to the VIC table.
**
** parameters:			ChannelNum, Duty cycle
** Returned value:		true or fase, if VIC table is full, return false
** 
******************************************************************************/
DWORD PWM_Init( DWORD ChannelNum, DWORD cycle )
{
  if ( ChannelNum == 0 )
  {
	match_counter0 = 0;
	PINSEL7 = 0x00002AAA;	/* set GPIOs for all PWM pins on PWM0 */

	PWM0TCR = TCR_RESET;	/* Counter Reset */ 
		
	PWM0PR = 0x00;			/* count frequency:Fpclk */
	PWM0MCR = PWMMR0I | PWMMR0R;	/* interrupt on PWMMR0, reset on PWMMR0, reset 
									TC if PWM0 matches */				
	PWM0MR0 = cycle;		/* set PWM cycle */
	PWM0MR1 = cycle * 5/6;
	PWM0MR2 = cycle * 2/3;
	PWM0MR3 = cycle * 1/2;
	PWM0MR4 = cycle * 1/3;
	PWM0MR5 = cycle * 1/6;	

    /* all PWM latch enabled */
    PWM0LER = LER0_EN | LER1_EN | LER2_EN | LER3_EN | LER4_EN | LER5_EN | LER6_EN;
  }
  else if ( ChannelNum == 1 )
  {
	match_counter1 = 0;
	PINSEL4 = 0x00001555;	/* set GPIOs for all PWM pins on PWM0 */

	PWM1TCR = TCR_RESET;	/* Counter Reset */ 
		
	PWM1PR = 0x00;			/* count frequency:Fpclk */
	PWM1MCR = PWMMR0I | PWMMR0R;	/* interrupt on PWMMR0, reset on PWMMR0, reset 
									TC if PWM0 matches */				
	PWM1MR0 = cycle;		/* set PWM cycle */
	PWM1MR1 = cycle * 5/6;
	PWM1MR2 = cycle * 2/3;
	PWM1MR3 = cycle * 1/2;
	PWM1MR4 = cycle * 1/3;
	PWM1MR5 = cycle * 1/6;	

	/* all PWM latch enabled */
	PWM1LER = LER0_EN | LER1_EN | LER2_EN | LER3_EN | LER4_EN | LER5_EN | LER6_EN;
  }
  else
  {
	return ( FALSE );		/* Unknown channel number */
  }
	
  if ( install_irq( PWM0_1_INT, (void *)PWMHandler, HIGHEST_PRIORITY ) == FALSE )
  {
	return ( FALSE );
  }
  return (TRUE);
}

/******************************************************************************
** Function name:		PWM_Set
**
** Descriptions:		PWM cycle setup
**
** parameters:			Channel number, PWM cycle, and offset
** Returned value:		None
** 
******************************************************************************/
void PWM_Set( DWORD ChannelNum, DWORD cycle, DWORD offset )
{			
  if ( ChannelNum == 0 )
  {
	PWM0MR0 = cycle;		/* set PWM cycle */
	PWM0MR1 = cycle * 5/6 + offset;
	PWM0MR2 = cycle * 2/3 + offset;
	PWM0MR3 = cycle * 1/2 + offset;
	PWM0MR4 = cycle * 1/3 + offset;
	PWM0MR5 = cycle * 1/6 + offset;

	/* The LER will be cleared when the Match 0 takes place, in order to
	load and execute the new value of match registers, all the PWMLERs need to
	reloaded. all PWM latch enabled */
	PWM0LER = LER0_EN | LER1_EN | LER2_EN | LER3_EN | LER4_EN | LER5_EN | LER6_EN;
  }
  else if ( ChannelNum == 1 )
  {
	PWM1MR0 = cycle;		/* set PWM cycle */
	PWM1MR1 = cycle * 5/6 + offset;
	PWM1MR2 = cycle * 2/3 + offset;
	PWM1MR3 = cycle * 1/2 + offset;
	PWM1MR4 = cycle * 1/3 + offset;
	PWM1MR5 = cycle * 1/6 + offset;

	/* The LER will be cleared when the Match 0 takes place, in order to
	load and execute the new value of match registers, all the PWMLERs need to
	reloaded. all PWM latch enabled */
	PWM1LER = LER0_EN | LER1_EN | LER2_EN | LER3_EN | LER4_EN | LER5_EN | LER6_EN;
  }
  return;
}

/******************************************************************************
** Function name:		PWM_Start
**
** Descriptions:		Enable PWM by setting the PCR, PTCR registers
**
** parameters:			channel number
** Returned value:		None
** 
******************************************************************************/
void PWM_Start( DWORD channelNum )
{
  if ( channelNum == 0 )
  {
	/* All single edge, all enable */
	PWM0PCR = PWMENA1 | PWMENA2 | PWMENA3 | PWMENA4 | PWMENA5 | PWMENA6;
	PWM0TCR = TCR_CNT_EN | TCR_PWM_EN;	/* counter enable, PWM enable */
  }
  else if ( channelNum == 1 )
  {
	/* All single edge, all enable */
	PWM1PCR = PWMENA1 | PWMENA2 | PWMENA3 | PWMENA4 | PWMENA5 | PWMENA6;
	PWM1TCR = TCR_CNT_EN | TCR_PWM_EN;	/* counter enable, PWM enable */
  }
  return;
}

/******************************************************************************
** Function name:		PWM_Stop
**
** Descriptions:		Stop all PWM channels
**
** parameters:			channel number
** Returned value:		None
** 
******************************************************************************/
void PWM_Stop( DWORD channelNum )
{
  if ( channelNum == 0 )
  {
	PWM0PCR = 0;
	PWM0TCR = 0x00;		/* Stop all PWMs */
  }
  else if ( channelNum == 1 )
  {
	PWM1PCR = 0;
	PWM1TCR = 0x00;		/* Stop all PWMs */
  }
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
