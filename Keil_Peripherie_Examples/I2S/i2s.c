/*****************************************************************************
 *   i2s.c:  I2S C file for NXP LPC23xx/24xx Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.07.19  ver 1.00    Prelimnary version, first Release
 *
*****************************************************************************/
#include "LPC23xx.h"                        /* LPC23xx/24xx definitions */
#include "type.h"
#include "irq.h"
#include "i2s.h"
#include "dma.h"

/* treat I2S TX and RX as a constant address, make the code and buffer 
easier for both DMA and non-DMA test */
volatile BYTE *I2STXBuffer = (BYTE *)(DMA_SRC); 
volatile BYTE *I2SRXBuffer = (BYTE *)(DMA_DST);
volatile DWORD I2SReadLength = 0;
volatile DWORD I2SWriteLength = 0;
volatile DWORD I2SRXDone = 0, I2STXDone = 0;

/*****************************************************************************
** Function name:		I2SHandler
**
** Descriptions:		I2S interrupt handler, only RX interrupt is enabled
**						for simplicity.
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void I2SHandler (void) __irq 
{
  DWORD RxCount = 0;

  if ( I2S_STATE & 0x01 )
  {
	RxCount = (I2S_STATE >> 8) & 0xFF;
	if ( (RxCount != RXFIFO_EMPTY) && !I2SRXDone )
	{
	  while ( RxCount > 0 )
	  {
		if ( I2SReadLength == BUFSIZE )
		{
		  /* Stop RX channel */
		  I2S_DAI |= ((0x01 << 3) | (0x01 << 4));
		  I2S_IRQ &= ~(0x01 << 0);	/* Disable RX */	
		  I2SRXDone = 1;
		  break;
		}
		else
		{
		  I2SRXBuffer[I2SReadLength++] = I2S_RX_FIFO;
		}
		RxCount--;
	  }
	}
  }
  VICVectAddr = 0;		/* Acknowledge Interrupt */
}

/*****************************************************************************
** Function name:		I2SStart
**
** Descriptions:		Start I2S DAI and DAO
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void I2SStart( void )
{
  DWORD DAIValue, DAOValue;
  
  /* Audio output is the master, audio input is the slave, */
  /* 16 bit data, stereo, reset, master mode, not mute. */
  DAOValue = I2S_DAO;
  DAIValue = I2S_DAI;
  I2S_DAO = DAOValue & (~((0x01 << 4)|(0x01 <<3)));
  /* 16 bit data, stereo, reset, slave mode, not mute. */
  I2S_DAI	= DAIValue & (~((0x01 << 4)|(0x01 <<3)));
  return;
}

/*****************************************************************************
** Function name:		I2SStop
**
** Descriptions:		Stop I2S DAI and DAO
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void I2SStop( void )
{
  DWORD DAIValue, DAOValue;

  /* Stop the I2S to start. Audio output is master, audio input is the slave. */
  /* 16 bit data, set STOP and RESET bits to reset the channels */
  DAOValue = I2S_DAO;
  /* Switch to master mode, TX channel, no mute */
  DAOValue &= ~((0x01 << 5)|(0x01 << 15));
  DAIValue = I2S_DAI;
  DAIValue &= ~(0x01 << 15);
  I2S_DAO = (0x01 << 4) | (0x01 << 3) | DAOValue;	/* Master */
  I2S_DAI = (0x01 << 4) | (0x01 << 3) | DAIValue;	/* Slave */
  return;
}

/*****************************************************************************
** Function name:		I2SInit
**
** Descriptions:		Initialize I2S controller
**
** parameters:			None
** Returned value:		true or false, return false if the I2S
**						interrupt handler was not installed correctly
** 
*****************************************************************************/
DWORD I2SInit( void ) 
{
  /*enable I2S in the PCONP register. I2S is disabled on reset*/
  PCONP |= (1 << 27);

  /*connect the I2S sigals to port pins(P0.4-P0.9)*/
  PINSEL0 &= ~0x000FFF00;
  PINSEL0 |= 0x00055500;

  /* For all the test program assuming USB is used, the CCLK is set to
  57.6Mhz while all the PCLK are the same, 1/2 of CCLK = 28.8Mhz. It also
  applys to I2S. To generate 48khz sample rate for 16-bit stereo data
  requires a bit rate of 48,000x16x2=1.536Mhz, the TX and RX clock rate
  register should be 28800000/1536000=18.75=0x13. 
	
  Please note, in order to generate accurate TX/RX clock rate for I2S, 
  PCLK and CCLK needs to be carefully reconsidered. For this test 
  program, the TX is looped back to RX without external I2S device, 
  clock rate is not critical in this matter. */
  I2S_TXRATE = 0x13;
  I2S_RXRATE = 0x13;

  I2SStop();
  if ( install_irq( I2S_INT, (void *)I2SHandler, HIGHEST_PRIORITY ) == FALSE )
  {
	return (FALSE);
  }
  return( TRUE );
}

/******************************************************************************
**                            End Of File
******************************************************************************/

