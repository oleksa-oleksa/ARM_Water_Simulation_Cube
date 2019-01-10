/*****************************************************************************
 *   ex_norflash.h:  Header file for NXP LPC23xx/24xx Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2007.01.10  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#ifndef __EX_NORFLASH_H 
#define __EX_NORFLASH_H

/*****************************************************************************
 * Defines and typedefs
 ****************************************************************************/
#define NOR_FLASH_BASE		0x80000000
#define NOR_FLASH_SIZE		0x00100000

#define GET_ADDR(addr)	(volatile WORD *)(NOR_FLASH_BASE | (addr<<1))

#define SECTOR_SIZE		0x800	/* Must be 2048 words for 39VF160 */
#define BLOCK_SIZE		0x8000	/* Must be 32K words for 39VF160  */

#define SST_ID			0xBF    /* SST Manufacturer's ID code   */
#define SST_39VF160		0x234B  /* SST 39VF160 device code      */

#define PROGRAM_TIMEOUT	0x08000000

extern void NORFLASHInit( void );
extern void NORFLASHErase( void );
extern DWORD NORFLASHCheckID( void );
extern DWORD NORFLASHWriteWord( DWORD Addr, WORD Data );
extern DWORD ToggleBitCheck( DWORD Addr, WORD Data ); 

#endif /* end __EX_NORFLASH_H */
/*****************************************************************************
**                            End Of File
******************************************************************************/
