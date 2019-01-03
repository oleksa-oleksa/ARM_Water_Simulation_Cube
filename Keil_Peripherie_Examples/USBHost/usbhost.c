/*****************************************************************************
 *   usbhost.c:  USB host initialization C file for NXP LPC23xx/24xx Family 
 *   Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2007.07.20  ver 1.00    Prelimnary version, first Release
 *
*****************************************************************************/
#include "LPC23xx.h"	/* 24xx Peripheral Registers */
						/* Note: lpc23xx.h is the superset for
						both LPC23xx and LPC24xx MCUs. */
#include "type.h"
#include "irq.h"
#include "target.h"
#include "timer.h"
#include "usbhost.h"

/* statistics of all the interrupts */
volatile DWORD intOverRunCnt = 0;
volatile DWORD intWriteBackDoneCnt = 0;
volatile DWORD intSOFCnt = 0;
volatile DWORD intResumeDetectCnt = 0;
volatile DWORD intUnrecoverErrCnt = 0;
volatile DWORD intFrameNumOverflowCnt = 0;
volatile DWORD intRHStatusChangeCnt = 0;
volatile DWORD intOwnershipChangeCnt = 0;

volatile DWORD NumDownStreamPorts = 0;

/*****************************************************************************
** Function name:		OHCIIntEnable
**
** Descriptions:		USB host OHCI Interrupt enable
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void OHCIIntEnable(void )
{
  if ( !(HC_INT_EN & 0x80000000) )
  {
	HC_INT_EN |= 0x80000000;
  }
  return;
}

/*****************************************************************************
** Function name:		OHCIIntDisable
**
** Descriptions:		USB host OHCI Interrupt disable
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void OHCIIntDisable(void )
{
  if ( HC_INT_EN & 0x80000000 )
  {
	HC_INT_DIS |= 0x80000000;
  }
  return;
}


/*****************************************************************************
** Function name:		OHCIInit
**
** Descriptions:		USB host OHCI initialization.
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
DWORD OHCIInit( void )
{
  DWORD regVal;

  /* Do a hardware Reset */
  HC_CONTROL = 0;
  delayMs(1, 1);

  /* Reset the Host controller */
  HC_CMD_STAT = 0x01;
  while ( HC_CMD_STAT & 0x01 );

  regVal = HC_REVISION;
  if ( (regVal & 0xFF) != 0x10)
  {
	return( FALSE );
  }

  /* Initialize OHCI Registers */
  /* Make the Host controller Operational, enable all control,
  bulk, and ISO, set CBSR to 0x03 */

  HC_CONTROL = (0x2<<6)|(0x01<<5)|(0x01<<4)|(0x01<<3)|(0x03);

  /* Disable all Interrupts */
  HC_INT_DIS = 0x7F;
  OHCIIntDisable();			/* Disable global MIE bit */
	
  HC_BULK_HEAD_ED = 0;
  HC_CTRL_HEAD_ED = 0;

  HC_HCCA = USB_OHCI_ADDR;

  HC_FM_INTERVAL = (FM_INTERVAL_FSMPS << 16)|FM_INTERVAL_FI;

  /* Use typical value described in OHCI spec. */
  HC_PERIOD_START = PERIODIC_START;
  HC_LS_THRHLD = LS_THRESHOLD;
	
  /* Enable all interrupts including MIE*/
  HC_INT_EN = 0x0000007F;
  HC_INT_STAT = 0x8000007F;		/* clear all interrupts */
  OHCIIntEnable();				/* Eable global MIE bit	*/

  regVal = (PWRON_TO_PWRGOOD << 24) & 0xFF000000;	
  HC_RH_DESCA = regVal;
  /* POTPGT value divided by two as delay */
  regVal = (regVal >> 24) << 1;		
  /* milliseconds, power on to power good time */
  delayMs(1, regVal);
  HC_RH_DESCB = 0x0000;

  /* Set global power, LPSC bit */ 
  HC_RH_STAT = 0x00010000;
  return( TRUE );
}

