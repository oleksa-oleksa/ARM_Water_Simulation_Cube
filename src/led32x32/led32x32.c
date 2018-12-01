/**
 * @file   led32x32.c
 * @brief  Library for one RGB LED matrix panel
 * @date   2018-11-08
 * @author Kayoko Abe
 */

#include "led32x32.h"
#include "utils.h"

/*****************************************************************************
 * Static Functions to set pin high/low
 *****************************************************************************/
static void lp32x32_setRgbPin(int pin)
{
    LED32X32_RGBPIN_SETTER |= (1 << pin);
}
static void lp32x32_clearRgbPin(int pin)
{
    LED32X32_RGBPIN_CLEANER |= (1 << pin);
}
static void lp32x32_setCtrlPin(int pin)
{
    LED32X32_CTRLPIN_SETTER |= (1 << pin);
}
static void lp32x32_clearCtrlPin(int pin)
{
    LED32X32_CTRLPIN_CLEANER |= (1 << pin);
}

/*****************************************************************************
 * Communication Steps
 *****************************************************************************/
void _led32x32_pwm_registersInit(void)
{
    PWM1IR = 0x70F; ///< Generate interrupts 0-6
    PWM1TC = 0;     ///< Reset timer counter
    PWM1TCR = 0x2;  ///< Reset counter and prescaler
    PWM1PR = 0x1;   ///< Load prescale register

    // TODO check configuration of MR registers
    PWM1MR0 = 0x100; ///< Set cycle rate of PWMs

    PWM1MR1 = 0x01; ///< Set rising edge of PWM1
    PWM1MR2 = 0x01; ///< Set rising edge of PWM2
    PWM1MR3 = 0x01; ///< Set rising edge of PWM3

    PWM1MR4 = 0x01; ///< Set rising edge of PWM4
    PWM1MR5 = 0x01; ///< Set rising edge of PWM5
    PWM1MR6 = 0x01; ///< Set rising edge of PWM6

    PWM1MCR = 0x03;   ///< Generate interrupt and reset counter on match
    PWM1LER = 0x7F;   ///< Enable shadow latch for match 0-6
    PWM1PCR = 0x7F00; ///< Set sin edge mode and enable output of PWM1-6
    PWM1TCR = 0x09;   ///< Enable counter and PWM, clear reset, release counter from reset
}

void led32x32_init(void)
{
    PCONP |= (1 << 6);  ///< Enable power of PWM1 on port 2.0-2.5
    PCLKSEL0 |= 0x2000; ///< Select peripheral clock for PWM1 and set to be equal to CCLK
    
    // Set port 2 as GPIO
    PINSEL10 = 0;    ///< Disable ETM interface, enable LEDs (turn all LEDs on)
    PINSEL4 = 0;     ///< Set function GPIO on port 2.0..7
    FIO2DIR  = 0xFF; ///< P2.0..7 defined as output
    FIO2MASK = 0;    ///< Enable write, set, clear, and read to R/W port 2

    // Set port 3
    PINSEL6 = 0;     ///< Set function GPIO on port 3.0..5
    FIO3DIR = 0xFF;  ///< P3.0..7 defined as output
    FIO3MASK = 0;    ///< Enable write, set, clear, and read to R/W port 3
}

void led32x32_pwmInit(void)
{
    led32x32_init();
    
    // Set port 2 as pwm
    PINSEL4 |= 0x555;              ///< Set function PWM 1.1-1.6 on port 2
    FIO2MASK = 0;                  ///< Enable write, set, clear, and read to R/W port 2
    _led32x32_pwm_registersInit();
}

void lp32x32_setpixels(void)
{

}

void lp32x32_clock(void)
{
    lp32x32_setCtrlPin(LED32X32_PIN_CLK);   ///< Set clock high
    lp32x32_clearCtrlPin(LED32X32_PIN_CLK); ///< Set clock low
}

void lp32x32_latch(void)
{
    lp32x32_setCtrlPin(LED32X32_PIN_LAT);   ///< Set latch high
    lp32x32_clearCtrlPin(LED32X32_PIN_LAT); ///< Set latch low
}

void _lp32x32_clearAllAddrPins(void)
{
    lp32x32_clearCtrlPin(LED32X32_PIN_A);
    lp32x32_clearCtrlPin(LED32X32_PIN_B);
    lp32x32_clearCtrlPin(LED32X32_PIN_C);
    lp32x32_clearCtrlPin(LED32X32_PIN_D);
}
void lp32x32_setRow(int row)
{
    _lp32x32_clearAllAddrPins();

    _Bool enableA = row & 0x1;
    _Bool enableB = row & 0x2;
    _Bool enableC = row & 0x4;
    _Bool enableD = row & 0x8;

    if (enableA) lp32x32_setCtrlPin(LED32X32_PIN_A);
    if (enableB) lp32x32_setCtrlPin(LED32X32_PIN_B);
    if (enableC) lp32x32_setCtrlPin(LED32X32_PIN_C);
    if (enableD) lp32x32_setCtrlPin(LED32X32_PIN_D);
}

void lp32x32_setTopColor(RGB color)
{
    if (color.red) lp32x32_setRgbPin(LED32X32_PIN_R1);
    if (color.green) lp32x32_setRgbPin(LED32X32_PIN_G1);
    if (color.blue) lp32x32_setRgbPin(LED32X32_PIN_B1);
}

void lp32x32_setBottomColor(RGB color)
{
    if (color.red) lp32x32_setRgbPin(LED32X32_PIN_R2);
    if (color.green) lp32x32_setRgbPin(LED32X32_PIN_G2);
    if (color.blue) lp32x32_setRgbPin(LED32X32_PIN_B2);
}

void lp32x32_refresh_fixed(void)
{    
    for (int row = 0; row < (ROW_NUM/2); ++row)
    {
        lp32x32_setCtrlPin(LED32X32_PIN_OE); ///< Set OE high
        lp32x32_setTopColor(RGB_BLACK);      ///< All LEDs on the panell off        
        lp32x32_setRow(row);                 ///< Set address between 0 - 15

        for (int col = 0; col < (COL_NUM - 1); ++col)
        {
            lp32x32_setTopColor(RGB_GREEN);
            lp32x32_setBottomColor(RGB_RED);

            lp32x32_clock();
        }

        lp32x32_latch();
        lp32x32_clearCtrlPin(LED32X32_PIN_OE); ///< Set OE low
        delay(); // TODO remove later. This is used for debug purpose.
    }
}

// TODO use pwm
void lp32x32_refresh(uint32_t panel[ROW_NUM][COL_NUM])
{    
    for (int row = 0; row < (ROW_NUM/2); ++row)
    {
        lp32x32_setCtrlPin(LED32X32_PIN_OE); ///< Set OE high
        lp32x32_setTopColor(RGB_BLACK);      ///< All LEDs on the panell off        
        lp32x32_setRow(row);                 ///< Set address between 0 - 15

        for (int col = 0; col < (COL_NUM - 1); ++col)
        {
            // TODO fix
            //lp32x32_setTopColor(panel[row][col]);
            //lp32x32_setBottomColor(panel[row + (ROW_NUM/2) - 1][col]);

            lp32x32_clock();
        }

        lp32x32_latch();
        lp32x32_clearCtrlPin(LED32X32_PIN_OE); ///< Set OE low
        delay(); // TODO remove later. This is used for debug purpose.
    }
}
