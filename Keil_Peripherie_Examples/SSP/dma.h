/*****************************************************************************
 *   dma.h:  Header file for NXP LPC23xx/24xx Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.07.20  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#ifndef __DMA_H 
#define __DMA_H

#define DMA_SRC			0x7FD00000
#define DMA_DST			0x7FD01000
#define DMA_SSP0DR		0xE0068008
#define DMA_SSP1DR		0xE0030008
#define DMA_SIZE		SSP_BUFSIZE		/* DMA_SIZE is the same BUFSIZE defined in ssp.h */

/* DMA mode */
#define M2M				0x00
#define M2P				0x01
#define P2M				0x02
#define P2P				0x03

extern void DMAHandler( void ) __irq; 
extern DWORD DMA_Init( DWORD ChannelNum, DWORD DMAMode );

#endif /* end __DMA_H */
/****************************************************************************
**                            End Of File
****************************************************************************/
