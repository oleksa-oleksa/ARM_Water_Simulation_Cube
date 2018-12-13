#include "utils.h"
#include "led32x32.h"
#include "paint_tool.h"

#include <stdbool.h>

#define LED32X32_PWMMODE 0
#define LED32X32         1
#define LED_PWMMODE      2
#define LED_DEFAULT      3

#define MODE LED32X32

void fill_panel_fixed_info(bool panel_temp[ROW_NUM][COL_NUM]); // Used in mode LED32X32

int main(void)
{
    //uint32_t demoGrid[ROW_NUM][COL_NUM]; ///< Include information on particle density per LED pixel

    #if MODE == LED32X32_PWMMODE
        led32x32_init();
        led_setPwm();

    #elif MODE == LED32X32
        led32x32_init();
        bool panel_temp[ROW_NUM][COL_NUM] = {false};
        fill_panel_fixed_info(panel_temp);

    #elif MODE == LED_PWMMODE
        led_init();
        led_setPwm();

        led_setPwmLvl(1000,500,200,100,70,50,10);
        led_setPwmLvl(1000,10,50,70,100,200,500);
        led_setPwmLvl(1000,10,500,10,500,10,500);
        led_setPwmLvl(1000,1000,1000,1000,200,200,200);

    #elif MODE == LED_DEFAULT
        led_init();

        delay(100);
        led_on(0);
        delay(100);
        led_on(1);
        delay(100);
        led_allOff();
        led_selectOn(0xF1);

    #endif

    while(1)
    {
        lp32x32_refresh_fixed(panel_temp);
        //lp32x32_refresh(demoGrid);
    }
}

void fill_panel_fixed_info(bool panel_temp[ROW_NUM][COL_NUM])
{
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
}
