/*****************************************************************************
 *   ex_nandflash.c:  External NAND FLASH memory module file for NXP 
 *   LPC23xx Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2007.01.13  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#include "LPC23xx.h"				/* LPC23xx/24xx definitions */
#include "type.h"
#include "irq.h"
#include "target.h"
#include "timer.h"
#include "ex_nandflash.h"

BYTE InvalidBlockTable[NANDFLASH_BLOCKNUM];

/*****************************************************************************
** Function name:		NANDFLASHRBCheck
**
** Descriptions:		Ready/Busy check, no timeout, basically, R/B bit
**						should toggle once to bail out from this routine.
**
** parameters:			None			 
** 						
** Returned value:		None
** 
*****************************************************************************/
void NANDFLASHRBCheck( void )
{
  while( FIO2PIN & (1 << 12) );		/* from high to low once */
  __asm
  {
	nop
	nop
  }
  while( !(FIO2PIN & (1 << 12)) );	/* from low to high once */
  return; 
}

/*****************************************************************************
** Function name:		NANDFLASHInit
**
** Descriptions:		initialize external NAND FLASH memory
**
** parameters:			None			 
** 						
** Returned value:		None
** 
*****************************************************************************/
void NANDFLASHInit( void )
{
  DWORD i;

  /**************************************************************************
   * Initialize EMC for NAND FLASH
   **************************************************************************/
  EMC_CTRL = 0x00000001;
  PCONP  |= 0x00000800;		/* Turn On EMC PCLK */

  FIO2DIR &= ~(1 << 12);	/* make P2.12 pin as input */
  FIO2MASK &= ~(1 << 12); 

  PINSEL4 = 0x50000000;
  PINSEL5 = 0x05050555;
  PINSEL6 = 0x55555555;
  PINSEL8 = 0x55555555;
  PINSEL9 = 0x50555555;

  /* for NAND FLASH */
  EMC_STA_CFG1      = 0x00000080;
  EMC_STA_WAITWEN1  = 0x2;
  EMC_STA_WAITOEN1  = 0x2;
  EMC_STA_WAITRD1   = 0x1F;
  EMC_STA_WAITPAGE1 = 0x1F;
  EMC_STA_WAITWR1   = 0x1F;
  EMC_STA_WAITTURN1 = 0xF;
  delayMs(1, 2);	/* Use timer 1 */

  /* assume all blocks are valid to begin with */
  for ( i = 0; i < NANDFLASH_BLOCKNUM; i++ )
  {
	InvalidBlockTable[i] = 0;
  }
  return;
}

/*****************************************************************************
** Function name:		NANDFLASHReset
**
** Descriptions:		Issue Reset command to NAND FLASH memory
**
** parameters:			None			 
** 						
** Returned value:		None
** 
*****************************************************************************/
void NANDFLASHReset( void )
{
  volatile BYTE *pCLE;
  
  /* Reset NAND FLASH  through NAND FLASH command */
  pCLE = K9F1G_CLE;
  *pCLE = K9FXX_RESET;
  delayMs( 1, 2 );	/* Use timer 1 */
  return;
		
}

/*****************************************************************************
** Function name:		NANDFLASHReadStatus
**
** Descriptions:		Read status from NAND FLASH memory
**
** parameters:			None			 
** 						
** Returned value:		Status
** 
*****************************************************************************/
DWORD NANDFLASHReadStatus( DWORD Cmd )
{
  volatile BYTE *pCLE;
  volatile BYTE *pDATA;
  BYTE StatusData;
  
  pCLE  = K9F1G_CLE;
  pDATA = K9F1G_DATA;
    
  *pCLE = K9FXX_READ_STATUS;
  /* Wait until bit 5 and 6 are 1, READY, bit 7 should be 1 too, not protected */
  /* if ready bit not set, it gets stuck here */
  while ( (*pDATA & 0xE0) != 0xE0 );
  StatusData = *pDATA;
  switch ( Cmd )
  {
	case K9FXX_BLOCK_PROGRAM_1:
	case K9FXX_BLOCK_ERASE_1:
	  if ( StatusData & 0x01 )	/* Erase/Program failure(1) or pass(0) */
		return( FALSE );
	  else
		return( TRUE );
	case K9FXX_READ_1:				/* bit 5 and 6, Read busy(0) or ready(1) */
	  return( TRUE );			
	default:
	  break;
  }  
  return( FALSE );
}

