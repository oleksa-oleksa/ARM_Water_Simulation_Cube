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

/**
 * One panel consisting of 'ROW_NUM x COL_NUM' pixels each having 32-bit integer information
 */
typedef int panel_t[ROW_NUM][COL_NUM];

#define CHAIN_LEN 6 ///< Number of panels to be chained

/********************************************************************
 * Configuration for PWM1 on port 2
 ********************************************************************/
// #define ROW_NUM 32
// #define COL_NUM 32

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


/*****************************************************************************
 * Static Functions to set pin high/low
 *****************************************************************************/

/**
 * @brief     Set pin relevant to control
 * @param[in] pin 
 */
static void lp32x32_setCtrlPin(int pin);
/**
 * @brief     Clear pin relevant to control
 * @param[in] pin
 */
static void lp32x32_clearCtrlPin(int pin);

static void sp32x32_setRgbPin(int pin);
static void sp32x32_clearRgbPin(int pin);

/**
 * @brief Initialize LED functionality and enable output on port 3
 */
void led32x32_init(void);

/*****************************************************************************
 * Communication Steps
 *****************************************************************************/

/**
 * @brief Set and clear clock pin
 */
void lp32x32_clock(void);
/**
 * @brief Set and clear latch pin
 */
void lp32x32_latch(void);

/**
 * @brief Clear all address pins
 */
void lp32x32_clearAllAddrPins(void);
/**
 * @brief     Set target row by setting control signals A,B,C,D
 * @param[in] raw Row number of a panel
 */
void lp32x32_setRow(int row);

/**
 * @brief Clear all RGB pins for upper panel
 */
void lp32x32_clearAllRgb1Pins(void);
/**
 * @brief Clear all RGB pins for bottom panel
 */
void lp32x32_clearAllRgb2Pins(void);

/**
 * @brief Set color (Blue) for upper half of the panel
 * @param[in] info Information on pixel color intensity
 */
void lp32x32_setUpperPixelInfo(int info);
/**
 * @brief Set color (Blue) for lower half of the panel
 * @param[in] info Information on pixel color intensity
 */
void lp32x32_setBottomPixelInfo(int info);

/**
 * @brief     Refresh chained panels
 * @param[in] panels Array of panels each has 2D-array for storing pixel information
 */
void lp32x32_refresh_chain(panel_t panels[CHAIN_LEN]);

#endif // LED32X32_H_
