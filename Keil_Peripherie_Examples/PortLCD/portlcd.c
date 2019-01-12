/*****************************************************************************
 *   portlcd.c:  4-bit port LCD C file for NXP LPC23xx/34xx Family 
 *   Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.07.12  ver 1.00    Prelimnary version, first Release
 *
*****************************************************************************/
#include "LPC23xx.H"                        /* LPC23xx/24xx definitions */
#include "type.h"
#include "irq.h"
#include "portlcd.h"

/* Local variables */
static DWORD lcd_ptr;

/* 8 user defined characters to be loaded into CGRAM (used for bargraph) */
static const BYTE UserFont[8][8] = {
  { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 },
  { 0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10 },
  { 0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18 },
  { 0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C },
  { 0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E },
  { 0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F },
  { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 },
  { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 }
};

/******************************************************************************
** Function name:		delay
**
** Descriptions:		Delay in while loop cycles.		
**
** parameters:			delay counter
** Returned value:		None
** 
******************************************************************************/
void delay (DWORD cnt) 
{
  while (cnt--);
  return;
}

/******************************************************************************
** Function name:		lcd_write_4bit
**
** Descriptions:		Write a 4-bit command to LCD controller.		
**
** parameters:			four bits to write
** Returned value:		None
** 
******************************************************************************/
void lcd_write_4bit( DWORD c ) 
{
  IO1DIR |= LCD_DATA | LCD_CTRL;
  IO1CLR  = LCD_RW   | LCD_DATA;
  IO1SET  = (c & 0xF) << 24;
  IO1SET  = LCD_E;
  delay (10);
  IO1CLR  = LCD_E;
  delay (10);
  return;
}

/******************************************************************************
** Function name:		lcd_write
**
** Descriptions:		Write data/command to LCD controller.		
**
** parameters:			word to write
** Returned value:		None
** 
******************************************************************************/
void lcd_write( DWORD c ) 
{
  lcd_write_4bit (c >> 4);
  lcd_write_4bit (c);
  return;
}

/******************************************************************************
** Function name:		lcd_read_stat
**
** Descriptions:		Read status of LCD controller (ST7066)		
**
** parameters:			None
** Returned value:		status
** 
******************************************************************************/
DWORD lcd_read_stat( void ) 
{
  DWORD stat;

  IO1DIR &= ~LCD_DATA;
  IO1CLR  = LCD_RS;
  IO1SET  = LCD_RW;
  delay (10);
  IO1SET  = LCD_E;
  delay (10);
  stat    = (IO1PIN >> 20) & 0xF0;
  IO1CLR  = LCD_E;
  delay (10);
  IO1SET  = LCD_E;
  delay (10);
  stat   |= (IO1PIN >> 24) & 0xF;
  IO1CLR  = LCD_E;
  return (stat);
}

/******************************************************************************
** Function name:		lcd_wait_busy
**
** Descriptions:		Wait until LCD controller (ST7066) is not busy.		
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void lcd_wait_busy( void ) 
{
  DWORD stat;

  do 
  {
	stat = lcd_read_stat();
  } while (stat & 0x80);               /* Wait for busy flag                */
  return;
}

/******************************************************************************
** Function name:		lcd_write_cmd
**
** Descriptions:		Write command to LCD controller.		
**
** parameters:			command word
** Returned value:		None
** 
******************************************************************************/
void lcd_write_cmd( DWORD c ) 
{
  lcd_wait_busy();
  IO1CLR = LCD_RS;
  lcd_write(c);
  return;
}

/******************************************************************************
** Function name:		lcd_write_data
**
** Descriptions:		Write data to LCD controller.		
**
** parameters:			data word
** Returned value:		None
** 
******************************************************************************/
void lcd_write_data( DWORD d ) 
{
  lcd_wait_busy();
  IO1SET = LCD_RS;
  lcd_write(d);
  return;
}

/******************************************************************************
** Function name:		LCD_init
**
** Descriptions:		Initialize the ST7066 LCD controller to 4-bit mode.		
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void LCD_init( void ) 
{
  PINSEL3 = 0x00000000; 
#if USE_FIO
  SCS |= 0x00000001;	/* set GPIOx to use Fast I/O */
