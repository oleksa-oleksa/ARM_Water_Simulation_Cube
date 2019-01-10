/*****************************************************************************
 *   nandflash_test.c:  External NAND FLASH test main C entry file for NXP 
 *   LPC23xx/24xx Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.07.14  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#include "LPC23xx.h"				/* LPC23xx/24xx definitions */
#include "type.h"
#include "irq.h"
#include "target.h"
#include "ex_nandflash.h"

BYTE ReadBuf[NANDFLASH_PAGE_SIZE], WriteBuf[NANDFLASH_PAGE_SIZE];
extern BYTE InvalidBlockTable[NANDFLASH_BLOCKNUM];

/*****************************************************************************
**   Main Function  main()
******************************************************************************/
int main (void)
{
  DWORD FlashID;
  DWORD i;

  /* initialize memory */
  NANDFLASHInit();
  NANDFLASHReset();
  
  FlashID = NANDFLASHReadID();
  if ( (FlashID & 0xFFFF0000) != K9FXX_ID )
  {
	while( 1 );	/* Fatal error */
  }

  /**************************************************************
  * NANDFLASHEraseBlock() could scrub off all the invalid 	    *
  * block infomation including the factory initial invalid	    *
  * block table information. Per Samsung's K9F1G08 Users Manual,*
  * "Any intentional erasure of the initial invalid block		*
  * information is prohibited.									*
  *	However, during the driver debugging, it may create lot of  *
  *	invalid blocks. Below NANDFLASHEraseBlock() is used to deal *
  * with situation like that.                                   * 
  *																*
  ***************************************************************/
  /* Erase the entire NAND FLASH */
  for ( i = 0; i < NANDFLASH_BLOCKNUM; i++ )
  {
	if ( NANDFLASHEraseBlock( i ) == FALSE )
	{
	  while( 1 );
	} 
  }

  /* For the test program, the pattern for the whole page 2048 bytes
  is organized as: 0x0, 0x1, ... 0xFF, 0x0, 0x01...... */ 
  for ( i = 0; i < NANDFLASH_RW_PAGE_SIZE; i++ )
  {
	ReadBuf[i] = 0;
	WriteBuf[i] = i;
  }

  if ( NANDFLASHCheckValidBlock() == FALSE )
  {
	while ( 1 );		/* Fatal error */
  }

  /* If it's a valid block, program all the pages of this block, 
  read back, and finally validate. */
  /* simple program to program the block 0 and page 0 */
  if ( InvalidBlockTable[0] == 0 )
  {
	if ( NANDFLASHPageProgram( 0, 0, &WriteBuf[0] ) == FALSE )
	{
	  while ( 1 );	/* Fatal error */
	}
	if ( NANDFLASHPageRead( 0, 0, &ReadBuf[0] ) == FALSE )
	{
	  while ( 1 );	/* Fatal error */
	}
	/* Comparison read and write buffer */
	for ( i = 0; i < NANDFLASH_RW_PAGE_SIZE; i++ )
	{
	  if ( ReadBuf[i] != WriteBuf[i] )
	  {
		while ( 1 );	/* Fatal error */
	  }
	}
  }  
  return 0;
}

/*****************************************************************************
**                            End Of File
*****************************************************************************/
