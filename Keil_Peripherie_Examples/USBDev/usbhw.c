/*----------------------------------------------------------------------------
 *      U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 *      Name:    USBHW.C
 *      Purpose: USB Hardware Layer Module for Philips LPC214x/LPC318x/23xx/24xx
 *      Version: V1.05
 *----------------------------------------------------------------------------
 *      This software is supplied "AS IS" without any warranties, express,
 *      implied or statutory, including but not limited to the implied
 *      warranties of fitness for purpose, satisfactory quality and
 *      noninfringement. Keil extends you a royalty-free right to reproduce
 *      and distribute executable files created using this software for use
 *      on Philips LPC2xxx microcontroller devices only. Nothing else gives
 *      you the right to use this software.
 *
 *      Copyright (c) 2005-2006 Keil Software.
 *---------------------------------------------------------------------------*/

#include "LPC23xx.h"                        /* LPC23xx/24xx definitions */

#include "type.h"
#include "irq.h"
#include "target.h"
#include "timer.h"

#include "usb.h"
#include "usbcfg.h"
#include "usbreg.h"
#include "usbhw.h"
#include "usbcore.h"
#include "usbuser.h"


volatile DWORD DeviceInterruptCount = 0;
volatile DWORD DevStatusReset = 0;
volatile DWORD DevStatusSuspend = 0;
volatile DWORD DevStatusResume = 0;
volatile DWORD DevStatusConnect = 0;
volatile DWORD DevStatusConnectChange = 0;
volatile DWORD DevStatusSuspendChange = 0;
volatile DWORD DeviceStatusValue = 0;

#if POWERDOWN_MODE_USB_WAKEUP
volatile DWORD SuspendFlag = 0;
extern volatile DWORD timer0_counter;
#endif

#define EP_MSK_CTRL 0x0001      /* Control Endpoint Logical Address Mask */
#define EP_MSK_BULK 0xC924      /* Bulk Endpoint Logical Address Mask */
#define EP_MSK_INT  0x4492      /* Interrupt Endpoint Logical Address Mask */
#define EP_MSK_ISO  0x1248      /* Isochronous Endpoint Logical Address Mask */


#if USB_DMA

DWORD UDCA[USB_EP_NUM] __at USB_RAM_ADR;    /* UDCA in USB RAM */
DWORD udca[USB_EP_NUM];                     /* UDCA saved values */

DWORD DDMemMap[2];                          /* DMA Descriptor Memory Usage */

#endif


/*
 *  Get Endpoint Physical Address
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    Endpoint Physical Address
 */

DWORD EPAdr (DWORD EPNum) 
{
  DWORD val;

  val = (EPNum & 0x0F) << 1;
  if (EPNum & 0x80) {
    val += 1;
  }
  return (val);
}


/*
 *  Write Command
 *    Parameters:      cmd:   Command
 *    Return Value:    None
 */
void WrCmd (DWORD cmd) 
{
  DEV_INT_CLR = CCEMTY_INT | CDFULL_INT;
  CMD_CODE = cmd;
  while ((DEV_INT_STAT & CCEMTY_INT) == 0);
}


/*
 *  Write Command Data
 *    Parameters:      cmd:   Command
 *                     val:   Data
 *    Return Value:    None
 */
void WrCmdDat (DWORD cmd, DWORD val) 
{
  DEV_INT_CLR = CCEMTY_INT;
  CMD_CODE = cmd;
  while ((DEV_INT_STAT & CCEMTY_INT) == 0);
  DEV_INT_CLR = CCEMTY_INT;
  CMD_CODE = val;
  while ((DEV_INT_STAT & CCEMTY_INT) == 0);
}


/*
 *  Read Command Data
 *    Parameters:      cmd:   Command
 *    Return Value:    Data Value
 */
DWORD RdCmdDat (DWORD cmd) 
{
  DEV_INT_CLR = CCEMTY_INT | CDFULL_INT;
  CMD_CODE = cmd;
  while ((DEV_INT_STAT & CDFULL_INT) == 0);
  return (CMD_DATA);
}


/*
 *  USB Initialize Function
 *   Called by the User to initialize USB
 *    Return Value:    None
 */
