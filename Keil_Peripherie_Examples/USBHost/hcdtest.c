/*****************************************************************************
 *   ssptest.c:  main C entry file for NXP LPC23xx/24xx Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.07.19  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#include "LPC23xx.h"                        /* LPC23xx/24xx definitions */
#include "type.h"
#include "irq.h"
#include "target.h"
#include "timer.h"
#include "usbhost.h"

BYTE *src_addr; 
BYTE *dest_addr;

/******************************************************************************
**   Main Function  main()
******************************************************************************/
int main (void)
{
  DWORD ConnectedFlag = 0x00;

  /* So far, this test program demonstrate how the USB host is 
  initialized, it's not meant to initialize all the queues for
  different endpoints and perform host request for enumeration. */
    
  /* on Embedded Artists LPC2400 OEM board, USB channel A is 
  on USB port 1, USB channel B can be configured as either device
  or host by setting the jumpers P0.14, P1.30, USB-B+, and USB-B-
  on the board. */
  if ( USBHostInit(TYPE_OTG, TYPE_HOST) == FALSE )
  {
	while( 1 );			/* fatal error */
  }
  
  if ( OHCIInit() == FALSE )
  {
	while( 1 );
  }
  
  if ( install_irq(USB_INT,(void *)USBHostHandler,
		HIGHEST_PRIORITY)==FALSE )
  {
	return (FALSE);
  }
  
  /* ConnectedFlag is a one-time event flag, if the RH_PORT_STAT1
  bit 0 is set(connected) and ConnectedFlag bit 0 is 0, the port1
  can be openned now; if the RH_PORT_STAT2 bit 0 is set(connected) 
  and ConnectedFlag bit 1 is 0, the port2 can be openned now. Once
  ConnectedFlag bit 0 and 1 are not, the port don't have to be 
  opened again until it's disconnected first. */ 

  while ( 1 )
  {
	if ( (HC_RH_PORT_STAT1 & 0x01) && !(ConnectedFlag & 0x01) )
	{
	  OHCIPortOpen(1);
	  OHCISetAddress();
	  ConnectedFlag |= 0x01;
	}
	if ( (HC_RH_PORT_STAT2 & 0x01) && !(ConnectedFlag & 0x02) )
	{
	  OHCIPortOpen(2);
	  OHCISetAddress();
	  ConnectedFlag |= 0x02;
	}
  }
  return 0;
}

/******************************************************************************
**                            End Of File
******************************************************************************/