/*****************************************************************************
** Function name:		OHCIPortOpen
**
** Descriptions:		USB host OHCI port open.
**
** parameters:			port number
** Returned value:		None
** 
*****************************************************************************/
void OHCIPortOpen( DWORD portNum )
{
  if ( portNum == 1 )
  {
	/* Port 1 is connected */
	delayMs(1, 100);		/* wait to eliminate signal glitches */
	HC_RH_PORT_STAT1 |= 0x10;
	delayMs(1, 10);
	/* PRSC should be set after port1 reset is finished. */
	while ( !(HC_RH_PORT_STAT1 & (0x10 << 16)) );
	/* Set bit 8(PPS) and 1(PES), port 1 power is on and enabled */
	HC_RH_PORT_STAT1 = 0x102;
  }
  else if ( portNum == 2 )
  {
	/* Port 2 is connected */
	delayMs(1, 100);		/* wait to eliminate signal glitches */
	HC_RH_PORT_STAT2 |= 0x10;
	delayMs(1, 10);
	/* PRSC should be set after port2 reset is finished. */
	while ( !(HC_RH_PORT_STAT2 & (0x10 << 16)) );
	/* Set bit 8(PPS) and 1(PES), port 2 power is on and enabled */
	HC_RH_PORT_STAT2 = 0x102;
  }
	
  /* OCPM is 1, over current is reported on per-port base
     PSM is 1, each port is powered individually
     NPS is 1, ports are always powered when HC is powered. */
  HC_RH_DESCA = 0x0B01;
  HC_RH_DESCB = 0x0000;
  return;	
}

/*****************************************************************************
** Function name:		OHCIPortClose
**
** Descriptions:		USB host OHCI port close.
**
** parameters:			port number
** Returned value:		None
** 
*****************************************************************************/
void OHCIPortClose( DWORD portNum )
{
  if ( portNum == 1 )
  {
	/* Port 1 is disconnected */
	delayMs(1, 100);		/* wait to eliminate signal glitches */
	HC_RH_PORT_STAT1 |= 0x10;
	delayMs(1, 10);
	/* PRSC should be set after port1 reset is finished. */
	while ( !(HC_RH_PORT_STAT1 & (0x10 << 16)) );
	/* Clear bit 8(PPS) and 1(PES), port 1 power is off and disabled */
	HC_RH_PORT_STAT1 &= ~0x102;
  }
  else if ( portNum == 2 )
  {
	/* Port 2 is disconnected */
	delayMs(1, 100);		/* wait to eliminate signal glitches */
	HC_RH_PORT_STAT2 |= 0x10;
	delayMs(1, 10);
	/* PRSC should be set after port2 reset is finished. */
	while ( !(HC_RH_PORT_STAT2 & (0x10 << 16)) );
	/* Clear bit 8(PPS) and 1(PES), port 2 power is off and disabled */
	HC_RH_PORT_STAT2 &= ~0x102;
  }
  return;	
}

/*****************************************************************************
** Function name:		OHCISetAddress
**
** Descriptions:		USB host OHCI Set device address
**
** parameters:			port type
** Returned value:		None
** 
*****************************************************************************/
void OHCISetAddress( void )
{
  return;  
}

/*****************************************************************************
** Function name:		ISP1301_I2CRequest
**
** Descriptions:		Send a I2c Request to OTG chip ISP 1301.
**						Normally, it's three bytes, in formatting the
**						request, first with a I2C_START, the last with 
**						a I2C_STOP.
**
** parameters:			pointer to the request, data length
** Returned value:		None
** 
*****************************************************************************/
void ISP1301_I2CRequest( DWORD *ReqPtr, DWORD DataLen )
{
  DWORD i;

  for ( i = 0; i < DataLen; i++ )
  {
	OTG_I2C_TX = *(ReqPtr+i);
  }
  /* Wait for TDI bit to be set */ 
  while (!(OTG_I2C_STS & 0x1));

  /* Clear TDI bit */
  OTG_I2C_STS = 0x1;
  return;
}