void USB_Init (DWORD BoardType) 
{
  DeviceInterruptCount = 0;
  DevStatusReset = 0;
  DevStatusSuspend = 0;
  DevStatusResume = 0;
  DevStatusConnect = 0;
  DevStatusConnectChange = 0;
  SuspendFlag = 0;

  if ( BoardType == ENG_BOARD_LPC24XX )
  {
	OTG_CLK_CTRL = 0x1B;	/* This module is sent for USB device compliance 
							test. Host and Device are enabled. The host enabling is
							to address USB needClk issue. */ 
	while ( (OTG_CLK_STAT & 0x1B) != 0x1B );
	/* Configure port 2 (side B) to be used as device */
	OTG_STAT_CTRL = 0x03;
 
	// P0.13   = USB-b:LEDn, P0.14   = USB-b:Device_En-Bn
	PINSEL0 &= ~0x3C000000;
	PINSEL0 |= 0x24000000;

	// P0.31    = USB:D+B
	PINSEL1 &= ~0xC0000000;
	PINSEL1 |= 0x40000000;

	// P1.30    = USB-b:VBUS-B
	PINSEL3 &= ~0x30000000;
	PINSEL3 |= 0x20000000;

	// Drive the host VBUS power switch enables low, to turn it off.
	IODIR0 |= ((1 << 12)|(1 << 19)); // P0.12 and P0.19 are output
	IOCLR0 |= ((1 << 12)|(1 << 19)); // P0.12 and P0.19 are 0
  }
  else if ( BoardType == KEIL_BOARD_LPC23XX )
  {
	OTG_CLK_CTRL = 0x12;	/* This module is sent for USB device compliance 
							test. Host and Device are enabled. The host enabling is
							to address USB needClk issue. */ 
	while ( (OTG_CLK_STAT & 0x12) != 0x12 );
	/* For both 100-pin and 144 pin package, if U1 is used, both host and device 
	clock need to be enabled, on 144-pin package, if U2 is used, OTG clock needs
	to enabled as well. In this case, set OTG_CLK_CTRL to 0x1F. */
#if 0
	/* Below lines will be needed if USB_U2 is used as USB Device. for old MCB2300
	board, U2 is used as USB device, on new MCB2300, U1 is used as USB device. */
	/* In order to setup OTG_STAT_CTRL register, OTG clock needs to be enabled as 
	well: */
	OTG_CLK_CTRL = 0x1B;
	while ( (OTG_CLK_STAT & 0x1B) != 0x1B ); 
	OTG_STAT_CTRL = 0x03;		/* This has to be set after OTG_CLK_CTRL configuration. */
#endif
 
	/* P2.9 is USB1 SoftConnect, P1.18 is GoodLink, P0.29 is USB1_D+, 
	P0.30 is USB1_D-. PINSEL1 bit 26~27 0x01, bit 28~29 0x01. 
	PINSEL3 bit 4~5 0x01. PINSEL4, bit 18~19 0x01. */
	PINSEL1 &= ~0x3C000000;
	PINSEL1 |= 0x14000000;

	/* P1.30 is VBus, for testing purpose, I will try both VBus enable and
	GPIO for polling. P1.30, PINSEL3 28~29, 0x10. It should not make any difference
	on the Keil MCB2300 board. */
	PINSEL3 &= ~0x30000030;
	PINSEL3 |= 0x20000010;

	/* The SoftConnect pin for LPC2300 on USB_U1 doesn't work for now, make it GPIO 
	and make it low to turn on pull-up on D+. Once it's fixed, make #if 1 and set it 
	as SoftConn Pin. */
#if 0
	/* This is for rev. "A". */
	PINSEL4 &= ~0x000C0000;
	PINSEL4 |= 0x000040000;
#else
	/* This is for rev. "-" */
	PINSEL4 &= ~0x000C0000;
	FIO2DIR |= (1 << 9);
	FIO2CLR = 1 << 9;
#endif
  }
  else if ( BoardType == EA_BOARD_LPC24XX )
  {
	/* On Embedded Artists's LPC24xx OEM board, U2 is used as USB device. */
	/* In order to setup OTG_STAT_CTRL register, OTG clock needs to be enabled 
	as well: */
	OTG_CLK_CTRL = 0x1B;
	while ( (OTG_CLK_STAT & 0x1B) != 0x1B ); 
	OTG_STAT_CTRL = 0x03;	/* This has to be set after OTG_CLK_CTRL configuration. */
 
	/* On USB port2, P0.13 is GoodLink, P0.14 is SoftConnect,
	P0.31 is D+, D- is a dedicated pin which doesn't belong to any port, 
	PINSEL0 bit 26~27 0x01. PINSEL0, bit 28~29 0x10.
	PINSEL1 bit 30~31 0x01. */
	PINSEL0 &= ((0x03 << 26) | (0x03 << 28));
	PINSEL0 |= ((0x01 << 26) | (0x02 << 28));
	PINSEL1 &= 0x3FFFFFFF;
	PINSEL1 |= (0x01 << 30);

	/* P1.30 is VBus, for testing purpose, I will try both VBus enable and
	GPIO for polling. P1.30, PINSEL3 28~29, 0x10. It should not make any difference
	on the Keil MCB2300 board. */
	PINSEL3 &= ~0x30000030;
	PINSEL3 |= 0x20000010;

	/* The SoftConnect pin for LPC2300 on USB_U1 doesn't work for now, make it GPIO 
	and make it low to turn on pull-up on D+. Once it's fixed, make #if 1 and set it 
	as SoftConn Pin. */
#if 0
	/* This is for rev. "A". */
	PINSEL4 &= ~0x000C0000;
	PINSEL4 |= 0x000040000;
#else
	/* This is for rev. "-". */
	PINSEL4 &= ~0x000C0000;
	FIO2DIR |= (1 << 9);
	FIO2CLR = 1 << 9;
#endif
  }
	
  install_irq( USB_INT, (void *)USB_ISR, HIGHEST_PRIORITY );
    
  USB_Reset();
  USB_SetAddress(0);

//  DEV_INT_EN = DEV_STAT_INT;	/* Enable Device Status Interrupt */

  return;

}


