/**
 * @file   led32x32.c
 * @brief  Library for one RGB LED matrix panel
 * @date   2018-11-08
 * @author Kayoko Abe
 */

#include "led32x32.h"
#include "utils.h"

#define LED32X32_RGB24_R_MASK 0x00FF0000
#define LED32X32_RGB24_G_MASK 0x0000FF00
#define LED32X32_RGB24_B_MASK 0x000000FF

/*****************************************************************************
 * Static functions to set pin high/low
 *****************************************************************************/
static inline void lp32x32_setCtrlPin(int pin)
{
    LED32X32_CTRLPIN_SETTER |= (1 << pin);
}
static inline void lp32x32_clearCtrlPin(int pin)
{
    LED32X32_CTRLPIN_CLEANER |= (1 << pin);
}
static inline void lp32x32_setRgbPin(int pin)
{
    LED32X32_RGBPIN_SETTER |= (1 << pin);
}
static inline void lp32x32_clearRgbPin(int pin)
{
    LED32X32_RGBPIN_CLEANER |= (1 << pin);
}

/*****************************************************************************
 * Communication Steps
 *****************************************************************************/
void led32x32_init(void)
{
    led_init();

    // Set port 3
    PINSEL6 = 0;     ///< Set function GPIO on port 3.0..5
    FIO3DIR = 0x7F;  ///< P3.0..6 defined as output
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


void lp32x32_setUpperPixelInfo(bool r, bool g, bool b)
{
    if(r)
    {
        lp32x32_setRgbPin(LED32X32_PIN_R1);
    }
    else
    {
        lp32x32_clearRgbPin(LED32X32_PIN_R1);
    }
    if(g)
    {
        lp32x32_setRgbPin(LED32X32_PIN_G1);
    }
    else
    {
        lp32x32_clearRgbPin(LED32X32_PIN_G1);
    }
    if(b)
    {
        lp32x32_setRgbPin(LED32X32_PIN_B1);
    }
    else
    {
        lp32x32_clearRgbPin(LED32X32_PIN_B1);
    }
}
void lp32x32_setBottomPixelInfo(bool r, bool g, bool b)
{
    if(r)
    {
        lp32x32_setRgbPin(LED32X32_PIN_R2);
    }
    else
    {
        lp32x32_clearRgbPin(LED32X32_PIN_R2);
    }
    if(g)
    {
        lp32x32_setRgbPin(LED32X32_PIN_G2);
    }
    else
    {
        lp32x32_clearRgbPin(LED32X32_PIN_G2);
    }
    if(b)
    {
        lp32x32_setRgbPin(LED32X32_PIN_B2);
    }
    else
    {
        lp32x32_clearRgbPin(LED32X32_PIN_B2);
    }
}

void lp32x32_refresh_chain_blue(panel_t panels[CHAIN_LEN])
{
    static uint8_t row = 0;
    uint8_t col;
    uint8_t panelIndex = 0;
    uint32_t upperPixel = 0;
    uint32_t bottomPixel = 0;

    for(col = 0; col < (COL_NUM * CHAIN_LEN); ++col)
    {
        panelIndex = col / COL_NUM;
        upperPixel = panels[panelIndex][row][col - COL_NUM * panelIndex].particle_count;
        bottomPixel = panels[panelIndex][row + ROW_NUM / 2][col - COL_NUM * panelIndex].particle_count;
        
        lp32x32_clearAllRgb1Pins();
        lp32x32_clearAllRgb2Pins();

        /* Upper area */
        if(upperPixel)
        {
            lp32x32_setUpperPixelInfo(false, false, true);
        }
        
        /* Bottom area */
        if(bottomPixel)
        {
            lp32x32_setBottomPixelInfo(false, false, true);
        }

        lp32x32_clock(); ///< Shift RGB information of each pixel to horizontal direction
    }

    lp32x32_setRow(row);
    lp32x32_setCtrlPin(LED32X32_PIN_OE);
    lp32x32_latch();
    lp32x32_clearCtrlPin(LED32X32_PIN_OE);
    ++row;
    if(row >= (ROW_NUM/2))
    {
        row = 0;
    }
}

void lp32x32_refresh_chain_24bit_rgb(panel_t panels[CHAIN_LEN])
{
    static uint8_t row = 0;
    static uint8_t color_bit = 0;
    uint8_t col;
    uint8_t panelIndex = 0;
    uint32_t upperPixel = 0;
    uint32_t bottomPixel = 0;

    for(col = 0; col < (COL_NUM * CHAIN_LEN); ++col)
    {
        panelIndex = col / COL_NUM;
        upperPixel = panels[panelIndex][row][col - COL_NUM * panelIndex].particle_count;
        bottomPixel = panels[panelIndex][row + ROW_NUM / 2][col - COL_NUM * panelIndex].particle_count;
        
        //for each color: out of 256 iterations display color channel (r, g, or b) for <color value> amounts (e.g. with r = 0x40 display red 64 out of 256 times)
        lp32x32_setUpperPixelInfo ((/*red=  */((upperPixel  & LED32X32_RGB24_R_MASK) >> 16) > color_bit), 
                                   (/*green=*/((upperPixel  & LED32X32_RGB24_G_MASK) >>  8) > color_bit),
                                   (/*blue= */((upperPixel  & LED32X32_RGB24_B_MASK)      ) > color_bit));
        lp32x32_setBottomPixelInfo((/*red=  */((bottomPixel & LED32X32_RGB24_R_MASK) >> 16) > color_bit), 
                                   (/*green=*/((bottomPixel & LED32X32_RGB24_G_MASK) >>  8) > color_bit),
                                   (/*blue= */((bottomPixel & LED32X32_RGB24_B_MASK)      ) > color_bit));

        lp32x32_clock(); ///< Shift RGB information of each pixel to horizontal direction
    }

    lp32x32_setRow(row);
    lp32x32_setCtrlPin(LED32X32_PIN_OE);
    lp32x32_latch();
    lp32x32_clearCtrlPin(LED32X32_PIN_OE);
    ++row;
    if(row >= (ROW_NUM/2))
    {
        row = 0;
        ++color_bit;
        if(color_bit >= 0xFF)
        {
            color_bit = 0;
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
