/**
 * @file   paint_tool.h
 * @brief  RGB colors, predefined painting tools
 * @date   2018-11-29
 * @author Kayoko Abe
 */
 
#ifndef PAINT_TOOL_H_
#define PAINT_TOOL_H_

#define ROW_NUM 32
#define COL_NUM 32

/*****************************************************************************
 * Color Pallet
 *****************************************************************************/
typedef struct
{
    int red;
    int green;
    int blue;
} RGB;

static RGB RGB_BLACK = { .red = 0, .green = 0, .blue = 0 };

static RGB RGB_BLUE = { .red = 0, .green = 0, .blue = 255 };

static RGB RGB_GREEN = { .red = 0, .green = 255, .blue = 0 };

static RGB RGB_SKYBLUE = { .red = 0, .green = 255, .blue = 255 };

static RGB RGB_RED = { .red = 255, .green = 0, .blue = 0 };

static RGB RGB_PINK = { .red = 255, .green = 0, .blue = 255 };

static RGB RGB_YELLOW = { .red = 255, .green = 255, .blue = 0 };

static RGB RGB_WHITE = { .red = 255, .green = 255, .blue = 255 };


/*****************************************************************************
 * Drawing Tools
 *****************************************************************************/
void lp32x32_paint2colors(RGB panel[ROW_NUM][COL_NUM]);

void lp32x32_drawPixel(short x, short y, RGB rgb);

void lp32x32_fillRect(short p0, short p1, short p2, short p3, RGB rgb);

void lp32x32_drawRect(short p0, short p1, short p2, short p3, RGB rgb);

void lp32x32_drawLine(short px_start, short py_start, short px_end, short py_end, RGB rgb);


#endif // PAINT_TOOL_H_
