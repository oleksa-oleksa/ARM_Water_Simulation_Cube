/*****************************************************************************
 *   ssptest.c:  main C entry file for NXP LPC23xx/24xx Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.07.19  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#include "LPC23xx.H"                        /* LPC23xx/24xx definitions */
#include "type.h"
#include "irq.h"
#include "ssp.h"
#include "dma.h"

volatile BYTE *src_addr; 
volatile BYTE *dest_addr;

/*****************************************************************************
** Function name:		LoopbackTest
**
** Descriptions:		Loopback test
**				
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void LoopbackTest( void )
{
  DWORD i;

#if DMA_ENABLED
#if !USE_CS
  IOCLR0 = SSP0_SEL;	/* SSEL is output */
#endif
  /* on DMA channel 0, Source is memory, destination is SSPO TX, 
  on DMA channel 1, source is SSP0 RX, Destination is memory */
  /* Enable channel and IE bit */

  DMA_Init( 0, M2P );
  GPDMA_CH0_CFG |= 0x18001 | (0x00 << 6) | (0x05 << 11);
  DMA_Init( 1, P2M );
  GPDMA_CH1_CFG |= 0x08001 | (0x01 << 1) | (0x06 << 11);

  /* Enable DMA TX and RX on SSP0 */
  SSP0DMACR = 0x03;
#if !USE_CS
  IOSET0 = SSP0_SEL;	/* SSEL is output */
#endif
#else					/* Not DMA mode */
#if !USE_CS
  IOCLR0 = SSP0_SEL;	/* SSEL is output */
#endif
  src_addr = (BYTE *)DMA_SRC;
  dest_addr = (BYTE *)DMA_DST;
  i = 0;
  while ( i <= SSP_BUFSIZE )
  {
	/* to check the RXIM and TXIM interrupt, I send a block data at one time 
	based on the FIFOSIZE(8). */
	SSP0Send( (BYTE *)src_addr, FIFOSIZE );
	/* If RX interrupt is enabled, below receive routine can be
	also handled inside the ISR. */
	SSP0Receive( (BYTE *)dest_addr, FIFOSIZE );
	src_addr += FIFOSIZE;
	dest_addr += FIFOSIZE;
	i += FIFOSIZE;
  }
#if !USE_CS
  IOSET0 = SSP0_SEL;	/* SSEL is output */
#endif
#endif
  /* verifying, ignore the difference in the first two bytes */
  for ( i = 0; i < SSP_BUFSIZE; i++ )
  {
	if ( src_addr[i] != dest_addr[i] )
	{
	  while( 1 );			/* Verification failed */
	}
  }
  return; 
}

/*****************************************************************************
** Function name:		SEEPROMTest
**
** Descriptions:		Serial EEPROM(Atmel 25xxx) test
**						For SEEPROM test, port 0.16 is configured as
**						GPIO, not SSEL. Then, a totoal freedom to toggle it.
**				
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void SEEPROMTest( void )
{
  DWORD i, timeout;

  /* Don't use SSEL when test SEPROM. */
  PINSEL1 &= ~0x00000003;		/* Set SSEL to GPIO pin */	
  IODIR0 = SSP0_SEL;			/* SSEL is output */
  IOSET0 = SSP0_SEL;			/* Initial state is always high */
  IOCLR0 = SSP0_SEL;			/* CS low */
  /* Test Atmel 25256 SPI SEEPROM. */
  src_addr[0] = WREN;			/* set write enable latch */
  SSP0Send( (BYTE *)&src_addr[0], 1 );
  IOSET0 = SSP0_SEL;			/* CS high */

  for ( i = 0; i < DELAY_COUNT; i++ );	/* delay minimum 250ns */

  IOCLR0 = SSP0_SEL;			/* CS low */
  src_addr[0] = RDSR;	/* check status to see if write enabled is latched */
  SSP0Send( (BYTE *)&src_addr[0], 1 );
  SSP0Receive( (BYTE *)&dest_addr[0], 1 );
  IOSET0 = SSP0_SEL;			/* CS high */
  if ( dest_addr[0] & (RDSR_WEN|RDSR_RDY) != RDSR_WEN ) 
  /* bit 0 to 0 is ready, bit 1 to 1 is write enable */
  {
	while ( 1 );		/* Fatal error */
  }

  src_addr = (BYTE *)DMA_SRC;
  dest_addr = (BYTE *)DMA_DST;
  /* please note the first two bytes of WR and RD buffer is used for
  commands and offset, so only 2 through SSP_BUFSIZE is used for data read,
  write, and comparison. */
  IOCLR0 = SSP0_SEL;	/* CS low */
  src_addr[0] = WRITE;	/* Write command is 0x02, low 256 bytes only */
  src_addr[1] = 0x00;	/* write address offset MSB is 0x00 */
  src_addr[2] = 0x00;	/* write address offset LSB is 0x00 */
  SSP0Send( (BYTE *)&src_addr[0], SSP_BUFSIZE );
  IOSET0 = SSP0_SEL;	/* CS high */

  for ( i = 0; i < (DELAY_COUNT * 2000); i++ );	/* delay, minimum 3ms */
  
  timeout = 0;
  while ( timeout < MAX_TIMEOUT )
  {
	IOCLR0 = SSP0_SEL;		/* CS low */		
	src_addr[0] = RDSR;		/* check status to see if write cycle is done or not */
	SSP0Send( (BYTE *)&src_addr[0], 1);
	SSP0Receive( (BYTE *)&dest_addr[0], 1 );
	IOSET0 = SSP0_SEL;		/* CS high */
	if ( (dest_addr[0] & RDSR_RDY) == 0x00 )	/* bit 0 to 0 is ready */
	{
	    break;
	}
	timeout++;
  }
  if ( timeout == MAX_TIMEOUT )
  {
	while (1);			/* Fatal error */
  }
  for ( i = 0; i < DELAY_COUNT; i++ );	/* delay, minimum 250ns */

  src_addr = (BYTE *)DMA_SRC;
  dest_addr = (BYTE *)DMA_DST;
  IOCLR0 = SSP0_SEL;		/* CS low */
  src_addr[0] = READ;		/* Read command is 0x03, low 256 bytes only */
  src_addr[1] = 0x00;		/* Read address offset MSB is 0x00 */
  src_addr[2] = 0x00;		/* Read address offset LSB is 0x00 */
  SSP0Send( (BYTE *)&src_addr[0], 3 ); 
  SSP0Receive( (BYTE *)&dest_addr[3], SSP_BUFSIZE-3 );
  IOSET0 = SSP0_SEL;		/* CS high */
  
  /* verifying, ignore the difference in the first two bytes */
  for ( i = 3; i < SSP_BUFSIZE; i++ )
  {
	if ( src_addr[i] != dest_addr[i] )
	{
	  while( 1 );			/* Verification failed */
	}
  }
  return;
}


/******************************************************************************
**   Main Function  main()
******************************************************************************/
int main (void)
{
  DWORD i;
    
  SSP0Init();	/* initialize SSP0(SSP) port, share pins with SPI0
				on port0(p0.15-18). */
  /* DMA_SRC and DMA_DST are RAM area shared by both DMA and non DMA operation */  
  src_addr = (BYTE *)DMA_SRC;
  dest_addr = (BYTE *)DMA_DST;
  for ( i = 0; i < SSP_BUFSIZE; i++ )
  {
	*src_addr++ = i;
	*dest_addr++ = 0;
  }
  
#if LOOPBACK_MODE
  LoopbackTest();
#else
  SEEPROMTest();  
#endif
  return 0;
}

/******************************************************************************
**                            End Of File
******************************************************************************/

