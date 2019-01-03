/*****************************************************************************
 *   pwmtest.c:  main C entry file for NXP LPC23xx/24xx Family Microprocessors
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
#include "usbaudio.h"

BYTE  Mute;                                 /* Mute State */
DWORD Volume;                               /* Volume Level */

#if USB_DMA
  #if KEIL_IDE
  DWORD InfoBuf[P_C] __at  DMA_BUF_ADR;       /* Packet Info Buffer */
  short DataBuf[B_S] __at (DMA_BUF_ADR+4*P_C);/* Data Buffer */
  #else
  DWORD *InfoBuf = (DWORD *)(DMA_BUF_ADR);
  short *DataBuf = (short *)(DMA_BUF_ADR + 4*P_C);
  #endif
#else
short DataBuf[B_S];                         /* Data Buffer */
#endif

WORD  DataOut;                              /* Data Out Index */
WORD  DataIn;                               /* Data In Index */

BYTE  DataRun;                              /* Data Stream Run State */

WORD  PotVal;                               /* Potenciometer Value */

DWORD VUM;                                  /* VU Meter */

DWORD Tick;                                 /* Time Tick */


/*
 * Get Potenciometer Value
 */

void get_potval (void) {
  DWORD val;

  AD0CR |= 0x01000000;                      /* Start A/D Conversion */
  do {
    val  = AD0GDR;                          /* Read A/D Data Register */
  } while ((val & 0x80000000) == 0);        /* Wait for end of A/D Conversion */
  AD0CR &= ~0x01000000;                     /* Stop A/D Conversion */
  PotVal = ((val >> 8) & 0xF8) +            /* Extract Potenciometer Value */
           ((val >> 7) & 0x08);
}


/*
 * Timer Counter 0 Interrupt Service Routine
 *   executed each 31.25us (32kHz frequency)
 */

void TIMER0_ISR (void) __irq {
  long  val;
  DWORD cnt;

  if (DataRun) {                            /* Data Stream is running */
    val = DataBuf[DataOut];                 /* Get Audio Sample */
    cnt = (DataIn - DataOut) & (B_S - 1);   /* Buffer Data Count */
    if (cnt == (B_S - P_C*P_S)) {           /* Too much Data in Buffer */
      DataOut++;                            /* Skip one Sample */
    }
    if (cnt > (P_C*P_S)) {                  /* Still enough Data in Buffer */
      DataOut++;                            /* Update Data Out Index */
    }
    DataOut &= B_S - 1;                     /* Adjust Buffer Out Index */
    if (val < 0) VUM -= val;                /* Accumulate Neg Value */
    else         VUM += val;                /* Accumulate Pos Value */
    val  *= Volume;                         /* Apply Volume Level */
    val >>= 16;                             /* Adjust Value */
    val  += 0x8000;                         /* Add Bias */
    val  &= 0xFFFF;                         /* Mask Value */
  } else {
    val = 0x8000;                           /* DAC Middle Point */
  }

  if (Mute) {
    val = 0x8000;                           /* DAC Middle Point */
  }

  DACR = val & 0xFFC0;                      /* Set Speaker Output */

  if ((Tick++ & 0x03FF) == 0) {             /* On every 1024th Tick */
    get_potval();                           /* Get Potenciometer Value */
    if (VolCur == 0x8000) {                 /* Check for Minimum Level */
      Volume = 0;                           /* No Sound */
    } else {
      Volume = VolCur * PotVal;             /* Chained Volume Level */
    }
    val = VUM >> 20;                        /* Scale Accumulated Value */
    VUM = 0;                                /* Clear VUM */
    if (val > 7) val = 7;                   /* Limit Value */
//    IOCLR2 = LEDMSK;                        /* Turn Off all LEDs */
//    IOSET2 = LEDMSK >> (7 - val);           /* LEDs show VU Meter */
  }

  T0IR = 1;                                 /* Clear Interrupt Flag */
  VICVectAddr = 0;                          /* Acknowledge Interrupt */
}



/*****************************************************************************
**   Main Function  main()
******************************************************************************/
int main (void)
{  
  PINSEL1 &= ~0x0030C000;	/* P0.23, A0.0, function 01, P0.26 AOUT, function 10 */
  PINSEL1 |= 0x00204000;

  /* Enable CLOCK into ADC controller */
  PCONP |= (1 << 12);

  AD0CR   = 0x00200E01;		/* ADC: 10-bit AIN0 @ 4MHz */
  DACR    = 0x00008000;		/* DAC Output set to Middle Point */

  T0MR0 = Fcclk/DATA_FREQ - 1;	/* TC0 Match Value 0 */
  T0MCR = 3;					/* TCO Interrupt and Reset on MR0 */
  T0TCR = 1;					/* TC0 Enable */

  install_irq( TIMER0_INT, (void *)TIMER0_ISR, HIGHEST_PRIORITY );
  USB_Init();				/* USB Initialization */
  USB_Connect(TRUE);		/* USB Connect */

  /********* The main Function is an endless loop ***********/ 
  while( 1 ); 
  return 0;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
