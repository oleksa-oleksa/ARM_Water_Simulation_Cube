/*****************************************************************************
 *   adc.c:  ADC module file for NXP LPC23xx Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.08.15  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#include "LPC23xx.h"                        /* LPC23xx definitions */
#include "type.h"
#include "irq.h"
#include "target.h"
#include "adc.h"

volatile DWORD ADC0Value[ADC_NUM];
volatile DWORD ADC0IntDone = 0;

#if ADC_INTERRUPT_FLAG
/******************************************************************************
** Function name:		ADC0Handler
**
** Descriptions:		ADC0 interrupt handler
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void ADC0Handler (void) __irq 
{
  DWORD regVal;
  
  regVal = AD0STAT;		/* Read ADC will clear the interrupt */
  if ( regVal & 0x0000FF00 )	/* check OVERRUN error first */
  {
	regVal = (regVal & 0x0000FF00) >> 0x08;
	/* if overrun, just read ADDR to clear */
	/* regVal variable has been reused. */
	switch ( regVal )
	{
	  case 0x01:
		regVal = AD0DR0;
	  break;
	  case 0x02:
		regVal = AD0DR1;
	  break;
	  case 0x04:
		regVal = AD0DR2;
	  break;
	  case 0x08:
		regVal = AD0DR3;
	  break;
	  case 0x10:
		regVal = AD0DR4;
	  break;
	  case 0x20:
		regVal = AD0DR5;
	  break;
	  case 0x40:
		regVal = AD0DR6;
	  break;
	  case 0x80:
		regVal = AD0DR7;
	  break;
	  default:
	  break;
	}
  }
    
  if ( regVal & ADC_ADINT )
  {
	switch ( regVal & 0xFF )	/* check DONE bit */
	{
	  case 0x01:
		ADC0Value[0] = ( AD0DR0 >> 6 ) & 0x3FF;
	  break;
	  case 0x02:
		ADC0Value[1] = ( AD0DR1 >> 6 ) & 0x3FF;
	  break;
	  case 0x04:
		ADC0Value[2] = ( AD0DR2 >> 6 ) & 0x3FF;
	  break;
	  case 0x08:
		ADC0Value[3] = ( AD0DR3 >> 6 ) & 0x3FF;
	  break;
	  case 0x10:
		ADC0Value[4] = ( AD0DR4 >> 6 ) & 0x3FF;
	  break;
	  case 0x20:
		ADC0Value[5] = ( AD0DR5 >> 6 ) & 0x3FF;
	  break;
	  case 0x40:
		ADC0Value[6] = ( AD0DR6 >> 6 ) & 0x3FF;
	  break;
	  case 0x80:
		ADC0Value[7] = ( AD0DR7 >> 6 ) & 0x3FF;
	  break;		
	  default:
	  break;
	}
  }
  AD0CR &= 0xF8FFFFFF;	/* stop ADC now */ 
  ADC0IntDone = 1;
  VICVectAddr = 0;		/* Acknowledge Interrupt */
}
#endif

/*****************************************************************************
** Function name:		ADCInit
**
** Descriptions:		initialize ADC channel
**
** parameters:			ADC clock rate
** Returned value:		true or false
** 
*****************************************************************************/
DWORD ADCInit( DWORD ADC_Clk )
{
  /* Enable CLOCK into ADC controller */
  PCONP |= (1 << 12);

  /* all the related pins are set to ADC inputs, AD0.0~7 */
  PINSEL0 |= 0x0F000000;	/* P0.12~13, A0.6~7, function 11 */	
  PINSEL1 &= ~0x003FC000;	/* P0.23~26, A0.0~3, function 01 */
  PINSEL1 |= 0x00154000;
  PINSEL3 |= 0xF0000000;	/* P1.30~31, A0.4~5, function 11 */

  AD0CR = ( 0x01 << 0 ) | 	/* SEL=1,select channel 0~7 on ADC0 */
		( ( Fpclk / ADC_Clk - 1 ) << 8 ) |  /* CLKDIV = Fpclk / 1000000 - 1 */ 
		( 0 << 16 ) | 		/* BURST = 0, no BURST, software controlled */
		( 0 << 17 ) |  		/* CLKS = 0, 11 clocks/10 bits */
		( 1 << 21 ) |  		/* PDN = 1, normal operation */
		( 0 << 22 ) |  		/* TEST1:0 = 00 */
		( 0 << 24 ) |  		/* START = 0 A/D conversion stops */
		( 0 << 27 );		/* EDGE = 0 (CAP/MAT singal falling,trigger A/D conversion) */ 

  /* If POLLING, no need to do the following */
#if ADC_INTERRUPT_FLAG
  AD0INTEN = 0x1FF;		/* Enable all interrupts */
  if ( install_irq( ADC0_INT, (void *)ADC0Handler, HIGHEST_PRIORITY ) == FALSE )
  {
	return (FALSE);
  }
#endif
  return (TRUE);
}

/*****************************************************************************
** Function name:		ADC0Read
**
** Descriptions:		Read ADC0 channel
**
** parameters:			Channel number
** Returned value:		Value read, if interrupt driven, return channel #
** 
*****************************************************************************/
DWORD ADC0Read( BYTE channelNum )
{
#if !ADC_INTERRUPT_FLAG
  DWORD regVal, ADC_Data;
#endif

  /* channel number is 0 through 7 */
  if ( channelNum >= ADC_NUM )
  {
	channelNum = 0;		/* reset channel number to 0 */
  }
  AD0CR &= 0xFFFFFF00;
  AD0CR |= (1 << 24) | (1 << channelNum);	
				/* switch channel,start A/D convert */
#if !ADC_INTERRUPT_FLAG
  while ( 1 )			/* wait until end of A/D convert */
  {
	regVal = *(volatile unsigned long *)(AD0_BASE_ADDR 
			+ ADC_OFFSET + ADC_INDEX * channelNum);
	/* read result of A/D conversion */
	if ( regVal & ADC_DONE )
	{
	  break;
	}
  }	
        
  AD0CR &= 0xF8FFFFFF;	/* stop ADC now */    
  if ( regVal & ADC_OVERRUN )	/* save data when it's not overrun, otherwise, return zero */
  {
	return ( 0 );
  }
  ADC_Data = ( regVal >> 6 ) & 0x3FF;
  return ( ADC_Data );	/* return A/D conversion value */
#else
  return ( channelNum );	/* if it's interrupt driven, the ADC reading is 
							done inside the handler. so, return channel number */
#endif
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
