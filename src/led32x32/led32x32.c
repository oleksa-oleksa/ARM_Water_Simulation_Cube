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

void _lp32x32_clearAllRgb2Pins(void)
{
    lp32x32_clearRgbPin(LED32X32_PIN_R2);
    lp32x32_clearRgbPin(LED32X32_PIN_G2);
    lp32x32_clearRgbPin(LED32X32_PIN_B2);
}

void lp32x32_setTopColor(void)
{
    _lp32x32_clearAllRgb1Pins();

    // if (color.red)
        lp32x32_setRgbPin(LED32X32_PIN_R1);
    // if (color.green)
        lp32x32_setRgbPin(LED32X32_PIN_G1);
    // if (color.blue)
        lp32x32_setRgbPin(LED32X32_PIN_B1);
}

void lp32x32_setBottomColor(void)
{
    _lp32x32_clearAllRgb2Pins();

    // if (color.red)
        lp32x32_setRgbPin(LED32X32_PIN_R2);
    // if (color.green)
        lp32x32_setRgbPin(LED32X32_PIN_G2);
    // if (color.blue)
        lp32x32_setRgbPin(LED32X32_PIN_B2);
}

//TODO remove this after debugging
void _lp32x32_setTop(bool led)
{
    _lp32x32_clearAllRgb1Pins();

    if (led)
    {
        lp32x32_setRgbPin(LED32X32_PIN_B1);
    }
}
//TODO remove this after debugging
void _lp32x32_setBottom(bool led, int layer)
{
    _lp32x32_clearAllRgb2Pins();

    if (led)
    {
        // lp32x32_setRgbPin(LED32X32_PIN_B2);

        switch (layer)
        {
        case 0:
            lp32x32_setRgbPin(LED32X32_PIN_B2);
            break;
        //default:
            // TODO complete switch
        }
    }
}

// TODO fix layer setting also for top side
void lp32x32_refresh_fixed(bool panel_temp[ROW_NUM][COL_NUM])
{
    int row;
    int col;
    int layer;

    for (row = 0; row < (ROW_NUM/2); ++row)
    {
        // TODO find the best match of the number of layers and clock speed
        for (layer = 0; layer < 1; ++layer)
        {
            for (col = 0; col < COL_NUM; ++col)
            {
                _lp32x32_setTop(panel_temp[row][col]);
                _lp32x32_setBottom(panel_temp[row + ROW_NUM/2][col], layer);
                lp32x32_clock(); ///< Shift RGB information of each column
            }

            lp32x32_setRow(row);
            lp32x32_setCtrlPin(LED32X32_PIN_OE);
            lp32x32_latch();
            lp32x32_clearCtrlPin(LED32X32_PIN_OE);
        }
    }

}
void lp32x32_refresh_fixed_scroll(bool panel_temp[ROW_NUM][COL_NUM])
{
    int row;
    int col;
    int layer;
    int step;

    int _row = 0;
    int _col = 0;

    for (step = 0; step < COL_NUM; ++step)
    {
        for (row = 0; row < (ROW_NUM/2); ++row)
        {
            // TODO find the best match of the number of layers and clock speed
            for (layer = 0; layer < 1; ++layer)
            {
                for (col = 0; col < COL_NUM; ++col)
                {
                    _row = row;
                    _col = col + step;

                    // TODO fix tearing in the second panel in daisy chain
                    if (_col > COL_NUM)
                    {
                        _row = _row-1;
                    }

                    _lp32x32_setTop(panel_temp[_row][_col]);
                    _lp32x32_setBottom(panel_temp[_row + ROW_NUM/2][_col], layer);
                    lp32x32_clock(); ///< Shift RGB information of each column
                }

                lp32x32_setRow(row);
                lp32x32_setCtrlPin(LED32X32_PIN_OE);
                lp32x32_latch();
                lp32x32_clearCtrlPin(LED32X32_PIN_OE);
            }
        }

        delay(50);
    }

}

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

//TODO remove this after debugging
void __lp32x32_setTop(bool led)
{
    if (led)
    {
        __led_setPwmLvl_top(150,150,1); // Do not set 0, otherwise strange behavior
    }
}

//TODO remove this after debugging
void __lp32x32_setBottom(bool led)
{
    if (led)
    {
        __led_setPwmLvl_bottom(50,1,50); // Do not set 0, otherwise strange behavior
    }
}

// TODO use pwm
void lp32x32_refresh(void)
{
    int row;
    int col;

    bool panel_temp[ROW_NUM][COL_NUM];

    // Draw top side "HI"
    panel_temp[1][2] = panel_temp[1][3] = panel_temp[1][7] = panel_temp[1][8] = panel_temp[1][11] = panel_temp[1][12] = true;
    panel_temp[2][2] = panel_temp[2][3] = panel_temp[2][7] = panel_temp[2][8] = panel_temp[2][11] = panel_temp[2][12] = true;
    panel_temp[5][2] = panel_temp[5][3] = panel_temp[5][7] = panel_temp[5][8] = panel_temp[5][11] = panel_temp[5][12] = true;
    panel_temp[6][2] = panel_temp[6][3] = panel_temp[6][7] = panel_temp[6][8] = panel_temp[6][11] = panel_temp[6][12] = true;

    panel_temp[3][2] = panel_temp[3][3] = panel_temp[3][4] = panel_temp[3][5] = panel_temp[3][6] = panel_temp[3][7] = panel_temp[3][8] = panel_temp[3][11] = panel_temp[3][12] = true;
    panel_temp[4][2] = panel_temp[4][3] = panel_temp[4][4] = panel_temp[4][5] = panel_temp[4][6] = panel_temp[4][7] = panel_temp[4][8] = panel_temp[4][11] = panel_temp[4][12] = true;

    // Draw bottom side "HI"
    panel_temp[17][2] = panel_temp[17][3] = panel_temp[17][7] = panel_temp[17][8] = panel_temp[17][11] = panel_temp[17][12] = true;
    panel_temp[18][2] = panel_temp[18][3] = panel_temp[18][7] = panel_temp[18][8] = panel_temp[18][11] = panel_temp[18][12] = true;
    panel_temp[21][2] = panel_temp[21][3] = panel_temp[21][7] = panel_temp[21][8] = panel_temp[21][11] = panel_temp[21][12] = true;
    panel_temp[22][2] = panel_temp[22][3] = panel_temp[22][7] = panel_temp[22][8] = panel_temp[22][11] = panel_temp[22][12] = true;

    panel_temp[19][2] = panel_temp[19][3] = panel_temp[19][4] = panel_temp[19][5] = panel_temp[19][6] = panel_temp[19][7] = panel_temp[19][8] = panel_temp[19][11] = panel_temp[19][12] = true;
    panel_temp[20][2] = panel_temp[20][3] = panel_temp[20][4] = panel_temp[20][5] = panel_temp[20][6] = panel_temp[20][7] = panel_temp[20][8] = panel_temp[20][11] = panel_temp[20][12] = true;

    lp32x32_setTopColor();
    lp32x32_setBottomColor();

    for (row = 0; row < (ROW_NUM/2); ++row)
    {
        for (col = 0; col < COL_NUM; ++col)
        {
            // TODO fix
            __lp32x32_setTop(panel_temp[row][col]);
            __lp32x32_setBottom(panel_temp[row + ROW_NUM/2][col]);
            lp32x32_clock(); ///< Shift RGB info. of each column
        }

        lp32x32_setRow(row);
        lp32x32_setCtrlPin(LED32X32_PIN_OE);
        lp32x32_latch();
        lp32x32_clearCtrlPin(LED32X32_PIN_OE);
    }
}
