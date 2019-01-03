/*****************************************************************************
 *   ssp.c:  SSP C file for NXP LPC23xx/24xx Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.07.20  ver 1.00    Prelimnary version, first Release
 *
*****************************************************************************/
#include "LPC23xx.h"			/* LPC23XX/24xx Peripheral Registers */
#include "type.h"
#include "irq.h"
#include "ssp.h"
#include "dma.h"

/* statistics of all the interrupts */
volatile DWORD interruptRxStat = 0;
volatile DWORD interruptOverRunStat = 0;
volatile DWORD interruptRxTimeoutStat = 0;

/*****************************************************************************
** Function name:		SSP0Handler
**
** Descriptions:		SSP port is used for SPI communication.
**						SSP0(SSP) interrupt handler
**						The algorithm is, if RXFIFO is at least half full, 
**						start receive until it's empty; if TXFIFO is at least
**						half empty, start transmit until it's full.
**						This will maximize the use of both FIFOs and performance.
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void SSP0Handler (void) __irq 
{
  DWORD regValue;
  
  IENABLE;				/* handles nested interrupt */

  regValue = SSP0MIS;
  if ( regValue & SSPMIS_RORMIS )	/* Receive overrun interrupt */
  {
	interruptOverRunStat++;
	SSP0ICR = SSPICR_RORIC;		/* clear interrupt */
  }
  if ( regValue & SSPMIS_RTMIS )	/* Receive timeout interrupt */
  {
	interruptRxTimeoutStat++;
	SSP0ICR = SSPICR_RTIC;		/* clear interrupt */
  }

  /* please be aware that, in main and ISR, CurrentRxIndex and CurrentTxIndex
  are shared as global variables. It may create some race condition that main
  and ISR manipulate these variables at the same time. SSPSR_BSY checking (polling)
  in both main and ISR could prevent this kind of race condition */
  if ( regValue & SSPMIS_RXMIS )	/* Rx at least half full */
  {
	interruptRxStat++;		/* receive until it's empty */		
  }

  IDISABLE;
  VICVectAddr = 0;		/* Acknowledge Interrupt */
}

/*****************************************************************************
** Function name:		SSP0Init
**
** Descriptions:		SSP0 port initialization routine
**				
** parameters:			None
** Returned value:		true or false, if the interrupt handler
**						can't be installed correctly, return false.
** 
*****************************************************************************/
DWORD SSP0Init( void )
{
  BYTE i, Dummy=Dummy;

  /* enable clock to SSP0 for security reason. By default, it's enabled already */
  PCONP |= (1 << 21);

  /* Configure PIN connect block */
  /* bit 32, 54, 76 are 0x10, bit 98 are 0x00 */
  /* port 0 bits 17, 18, 19, 20 are SSP port SCK0, MISO0, MOSI0, and SSEL0 */
  /* set SSEL to GPIO pin that you will have the totoal freedom to set/reset 
  the SPI chip-select pin */
  /* When DMA is enabled, enable USE_CS, or be careful with SSP0_SEL pin,
  clear SSP0_SEL before DMA starts, and set SSP0_SEL after finishing. */ 
#if USE_CS
  PINSEL0 |= 0x80000000;
  PINSEL1 |= 0x0000002A;
#else
  PINSEL0 |= 0x80000000;
  PINSEL1 |= 0x00000028;
  IODIR0 = SSP0_SEL;	/* SSEL is output */
  IOSET0 = SSP0_SEL;	/* set SSEL to high */
#endif
		
  /* Set DSS data to 8-bit, Frame format SPI, CPOL = 0, CPHA = 0, and SCR is 15 */
  SSP0CR0 = 0x0707;

  /* SSPCPSR clock prescale register, master mode, minimum divisor is 0x02 */
#if LOOPBACK_MODE
  SSP0CPSR = 0x2;
#else
  /* Much slower clock is needed in order to test serial EEPROM. */
  SSP0CPSR = 0x40;
#endif

  for ( i = 0; i < FIFOSIZE; i++ )
  {
	Dummy = SSP0DR;		/* clear the RxFIFO */
  }

  if ( install_irq( SPI0_INT, (void *)SSP0Handler, HIGHEST_PRIORITY ) == FALSE )
  {
	return (FALSE);
  }
	
  /* Device select as master, SSP Enabled */
#if LOOPBACK_MODE
  SSP0CR1 = SSPCR1_LBM | SSPCR1_SSE;
#else
  SSP0CR1 = SSPCR1_SSE;
#endif
  /* Set SSPINMS registers to enable interrupts */
  /* enable all error related interrupts */
  SSP0IMSC = SSPIMSC_RORIM | SSPIMSC_RTIM;
  return( TRUE );
}

/*****************************************************************************
** Function name:		SSP0Send
**
** Descriptions:		Send a block of data to the SSP0 port, the 
**						first parameter is the buffer pointer, the 2nd 
**						parameter is the block length.
**
** parameters:			buffer pointer, and the block length
** Returned value:		None
** 
*****************************************************************************/
void SSP0Send( BYTE *buf, DWORD Length )
{
  DWORD i;
  BYTE Dummy = Dummy;
    
  for ( i = 0; i < Length; i++ )
  {
	/* Move on only if NOT busy and TX FIFO not full. */
	while ( (SSP0SR & (SSPSR_TNF|SSPSR_BSY)) != SSPSR_TNF );
	SSP0DR = *buf;
	buf++;
#if !LOOPBACK_MODE
	while ( (SSP0SR & (SSPSR_BSY|SSPSR_RNE)) != SSPSR_RNE );
	/* Whenever a byte is written, MISO FIFO counter increments, Clear FIFO 
	on MISO. Otherwise, when SSP0Receive() is called, previous data byte
	is left in the FIFO. */
	Dummy = SSP0DR;
#else
	/* Wait until the Busy bit is cleared. */
	while ( SSP0SR & SSPSR_BSY );
#endif
  }
  return; 
}

/*****************************************************************************
** Function name:		SSP0Receive
** Descriptions:		the module will receive a block of data from 
**						the SSP0, the 2nd parameter is the block 
**						length.
** parameters:			buffer pointer, and block length
** Returned value:		None
** 
*****************************************************************************/
void SSP0Receive( BYTE *buf, DWORD Length )
{
  DWORD i;
 
  for ( i = 0; i < Length; i++ )
  {
	/* As long as Receive FIFO is not empty, I can always receive. */
	/* If it's a loopback test, clock is shared for both TX and RX,
	no need to write dummy byte to get clock to get the data */
	/* if it's a peer-to-peer communication, SSPDR needs to be written
	before a read can take place. */
#if !LOOPBACK_MODE
	SSP0DR = 0xFF;
	/* Wait until the Busy bit is cleared */
	while ( (SSP0SR & (SSPSR_BSY|SSPSR_RNE)) != SSPSR_RNE );
#else
	while ( !(SSP0SR & SSPSR_RNE) );
#endif
	*buf = SSP0DR;
	buf++;
  }
  return; 
}

/******************************************************************************
**                            End Of File
******************************************************************************/

