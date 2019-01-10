/*----------------------------------------------------------------------------
 *      Name:    serial.h
 *      Purpose: serial port handling
 *      Version: V1.10
 *----------------------------------------------------------------------------
 *      This file is part of the uVision/ARM development tools.
 *      This software may only be used under the terms of a valid, current,
 *      end user licence from KEIL for a compatible version of KEIL software
 *      development tools. Nothing else gives you the right to use it.
 *
 *      Copyright (c) 2005-2008 Keil Software.
 *---------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 Serial interface related prototypes
 *---------------------------------------------------------------------------*/
extern void  ser_OpenPort  (void);
extern void  ser_ClosePort (void); 
extern void  ser_InitPort  (unsigned long baudrate, unsigned int databits, unsigned int parity, unsigned int stopbits);
extern void  ser_AvailChar (int *availChar);
extern int   ser_Write     (const char *buffer, int *length);
extern int   ser_Read      (char *buffer, const int *length);
extern void  ser_LineState (unsigned short *lineState);