/*
 *  USB Connect Function
 *   Called by the User to Connect/Disconnect USB
 *    Parameters:      con:   Connect/Disconnect
 *    Return Value:    None
 */

void USB_Connect (BOOL con) {
  WrCmdDat(CMD_SET_DEV_STAT, DAT_WR_BYTE(con ? DEV_CON : 0));
}


/*
 *  USB Reset Function
 *   Called automatically on USB Reset
 *    Return Value:    None
 */

void USB_Reset (void) {
#if USB_DMA
  DWORD n;
#endif

  EP_INDEX = 0;
  MAXPACKET_SIZE = USB_MAX_PACKET0;
  EP_INDEX = 1;
  MAXPACKET_SIZE = USB_MAX_PACKET0;
  while ((DEV_INT_STAT & EP_RLZED_INT) == 0);

  EP_INT_CLR  = 0xFFFFFFFF;
  EP_INT_EN   = 0xFFFFFFFF ^ USB_DMA_EP;
  DEV_INT_CLR = 0xFFFFFFFF;
  DEV_INT_EN  = DEV_STAT_INT    | EP_SLOW_INT    |
               (USB_SOF_EVENT   ? FRAME_INT : 0) |
               (USB_ERROR_EVENT ? ERR_INT   : 0);
#if USB_DMA
  UDCA_HEAD   = USB_RAM_ADR;
  DMA_REQ_CLR = 0xFFFFFFFF;
  EP_DMA_DIS  = 0xFFFFFFFF;
  EP_DMA_EN   = USB_DMA_EP;
  EOT_INT_CLR = 0xFFFFFFFF;
  NDD_REQ_INT_CLR = 0xFFFFFFFF;
  SYS_ERR_INT_CLR = 0xFFFFFFFF;
  DMA_INT_EN  = 0x00000007;
  DDMemMap[0] = 0x00000000;
  DDMemMap[1] = 0x00000000;
  for (n = 0; n < USB_EP_NUM; n++) {
    udca[n] = 0;
    UDCA[n] = 0;
  }
#endif
}


/*
 *  USB Suspend Function
 *   Called automatically on USB Suspend
 *    Return Value:    None
 */

void USB_Suspend (void) {
  /* Performed by Hardware */
#if POWERDOWN_MODE_USB_WAKEUP
  timer0_counter = 0;
  enable_timer( 0 );
  
  if ( SuspendFlag == 0 ) {
	SuspendFlag = 1;
  }
#endif
}


/*
 *  USB Resume Function
 *   Called automatically on USB Resume
 *    Return Value:    None
 */

void USB_Resume (void) {
  /* Performed by Hardware */
#if POWERDOWN_MODE_USB_WAKEUP
  disable_timer( 0 );
  timer0_counter = 0;
  if ( SuspendFlag == 1 ) {
	SuspendFlag = 0;
  }
#endif
}


