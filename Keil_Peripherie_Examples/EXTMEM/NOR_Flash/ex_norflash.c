/*****************************************************************************
 *   ex_norflash.c:  External FLASH memory module file for NXP LPC23xx/24xx 
 *	 Family Microprocessors
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
#include "ex_norflash.h"

/*****************************************************************************
** Function name:		delay
**
** Descriptions:		for loop delay
**
** parameters:			Delay value in nilo second range			 
** 						
** Returned value:		None
** 
*****************************************************************************/
void delay(DWORD delayCnt)
{
  DWORD i;

  for ( i = 0; i < delayCnt; i++ );
  return;
}

/*****************************************************************************
** Function name:		NORFLASHInit
**
** Descriptions:		initialize external NOR FLASH memory
**
** parameters:			None			 
** 						
** Returned value:		None
** 
*****************************************************************************/
void NORFLASHInit( void )
{
  /**************************************************************************
   * Initialize EMC for NOR FLASH
  **************************************************************************/
  EMC_CTRL = 0x00000001;

  PCONP  |= 0x00000800;		/* Turn On EMC PCLK */
  PINSEL4 = 0x50000000;
  PINSEL5 = 0x05050555;
  PINSEL6 = 0x55555555;
  PINSEL8 = 0x55555555;
  PINSEL9 = 0x50555555;

  delayMs(1, 100);		/* use timer 1 */
  EMC_STA_CFG0      = 0x00000081;
  EMC_STA_WAITWEN0  = 0x2;
  EMC_STA_WAITOEN0  = 0x2;
  EMC_STA_WAITRD0   = 0x1f;
  EMC_STA_WAITPAGE0 = 0x1f;
  EMC_STA_WAITWR0   = 0x1f;
  EMC_STA_WAITTURN0 = 0xf;
  delayMs(1, 10);			/* use timer 1 */
  return;
}

/*****************************************************************************
** Function name:		ToggleBitCheck
**
** Descriptions:		Toggle Bit check if the data is written or erased
**
** parameters:			Address and Expected data			 
** 						
** Returned value:		done(TRUE) or timeout(FALSE)
** 
*****************************************************************************/
DWORD ToggleBitCheck( DWORD Addr, WORD Data )
{
  volatile WORD *ip;
  WORD temp1, temp2;
  DWORD TimeOut = PROGRAM_TIMEOUT;

  while( TimeOut > 0 )
  {
	ip = GET_ADDR(Addr);
	temp1 = *ip;
	ip = GET_ADDR(Addr);
	temp2 = *ip;
  
	if ( (temp1 == temp2) && (temp1 == Data) )
	{
	  return( TRUE );
	}
	TimeOut--;
  }
  return ( FALSE );
}

/*****************************************************************************
** Function name:		NORFLASHCheckID
**
** Descriptions:		Check ID from external NOR FLASH memory
**
** parameters:			None			 
** 						
** Returned value:		Flash ID read is correct or not
** 
*****************************************************************************/
DWORD NORFLASHCheckID( void )
{
  volatile WORD *ip;
  WORD SST_id1, SST_id2;

  /*  Issue the Software Product ID code to 39VF160   */
  ip  = GET_ADDR(0x5555);
  *ip = 0x00AA;
  ip  = GET_ADDR(0x2AAA);
  *ip = 0x0055;
  ip  = GET_ADDR(0x5555);
  *ip = 0x0090;
  delay(10);

  /* Read the product ID from 39VF160 */
  ip  = GET_ADDR(0x0000); 
  SST_id1 = *ip & 0x00FF;
  ip  = GET_ADDR(0x0001);
  SST_id2 = *ip;             

  /* Issue the Soffware Product ID Exit code thus returning the 39VF160 */
  /* to the read operating mode */
  ip  = GET_ADDR(0x5555);
  *ip = 0x00AA;
  ip  = GET_ADDR(0x2AAA);
  *ip = 0x0055;
  ip  = GET_ADDR(0x5555);
  *ip = 0x00F0;    
  delay(10);

  /* Check ID */
  if ((SST_id1 == SST_ID) && (SST_id2 ==SST_39VF160))
	return( TRUE );
  else
	return( FALSE );
}

/*****************************************************************************
** Function name:		NORFLASHErase
**
** Descriptions:		Erase external NOR FLASH memory
**
** parameters:			None			 
** 						
** Returned value:		None
** 
*****************************************************************************/
void NORFLASHErase( void ) 
{
  volatile WORD *ip;

  ip  = GET_ADDR(0x5555);
  *ip = 0x00AA;
  ip  = GET_ADDR(0x2AAA);
  *ip = 0x0055;
  ip  = GET_ADDR(0x5555);
  *ip = 0x0080;
  ip  = GET_ADDR(0x5555);
  *ip = 0x00AA;
  ip  = GET_ADDR(0x2AAA);
  *ip = 0x0055;
  ip  = GET_ADDR(0x5555);
  *ip = 0x0010;
  delayMs(1, 100);				/* Use timer 1 */
  return;

}

/*****************************************************************************
** Function name:		NORFLASHWriteWord
**
** Descriptions:		Program one 16-bit data into external NOR FLASH memory
**						This "WORD" for the external flash is 16 bits!!!
**
** parameters:			DWORD address and WORD data			 
** 						
** Returned value:		Written successful or not
** 
*****************************************************************************/
DWORD NORFLASHWriteWord( DWORD Addr, WORD Data ) 
{
  volatile WORD *ip;

  ip  = GET_ADDR(0x5555);
  *ip = 0x00AA;
  ip  = GET_ADDR(0x2aaa);
  *ip = 0x0055;
  ip  = GET_ADDR(0x5555);
  *ip = 0x00A0;
    
  ip = GET_ADDR(Addr);		/* Program 16-bit word */
  *ip = Data;
  return ( ToggleBitCheck( Addr, Data ) );
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