/*****************************************************************************
** Function name:		NANDFLASHReadID
**
** Descriptions:		Read ID from external NAND FLASH memory
**
** parameters:			None			 
** 						
** Returned value:		Device ID info.
** 
*****************************************************************************/
DWORD NANDFLASHReadID( void )
{
  BYTE a, b, c, d;
  volatile BYTE *pCLE;
  volatile BYTE *pALE;
  volatile BYTE *pDATA;
  
  pCLE  = K9F1G_CLE;
  pALE  = K9F1G_ALE;
  pDATA = K9F1G_DATA;
    
  *pCLE = K9FXX_READ_ID;
  *pALE = 0;
    
  a = *pDATA;
  b = *pDATA;
  d = *pDATA;
  c = *pDATA;
  return ((a << 24) | (b << 16) | (c << 8) | d);
}

/*****************************************************************************
** Function name:		NANDFLASHEraseBlock
**
** Descriptions:		Erase the whole NAND FLASH memory block based on the
**						block number
**
** parameters:			block number			 
** 						
** Returned value:		Erase status: succeed or fail
** 
*****************************************************************************/
DWORD NANDFLASHEraseBlock( DWORD blockNum )
{
  volatile BYTE *pCLE;
  volatile BYTE *pALE;
  DWORD rowAddr;
  
  pCLE  = K9F1G_CLE;
  pALE  = K9F1G_ALE;

  rowAddr = (NANDFLASH_BASE_ADDR 
  		+ blockNum * NANDFLASH_PAGE_PER_BLOCK * NANDFLASH_RW_PAGE_SIZE) >> 16;    
  *pCLE = K9FXX_BLOCK_ERASE_1;
  *pALE = (BYTE)(rowAddr & 0x00FF);			/* column address low */
  *pALE = (BYTE)((rowAddr & 0xFF00) >> 8);	/* column address high */
  *pCLE = K9FXX_BLOCK_ERASE_2;

  NANDFLASHRBCheck();
  return( NANDFLASHReadStatus( K9FXX_BLOCK_ERASE_1 ) );
}

/*****************************************************************************
** Function name:		NANDFLASHCheckValidBlock
**
** Descriptions:		This routine is used to check if the block is valid or 
**						not.
**
** parameters:			None			 
** 						
** Returned value:		FALSE is invalid block is found, an initial invalid
**						table will be created. TRUE if all blocks are valid.   
** 
*****************************************************************************/
DWORD NANDFLASHCheckValidBlock( void )
{
  volatile BYTE *pCLE;
  volatile BYTE *pALE;
  volatile BYTE *pDATA;
  DWORD block, page, retValue = TRUE;
  DWORD curAddr;

  pCLE  = K9F1G_CLE;
  pALE  = K9F1G_ALE;
  pDATA = K9F1G_DATA;

  for ( block = 0; block < NANDFLASH_BLOCKNUM; block++ )
  {  
	curAddr = NANDFLASH_BASE_ADDR 
		+ block * NANDFLASH_PAGE_PER_BLOCK * NANDFLASH_RW_PAGE_SIZE;
	for ( page = 0; page < 2; page++ )
	{
	  /* Check column address 2048 at first page and second page */
	  curAddr += page * NANDFLASH_RW_PAGE_SIZE;       
	  *pCLE = K9FXX_READ_1;
	  *pALE =  (BYTE)(curAddr & 0x00000000);			/* column address low */
	  *pALE = (BYTE)((curAddr | 0x00000800) >> 8);	/* column address high */
	  *pALE = (BYTE)((curAddr & 0x00FF0000) >> 16);	/* row address low */
	  *pALE = (BYTE)((curAddr & 0xFF000000) >> 24);	/* row address high */
	  *pCLE = K9FXX_READ_2;

	  NANDFLASHRBCheck();
	  if ( *pDATA != 0xFF )
	  {
		/* found invalid block number, mark block number in the invalid 
		block table. */
		InvalidBlockTable[block] = 1;
		retValue = FALSE;		/* At least one block is invalid. */
	  }
	}
  }  			
  return( retValue );
}

