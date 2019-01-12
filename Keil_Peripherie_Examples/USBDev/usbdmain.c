/*****************************************************************************
 *   usbmain.c:  main C entry file for NXP LPC23xx/24xx Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.07.20  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#include "LPC23xx.h"                        /* LPC23xx/24xx definitions */
#include "type.h"
#include "irq.h"
#include "target.h"
#include "timer.h"

#include "usb.h"
#include "usbcfg.h"
#include "usbhw.h"
#include "usbcore.h"
#include "hiduser.h"

extern volatile DWORD SuspendFlag;
extern volatile DWORD timer0_counter;

BYTE InReport;			/* HID Input Report */
						/*   Bit0:    Button */
						/*   Bit1..7: Reserved */
BYTE OutReport;			/* HID Out Report */
						/*   Bit0..7: LEDs */
/*
 *  Get HID Input Report -> InReport
 */
void GetInReport (void) {
  if ( FIO2PIN & (1 << 10) ) {	/* Check if push button, EXTINT0, is pressed */
    InReport = 0x00;
  } else {
    InReport = 0x01;
  }
}

/*
 *  Set HID Output Report <- OutReport
 */
void SetOutReport (void) {
  
  FIO2CLR = 0x00FF;
  FIO2SET = OutReport;	
}

/*****************************************************************************
**   Main Function  main()
******************************************************************************/
int main (void)
{
#if POWERDOWN_MODE_USB_WAKEUP
    DWORD WakeupFlag = 0;

    init_timer(0, TIME_INTERVAL);		/* the timer is for USB suspend and resume */
#endif

#if EA_BOARD_LPC24XX
	USB_Init( EA_BOARD_LPC24XX );
#else
#if KEIL_BOARD_LPC23XX
    USB_Init( KEIL_BOARD_LPC23XX );	/* USB Initialization */
#endif
#endif
    USB_Connect(TRUE);			/* USB Connect */

	FIO2DIR |= 0x000000FF;		/* LEDs, port 2, bit 0~7 output only */

#if POWERDOWN_MODE_USB_WAKEUP
	INTWAKE |= (1 << 5);		/* Wake up from USB activities */
#endif

    /********* The main Function is an endless loop ***********/ 
    while( 1 ) 
    {
#if POWERDOWN_MODE_USB_WAKEUP
	if ( WakeupFlag == 1 )
	{
	    WakeupFlag = 0;
		ConfigurePLL();
#if EA_BOARD_LPC24XX
		USB_Init( EA_BOARD_LPC24XX );
#else
#if KEIL_BOARD_LPC23XX
    	USB_Init( KEIL_BOARD_LPC23XX );	/* USB Initialization */
#endif
#endif
		USB_Connect(TRUE);				/* USB Connect */
	}

	/* bit 8 on USN_INT_STAT is USB NeedClk bit. */
	if ( SuspendFlag == 1 )
	{
		if ( timer0_counter > 200 )
		{
			/* If it's a true suspend, turn off USB device bit 
			in OTG_CLK_CTRL(lpc2400) or USBClkCtrl(lpc2300),
			the USBNeed_clk will go away once USB device is disabled. */
			OTG_CLK_CTRL = 0x00;
			while ( USB_INT_STAT & (1 << 8) );
			WakeupFlag = 1;	
			PCON = 0x2;
		}
	}
#endif
    }
    return 0;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
