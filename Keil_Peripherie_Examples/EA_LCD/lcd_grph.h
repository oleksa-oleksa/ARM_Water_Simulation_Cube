/*
    This module implements a linux character device driver for the XXX chip.
    
      Copyright (C) 2006  Embedded Artists AB (www.embeddedartists.com)

    
      This program is free software; you can redistribute it and/or modify
    
      it under the terms of the GNU General Public License as published by
    
      the Free Software Foundation; either version 2 of the License, or
    
      (at your option) any later version.

    
      This program is distributed in the hope that it will be useful,
    
      but WITHOUT ANY WARRANTY; without even the implied warranty of
    
      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    
      GNU General Public License for more details.

    
      You should have received a copy of the GNU General Public License
    
      along with this program; if not, write to the Free Software
    
      Foundation, Inc., 
      59 Temple Place, Suite 330, 
      Boston, MA  02111-1307  USA
 */



#ifndef _LCD_GRPH_

#define _LCD_GRPH_



#define DISPLAY_WIDTH  240

#define DISPLAY_HEIGHT 320

typedef WORD lcd_color_t;


#define   BLACK			0x0000		/*   0,   0,   0 */
#define   NAVY			0x000F      /*   0,   0, 128 */
#define   DARK_GREEN	0x03E0      /*   0, 128,   0 */
#define   DARK_CYAN		0x03EF      /*   0, 128, 128 */
#define   MAROON		0x7800      /* 128,   0,   0 */
#define   PURPLE		0x780F      /* 128,   0, 128 */
#define   OLIVE			0x7BE0      /* 128, 128,   0 */
#define   LIGHT_GRAY	0xC618      /* 192, 192, 192 */
#define   DARK_GRAY		0x7BEF      /* 128, 128, 128 */
#define   BLUE			0x001F      /*   0,   0, 255 */
#define   GREEN			0x07E0      /*   0, 255,   0 */
#define   CYAN          0x07FF      /*   0, 255, 255 */
#define   RED           0xF800      /* 255,   0,   0 */
#define   MAGENTA		0xF81F      /* 255,   0, 255 */
#define   YELLOW		0xFFE0      /* 255, 255, 0   */
#define   WHITE			0xFFFF      /* 255, 255, 255 */


void lcd_movePen(WORD x, WORD y);

void lcd_fillScreen(lcd_color_t color);

void lcd_point(WORD x, WORD y, lcd_color_t color);

void lcd_drawRect(WORD x0, WORD y0, WORD x1, WORD y1, lcd_color_t color);

void lcd_fillRect(WORD x0, WORD y0, WORD x1, WORD y1, lcd_color_t color);

void lcd_line(WORD x0, WORD y0, WORD x1, WORD y1, lcd_color_t color);

void lcd_circle(WORD x0, WORD y0, WORD r, lcd_color_t color);

DWORD lcd_putChar(WORD x, WORD y, BYTE ch);

void lcd_putString(WORD x, WORD y, BYTE *pStr);

void lcd_fontColor(lcd_color_t foreground, lcd_color_t background);

void lcd_picture(WORD x, WORD y, WORD width, WORD height, WORD *pPicture);


void lcd_pictureBegin(WORD x, WORD y, WORD width, WORD height);

void lcd_pictureData(WORD *pPicture, WORD len);

void lcd_pictureEnd(void);



#endif /* _LCD_GRPH_ */