/*****************************************************************************
** Function name:		USBHostHandler
**
** Descriptions:		USB host interrupt handler.
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void USBHostHandler (void) __irq 
{ 
  OHCIIntDisable();
  if ( HC_INT_STAT & 0x01 )		// Scheduling overrun
  {
	 HC_INT_STAT |= 0x01;
	 intOverRunCnt++;
  }
  if ( HC_INT_STAT & 0x02 )		// Write back done
  {
	HC_INT_STAT |= 0x02;
	intWriteBackDoneCnt++;	
  }
  if ( HC_INT_STAT & 0x04 )		// SOF packet
  {
	HC_INT_STAT |= 0x04;
	intSOFCnt++;	
  }
  if ( HC_INT_STAT & 0x08 )		// Resume detected
  {
	HC_INT_STAT |= 0x08;
	intResumeDetectCnt++;
	OHCIInit();	
  }
  if ( HC_INT_STAT & 0x10 )		// Unrecoverable error
  {
	HC_INT_STAT |= 0x10;
	intUnrecoverErrCnt++;	
  }
  if ( HC_INT_STAT & 0x20 )		// Frame Number overflow
  {
	HC_INT_STAT |= 0x20;
	intFrameNumOverflowCnt++;	
  }
  if ( HC_INT_STAT & 0x40 )		// Root hub status change
  {
	HC_INT_STAT |= 0x40;
	/* Update number of downstream port once RH status changes. */
	NumDownStreamPorts = HC_RH_DESCA & 0xFF;
	intRHStatusChangeCnt++;	
  }
  if ( HC_INT_STAT & 0x80 )		// Ownership change
  {
	HC_INT_STAT |= 0x80;
	intOwnershipChangeCnt++;	
  }
  OHCIIntEnable();
  VICVectAddr = 0;		/* Acknowledge Interrupt */
}

