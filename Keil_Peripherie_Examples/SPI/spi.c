/*****************************************************************************
 *   spi.c:  SPI C file for NXP LPC23xx/24xx Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.07.19  ver 1.00    Prelimnary version, first Release
 *
*****************************************************************************/
#include "LPC23xx.h"			/* LPC23xx/24xx Peripheral Registers	*/
#include "type.h"
#include "irq.h"
#include "spi.h"

volatile DWORD SPI0Status = 0;
volatile DWORD TxCounter = 0;

/*****************************************************************************
** Function name:		SPI0Handler
**
** Descriptions:		SPI0 interrupt handler
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void SPI0Handler (void) __irq 
{
  DWORD regValue;
  
  S0SPINT = SPI0_INT_FLAG;		/* clear interrupt flag */
  IENABLE;				/* handles nested interrupt */

  regValue = S0SPSR;
  if ( regValue & WCOL )
  {
	SPI0Status |= SPI0_COL;
  }
  if ( regValue & SPIF )
  {
	SPI0Status |= SPI0_TX_DONE;
	TxCounter++;
  }
  IDISABLE;
  VICVectAddr = 0;		/* Acknowledge Interrupt */
}

/*****************************************************************************
** Function name:		SPIInit
**
** Descriptions:		SPI port initialization routine
**				
** parameters:			None
** Returned value:		true or false, if the interrupt handler
**				can't be installed correctly, return false.
** 
*****************************************************************************/
DWORD SPIInit( void )
{
  TxCounter = 0;

  PCONP |= (1 << 8);	/* by default, it's enabled already, for safety reason */

  S0SPCR = 0x00;
  /* Port 0.15 SPI SCK, port0.16 uses GPIO SPI_SEL, 
  port0.17 MISO, port0.18 MOSI */
  PINSEL0 |= 0xC0000000;
  PINSEL1 |= 0x0000003C;
  IODIR0 = SPI0_SEL;
  IOSET0 = SPI0_SEL;

  /* Setting SPI0 clock, for Atmel SEEPROM, SPI clock should be no more 
  than 3Mhz on 4.5V~5.5V, no more than 2.1Mhz on 2.7V~5.5V */
  S0SPCCR = 0x8;
#if INTERRUPT_MODE
  if ( install_irq( SPI0_INT, (void *)SPI0Handler, HIGHEST_PRIORITY ) == FALSE )
  {
	return (FALSE);
  }
  /* 8 bit, CPOL=CPHA=0, master mode, MSB first, interrupt enabled */
  S0SPCR = SPI0_SPIE | SPI0_MSTR;
#else
  S0SPCR = SPI0_MSTR;
#endif
  return( TRUE );
}

/*****************************************************************************
** Function name:		SPISend
**
** Descriptions:		Send a block of data to the SPI port, the first
**				parameter is the buffer pointer, the 2nd 
**				parameter is the block length.
**
** parameters:			buffer pointer, and the block length
** Returned value:		None
** 
*****************************************************************************/
void SPISend( BYTE *buf, DWORD Length )
{
  DWORD i;
  BYTE Dummy;

  if ( Length == 0 )
	return;
  for ( i = 0; i < Length; i++ )
  {
	S0SPDR = *buf;
#if INTERRUPT_MODE
	/* In the interrupt, there is nothing to be done if TX_DONE, SPI transfer 
	complete bit, is not set, so it's polling if the flag is set or not which 
	is being handled inside the ISR. Not an ideal example but show how the 
	interrupt is being set and handled. */ 
	while ( (SPI0Status & SPI0_TX_DONE) != SPI0_TX_DONE );
	SPI0Status &= ~SPI0_TX_DONE;
#else
	while ( !(S0SPSR & SPIF) );
#endif
	Dummy = S0SPDR;		/* Flush the RxFIFO */
	buf++;
  }
  return; 
}

/*****************************************************************************
** Function name:		SPIReceive
** Descriptions:		the module will receive a block of data from 
**				the SPI, the 2nd parameter is the block length.
** parameters:			buffer pointer, and block length
** Returned value:		None
** 
*****************************************************************************/
void SPIReceive( BYTE *buf, DWORD Length )
{
  DWORD i;

  for ( i = 0; i < Length; i++ )
  {
	*buf = SPIReceiveByte();
	buf++;
  }
  return; 
}

/*****************************************************************************
** Function name:		SPIReceiveByte
**
** Descriptions:		Receive one byte of data from the SPI port
**				Write a dummy byte, wait until SPI transfer
**				complete, then, read the data register to
**				get the SPI data.
**
** parameters:			None
** Returned value:		the data byte received
** 
*****************************************************************************/
BYTE SPIReceiveByte( void )
{
  BYTE data;

  /* wrtie dummy byte out to generate clock, then read data from MISO */
  S0SPDR = 0xFF;
  /* Wait for transfer complete, SPIF bit set */
#if INTERRUPT_MODE
  /* In the receive routine, there is nothing to be done if TX_DONE, or
  SPI transfer complete bit, is not set, so it's polling if the flag is set 
  or not which is being handled inside the ISR. Not an ideal example but 
  show how the interrupt is being set and handled. */ 
  while ( (SPI0Status & SPI0_TX_DONE) != SPI0_TX_DONE );
  SPI0Status &= ~SPI0_TX_DONE;
#else
  while ( !(S0SPSR & SPIF) );
#endif
  data = S0SPDR;
  return ( data ); 
}

/******************************************************************************
**                            End Of File
******************************************************************************/

