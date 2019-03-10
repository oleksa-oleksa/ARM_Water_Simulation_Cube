#ifndef UTILS_H_
#define UTILS_H_

#include <LPC23xx.H>
#include <stdint.h>

#define SEC_TO_USEC(sec) (sec * 1000000)

#define SEC_TO_MSEC(sec) (sec * 1000)

#define SYS_CLK 1000 // [kHz]

/**
 * @brief delay
 * @param[in] delay_ms time in milli second
 */
void delay(int delay_ms);

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
 * @brief Set port 2 (on-board LEDs) as PWM
 */
void led_setPwm(void);

/**
 * @brief Reset PWM timer, set PWM duty cycle and start timer
 * @param[in] Duty cycle parameters
 */
void led_setPwmLvl(uint32_t mr0, uint32_t mr1, uint32_t mr2, uint32_t mr3, uint32_t mr4, uint32_t mr5, uint32_t mr6);

#endif // UTILS_H_
