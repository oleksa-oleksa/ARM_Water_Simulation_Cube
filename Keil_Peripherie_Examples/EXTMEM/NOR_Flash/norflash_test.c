/*****************************************************************************
 *   norflash_test.c:  External NOR FLASH test main C entry file for NXP 
 *   LPC23xx/24xx Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.07.14  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#include "LPC23xx.h"					/* LPC23xx/24xx definitions */
#include "type.h"
#include "irq.h"
#include "target.h"
#include "ex_norflash.h"

/*****************************************************************************
**   Main Function  main()
******************************************************************************/
int main (void)
{
  DWORD i;
  volatile WORD *ip;
  DWORD NorFlashAdr;

  NORFLASHInit();

  if ( NORFLASHCheckID() == FALSE )
  {
	while ( 1 );		/* Fatal error */
  }

  NORFLASHErase();		/* Chip erase */

  /* Write to flash with pattern 0xAA55 and 0xA55A */
  NorFlashAdr = NOR_FLASH_BASE;
  for ( i = 0; i < NOR_FLASH_SIZE/2; i++ )
  {
	NORFLASHWriteWord( NorFlashAdr, 0xAA55 );
	NorFlashAdr++;
	NORFLASHWriteWord( NorFlashAdr, 0xA55A );
	NorFlashAdr++;
  }

  /* Verify */
  NorFlashAdr = NOR_FLASH_BASE;
  for ( i = 0; i < NOR_FLASH_SIZE/2; i+=2 )
  {
	ip  = GET_ADDR(i); 
	if ( (*ip & 0xFFFF) != 0xAA55 )
	{
	  while ( 1 );	/* Fatal error */
	}
	ip  = GET_ADDR(i+1); 
	if ( (*ip & 0xFFFF) != 0xA55A )
	{
	  while ( 1 );	/* Fatal error */
	}
  }
  return 0;
}

/*****************************************************************************
**                            End Of File
*****************************************************************************/