/*
 *  USB Remote Wakeup Function
 *   Called automatically on USB Remote Wakeup
 *    Return Value:    None
 */

void USB_WakeUp (void) {

  if (USB_DeviceStatus & USB_GETSTATUS_REMOTE_WAKEUP) {
    WrCmdDat(CMD_SET_DEV_STAT, DAT_WR_BYTE(DEV_CON));
  }
}


/*
 *  USB Remote Wakeup Configuration Function
 *    Parameters:      cfg:   Enable/Disable
 *    Return Value:    None
 */

void USB_WakeUpCfg (BOOL cfg) {
  cfg = cfg;  /* Not needed */
}


/*
 *  USB Set Address Function
 *    Parameters:      adr:   USB Address
 *    Return Value:    None
 */

void USB_SetAddress (DWORD adr) {
  WrCmdDat(CMD_SET_ADDR, DAT_WR_BYTE(DEV_EN | adr)); /* Don't wait for next */
  WrCmdDat(CMD_SET_ADDR, DAT_WR_BYTE(DEV_EN | adr)); /*  Setup Status Phase */
}


/*
 *  USB Configure Function
 *    Parameters:      cfg:   Configure/Deconfigure
 *    Return Value:    None
 */

void USB_Configure (BOOL cfg) {

  WrCmdDat(CMD_CFG_DEV, DAT_WR_BYTE(cfg ? CONF_DVICE : 0));

  REALIZE_EP = 0x00000003;
  while ((DEV_INT_STAT & EP_RLZED_INT) == 0);
  DEV_INT_CLR = EP_RLZED_INT;
}


/*
 *  Configure USB Endpoint according to Descriptor
 *    Parameters:      pEPD:  Pointer to Endpoint Descriptor
 *    Return Value:    None
 */

void USB_ConfigEP (USB_ENDPOINT_DESCRIPTOR *pEPD) {
  DWORD num;

  num = EPAdr(pEPD->bEndpointAddress);
  REALIZE_EP |= (1 << num);
  EP_INDEX = num;
  MAXPACKET_SIZE = pEPD->wMaxPacketSize;
  while ((DEV_INT_STAT & EP_RLZED_INT) == 0);
  DEV_INT_CLR = EP_RLZED_INT;
}


/*
 *  Set Direction for USB Control Endpoint
 *    Parameters:      dir:   Out (dir == 0), In (dir <> 0)
 *    Return Value:    None
 */

void USB_DirCtrlEP (DWORD dir) {
  dir = dir;  /* Not needed */
}


/*
 *  Enable USB Endpoint
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */

void USB_EnableEP (DWORD EPNum) {
  WrCmdDat(CMD_SET_EP_STAT(EPAdr(EPNum)), DAT_WR_BYTE(0));
}


/*
 *  Disable USB Endpoint
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */

void USB_DisableEP (DWORD EPNum) {
  WrCmdDat(CMD_SET_EP_STAT(EPAdr(EPNum)), DAT_WR_BYTE(EP_STAT_DA));
}


/*
 *  Reset USB Endpoint
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */

void USB_ResetEP (DWORD EPNum) {
  WrCmdDat(CMD_SET_EP_STAT(EPAdr(EPNum)), DAT_WR_BYTE(0));
}


/*
 *  Set Stall for USB Endpoint
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */

void USB_SetStallEP (DWORD EPNum) {
  WrCmdDat(CMD_SET_EP_STAT(EPAdr(EPNum)), DAT_WR_BYTE(EP_STAT_ST));
}


/*
 *  Clear Stall for USB Endpoint
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */

void USB_ClrStallEP (DWORD EPNum) {
  WrCmdDat(CMD_SET_EP_STAT(EPAdr(EPNum)), DAT_WR_BYTE(0));
}


/*
 *  Read USB Endpoint Data
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *                     pData: Pointer to Data Buffer
 *    Return Value:    Number of bytes read
 */

