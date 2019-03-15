/**
 * @file   accelerometer.h
 * @brief
 * @date   2019-03-13
 * @author Kayoko Abe
 */

#ifndef ACCELEROMETER_H_
#define ACCELEROMETER_H_

#include <stdint.h>

#include "LCD.h"     /* Graphic LCD function prototypes */

#define ldelay 5000000
#define sdelay 500000

/**
 *   @brief hard-coded delay
 */
void delay(int delay);

/**
 * @brief LCD design for printing initial greeting
 */
void lcd_print_greeting(void);
/**
 * @brief LCD design for printing any message
 */
void lcd_print_message(unsigned char *msg);
/**
 * @brief LCD design for printing coordinates x,y,z from accelerometer
 */
void lcd_print_coordinates(unsigned char *msg);

/**
 * @brief  Gets the most recent X axis value
 */
int16_t getX(void);
/**
 * @brief  Gets the most recent Y axis value
 */
int16_t getY(void);
/**
 * @brief  Gets the most recent Z axis value
 */
int16_t getZ(void);

/**
 * @brief  I2C init
 * 1. configure the VIC to respond to a I2C interrupt
 * 2. configure pinselect block to connect the I2C data and clock lines to the external pins
 * 3. set the bit rate by programming I2SCLH and I2SCLL.
 *      Only the first 16 bits are used to hold the timing values
*/
void i2c_init(void);

/**
 * @brief  ADXL345 Init
 * Note: I2C should already be enabled
 */
int accelerometer_init(void);


#endif // ACCELEROMETER_H_
