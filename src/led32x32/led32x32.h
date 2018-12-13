/**
 * @file   led32x32.h
 * @brief  Library for one RGB LED matrix panel
 * @date   2018-11-08
 * @author Kayoko Abe
 */
 
#ifndef LED32X32_H_
#define LED32X32_H_

#include <LPC23xx.H>
#include <stdint.h>
#include <stdbool.h>

#include "paint_tool.h"

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

/**
 * @brief Initialize LED functionality and enable output on port 3
 */
void led32x32_init(void);

/*****************************************************************************
 * Static Functions to set pin high/low
 *****************************************************************************/
static void lp32x32_setCtrlPin(int pin);
static void lp32x32_clearCtrlPin(int pin);
static void sp32x32_setRgbPin(int pin);
static void sp32x32_clearRgbPin(int pin);

/*****************************************************************************
 * Communication Steps
 *****************************************************************************/
void lp32x32_setPixels(void);

void lp32x32_clock(void);

void lp32x32_latch(void);

/**
 * @brief Set target row by setting control signals A,B,C,D
 */
void lp32x32_setRow(int row);

/**
 * @brief Set color for upper half of the panel
 */
void lp32x32_setTopColor(/*RGB color*/);

/**
 * @brief Set color for lower half of the panel
 */
void lp32x32_setBottomColor(/*RGB color*/);

/**
 * @brief Refresh the panel
 * @details Each pixel condition has to previously be defined before
 *          calling this function
 */
void lp32x32_refresh_fixed(bool panel_temp[ROW_NUM][COL_NUM]);
void lp32x32_refresh(/*uint32_t panel[ROW_NUM][COL_NUM]*/);

#endif // LED32X32_H_
