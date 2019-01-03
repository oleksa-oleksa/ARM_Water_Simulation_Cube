/*****************************************************************************
 *   rtc.h:  Header file for NXP LPC23xx/24xx Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.07.13  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#ifndef __PORTLCD_H 
#define __PORTLCD_H

#define USE_FIO		0

#if USE_FIO
#define IO1DIR    FIO1DIR
#define IO1SET    FIO1SET
#define IO1CLR    FIO1CLR
#define IO1PIN    FIO1PIN
#else
#define IO1DIR    IODIR1
#define IO1SET    IOSET1
#define IO1CLR    IOCLR1
#define IO1PIN    IOPIN1
#endif

/* Please note, on old MCB2300 board, the LCD_E bit is p1.30, on the new board
it's p1.31, please check the schematic carefully, and change LCD_CTRL and LCD_E 
accordingly if you have a different board. */ 
/* LCD IO definitions */
#define LCD_E     0x80000000            /* Enable control pin                */
#define LCD_RW    0x20000000            /* Read/Write control pin            */
#define LCD_RS    0x10000000            /* Data/Instruction control          */
#define LCD_CTRL  0xB0000000            /* Control lines mask                */
#define LCD_DATA  0x0F000000            /* Data lines mask                   */

extern void LCD_init(void);
extern void LCD_load(BYTE *fp, DWORD cnt);
extern void LCD_gotoxy(DWORD x, DWORD y);
extern void LCD_cls(void);
extern void LCD_cur_off(void);
extern void LCD_on(void);
extern void LCD_putc(BYTE c);
extern void LCD_puts(BYTE *sp);
extern void LCD_bargraph(DWORD val, DWORD size);

/* Local Function Prototypes */
extern void delay( DWORD cnt );
extern void lcd_write( DWORD c );
extern void lcd_write_4bit( DWORD c );
extern DWORD lcd_read_stat( void );
extern void lcd_write_cmd( DWORD c );
extern void lcd_write_data( DWORD d );
extern void lcd_wait_busy( void );

#endif /* end __PORTLCD_H */
/*****************************************************************************
**                            End Of File
******************************************************************************/
