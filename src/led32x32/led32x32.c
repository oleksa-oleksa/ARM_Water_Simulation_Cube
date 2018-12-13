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
/*
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
*/
void led32x32_init(void)
{
    led_init();

    // Set port 3
    PINSEL6 = 0;     ///< Set function GPIO on port 3.0..5
    FIO3DIR = 0xFF;  ///< P3.0..7 defined as output
    FIO3MASK = 0;    ///< Enable write, set, clear, and read to R/W port 3
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

    bool enableA = row & 0x1;
    bool enableB = row & 0x2;
    bool enableC = row & 0x4;
    bool enableD = row & 0x8;

    if (enableA) lp32x32_setCtrlPin(LED32X32_PIN_A);
    if (enableB) lp32x32_setCtrlPin(LED32X32_PIN_B);
    if (enableC) lp32x32_setCtrlPin(LED32X32_PIN_C);
    if (enableD) lp32x32_setCtrlPin(LED32X32_PIN_D);
}

void _lp32x32_clearAllRgb1Pins(void)
{
    lp32x32_clearRgbPin(LED32X32_PIN_R1);
    lp32x32_clearRgbPin(LED32X32_PIN_G1);
    lp32x32_clearRgbPin(LED32X32_PIN_B1);
}
void lp32x32_setTopColor(RGB color)
{
    _lp32x32_clearAllRgb1Pins();

    if (color.red) lp32x32_setRgbPin(LED32X32_PIN_R1);
    if (color.green) lp32x32_setRgbPin(LED32X32_PIN_G1);
    if (color.blue) lp32x32_setRgbPin(LED32X32_PIN_B1);
}

void _lp32x32_clearAllRgb2Pins(void)
{
    lp32x32_clearRgbPin(LED32X32_PIN_R2);
    lp32x32_clearRgbPin(LED32X32_PIN_G2);
    lp32x32_clearRgbPin(LED32X32_PIN_B2);
}
void lp32x32_setBottomColor(RGB color)
{
    _lp32x32_clearAllRgb2Pins();

    if (color.red) lp32x32_setRgbPin(LED32X32_PIN_R2);
    if (color.green) lp32x32_setRgbPin(LED32X32_PIN_G2);
    if (color.blue) lp32x32_setRgbPin(LED32X32_PIN_B2);
}

//TODO remove this after debug
void _lp32x32_setTop(bool led)
{
    _lp32x32_clearAllRgb1Pins();

    if (led) // Use pink
    {
        lp32x32_setRgbPin(LED32X32_PIN_R1);
        lp32x32_setRgbPin(LED32X32_PIN_B1);
    }
}
//TODO remove this after debug
void _lp32x32_setBottom(bool led)
{
    _lp32x32_clearAllRgb2Pins();

    if (led) // Use yellow
    {
        lp32x32_setRgbPin(LED32X32_PIN_R2);
        lp32x32_setRgbPin(LED32X32_PIN_G2);
    }
}

void lp32x32_refresh_fixed(bool panel_temp[ROW_NUM][COL_NUM])
{
    int row;
    int col;

    for (row = 0; row < (ROW_NUM/2); ++row)
    {
        for (col = 0; col < COL_NUM; ++col)
        {
            _lp32x32_setTop(panel_temp[row][col]);
            _lp32x32_setBottom(panel_temp[row + ROW_NUM/2][col]);

            lp32x32_clock(); ///< Shift RGB info. of each column
        }

        lp32x32_setRow(row);
        lp32x32_setCtrlPin(LED32X32_PIN_OE);
        lp32x32_latch();
        lp32x32_clearCtrlPin(LED32X32_PIN_OE);
    }
}

// TODO use pwm
void lp32x32_refresh(uint32_t panel[ROW_NUM][COL_NUM])
{
    for (int row = 0; row < (ROW_NUM/2); ++row)
    {
        for (int col = 0; col < COL_NUM; ++col)
        {
            // TODO fix
            //lp32x32_setTopColor(panel[row][col]);
            //lp32x32_setBottomColor(panel[row + (ROW_NUM/2) - 1][col]);

            lp32x32_clock(); ///< Shift RGB info. of each column
        }

        lp32x32_setRow(row);
        lp32x32_setCtrlPin(LED32X32_PIN_OE);
        lp32x32_latch();
        lp32x32_clearCtrlPin(LED32X32_PIN_OE);
    }
}