#endif
  IO1DIR |= LCD_CTRL | LCD_DATA;
  IO1CLR  = LCD_RW   | LCD_RS   | LCD_DATA;

  lcd_write_4bit(0x3);                /* Select 4-bit interface            */
  delay (100000);
  lcd_write_4bit(0x3);
  delay (10000);
  lcd_write_4bit(0x3);
  lcd_write_4bit(0x2);

  lcd_write_cmd(0x28);                /* 2 lines, 5x8 character matrix     */
  lcd_write_cmd(0x0e);                /* Display ctrl:Disp/Curs/Blnk=ON    */
  lcd_write_cmd(0x06);                /* Entry mode: Move right, no shift  */

  LCD_load( (BYTE *)&UserFont, sizeof (UserFont) );
  LCD_cls();
  return;
}

/******************************************************************************
** Function name:		LCD_load
**
** Descriptions:		Load user-specific characters into CGRAM		
**
** parameters:			pointer to the buffer and counter
** Returned value:		None
** 
******************************************************************************/
void LCD_load( BYTE *fp, DWORD cnt ) 
{
  DWORD i;

  lcd_write_cmd( 0x40 );	/* Set CGRAM address counter to 0    */
  for (i = 0; i < cnt; i++, fp++)  
  {
	lcd_write_data( *fp );
  }
  return;
}

/******************************************************************************
** Function name:		LCD_gotoxy
**
** Descriptions:		Set cursor position on LCD display. 
**						Left corner: 1,1, right: 16,2 
**
** parameters:			pixel X and Y
** Returned value:		None
** 
******************************************************************************/
void LCD_gotoxy( DWORD x, DWORD y ) 
{
  DWORD c;

  c = --x;
  if (--y) 
  {
	c |= 0x40;
  }
  lcd_write_cmd (c | 0x80);
  lcd_ptr = y*16 + x;
  return;
}

/******************************************************************************
** Function name:		LCD_cls
**
** Descriptions:		Clear LCD display, move cursor to home position.		
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void LCD_cls( void ) 
{
  lcd_write_cmd (0x01);
  LCD_gotoxy (1,1);
  return;
}

/******************************************************************************
** Function name:		LCD_cur_off
**
** Descriptions:		Switch off LCD cursor.
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void LCD_cur_off( void ) 
{
  lcd_write_cmd(0x0c);
  return;
}


/******************************************************************************
** Function name:		LCD_on
**
** Descriptions:		Switch on LCD and enable cursor.		
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void LCD_on( void ) 
{
  lcd_write_cmd (0x0e);
  return;
}

/******************************************************************************
** Function name:		LCD_putc
**
** Descriptions:		Print a character to LCD at current cursor position.		
**
** parameters:			byte character
** Returned value:		None
** 
******************************************************************************/
void LCD_putc( BYTE c ) 
{ 
  if (lcd_ptr == 16) 
  {
	lcd_write_cmd (0xc0);
  }
  lcd_write_data(c);
  lcd_ptr++;
  return;
}

/******************************************************************************
** Function name:		LCD_puts
**
** Descriptions:		Print a string to LCD display.		
**
** parameters:			pointer to the buffer
** Returned value:		None
** 
******************************************************************************/
void LCD_puts ( BYTE *sp ) 
{
  while (*sp) 
  {
	LCD_putc (*sp++);
  }
  return;
}

/******************************************************************************
** Function name:		LCD_bargraph
**
** Descriptions:		Print a bargraph to LCD display.
**						- val:  value 0..100 %
**						- size: size of bargraph 1..16		
**
** parameters:			value and size
** Returned value:		None
** 
******************************************************************************/
void LCD_bargraph( DWORD val, DWORD size ) 
{
  DWORD i;

  val = val * size / 20;               /* Display matrix 5 x 8 pixels       */
  for (i = 0; i < size; i++) 
  {
	if (val > 5) 
	{
	  LCD_putc(5);
	  val -= 5;
	}
	else 
	{
	  LCD_putc(val);
	  break;
    }
  }
  return;
}

/*****************************************************************************
**                            End Of File
******************************************************************************/

