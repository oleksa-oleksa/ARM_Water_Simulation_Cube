/**
 * @file   clocksetter.c
 * @brief  Set clock configurations
 * @date   2019-03-16
 * @author Kayoko Abe
 */

#include "clocksetter.h"

#include <LPC23xx.H>

// TODO set checker avoiding setting of a frequency
// higher than the CPU operating frequency of 72 MHz.
int changeCpuClock(void)
{
    CCLKCFG = 0x3; // generates 72 MHz
}

/**
 * @brief Feed step required to activate change in PLL config
 */
void _setOneFeed(void)
{
    PLLFEED = 0xAA;
    PLLFEED = 0x55;
}
void changePLLConfig(void)
{
    // Disconnect PLL if being connected and set one feed
    PLLCON &= 0xE;
    _setOneFeed();
    // Disable PLL
    PLLCON &= 0xD;
    _setOneFeed();
    // Change CPU clock divider setting without PLL if desired

    // Write to clock source selection control register to change clock source

    // Write to PLLCFG
    // 14:0 MSEL, 15 -, 23:16 NSEL, 31:24 -
    PLLCFG |= 0x000B; // default value B
    _setOneFeed();
    // Enable PLL
    PLLCON = 1;
    _setOneFeed();
    // Change CPU clock divider setting with PLL

    // Wait until PLOCK is set to 1
    while (!(PLLSTAT & 0x04000000))
    {
        ;
    }
    // Connect PLL
    PLLCON |= 0x2;
    _setOneFeed();
}

