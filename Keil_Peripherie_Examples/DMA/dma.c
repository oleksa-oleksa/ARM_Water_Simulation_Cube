/*****************************************************************************
 *   dma.c:  DMA module file for NXP LPC23xx/24xx Family Microprocessors
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
#include "dma.h"

volatile DWORD DMATCCount = 0;
volatile DWORD DMAErrCount = 0;

/******************************************************************************
** Function name:		DMAHandler
**
** Descriptions:		DMA interrupt handler
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void DMAHandler (void) __irq 
{
  DWORD regVal;

  IENABLE;				/* handles nested interrupt */
  regVal = GPDMA_INT_TCSTAT;
  if ( regVal )
  {
	DMATCCount++;
	GPDMA_INT_TCCLR |= regVal;
  } 

  regVal = GPDMA_INT_ERR_STAT;
  if ( regVal )
  {
	DMAErrCount++;
	GPDMA_INT_ERR_CLR |= regVal;
  } 
  IDISABLE;
  VICVectAddr = 0;		/* Acknowledge Interrupt */
}

/******************************************************************************
** Function name:		DMA_Init
**
** Descriptions:		
**
** parameters:			
** Returned value:		
** 
******************************************************************************/
DWORD DMA_Init( DWORD DMAMode )
{
  /* USB RAM is used for test.
  Please note, Ethernet has its own SRAM, but GPDMA can't access
  that. GPDMA can only access USB SRAM and IRAM. Ethernet DMA controller 
  can access both IRAM and Ethernet SRAM. */
  PCONP |= (1 << 29);	/* Enable GPDMA clock */

  /* clear all interrupts on channel 0 */
  GPDMA_INT_TCCLR = 0x01;
  GPDMA_INT_ERR_CLR = 0x01;
   
  if ( DMAMode == M2M )
  {
	/* Ch0 is used for M2M test, for M2P and P2M, go to peripheral directories
	where both DAM and non DMA examples are provided. */
	GPDMA_CH0_SRC = DMA_SRC;
	GPDMA_CH0_DEST = DMA_DST;
	/* Terminal Count Int enable */
	GPDMA_CH0_CTRL = ((DMA_SIZE/4) & 0x0FFF) | (0x04 << 12) | (0x04 << 15) 
		| (0x02 << 18) | (0x02 << 21) | (1 << 26) | (1 << 27) | 0x80000000;	
  }
  else
  {
	return ( FALSE );
  }
	
  GPDMA_CONFIG = 0x01;	/* Enable DMA channels, little endian */
  while ( !(GPDMA_CONFIG & 0x01) );    
		
  if ( install_irq( GPDMA_INT, (void *)DMAHandler, HIGHEST_PRIORITY ) == FALSE )
  {
	return ( FALSE );
  }
  return (TRUE);
}

/******************************************************************************
**                            End Of File
******************************************************************************/
