#include "utils.h"

/**
 * @details Peripheral Frequency = Oscillator Frequency / VPB divider
 *                               = 20 MHz / 4 = 5 MHz (as example)
 *          Count = (5MHz * Requested Time) -1
 *                = (5000000 * 100 ms) - 1
 *                = 499999 => T0MR0
 *
 *          (In our case)
 *          Count = (1MHz * Requested Time) -1
 *                = (1000000 * 100 ms) - 1
 *                = 99999
 */
void delay(int delay_ms)
{
    uint32_t freq = SYS_CLK * delay_ms - 1;

    T0TCR = (1 << 1); ///< Reset Timer 0 (TC and PC)
    T0MR0 = freq;   ///< Load match register
    T0PR = 10;        ///< Load prescale register
    T0PC = 0;         ///< Reset prescale counter
    T0TC = 0;         ///< Reset timer counter
    T0MCR = (1 << 0) | (1 << 2); ///< Generate interrupt and reset TC on match
    
    T0TCR = (1 << 0);            ///< Start Timer 0 by enabling counters PC,TC
    
    while ( !(T0IR & (1 << 0)) ) ///< Wait for interrupt0 being generated
        ;
    
    T0IR = (1 << 0);             ///< Clear interrupt register
}

/********************************************************************
 * On-board LEDs Normal Mode
 ********************************************************************/
void led_init(void)
{
    PINSEL10 = 0;    ///< Disable ETM interface, enable LEDs (turn all LEDs on)
    PINSEL4 = 0;     ///< Set function GPIO on port 2
    FIO2DIR  = 0xFF; ///< P2.0..7 defined as output
    FIO2MASK = 0;    ///< Enable write, set, clear, and read to R/W port 2
}

void led_on(unsigned int num)
{
    FIO2SET = (1 << num); ///< LED[num] on
}

void led_off(unsigned int num)
{
    FIO2CLR = (1 << num); ///< LED[num] off
}

void led_allOff(void)
{
    FIO2CLR = 0xFF;
}

void led_allOn(void)
{
    FIO2SET = 0xFF;
}

void led_selectOn(unsigned int nums)
{
    led_allOff();
    FIO2SET = (nums & 0xFF); ///< Only selected LEDs on
}

/********************************************************************
 * On-board LEDs PWM Mode
 ********************************************************************/

void led_setPwm(void)
{
    PCONP |= (1 << 6);  ///< Enable power of PWM1 on port 2.0-2.5
    PCLKSEL0 |= 0x2000; ///< Select peripheral clock for PWM1 and set to be equal to CCLK
    PINSEL4 |= 0x555;   ///< Set function PWM 1.1-1.6 on port 2
    FIO2MASK = 0;       ///< Enable write, set, clear, and read to R/W port 2

    PWM1PCR = 0x7F00;   ///< PWM Control Register: set single edge mode and enable output of PWM1-6
    PWM1LER = 0xFF;     ///< Load Enable Register: enable use of new PWM match values
}

void led_setPwmLvl(uint32_t mr0, uint32_t mr1, uint32_t mr2, uint32_t mr3, uint32_t mr4, uint32_t mr5, uint32_t mr6)
{
    PWM1TC = 0;    ///< Reset timer counter
    PWM1TCR = 0x2; ///< Reset counter and prescaler

    PWM1MR0 = mr0; ///< Set cycle rate of PWMs

    PWM1MR1 = mr1; ///< Set rising edge of PWM1
    PWM1MR2 = mr2; ///< Set rising edge of PWM2
    PWM1MR3 = mr3; ///< Set rising edge of PWM3

    PWM1MR4 = mr4; ///< Set rising edge of PWM4
    PWM1MR5 = mr5; ///< Set rising edge of PWM5
    PWM1MR6 = mr6; ///< Set rising edge of PWM6

    PWM1TCR = 0x9; ///< Enable counter, clear reset and release counter from reset, and enable PWM
}
