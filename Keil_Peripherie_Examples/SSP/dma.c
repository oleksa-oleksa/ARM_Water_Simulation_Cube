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
#include "ssp.h"
#include "dma.h"

#if DMA_ENABLED

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
void DMAHandler( void ) __irq 
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
DWORD DMA_Init( DWORD ChannelNum, DWORD DMAMode )
{
  /* USB RAM is used for test.
  Please note, Ethernet has its own SRAM, but GPDMA can't access
  that. GPDMA can access USB SRAM and IRAM. Ethernet DMA controller can 
  access both IRAM and Ethernet SRAM. */
  PCONP |= (1 << 29);	/* Enable GPDMA clock */

  GPDMA_INT_TCCLR = 0x03;
  GPDMA_INT_ERR_CLR = 0x03;

  if ( ChannelNum == 0 )
  {   
	if ( DMAMode == M2M )
	{
	  /* Ch0 is set for M2M tranfer from AHB1 to AHB2 SRAM */
	  GPDMA_CH0_SRC = DMA_SRC;
	  GPDMA_CH0_DEST = DMA_DST;
	  /* Terminal Count Int enable */
	  GPDMA_CH0_CTRL = (DMA_SIZE & 0x0FFF) | (0x01 << 12) | (0x01 << 15) 
		| (0x02 << 18) | (0x02 << 21) | (1 << 26) | (1 << 27) | 0x80000000;	
	}
	else if ( DMAMode == M2P )
	{
	  /* Ch0 set for M2P transfer from mempry to SSP0. */
	  GPDMA_CH0_SRC = DMA_SRC;
	  GPDMA_CH0_DEST = DMA_SSP0DR;
	  /* The burst size is set to 8, the size is 8 bit too. */
	  /* Terminal Count Int enable */
	  GPDMA_CH0_CTRL = (DMA_SIZE & 0x0FFF) | (0x02 << 12) | (0x02 << 15)
		| (1 << 26) | 0x80000000;
	}
	else if ( DMAMode == P2M )
	{
	  /* Ch0 set for P2M transfer from SSP0 to memory. */
	  GPDMA_CH0_SRC = DMA_SSP0DR;
	  GPDMA_CH0_DEST = DMA_DST;
	  /* The burst size is set to 8, the size is 8 bit too. */
	  /* Terminal Count Int enable */
	  GPDMA_CH0_CTRL = (DMA_SIZE & 0x0FFF) | (0x02 << 12) | (0x02 << 15)
		| (1 << 27) | 0x80000000;
	}
	else if ( DMAMode == P2P )
	{
	  /* Ch0 set for P2P transfer from SSP0 to SSP1. */
	  GPDMA_CH0_SRC = DMA_SSP0DR;
	  GPDMA_CH0_DEST = DMA_SSP1DR;
	  /* The burst size is set to 8, the size is 8 bit too. */
	  /* Terminal Count Int enable */
	  GPDMA_CH0_CTRL = (DMA_SIZE & 0x0FFF) | (0x02 << 12) | (0x02 << 15) 
		| 0x80000000;
	}
	else
	{
	  return ( FALSE );
	}
  }
  else if ( ChannelNum == 1 )
  {   
	if ( DMAMode == M2M )
	{
	  /* Ch1 is set for M2M tranfer */
	  GPDMA_CH1_SRC = DMA_SRC;
	  GPDMA_CH1_DEST = DMA_DST;
	  /* Terminal Count Int enable */
	  GPDMA_CH1_CTRL = (DMA_SIZE & 0x0FFF) | (0x01 << 12) | (0x01 << 15) 
		| (0x02 << 18) | (0x02 << 21) | (1 << 26) | (1 << 27) | 0x80000000;	
	}
	else if ( DMAMode == M2P )
	{
	  /* Ch1 set for M2P transfer from mempry to SSP1. */
	  GPDMA_CH1_SRC = DMA_SRC;
	  GPDMA_CH1_DEST = DMA_SSP0DR;
	  /* The burst size is set to 8, the size is 8 bit too. */
	  /* Terminal Count Int enable */
	  GPDMA_CH1_CTRL = (DMA_SIZE & 0x0FFF) | (0x02 << 12) | (0x02 << 15)
		| (1 << 26) | 0x80000000;
	}
	else if ( DMAMode == P2M )
	{
	  /* Ch1 set for P2M transfer from SSP1 to memory. */
	  GPDMA_CH1_SRC = DMA_SSP0DR;
	  GPDMA_CH1_DEST = DMA_DST;
	  /* The burst size is set to 8, the size is 8 bit too. */
	  /* Terminal Count Int enable */
	  GPDMA_CH1_CTRL = (DMA_SIZE & 0x0FFF) | (0x02 << 12) | (0x02 << 15)
		| (1 << 27) | 0x80000000;
	}
	else if ( DMAMode == P2P )
	{
	  /* Ch1 set for P2P transfer from SSP0 to SSP1. */
	  GPDMA_CH1_SRC = DMA_SSP1DR;
	  GPDMA_CH1_DEST = DMA_SSP0DR;
	  /* The burst size is set to 8, the size is 8 bit too. */
	  /* Terminal Count Int enable */
	  GPDMA_CH1_CTRL = (DMA_SIZE & 0x0FFF) | (0x02 << 12) | (0x02 << 15) 
		| 0x80000000;
	}
	else
	{
	  return ( FALSE );
	}
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

#endif	/* end if DMA_ENABLED */

/******************************************************************************
**                            End Of File
******************************************************************************/
