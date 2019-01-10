/*****************************************************************************
 *   ex_nandflash.h:  Header file for NXP LPC23xx/24xx Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2007.01.10  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#ifndef __EX_NANDFLASH_H 
#define __EX_NANDFLASH_H

/*****************************************************************************
 * Defines and typedefs
 ****************************************************************************/
#define K9F1G_CLE   ((volatile BYTE *)0x81100000)
#define K9F1G_ALE	((volatile BYTE *)0x81080000)
#define K9F1G_DATA  ((volatile BYTE *)0x81000000)

#define NANDFLASH_BASE_ADDR			0x00000000
#define NANDFLASH_BLOCKNUM			0x400		/* total 1024 blocks in a device */
#define NANDFLASH_PAGE_PER_BLOCK	0x40		/* total pages in a block */

#define NANDFLASH_PAGE_SIZE			0x840		/* 2112 bytes/page */
#define NANDFLASH_RW_PAGE_SIZE		0x800		/* 2048 bytes/page */

#define K9FXX_ID					0xECF10000	/* Byte 3 and 2 only */

#define K9FXX_READ_1            	0x00                
#define K9FXX_READ_2            	0x30                
#define K9FXX_READ_ID           	0x90                
#define K9FXX_RESET             	0xFF                
#define K9FXX_BLOCK_PROGRAM_1   	0x80                
#define K9FXX_BLOCK_PROGRAM_2   	0x10                
#define K9FXX_BLOCK_ERASE_1     	0x60                
#define K9FXX_BLOCK_ERASE_2     	0xD0                
#define K9FXX_READ_STATUS       	0x70                
#define K9FXX_BUSY              	(1 << 6)            
#define K9FXX_OK                	(1 << 0)            

extern void NANDFLASHInit( void );
extern void NANDFLASHReset( void );
extern void NANDFLASHRBCheck( void );	/* same as CheckBusy, no time out */
extern DWORD NANDFLASHReadID( void );
extern DWORD NANDFLASHReadStatus( DWORD Cmd );
extern DWORD NANDFLASHEraseBlock( DWORD blockNum );
extern DWORD NANDFLASHCheckValidBlock( void );
extern DWORD NANDFLASHPageProgram( DWORD pageNum, DWORD blockNum, BYTE *bufPtr );
extern DWORD NANDFLASHPageRead( DWORD pageNum, DWORD blockNum, BYTE *bufPtr );

#endif /* end __EX_NANDFLASH_H */
/*****************************************************************************
**                            End Of File
******************************************************************************/
