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
#include "i2s.h"
#include "dma.h"

#if I2S_DMA_ENABLED
volatile DWORD DMATCCount = 0;
volatile DWORD DMAErrCount = 0;
volatile DWORD I2SDMA0Done = 0;
volatile DWORD I2SDMA1Done = 0;

/******************************************************************************
** Function name:		DMAHandler
**
** Descriptions:		DMA interrupt handler
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void DMAHandler(void) __irq 
{
  DWORD regVal;

  regVal = GPDMA_INT_TCSTAT;
  if ( regVal )
  {
	DMATCCount++;
	GPDMA_INT_TCCLR |= regVal;
	if ( regVal & 0x01 )
	{
	  I2SDMA0Done = 1;
	}
	else if ( regVal & 0x02 )
	{
	  I2SDMA1Done = 1;
	}
  } 

  regVal = GPDMA_INT_ERR_STAT;
  if ( regVal )
  {
	DMAErrCount++;
	GPDMA_INT_ERR_CLR |= regVal;
  }
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
  if ( ChannelNum == 0 )
  {
	GPDMA_INT_TCCLR = 0x01;   
	if ( DMAMode == M2P )
	{
	  /* Ch0 set for M2P transfer from mempry to I2S TX FIFO. */
	  GPDMA_CH0_SRC = DMA_SRC;
	  GPDMA_CH0_DEST = DMA_I2S_TX_FIFO;
	  /* The burst size is set to 1. Terminal Count Int enable */
	  GPDMA_CH0_CTRL = (DMA_SIZE & 0x0FFF) | (0x00 << 12) | (0x00 << 15)
		| (1 << 26) | 0x80000000;
	}
	else if ( DMAMode == P2M )
	{
	  /* Ch0 set for P2M transfer from I2S RX FIFO to memory. */
	  GPDMA_CH0_SRC = DMA_I2S_RX_FIFO;
	  GPDMA_CH0_DEST = DMA_DST;
	  /* The burst size is set to 1. Terminal Count Int enable. */
	  GPDMA_CH0_CTRL = (DMA_SIZE & 0x0FFF) | (0x00 << 12) | (0x00 << 15)
		| (1 << 27) | 0x80000000;
	}
	else if ( DMAMode == P2P )
	{
	  /* Ch0 set for P2P transfer from I2S DAO to I2S DAI. */
	  GPDMA_CH0_SRC = DMA_I2S_TX_FIFO;
	  GPDMA_CH0_DEST = DMA_I2S_RX_FIFO;
	  /* The burst size is set to 32. */
	  GPDMA_CH0_CTRL = (DMA_SIZE & 0x0FFF) | (0x04 << 12) | (0x04 << 15) 
		| 0x80000000;
	}
	else
	{
	  return ( FALSE );
	}
  }
  else if ( ChannelNum == 1 )
  {   
	GPDMA_INT_TCCLR = 0x02;
	if ( DMAMode == M2P )
	{
	  /* Ch1 set for M2P transfer from mempry to I2S TX FIFO. */
	  GPDMA_CH1_SRC = DMA_SRC;
	  GPDMA_CH1_DEST = DMA_I2S_TX_FIFO;
	  /* The burst size is set to 1. Terminal Count Int enable. */
	  GPDMA_CH1_CTRL = (DMA_SIZE & 0x0FFF) | (0x00 << 12) | (0x00 << 15)
		| (1 << 26) | 0x80000000;
	}
	else if ( DMAMode == P2M )
	{
	  /* Ch1 set for P2M transfer from I2S RX FIFO to memory. */
	  GPDMA_CH1_SRC = DMA_I2S_RX_FIFO;
	  GPDMA_CH1_DEST = DMA_DST;
	  /* The burst size is set to 1. Terminal Count Int enable. */
	  GPDMA_CH1_CTRL = (DMA_SIZE & 0x0FFF) | (0x00 << 12) | (0x00 << 15)
		| (1 << 27) | 0x80000000;
	}
	else if ( DMAMode == P2P )
	{
	  /* Ch1 set for P2P transfer from I2S DAO to I2S DAI. */
	  GPDMA_CH1_SRC = DMA_I2S_TX_FIFO;
	  GPDMA_CH1_DEST = DMA_I2S_RX_FIFO;
	  /* The burst size is set to 32. */
	  GPDMA_CH1_CTRL = (DMA_SIZE & 0x0FFF) | (0x04 << 12) | (0x04 << 15) 
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
  return( TRUE );
}

#endif	/* end if DMA_ENABLED */

/******************************************************************************
**                            End Of File
******************************************************************************/