/*****************************************************************************
** Function name:		USBHostInit
**
** Descriptions:		USB host port initialization routine
**						Set USB clock, GPIO pins for USB port1 and USB 
**						port2.
**				
** parameters:			None
** Returned value:		true or false, if the interrupt handler
**						can't be installed correctly, return false.
** 
*****************************************************************************/
DWORD USBHostInit( DWORD port1Type, DWORD port2Type )
{
  DWORD I2CReq[3];

  /* Turn on the USB in PCONP */
  PCONP |= 0x80000000;
	
  /* Start all USB clocks */
  OTG_CLK_CTRL = 0x1F;
  while(OTG_CLK_STAT != 0x1F);

  if ( (port1Type == TYPE_OTG) && (port2Type == TYPE_HOST) )
  {
	/* Configure the pins for port1 OTG, port2 Host port
	P0.0-11	= GPIO,			P0.12    = USB-2:PORT_PWRn
	P0.13    = USB-2:LEDn,	P0.14    = PSB2:HOST_ENn
	P0.15 = GPIO	*/
	PINSEL0 = 0x15000000;

	/* P0.16-28 = GPIO,		P0.29 = USB1:D+
	P0.30    = USB1:D-,	P0.31 = USB2:D+ */
	PINSEL1 = 0x54000000;
	
	/* P1.16-17 = GPIO,			P1.18 = USBOTG:LEDn
	   P1.19-26 = GPIO,			P1.27 = USBOTG:INTn
	   P1.28    = USBOTG:SCL,	P1.29 = USBOTG:SDA
	   P1.30-31 = GPIO,			P1.30 = USB2:POWEREDn
	   P1.31    = USB2:OVR_CRNTn */
	PINSEL3 = 0x55400010;
  }
  else if ( (port1Type == TYPE_HOST) && (port2Type == TYPE_HOST) )
  {
	/* P0.0-11   = GPIO,		P0.12    = USB2:PORT_PWRn
	P0.13        = USB2:LEDn,	P0.14    = PSB2:HOST_ENn
	P0.15 = GPIO	*/
	PINSEL0 = 0x15000000;
	
	/* P0.10 is used to control VBUS for the port 1.
	   P0.14 is used as USB2:HOST_ENn */
	IODIR0 = 0x00000400;	/* P0.10 is output */
	IOCLR0 = (1 << 10);		/* P0.10 is 0, enabling VBUS */

	/* P0.16-28 = GPIO,		P0.29 = USB1:D+
	   P0.30    = USB1:D-,	P0.31 = USB2:D+ */
	PINSEL1 = 0x54000000;
	
	/* P1.16-17 = GPIO,				P1.18 = USB1:LEDn
	   P1.19-21 = GPIO,				P1.22 = USB1:POWEREDn
	   P1.23-24 = GPIO,				P1.25 = USB1:HOST_ENn
	   P1.26    = GPIO,				P1.27 = USB1:OVR_CRNTn
	   P1.28    = SCL-1,			P1.29 = SDA-1
	   P1.30    = USB2:POWEREDn,	P1.31 = USB2:OVR_CRNTn */
	PINSEL3 = 0x55882010;
	
	/* P1.19 cannot be configured as USB1:PORT_PWRn, because this
	pin is also used to control the OE_N pin of ISP1301, which must
	be high to ensure ISP1301 is not driving the USB1-D+ and USB1-D- 
	pins. ISP1301 is configured for USB DAT_SE0 mode in
	order to use its internal pull-down resistors for the USB1-D+ 
	and USB1-D- lines.  So, drive P1.19 high. */
	IODIR1 = (1 << 19); /* P1.19 is output */
	IOSET1 = (1 << 19); /* P1.19 is 1 */

	/* Configure port 1 as host */
	OTG_STAT_CTRL = 0x1;
	
	/* Configure ISP1301 to be in USB DAT_SE0 mode */
	I2CReq[0] = ISP1301_START | ISP1301_ADDR;
	I2CReq[1] = ISP1301_MODE1_SET;
	/* In MODE1 CTRL(0x04) register, set bit 0 and 2 for full-speed 
	and DATA_SE0 mode. */
	I2CReq[2] = ISP1301_STOP | 0x05;
	ISP1301_I2CRequest( I2CReq, 3 );

	/* Turn on the D+ and D- pull-down resistors within ISP1301 */
	I2CReq[0] = ISP1301_START | ISP1301_ADDR;
	I2CReq[1] = ISP1301_OTG_CTRL_SET;
	/* In OTG CTRL SET(0x06) register, set bit 2 and 3 for both D+ D- 
	connected with pull-down. */
	I2CReq[2] = ISP1301_STOP | 0x0C;
	ISP1301_I2CRequest( I2CReq, 3 );
  }
  else if ( (port1Type == TYPE_HOST) && (port2Type == TYPE_DEVICE) )
  {
	/* P0.0-12	= GPIO, 				P0.13 = USB2:LEDn
       P0.14    = USB2:Device_En-Bn,	P0.15 = GPIO */
    PINSEL0 = 0x24000000;

	/* P0.10 is used to control VBUS for the USB port 1. */
	IODIR0 = (1 << 10); /* P0.10 is an output */
	IOCLR0 = (1 << 10); /* P0.10 is 0, enabling USB1 VBUS */
	
	/* P0.16-28 = GPIO,		P0.29 = USB1:D+
	   P0.30    = USB1:D-,	P0.31 = USB2:D+ */
	PINSEL1 = 0x54000000;
	
	/* P1.16-17 = GPIO,			P1.18 = USB1:LEDn
	   P1.19-21 = GPIO,			P1.22 = USB1:POWEREDn
	   P1.23-24 = GPIO,			P1.25 = USB1:HOST_ENn
	   P1.26    = GPIO,			P1.27 = USB1:OVR_CRNTn
	   P1.28    = SCL-1,		P1.29 = SDA-1
       P1.30    = USB2:VBUS-B,	P1.31 = GPIO */
	PINSEL3 = 0x25882010;
	
	/* P1.19 cannot be configured as USB-a:PORT_PWRn, because this
	   pin is also used to control the OE_N pin of ISP1301, which must
	   be high to ensure ISP1301 is not driving the USB1 D+ and D- pins.
	   ISP1301 is configured for USB DAT_SE0 mode in
	   order to use its internal pull-down resistors for the USB1 D+ 
	   and D- lines.  So, drive P1.19 high. */
	IODIR1 = (1 << 19); /* P1.19 is output */
	IOSET1 = (1 << 19); /* P1.19 is 1 */
  }
  else
  {
	/* unknown port1 and port2 combination */
	return( FALSE );
  }

  /* In all configurations, disable the pullups on P1.27 and P1.30 */
  PINMODE3 = 0x20800000;
  return( TRUE );
}

/******************************************************************************
**                            End Of File
******************************************************************************/

