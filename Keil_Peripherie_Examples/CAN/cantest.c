/*****************************************************************************
 *   cantest.c:  CAN test module file for NXP LPC23xx Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.09.13  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#include "LPC23xx.h"			/* LPC23xx definitions */
#include "type.h"
#include "irq.h"
#include "can.h"

CAN_MSG MsgBuf_TX1, MsgBuf_TX2; // TX and RX Buffers for CAN message
CAN_MSG MsgBuf_RX1, MsgBuf_RX2; // TX and RX Buffers for CAN message

volatile DWORD CAN1RxDone, CAN2RxDone;

/*****************************************************************************
** Function name:		main
**
** Descriptions:		main routine for CAN module test
**
** parameters:			None
** Returned value:		int
** 
*****************************************************************************/
int main( void )
{
  /* Please note, this PCLK is set in the target.h file. Since the example
  program is set to test USB device, there is only a limited number of
  options to set CCLK and PCLK when USB is used. The default setting is 
  CCLK 48MHz, PCLK is 1/2 CCLK 24MHz. The bit timing is based on the 
  setting of the PCLK, if different PCLK is used, please read can.h carefully 
  and set your CAN bit timing accordingly. */  
  CAN_Init( BITRATE100K24MHZ );
	
  /* This test program is connect CAN1 and CAN2 on the MCB2300 board,
  send one message from CAN1(TX) and verify received message on CAN2(RX)
  if it's a match, both CAN TX and RX are working. 
	
  For more details on acceptance filter program, see Philips
  appnote AN10438 and the zip file associated with this appnote. */

#if !ACCEPTANCE_FILTER_ENABLED
  /* Initialize MsgBuf */
  MsgBuf_TX1.Frame = 0x80080000; /* 29-bit, no RTR, DLC is 8 bytes */
  MsgBuf_TX1.MsgID = 0x00012345; /* CAN ID */
  MsgBuf_TX1.DataA = 0x3C3C3C3C;
  MsgBuf_TX1.DataB = 0xC3C3C3C3;

  MsgBuf_RX2.Frame = 0x0;
  MsgBuf_RX2.MsgID = 0x0;
  MsgBuf_RX2.DataA = 0x0;
  MsgBuf_RX2.DataB = 0x0;
  CAN_SetACCF( ACCF_BYPASS );

  /* Test bypass */
  while ( 1 )
  {
	/* Transmit initial message on CAN 1 */
	while ( !(CAN1GSR & (1 << 3)) );
	if ( CAN1_SendMessage( &MsgBuf_TX1 ) == FALSE )
	{
	  continue;
	}
 	if ( CAN2RxDone == TRUE )
	{
	  CAN2RxDone = FALSE;
	  if ( MsgBuf_RX2.Frame & (1 << 10) )	/* by pass mode */
	  {
		MsgBuf_RX2.Frame &= ~(1 << 10 );
	  }
	  if ( ( MsgBuf_TX1.Frame != MsgBuf_RX2.Frame ) ||
			( MsgBuf_TX1.MsgID != MsgBuf_RX2.MsgID ) ||
			( MsgBuf_TX1.DataA != MsgBuf_RX2.DataA ) ||
			( MsgBuf_TX1.DataB != MsgBuf_RX2.DataB ) )
	  {
		while ( 1 );
	  }
	  /* Everything is correct, reset buffer */
	  MsgBuf_RX2.Frame = 0x0;
	  MsgBuf_RX2.MsgID = 0x0;
	  MsgBuf_RX2.DataA = 0x0;
	  MsgBuf_RX2.DataB = 0x0;
	} /* Message on CAN 2 received */
  }
#else
  /* Test Acceptance Filter */
  /* Even though the filter RAM is set for all type of identifiers,
  the test module tests explicit standard identifier only */
  MsgBuf_TX1.Frame = 0x00080000; /* 11-bit, no RTR, DLC is 8 bytes */
  MsgBuf_TX1.MsgID = EXP_STD_ID; /* Explicit Standard ID */
  MsgBuf_TX1.DataA = 0x55AA55AA;
  MsgBuf_TX1.DataB = 0xAA55AA55;

  MsgBuf_RX2.Frame = 0x0;
  MsgBuf_RX2.MsgID = 0x0;
  MsgBuf_RX2.DataA = 0x0;
  MsgBuf_RX2.DataB = 0x0;
  CAN_SetACCF( ACCF_ON );

  while ( 1 )
  {
	/* Transmit initial message on CAN 1 */
	while ( !(CAN1GSR & (1 << 3)) );
	if ( CAN1_SendMessage( &MsgBuf_TX1 ) == FALSE )
	{
	  continue;
	}

	/* please note: FULLCAN identifier will NOT be received as it's not set 
	in the acceptance filter. */
 	if ( CAN2RxDone == TRUE )
	{
	  CAN2RxDone = FALSE;
	  /* The frame field is not checked, as ID index varies based on the
	  entries set in the filter RAM. */
	  if ( ( MsgBuf_TX1.MsgID != MsgBuf_RX2.MsgID ) ||
			( MsgBuf_TX1.DataA != MsgBuf_RX2.DataA ) ||
			( MsgBuf_TX1.DataB != MsgBuf_RX2.DataB ) )
	  {
		while ( 1 );
	  }
	  /* Everything is correct, reset buffer */
	  MsgBuf_RX2.Frame = 0x0;
	  MsgBuf_RX2.MsgID = 0x0;
	  MsgBuf_RX2.DataA = 0x0;
	  MsgBuf_RX2.DataB = 0x0;
	} /* Message on CAN 2 received */
  }
#endif
  return ( 0 );
}

/******************************************************************************
**                            End Of File
******************************************************************************/
