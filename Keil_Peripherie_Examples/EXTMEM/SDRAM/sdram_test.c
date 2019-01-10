/*****************************************************************************
 *   sdram_test.c:  External SDRAM test main C entry file for NXP 
 *   LPC23xx/24xx Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.07.14  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#include "LPC23xx.h"                        /* LPC23xx/24xx definitions */
#include "type.h"
#include "irq.h"
#include "target.h"
#include "ex_sdram.h"
#if FLASH_DEBUG
#if EA_BOARD_LPC24XX
#include "uart.h"
#endif
#endif

/*****************************************************************************
**   Main Function  main()
******************************************************************************/
int main (void)
{
  volatile DWORD *wr_ptr; 
  volatile BYTE *char_wr_ptr;
  volatile WORD *short_wr_ptr;
  DWORD  i;

#if FLASH_DEBUG
#if EA_BOARD_LPC24XX
  /* For terminal display, use UART1, 115,200/8/None/1/None */
  UARTInit(1, 115200);
#endif
#endif
  /* initialize memory */
  SDRAMInit();

  wr_ptr = (DWORD *)SDRAM_BASE_ADDR;
  char_wr_ptr = (BYTE *)wr_ptr;
  /* Clear content before 8 bit access test */
  for ( i= 0; i < SDRAM_SIZE/4; i++ )
  {
	*wr_ptr++ = 0;
  }

  /* 8 bit write */
  for (i=0; i<SDRAM_SIZE; i++)
  {
	*char_wr_ptr++ = 0xAA;
	*char_wr_ptr++ = 0x55;
	*char_wr_ptr++ = 0x5A;
	*char_wr_ptr++ = 0xA5;
  }

  /* verifying */
  wr_ptr = (DWORD *)SDRAM_BASE_ADDR;
  for ( i= 0; i < SDRAM_SIZE/4; i++ )
  {
	if ( *wr_ptr != 0xA55A55AA )	/* be aware of endianess */
	{
#if FLASH_DEBUG
#if EA_BOARD_LPC24XX
	  /* byte comparison failure */
	  UARTSend(1, "BBBBBFFFFF", 10 );
#endif
#endif
	  while ( 1 );	/* fatal error */
	}
	wr_ptr++;
  }

#if FLASH_DEBUG
#if EA_BOARD_LPC24XX
  /* byte comparison succeed. */
  UARTSend(1, "HHHHHHHHHH", 10 );
#endif
#endif  
  wr_ptr = (DWORD *)SDRAM_BASE_ADDR;
  short_wr_ptr = (WORD *)wr_ptr;
  /* Clear content before 16 bit access test */
  for ( i= 0; i < SDRAM_SIZE/4; i++ )
  {
	*wr_ptr++ = 0;
  }

  /* 16 bit write */
  for (i=0; i<(SDRAM_SIZE/2); i++)
  {
	*short_wr_ptr++ = 0x5AA5;
	*short_wr_ptr++ = 0xAA55;
  }

  /* Verifying */
  wr_ptr = (DWORD *)SDRAM_BASE_ADDR;
  for ( i= 0; i < SDRAM_SIZE/4; i++ )
  {
	if ( *wr_ptr != 0xAA555AA5 )	/* be aware of endianess */
	{
#if FLASH_DEBUG
#if EA_BOARD_LPC24XX
	  /* 16-bit half word failure */
	  UARTSend(1, "WWWWWFFFFF", 10 );
#endif
#endif
	  while ( 1 );	/* fatal error */
	}
	wr_ptr++;
  }
#if FLASH_DEBUG
#if EA_BOARD_LPC24XX
  /* 16-bit half word comparison succeed. */
  UARTSend(1, "SSSSSSSSSS", 10 );
#endif
#endif
  return 0;
}

/*****************************************************************************
**                            End Of File
*****************************************************************************/

