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
static void lp32x32_setCtrlPin(int pin)
{
    LED32X32_CTRLPIN_SETTER |= (1 << pin);
}
static void lp32x32_clearCtrlPin(int pin)
{
    LED32X32_CTRLPIN_CLEANER |= (1 << pin);
}
static void lp32x32_setRgbPin(int pin)
{
    LED32X32_RGBPIN_SETTER |= (1 << pin);
}
static void lp32x32_clearRgbPin(int pin)
{
    LED32X32_RGBPIN_CLEANER |= (1 << pin);
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

void lp32x32_clearAllAddrPins(void)
{
    lp32x32_clearCtrlPin(LED32X32_PIN_A);
    lp32x32_clearCtrlPin(LED32X32_PIN_B);
    lp32x32_clearCtrlPin(LED32X32_PIN_C);
    lp32x32_clearCtrlPin(LED32X32_PIN_D);
}
void lp32x32_setRow(int row)
{
    lp32x32_clearAllAddrPins();

    bool enableA = row & 0x1;
    bool enableB = row & 0x2;
    bool enableC = row & 0x4;
    bool enableD = row & 0x8;

    if (enableA) lp32x32_setCtrlPin(LED32X32_PIN_A);
    if (enableB) lp32x32_setCtrlPin(LED32X32_PIN_B);
    if (enableC) lp32x32_setCtrlPin(LED32X32_PIN_C);
    if (enableD) lp32x32_setCtrlPin(LED32X32_PIN_D);
}

void lp32x32_clearAllRgb1Pins(void)
{
    lp32x32_clearRgbPin(LED32X32_PIN_R1);
    lp32x32_clearRgbPin(LED32X32_PIN_G1);
    lp32x32_clearRgbPin(LED32X32_PIN_B1);
}
void lp32x32_clearAllRgb2Pins(void)
{
    lp32x32_clearRgbPin(LED32X32_PIN_R2);
    lp32x32_clearRgbPin(LED32X32_PIN_G2);
    lp32x32_clearRgbPin(LED32X32_PIN_B2);
}


void lp32x32_setUpperPixelInfo(int info)
{
    lp32x32_setRgbPin(LED32X32_PIN_B1);
}
void lp32x32_setBottomPixelInfo(int info)
{
    lp32x32_setRgbPin(LED32X32_PIN_B2);
}

void lp32x32_refresh_chain(panel_t panels[CHAIN_LEN])
{
    uint8_t row;
    uint8_t col;
    uint8_t layer;
    uint8_t panelIndex = 0;
    int upperPixel = 0;
    int bottomPixel = 0;

    for (row = 0; row < (ROW_NUM/2); ++row)
    {
        // TODO find the best match of the number of layers and clock speed
        // to make color intensity
        for (layer = 0; layer < 1; ++layer)
        {
            for (col = 0; col < (COL_NUM * CHAIN_LEN); ++col)
            {
                panelIndex = col / COL_NUM;
                upperPixel = panels[panelIndex][row][col - COL_NUM * panelIndex];
                bottomPixel = panels[panelIndex][row + ROW_NUM / 2][col - COL_NUM * panelIndex];
                
                lp32x32_clearAllRgb1Pins();
                lp32x32_clearAllRgb2Pins();

                /* Upper area */
                if (upperPixel)
                {
                    lp32x32_setUpperPixelInfo(upperPixel); // TODO consider layer
                }
                
                /* Bottom area */
                if (bottomPixel)
                {
                    lp32x32_setUpperPixelInfo(bottomPixel); // TODO consider layer
                }

                lp32x32_clock(); ///< Shift RGB information of each column
            }

            lp32x32_setRow(row);
            lp32x32_setCtrlPin(LED32X32_PIN_OE);
            lp32x32_latch();
            lp32x32_clearCtrlPin(LED32X32_PIN_OE);
        }
    }
}

/**
 * Functions to test pwm mode on the board for LED panels
 * TODO delete if not used
 */
void __led_setPwmLvl_top(uint32_t mr1, uint32_t mr2, uint32_t mr3)
{
    PWM1TC = 0;    ///< Reset timer counter
    PWM1TCR = 0x2; ///< Reset counter and prescaler

    PWM1MR1 = mr1; ///< Set rising edge of PWM1
    PWM1MR2 = mr2; ///< Set rising edge of PWM2
    PWM1MR3 = mr3; ///< Set rising edge of PWM3

    PWM1TCR = 0x9; ///< Enable counter, clear reset and release counter from reset, and enable PWM
}

void __led_setPwmLvl_bottom(uint32_t mr4, uint32_t mr5, uint32_t mr6)
{
    PWM1TC = 0;    ///< Reset timer counter
    PWM1TCR = 0x2; ///< Reset counter and prescaler

    PWM1MR4 = mr4; ///< Set rising edge of PWM4
    PWM1MR5 = mr5; ///< Set rising edge of PWM5
    PWM1MR6 = mr6; ///< Set rising edge of PWM6

    PWM1TCR = 0x9; ///< Enable counter, clear reset and release counter from reset, and enable PWM
}