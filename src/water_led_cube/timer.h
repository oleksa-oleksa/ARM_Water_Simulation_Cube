/*****************************************************************************
 *   timer.h:  Header file for NXP LPC23xx/24xx Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.07.13  ver 1.00    Prelimnary version, first Release
 *   2019.03.16  Modified for project - Heiko Radde
 *
******************************************************************************/
#ifndef __TIMER_H 
#define __TIMER_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* System frequence,should be less than 72MHz. */
#define Fcclk	96000000
#define Fpclk	(Fcclk / 4)

/* depending on the CCLK and PCLK setting, e.g. CCLK = 60Mhz, 
PCLK = 1/4 CCLK, then, 10mSec = 150.000-1 counts */

#define TIME_INTERVAL	(Fpclk/100 - 1)

typedef void (*timer_isr_t)(void);

extern bool init_timer(uint8_t timer_num, timer_isr_t timer_isr);

#endif /* end __TIMER_H */
/*****************************************************************************
**                            End Of File
******************************************************************************/
