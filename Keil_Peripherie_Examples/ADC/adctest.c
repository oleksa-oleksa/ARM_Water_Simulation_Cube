/*****************************************************************************
 *   adctest.c:  main C entry file for NXP LPC23xx Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.08.16  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#include "LPC23xx.h"                        /* LPC23xx definitions */
#include "type.h"
#include "target.h"
#include "irq.h"
#include "adc.h"

extern volatile DWORD ADC0Value[ADC_NUM];
extern volatile DWORD ADC0IntDone;

/*****************************************************************************
**   Main Function  main()
******************************************************************************/
int main (void)
{
  DWORD i;

  /* Initialize ADC  */
  ADCInit( ADC_CLK );

#if ADC_INTERRUPT_FLAG			/* Interrupt driven */
  for ( i = 0; i < ADC_NUM; i++ )
  {
	ADC0Read( i );
	while ( !ADC0IntDone );
	ADC0IntDone = 0;
  }
#else  					/* Polling */
  for ( i = 0; i < ADC_NUM; i++ )
  {
	ADC0Value[i] = ADC0Read( i );
  }
#endif
  return 0;
}

/*****************************************************************************
**                            End Of File
*****************************************************************************/