DWORD USB_ReadEP (DWORD EPNum, BYTE *pData) {
  DWORD cnt, n;

  USB_CTRL = ((EPNum & 0x0F) << 2) | CTRL_RD_EN;

  do {
    cnt = RX_PLENGTH;
  } while ((cnt & PKT_RDY) == 0);
  cnt &= PKT_LNGTH_MASK;

  for (n = 0; n < (cnt + 3) / 4; n++) {
    *((__packed DWORD *)pData) = RX_DATA;
    pData += 4;
  }

  USB_CTRL = 0;

  if (((EP_MSK_ISO >> EPNum) & 1) == 0) {   /* Non-Isochronous Endpoint */
    WrCmd(CMD_SEL_EP(EPAdr(EPNum)));
    WrCmd(CMD_CLR_BUF);
  }

  return (cnt);
}


/*
 *  Write USB Endpoint Data
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *                     pData: Pointer to Data Buffer
 *                     cnt:   Number of bytes to write
 *    Return Value:    Number of bytes written
 */

DWORD USB_WriteEP (DWORD EPNum, BYTE *pData, DWORD cnt) {
  DWORD n;

  USB_CTRL = ((EPNum & 0x0F) << 2) | CTRL_WR_EN;

  TX_PLENGTH = cnt;

  for (n = 0; n < (cnt + 3) / 4; n++) {
    TX_DATA = *((__packed DWORD *)pData);
    pData += 4;
  }

  USB_CTRL = 0;

  WrCmd(CMD_SEL_EP(EPAdr(EPNum)));
  WrCmd(CMD_VALID_BUF);

  return (cnt);
}


#if USB_DMA

/* DMA Descriptor Memory Layout */
const DWORD DDAdr[2] = { DD_NISO_ADR, DD_ISO_ADR };
const DWORD DDSz [2] = { 16,          20         };

/*
 *  Setup USB DMA Transfer for selected Endpoint
 *    Parameters:      EPNum: Endpoint Number
 *                     pDD: Pointer to DMA Descriptor
 *    Return Value:    TRUE - Success, FALSE - Error
 */

BOOL USB_DMA_Setup(DWORD EPNum, USB_DMA_DESCRIPTOR *pDD) {
  DWORD num, ptr, nxt, iso, n;

  iso = pDD->Cfg.Type.IsoEP;                /* Iso or Non-Iso Descriptor */
  num = EPAdr(EPNum);                       /* Endpoint's Physical Address */

  ptr = 0;                                  /* Current Descriptor */
  nxt = udca[num];                          /* Initial Descriptor */
  while (nxt) {                             /* Go through Descriptor List */
    ptr = nxt;                              /* Current Descriptor */
    if (!pDD->Cfg.Type.Link) {              /* Check for Linked Descriptors */
      n = (ptr - DDAdr[iso]) / DDSz[iso];   /* Descriptor Index */
      DDMemMap[iso] &= ~(1 << n);           /* Unmark Memory Usage */
    }
    nxt = *((DWORD *)ptr);                  /* Next Descriptor */
  }

  for (n = 0; n < 32; n++) {                /* Search for available Memory */
    if ((DDMemMap[iso] & (1 << n)) == 0) {
      break;                                /* Memory found */
    }
  }
  if (n == 32) return (FALSE);              /* Memory not available */

  DDMemMap[iso] |= 1 << n;                  /* Mark Memory Usage */
  nxt = DDAdr[iso] + n * DDSz[iso];         /* Next Descriptor */

  if (ptr && pDD->Cfg.Type.Link) {
    *((DWORD *)(ptr + 0))  = nxt;           /* Link in new Descriptor */
    *((DWORD *)(ptr + 4)) |= 0x00000004;    /* Next DD is Valid */
  } else {
    udca[num] = nxt;                        /* Save new Descriptor */
    UDCA[num] = nxt;                        /* Update UDCA in USB */
  }

  /* Fill in DMA Descriptor */
  *(((DWORD *)nxt)++) =  0;                 /* Next DD Pointer */
  *(((DWORD *)nxt)++) =  pDD->Cfg.Type.ATLE |
                       (pDD->Cfg.Type.IsoEP << 4) |
                       (pDD->MaxSize <<  5) |
                       (pDD->BufLen  << 16);
  *(((DWORD *)nxt)++) =  pDD->BufAdr;
  *(((DWORD *)nxt)++) =  pDD->Cfg.Type.LenPos << 8;
  if (iso) {
    *((DWORD *)nxt) =  pDD->InfoAdr;
  }

  return (TRUE); /* Success */
}


/*
 *  Enable USB DMA Endpoint
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */

void USB_DMA_Enable (DWORD EPNum) {
  EP_DMA_EN = 1 << EPAdr(EPNum);
}


/*
 *  Disable USB DMA Endpoint
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */

void USB_DMA_Disable (DWORD EPNum) {
  EP_DMA_DIS = 1 << EPAdr(EPNum);
}


/*
 *  Get USB DMA Endpoint Status
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    DMA Status
 */

DWORD USB_DMA_Status (DWORD EPNum) {
  DWORD ptr, val;

  ptr = UDCA[EPAdr(EPNum)];                 /* Current Descriptor */
  if (ptr == 0) return (USB_DMA_INVALID);

  val = *((DWORD *)(ptr + 3*4));            /* Status Information */
  switch ((val >> 1) & 0x0F) {
    case 0x00:                              /* Not serviced */
      return (USB_DMA_IDLE);
    case 0x01:                              /* Being serviced */
      return (USB_DMA_BUSY);
    case 0x02:                              /* Normal Completition */
      return (USB_DMA_DONE);
    case 0x03:                              /* Data Under Run */
      return (USB_DMA_UNDER_RUN);
    case 0x08:                              /* Data Over Run */
      return (USB_DMA_OVER_RUN);
    case 0x09:                              /* System Error */
      return (USB_DMA_ERROR);
  }

  return (USB_DMA_UNKNOWN);
}


/*
 *  Get USB DMA Endpoint Current Buffer Address
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    DMA Address (or -1 when DMA is Invalid)
 */

DWORD USB_DMA_BufAdr (DWORD EPNum) {
  DWORD ptr, val;

  ptr = UDCA[EPAdr(EPNum)];                 /* Current Descriptor */
  if (ptr == 0) return (-1);                /* DMA Invalid */

  val = *((DWORD *)(ptr + 2*4));            /* Buffer Address */

  return (val);                             /* Current Address */
}


/*
 *  Get USB DMA Endpoint Current Buffer Count
 *   Number of transfered Bytes or Iso Packets
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    DMA Count (or -1 when DMA is Invalid)
 */

DWORD USB_DMA_BufCnt (DWORD EPNum) {
  DWORD ptr, val;

  ptr = UDCA[EPAdr(EPNum)];                 /* Current Descriptor */
  if (ptr == 0) return (-1);                /* DMA Invalid */

  val = *((DWORD *)(ptr + 3*4));            /* Status Information */

  return (val >> 16);                       /* Current Count */
}


#endif /* USB_DMA */


/*
 *  Get USB Last Frame Number
 *    Parameters:      None
 *    Return Value:    Frame Number
 */

DWORD USB_GetFrame (void) {
  DWORD val;

  WrCmd(CMD_RD_FRAME);
  val = RdCmdDat(DAT_RD_FRAME);
  val = val | (RdCmdDat(DAT_RD_FRAME) << 8);

  return (val);
}


/*
 *  USB Interrupt Service Routine
 */

