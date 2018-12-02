#include "utils.h"
#include "led32x32.h"
#include "paint_tool.h"

#include <stdio.h>

#define LED32X32_PWMMODE
#define LED32X32
#define LED_PWMMODE
#define LED_DEFAULT

int main(void)
{
    RGB panel[ROW_NUM][COL_NUM];
    // TODO fill panel

    #if defined(LED32X32_PWMMODE)
       led32x32_pwmInit();
    #elif defined(LED32X32)
       led32x32_init();
    #elif defined(LED_PWMMODE)
       led_pwmInit();
    #elif defined(LED_DEFAULT)
       led_init();
       delay();
       led_on(0);
       delay();
       led_on(1);
       delay();
       led_allOff();
       led_selectOn(0xF1);
    #endif

    led32x32_init();
    while(1)
    {
        //lp32x32_refresh_fixed();
        lp32x32_refresh(panel);
    }
}
