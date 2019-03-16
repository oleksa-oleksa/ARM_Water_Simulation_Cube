/**
 * @file   clocksetter.h
 * @brief  Set clock configurations
 * @date   2019-03-16
 * @author Kayoko Abe
 */

#ifndef CLOCKSETTER_H_
#define CLOCKSETTER_H_

/**
 * @brief Change CPU clock frequency
 * The frequency is calculated by:
 * PLLOUT / (CCLKSEL + 1)
 */
int changeCpuClock(void);
/**
 * @brief Change PLL configuration
 */
void changePLLConfig(void);

