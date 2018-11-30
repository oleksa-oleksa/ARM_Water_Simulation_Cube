#ifndef UTILS_H_
#define UTILS_H_

#include <LPC23xx.H>

void delay(void);

/********************************************************************
 * On-board LEDs Normal Mode
 ********************************************************************/
void led_init(void);
void led_on(unsigned int num);
void led_off(unsigned int num);
void led_allOff(void);
void led_allOn(void);
void led_selectOn(unsigned int nums);

/********************************************************************
 * On-board LEDs PWM Mode
 ********************************************************************/
void led_pwmInit(void);

#endif // UTILS_H_