/*****************************************************************************
** Function name:		NANDFLASHPageProgram
**
** Descriptions:		Write a full page of program into NAND flash based on the
**						page number, write up to 2112 bytes of data at a time.  
**
** parameters:			page number, block number, and pointer to the buffer			 
** 						
** Returned value:		Erase status: succeed or fail
** 
*****************************************************************************/
DWORD NANDFLASHPageProgram( DWORD pageNum, DWORD blockNum, BYTE *bufPtr )
{
  volatile BYTE *pCLE;
  volatile BYTE *pALE;
  volatile BYTE *pDATA;
  DWORD i, curAddr;
  
  pCLE  = K9F1G_CLE;
  pALE  = K9F1G_ALE;
  pDATA = K9F1G_DATA;
  
  curAddr = NANDFLASH_BASE_ADDR + blockNum * NANDFLASH_BLOCKNUM
  							+ pageNum * NANDFLASH_RW_PAGE_SIZE;  
  *pCLE = K9FXX_BLOCK_PROGRAM_1;
  *pALE =  (BYTE)(curAddr & 0x000000FF);		/* column address low */
  *pALE = (BYTE)((curAddr & 0x00000F00) >> 8);	/* column address high */
  *pALE = (BYTE)((curAddr & 0x00FF0000) >> 16);	/* row address low */
  *pALE = (BYTE)((curAddr & 0xFF000000) >> 24);	/* row address high */
  __asm
  {
	nop
	nop
  }
  for ( i = 0; i < NANDFLASH_RW_PAGE_SIZE; i++ )
  {
	*pDATA = *bufPtr++;
  }
  *pCLE = K9FXX_BLOCK_PROGRAM_2;

  __asm
  {
	nop
	nop
  }
  NANDFLASHRBCheck();
  return( NANDFLASHReadStatus( K9FXX_BLOCK_PROGRAM_1 ) );
}

/*****************************************************************************
** Function name:		NANDFLASHPageRead
**
** Descriptions:		Read the whole NAND FLASH memory page based on the
**						page number, the data will be stored in the pointer
**						to the buffer.
**
** parameters:			page number, block number, buffer pointer;			 
** 						
** Returned value:		Erase status: succeed or fail
** 
*****************************************************************************/
DWORD NANDFLASHPageRead( DWORD pageNum, DWORD blockNum, BYTE *bufPtr )
{
  volatile BYTE *pCLE;
  volatile BYTE *pALE;
  volatile BYTE *pDATA;
  DWORD i, curAddr;
  
  pCLE  = K9F1G_CLE;
  pALE  = K9F1G_ALE;
  pDATA = K9F1G_DATA;
  
   curAddr = NANDFLASH_BASE_ADDR + blockNum * NANDFLASH_BLOCKNUM
  							+ pageNum * NANDFLASH_RW_PAGE_SIZE;
  *pCLE = K9FXX_READ_1;
  *pALE =  (BYTE)(curAddr & 0x000000FF);		/* column address low */
  *pALE = (BYTE)((curAddr & 0x00000F00) >> 8);	/* column address high */
  *pALE = (BYTE)((curAddr & 0x00FF0000) >> 16);	/* row address low */
  *pALE = (BYTE)((curAddr & 0xFF000000) >> 24);	/* row address high */
  *pCLE = K9FXX_READ_2;

  NANDFLASHRBCheck();
  for ( i = 0; i < NANDFLASH_RW_PAGE_SIZE; i++ )
  {
	*bufPtr = *pDATA;
	bufPtr++;
  }
  return( TRUE );
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
