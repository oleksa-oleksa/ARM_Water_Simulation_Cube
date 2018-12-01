#ifndef UTILS_H_
#define UTILS_H_

#include <LPC23xx.H>
#include <stdint.h>

void delay(void);

/********************************************************************
 * On-board LEDs Normal Mode
 ********************************************************************/

/**
 * @brief Set port 2 as LED (GPIO), set direction of pins as output
 */
void led_init(void);

void led_on(unsigned int num);
void led_off(unsigned int num);
void led_allOff(void);
void led_allOn(void);
void led_selectOn(unsigned int nums);

/********************************************************************
 * On-board LEDs PWM Mode
 ********************************************************************/

/**
 * @brief Set port 2 as PWM
 */
void led_setPwm(void);

/**
 * @brief Reset PWM timer, set PWM duty cycle and start timer
 * @param[in] Duty cycle parameters
 */
void led_setPwmLvl(uint32_t mr0, uint32_t mr1, uint32_t mr2, uint32_t mr3, uint32_t mr4, uint32_t mr5, uint32_t mr6);

#endif // UTILS_H_
