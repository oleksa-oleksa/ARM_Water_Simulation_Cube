/*****************************************************************************
 *   mcitest.c:  main C entry file for NXP LPC23xx/24xx Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.07.20  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#include "LPC23xx.h"                       /* LPC23xx/24xx definitions */
#include "type.h"
#include "irq.h"
#include "mci.h"
#include "dma.h"

extern volatile DWORD MCI_CardType;
extern volatile DWORD MCI_Block_End_Flag;

extern volatile BYTE *WriteBlock, *ReadBlock;

/******************************************************************************
**   Main Function  main()
******************************************************************************/
int main (void)
{
  DWORD i, j;

  /* Fill block data pattern in write buffer and clear everything 
  in read buffer. */
  for ( i = 0; i < BLOCK_LENGTH; i++ )
  {
	WriteBlock[i] = i;
	ReadBlock[i] = 0;
  }
#if MCI_DMA_ENABLED
  /* on DMA channel 0, source is memory, destination is MCI FIFO. */
  /* On DMA channel 1, source is MCI FIFO, destination is memory. */
  DMA_Init();
#endif
	
  /* For the SD card I tested, the minimum required block length is 512 */
  /* For MMC, the restriction is loose, due to the variety of SD and MMC
  card support, ideally, the driver should read CSD register to find the 
  right speed and block length for the card, and set them accordingly.
  In this driver example, it will support both MMC and SD cards, and it
  does read the information by send SEND_CSD to poll the card status,
  however, to simplify the example, it doesn't configure them accordingly
  based on the CSD register value. This is not intended to support all 
  the SD and MMC cards. */
  if ( MCI_Init() != TRUE )
  {
	while( 1 );			/* fatal error */
  }

  MCI_CardType = MCI_CardInit();

  if ( MCI_CardType == CARD_UNKNOWN )
  {
	while ( 1 );		/* fatal error */
  }

  if (MCI_Check_CID() != TRUE)
  {
	while ( 1 );		/* fatal error */
  }

  if ( MCI_Set_Address() != TRUE )
  {
	while ( 1 );		/* fatal error */
  }

  if ( MCI_Send_CSD() != TRUE )
  {
	while ( 1 );		/* fatal error */
  }

  if ( MCI_Select_Card() != TRUE )
  {
	while ( 1 );		/* fatal error */
  }

  if ( MCI_CardType == SD_CARD )
  {
	MCI_Set_MCIClock( NORMAL_RATE );
	if (SD_Set_BusWidth( SD_4_BIT ) != TRUE )
	{
	  while ( 1 );	/* fatal error */
	}
  }

  if ( MCI_Set_BlockLen( BLOCK_LENGTH ) != TRUE )
  {
	while ( 1 );		/* fatal error */
  }

  /***************************************************************/
  /* TEST - Write and Read number of blocks defined by BLOCK_NUM */
  /***************************************************************/
  for ( i = 0; i < BLOCK_NUM; i++ )
  {
	if ( MCI_Write_Block( i ) != TRUE )
	{
	  while ( 1 );		/* Fatal error */
	}
	/* When MCI_Block_End_Flag is clear, it indicates   
	Write_Block is complete, next task, Read_Block to check write */
	while ( MCI_Block_End_Flag == 1 );

	if ( MCI_Read_Block( i ) != TRUE )
	{
	  while ( 1 );		/* Fatal error */
	}
	/* When MCI_Block_End_Flag is clear, it indicates RX is done 
	with Read_Block,  validation of RX and TX buffers next. */
	while ( MCI_Block_End_Flag == 1 );
		
	for ( j = 0; j < (BLOCK_LENGTH); j++ )
	{
	  if ( WriteBlock[j] != ReadBlock[j] )
	  {
		while ( 1 );	/* Data comparison failure, fatal error */
	  }
	}
	/*  clear read buffer for next block comparison */
	for ( j = 0; j < (BLOCK_LENGTH); j++ )
	{
	  ReadBlock[j] = 0;
	}
  }	 
  return 0;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
