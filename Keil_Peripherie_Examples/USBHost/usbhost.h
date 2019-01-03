/*****************************************************************************
 *   usbhost.h:  Header file for NXP LPC23xx/24xx Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.07.19  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#ifndef __USBHOST_H__
#define __USBHOST_H__

/* Use the USB RAM as the OHCI HCCA base address */
#define USB_OHCI_ADDR	0x7FD00000
#define USB_OHCI_SIZE	0x4000

#define TYPE_HOST		1
#define TYPE_OTG		2
#define TYPE_DEVICE		3

#define ISP1301_ADDR	0x5A
#define ISP1301_START	1 << 8
#define ISP1301_STOP	1 << 9

#define ISP1301_VENDOR_ID		0x00
#define ISP1301_PRODUCT_ID		0x02
#define ISP1301_MODE1_SET		0x04
#define ISP1301_MODE1_CLR		0x05
#define ISP1301_OTG_CTRL_SET	0x06
#define ISP1301_OTG_CTRL_CLR	0x07
#define ISP1301_OTG_STATUS		0x10
#define ISP1301_MODE2_SET		0x12
#define ISP1301_MODE2_CLR		0x13

#define FM_INTERVAL_FSMPS		0x2374
#define FM_INTERVAL_FI			0x2EDF

#define PWRON_TO_PWRGOOD		50
#define PERIODIC_START			0x3E67
#define LS_THRESHOLD			0x0628				

extern void ISP1301_I2CRequest( DWORD *ReqPtr, DWORD DataLen );
extern void USBHostHandler (void) __irq;
extern DWORD USBHostInit( DWORD port1Type, DWORD port2Type );
extern DWORD OHCIInit( void );
extern void OHCIPortOpen( DWORD portNum );
extern void OHCIPortClose( DWORD portNum );
extern void OHCISetAddress(void);

#endif  /* __USBHOST_H__ */
/*****************************************************************************
**                            End Of File
******************************************************************************/

