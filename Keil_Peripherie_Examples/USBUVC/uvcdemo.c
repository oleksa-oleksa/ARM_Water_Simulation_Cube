/*----------------------------------------------------------------------------
 *      Name:    uvcdemo.c
 *      Purpose: USB video class Demo
 *      Version: V1.00
 *----------------------------------------------------------------------------
 *      This file is part of the uVision/ARM development tools.
 *      This software may only be used under the terms of a valid, current,
 *      end user licence from KEIL for a compatible version of KEIL software
 *      development tools. Nothing else gives you the right to use it.
 *
 *      Copyright (c) 2005-2007 Keil Software.
 *---------------------------------------------------------------------------*/

#include <LPC23xx.H>                                  /* LPC23xx definitions */

#include "type.h"

#include "usb.h"
#include "usbcfg.h"
#include "usbhw.h"
#include "usbcore.h"
#include "uvc.h"
#include "uvcuser.h"
#include "uvcdemo.h"
#include "LCD.h"


/*----------------------------------------------------------------------------
  Main Program
 *---------------------------------------------------------------------------*/
int main (void) {

  PINSEL10 = 0;                             /* Disable ETM interface */
  FIO2DIR  = LEDMSK;                        /* LED's defined as Outputs */

  LCD_init();                               /* Initialize LCD display module */
  LCD_cur_off();
  LCD_cls();
  LCD_puts("MCB2300 USB UVC");
  LCD_puts("  www.keil.com  ");


  USB_Init();                               /* USB Initialization */
  USB_Connect(TRUE);                        /* USB Connect */

  while (!USB_Configuration) ;              /* wait until USB is configured */

  while (1) {                               /* Loop forever */
    ;
  } // end while											   
} // end main ()

