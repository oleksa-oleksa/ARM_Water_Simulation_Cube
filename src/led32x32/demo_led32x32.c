#include "utils.h"
#include "led32x32.h"
#include "paint_tool.h"

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define LED32X32         1
#define LED_PWMMODE      2
#define LED_DEFAULT      3

#define MODE LED32X32

/* Prepare panels to be chained */
static panel_t panel_1; // Each pixel has 32-bit
static panel_t panel_2;
static panel_t panel_3;
static panel_t panel_4;
static panel_t panel_5;
static panel_t panel_6;
static panel_t panels[CHAIN_LEN];

// TODO replace with data from simulation
void fillPanel(void)
{
    int r,c;

    for (r = 0; r < ROW_NUM/2; ++r)
    {
        for (c = 0; c < COL_NUM; ++c)
        {
            panel_1[r][c] = 1;             // Upper
            panel_1[r + ROW_NUM/2][c] = 0; // Bottom

            panel_2[r][c] = 0;
            panel_2[r + ROW_NUM/2][c] = 1;

            panel_3[r][c] = 1;
            panel_3[r + ROW_NUM/2][c] = 0;

            panel_4[r][c] = 0;
            panel_4[r + ROW_NUM/2][c] = 1;

            panel_5[r][c] = 1;
            panel_5[r + ROW_NUM/2][c] = 0;

            panel_6[r][c] = 0;
            panel_6[r + ROW_NUM/2][c] = 0;
        }
    }

    /* Store panels to array */
    memcpy(panels[0], panel_1, sizeof(panel_t));
    memcpy(panels[1], panel_2, sizeof(panel_t));
    memcpy(panels[2], panel_3, sizeof(panel_t));
    memcpy(panels[3], panel_4, sizeof(panel_t));
    memcpy(panels[4], panel_5, sizeof(panel_t));
    memcpy(panels[5], panel_6, sizeof(panel_t));
}

int main(void)
{
    #if MODE == LED32X32
        /* Initialize hardware */
        led32x32_init();

        /* Keep updating and refreshing panels */
        while(1)
        {
            fillPanel();
            lp32x32_refresh_chain(panels);
        }

    #elif MODE == LED_PWMMODE
        led_init();
        led_setPwm();

        led_setPwmLvl(1000,500,200,100,70,50,10);
        led_setPwmLvl(1000,10,50,70,100,200,500);
        led_setPwmLvl(1000,10,500,10,500,10,500);
        led_setPwmLvl(1000,1000,1000,1000,200,200,200);

        while(1)
        {
        }

    #elif MODE == LED_DEFAULT
        led_init();

        delay(100);
        led_on(0);
        delay(100);
        led_on(1);
        delay(100);
        led_allOff();
        led_selectOn(0xF1);

        while(1)
        {
        }

    #endif

}
