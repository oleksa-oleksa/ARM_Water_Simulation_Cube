/**
 * @file   led32x32.c
 * @brief  Library for one RGB LED matrix panel
 * @date   2018-11-08
 * @author Kayoko Abe
 */

#include "led32x32.h"

/*****************************************************************************
 * Static Functions to set pin high/low
 *****************************************************************************/
static void lp32x32_setCtrlPin(int pin)
{
    LED32X32_CTRLPIN_SETTER = (1 << pin);
}

static void lp32x32_clearCtrlPin(int pin)
{
    LED32X32_CTRLPIN_CLEANER = (1 << pin);
}

static void lp32x32_setRgbPin(int pin)
{
    LED32X32_RGBPIN_SETTER = (1 << pin);
}

static void lp32x32_clearRgbPin(int pin)
{
    LED32X32_RGBPIN_CLEANER = (1 << pin);
}

/*****************************************************************************
 * Communication Steps
 *****************************************************************************/
void led32x32_init(void)
{
    PCONP |= (1 << 6);  ///< Enable power of PWM1 on port 2.0-2.5
    PCLKSEL0 |= 0x2000; ///< Select peripheral clock for PWM1 and set to be equal to CCLK
    
    PINSEL10 = 0;          ///< Disable ETM interface, enable LEDs (turn all LEDs on)
    PINSEL4 = 0;           ///< Set function GPIO on port 2
    FIO2DIR  = 0xFF; ///< P2.0..7 defined as output
    FIO2MASK = 0; ///< Enable write, set, clear, and read to R/W port 2
    
    //PINSEL4 |= 0x555;   ///< Set function PWM 1.1-1.6 on port 2
    FIO2MASK = 0;       ///< Enable write, set, clear, and read to R/W port 2

    PINSEL6 = 0;    ///< Set function D0-D6 on port 3.0-3.5
    FIO3DIR = 0xFF;     ///< P3.0..7 defined as output
    FIO3MASK = 0;       ///< Enable write, set, clear, and read to R/W port 3
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

Address lp32x32_itoAddr(int i)
{
    Address address;
    address.a = i & 1;
    address.b = i & 2;
    address.c = i & 4;
    address.d = i & 8;

    return address;
}

RGB lp32x32_itoRgb(int i)
{
    RGB rgb;
    rgb.r = i & 1;
    rgb.g = i & 2;
    rgb.b = i & 4;

    return rgb;
}

void lp32x32_setRow(int row)
{
    Address address = lp32x32_itoAddr(row);

    ///< Set address A high
    ///< Set address B high
    ///< Set address C high
    ///< Set address D high
}

void lp32x32_setTopColor(Color color)
{
    RGB rgb = lp32x32_itoRgb(color);

    ///< Set R1 high
    ///< Set G1 high
    ///< Set B1 high
}

void lp32x32_setBottomColor(Color color)
{
    RGB rgb = lp32x32_itoRgb(color);

    ///< Set R2 high
    ///< Set G2 high
    ///< Set B2 high
}

void lp32x32_refresh()
{
    Color cl_black = Black;
    
    for (int row = 0; row < (ROW_NUM/2) - 1; ++row)
    {
        lp32x32_setCtrlPin(LED32X32_PIN_OE); ///< Set OE high

        lp32x32_setTopColor(cl_black); ///< All LEDs on the panell off
        lp32x32_setRow(row);

        for (int col = 0; col < COL_NUM - 1; ++col)
        {
//            lp32x32_setTopColor(panel[row][col]);
//            lp32x32_setBottomColor(panel[row + (ROW_NUM/2)][col]);
            lp32x32_clock();
        }

        lp32x32_latch();

        lp32x32_clearCtrlPin(LED32X32_PIN_OE); ///< Set OE low

        // Delay
    }
}

void lp32x32_refresh_demo(void)
{
    lp32x32_setCtrlPin(LED32X32_PIN_OE); ///< Set OE high to disable output
    
    // Set Address
    lp32x32_setCtrlPin(LED32X32_PIN_A);
    lp32x32_clearCtrlPin(LED32X32_PIN_B);
    lp32x32_setCtrlPin(LED32X32_PIN_C);
    lp32x32_clearCtrlPin(LED32X32_PIN_D);
    
    // Set RGBs
    lp32x32_setRgbPin(LED32X32_PIN_R1);
    lp32x32_setRgbPin(LED32X32_PIN_G1);
    lp32x32_setRgbPin(LED32X32_PIN_B1);

    lp32x32_setRgbPin(LED32X32_PIN_R2);
    lp32x32_setRgbPin(LED32X32_PIN_G2);
    lp32x32_setRgbPin(LED32X32_PIN_B2);

    lp32x32_clock();
    lp32x32_latch();
    
    lp32x32_clearCtrlPin(LED32X32_PIN_OE); ///< Set OE low to enable output

    // Delay
}