void USB_ISR (void) __irq
{
  DWORD disr, val, n, m;

  disr = DEV_INT_STAT;                      /* Device Interrupt Status */
  DEV_INT_CLR = disr;                       /* A known issue on LPC214x */
  DeviceStatusValue = disr;

  /* Device Status Interrupt (Reset, Suspend/Resume, Connect change) */
  if (disr & DEV_STAT_INT) {
	DeviceInterruptCount++;

    WrCmd(CMD_GET_DEV_STAT);
    val = RdCmdDat(DAT_GET_DEV_STAT);       /* Device Status */
  	if ( val & DEV_RST )
		DevStatusReset++;
	if ( val & DEV_CON_CH )
		DevStatusConnectChange++;
	if ( val & DEV_CON )
		DevStatusConnect++;
	if ( val & DEV_SUS_CH ) {
	    DevStatusSuspendChange++;
	if ( val & DEV_SUS )
		DevStatusSuspend++;
	else
		DevStatusResume++;
	}

    if (val & DEV_RST) {                    /* Reset */
      USB_Reset();
#if   USB_RESET_EVENT
      USB_Reset_Event();
#endif
//      goto isr_end;
    }

    if (val & DEV_SUS_CH) {                 /* Suspend/Resume */
      if (val & DEV_SUS) {                  /* Suspend */
        USB_Suspend();
#if     USB_SUSPEND_EVENT
        USB_Suspend_Event();
#endif
      } else {                              /* Resume */
		USB_Resume();
#if     USB_RESUME_EVENT
        USB_Resume_Event();
#endif
      }
//      goto isr_end;
    }
    
    if (val & DEV_CON_CH ) {                 /* Connect change */
#if   USB_POWER_EVENT
      USB_Power_Event(val & DEV_CON);
#endif
//      goto isr_end;
    }
    goto isr_end;
  }

#if USB_SOF_EVENT
  /* Start of Frame Interrupt */
  if (disr & FRAME_INT) {
    USB_SOF_Event();
  }
#endif

#if USB_ERROR_EVENT
  /* Error Interrupt */
  if (disr & ERR_INT) {
    WrCmd(CMD_RD_ERR_STAT);
    val = RdCmdDat(DAT_RD_ERR_STAT);
    USB_Error_Event(val);
    goto isr_end;
  }
#endif

  /* Endpoint's Slow Interrupt */
  if (disr & EP_SLOW_INT) {

    while (EP_INT_STAT) {                   /* Endpoint Interrupt Status */

      for (n = 0; n < USB_EP_NUM; n++) {    /* Check All Endpoints */
        if (EP_INT_STAT & (1 << n)) {
          m = n >> 1;

          EP_INT_CLR = 1 << n;
          while ((DEV_INT_STAT & CDFULL_INT) == 0);
          val = CMD_DATA;

          if ((n & 1) == 0) {               /* OUT Endpoint */
            if (n == 0) {                   /* Control OUT Endpoint */
              if (val & EP_SEL_STP) {       /* Setup Packet */
                if (USB_P_EP[0]) {
                  USB_P_EP[0](USB_EVT_SETUP);
                  continue;
                }
              }
            }
            if (USB_P_EP[m]) {
              USB_P_EP[m](USB_EVT_OUT);
            }
          } else {                          /* IN Endpoint */
            if (USB_P_EP[m]) {
              USB_P_EP[m](USB_EVT_IN);
            }
          }
        }
      }
    }
  }

#if USB_DMA

  if (DMA_INT_STAT & 0x00000001) {          /* End of Transfer Interrupt */
    val = EOT_INT_STAT;
    for (n = 2; n < USB_EP_NUM; n++) {      /* Check All Endpoints */
      if (val & (1 << n)) {
        m = n >> 1;
        if ((n & 1) == 0) {                 /* OUT Endpoint */
          if (USB_P_EP[m]) {
            USB_P_EP[m](USB_EVT_OUT_DMA_EOT);
          }
        } else {                            /* IN Endpoint */
          if (USB_P_EP[m]) {
            USB_P_EP[m](USB_EVT_IN_DMA_EOT);
          }
        }
      }
    }
    EOT_INT_CLR = val;
  }

  if (DMA_INT_STAT & 0x00000002) {          /* New DD Request Interrupt */
    val = NDD_REQ_INT_STAT;
    for (n = 2; n < USB_EP_NUM; n++) {      /* Check All Endpoints */
      if (val & (1 << n)) {
        m = n >> 1;
        if ((n & 1) == 0) {                 /* OUT Endpoint */
          if (USB_P_EP[m]) {
            USB_P_EP[m](USB_EVT_OUT_DMA_NDR);
          }
        } else {                            /* IN Endpoint */
          if (USB_P_EP[m]) {
            USB_P_EP[m](USB_EVT_IN_DMA_NDR);
          }
        }
      }
    }
    NDD_REQ_INT_CLR = val;
  }

  if (DMA_INT_STAT & 0x00000004) {          /* System Error Interrupt */
    val = SYS_ERR_INT_STAT;
    for (n = 2; n < USB_EP_NUM; n++) {      /* Check All Endpoints */
      if (val & (1 << n)) {
        m = n >> 1;
        if ((n & 1) == 0) {                 /* OUT Endpoint */
          if (USB_P_EP[m]) {
            USB_P_EP[m](USB_EVT_OUT_DMA_ERR);
          }
        } else {                            /* IN Endpoint */
          if (USB_P_EP[m]) {
            USB_P_EP[m](USB_EVT_IN_DMA_ERR);
          }
        }
      }
    }
    SYS_ERR_INT_CLR = val;
  }
#endif /* USB_DMA */

isr_end:
//  DEV_INT_CLR = disr;
  VICVectAddr = 0;		/* Acknowledge Interrupt */
  return;
}
