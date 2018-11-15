/**
 * @file   led32x32.h
 * @brief  Library for one RGB LED matrix panel
 * @date   2018-11-08
 * @author Kayoko Abe
 */
 
#ifndef LED32X32_H_
#define LED32X32_H_

#include <LPC23xx.H>

/********************************************************************
 * Configuration for PWM1 on port 2
 ********************************************************************/
#define ROW_NUM 32
#define COL_NUM 32

#define LED32X32_RGBPIN_SETTER   FIO2SET
#define LED32X32_RGBPIN_CLEANER  FIO2CLR
#define LED32X32_CTRLPIN_SETTER  FIO3SET
#define LED32X32_CTRLPIN_CLEANER FIO3CLR

#define LED32X32_PIN_R1 0
#define LED32X32_PIN_G1 1
#define LED32X32_PIN_B1 2
#define LED32X32_PIN_R2 3
#define LED32X32_PIN_G2 4
#define LED32X32_PIN_B2 5

#define LED32X32_PIN_CLK 0
#define LED32X32_PIN_OE  1
#define LED32X32_PIN_LAT 2
#define LED32X32_PIN_A   3
#define LED32X32_PIN_B   4
#define LED32X32_PIN_C   5
#define LED32X32_PIN_D   6


typedef struct T_RGB
{
    int r;
    int g;
    int b;
} RGB;

typedef struct T_Address
{
    int a;
    int b;
    int c;
    int d;
} Address;

typedef enum E_Color
{
    Black = 0, Red, Green, Blue, White
} Color;

void led32x32_init(void);

/*****************************************************************************
 * Static Functions to set pin high/low
 *****************************************************************************/
static void lp32x32_setCtrlPin(int pin);
static void lp32x32_clearCtrlPin(int pin);
static void sp32x32_setRgbPin(int pin);
static void sp32x32_clearRgbPin(int pin);

static void lp32x32_pinR1(int level);  // P2.0: PWM1[1]
static void lp32x32_pinG1(int level);  // P2.1: PWM1[2]
static void lp32x32_pinB1(int level);  // P2.2: PWM1[3]
static void lp32x32_pinR2(int level);  // P2.3: PWM1[4]
static void lp32x32_pinG2(int level);  // P2.4: PWM1[5]
static void lp32x32_pinB2(int level);  // P2.5: PWM1[6]

/*****************************************************************************
 * Communication Steps
 *****************************************************************************/
void lp32x32_setPixels(void);

void lp32x32_clock(void);

void lp32x32_latch(void);

RGB lp32x32_itoRgb(int i);

Address lp32x32_itoAddr(int i);

void lp32x32_setRow(int row);
/**
 * @brief Set color for upper half of the panel
 */
void lp32x32_setTopColor(Color color);
/**
 * @brief Set color for lower half of the panel
 */
void lp32x32_setBottomColor(Color color);
/**
 * @brief Refresh the panel
 * @details Each pixel condition has to previously be defined before
 *          calling this function
 */
void lp32x32_refresh(void);

void lp32x32_refresh_demo(void);

/*****************************************************************************
 * Drawing Tools
 *****************************************************************************/
void lp32x32_drawPixel(short x, short y, RGB rgb);

void lp32x32_fillRect(short p0, short p1, short p2, short p3, RGB rgb);

void lp32x32_drawRect(short p0, short p1, short p2, short p3, RGB rgb);

void lp32x32_drawLine(short px_start, short py_start, short px_end, short py_end, RGB rgb);


#endif // LED32X32_H_
