/*****************************************************************************
 *   dac.c:  ADC module file for NXP LPC23xx Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.08.20  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#include "LPC23xx.h"                        /* LPC23xx definitions */
#include "type.h"
#include "dac.h"

/*****************************************************************************
** Function name:		DACInit
**
** Descriptions:		initialize DAC channel
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void DACInit( void )
{
  /* setup the related pin to DAC output */
  PINSEL1 = 0x00200000;	/* set p0.26 to DAC output */   
  return;
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
