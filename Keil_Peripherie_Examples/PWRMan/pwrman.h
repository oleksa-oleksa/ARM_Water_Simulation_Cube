/*****************************************************************************
 *   pwrman.h:  Header file for NXP LPC23xx/24xx Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.07.20  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#ifndef __PWRMAN_H 
#define __PWRMAN_H

#define EINT0		0x00000001
#define EINT1		0x00000002
#define EINT2		0x00000004
#define EINT3		0x00000008

#define EINT0_EDGE	0x00000001
#define EINT1_EDGE	0x00000002
#define EINT2_EDGE	0x00000004
#define EINT3_EDGE	0x00000008

#define EINT0_RISING	0x00000001
#define EINT1_RISING	0x00000002
#define EINT2_RISING	0x00000004
#define EINT3_RISING	0x00000008

extern void EINT0_Handler(void) __irq;
extern DWORD EINT0Init( void );
extern void EINT3_Handler(void) __irq;
extern DWORD EINT3Init( void );
extern DWORD PM_Init( void );
extern void Reset_MainClock( void );

#endif /* end __PWRMAN_H */
/****************************************************************************
**                            End Of File
****************************************************************************/
