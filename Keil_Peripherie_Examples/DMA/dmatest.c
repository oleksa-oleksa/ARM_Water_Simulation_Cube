/*****************************************************************************
 *   dmatest.c:  main C entry file for NXP LPC23xx/24xx Family Microprocessors
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

extern volatile DWORD DMATCCount;

/******************************************************************************
**   Main Function  main()
******************************************************************************/
int main (void)
{	
  DWORD i;
  DWORD *src_addr, *dest_addr;
      
  src_addr = (DWORD *)DMA_SRC;
  dest_addr = (DWORD *)DMA_DST;
  for ( i = 0; i < DMA_SIZE/4; i++ )
  {
	*src_addr++ = i;
	*dest_addr++ = 0;
  }
	    
  DMA_Init( M2M );
  GPDMA_CH0_CFG |= 0x08001;	/* Enable channel and IE bit */

  while ( !DMATCCount );		/* Wait until DMA is done */

  /* Verify result */
  src_addr = (DWORD *)DMA_SRC;
  dest_addr = (DWORD *)DMA_DST;
  for ( i = 0; i < DMA_SIZE/4; i++ )
  {
	if ( *src_addr++ != *dest_addr++ )
	{
	  while ( 1 );	/* fatal error */
	}
  }
  return 0;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
