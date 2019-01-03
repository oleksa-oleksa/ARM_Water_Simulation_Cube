/*****************************************************************************
 *   i2s.h:  Header file for NXP LPC23xx/24xx Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.07.19  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#ifndef __I2S_H 
#define __I2S_H

#define I2S_DMA_ENABLED		0

#define BUFSIZE				0x200
#define RXFIFO_EMPTY		0
#define TXFIFO_FULL			8

extern void I2SHandler( void ) __irq;
extern void I2SStart( void );
extern void I2SStop( void );
extern DWORD I2SInit( void );

#endif /* end __I2S_H */
/****************************************************************************
**                            End Of File
*****************************************************************************/
