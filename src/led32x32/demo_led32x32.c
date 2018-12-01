#include "utils.h"
#include "led32x32.h"
#include "paint_tool.h"

#define LED32X32_PWMMODE 0
#define LED32X32         1
#define LED_PWMMODE      2
#define LED_DEFAULT      3

#define MODE LED_PWMMODE

int main(void)
{
    uint32_t demoGrid[ROW_NUM][COL_NUM]; ///< Include information on particle density per LED pixel

    #if MODE == 0
       led32x32_pwmInit();
    #elif MODE == 1
       led32x32_init();
    #elif MODE == 2
       led_pwmInit();
    #elif MODE == 3
       led_init();
       delay();
       led_on(0);
       delay();
       led_on(1);
       delay();
       led_allOff();
       led_selectOn(0xF1);
    #endif

    while(1)
    {
        //lp32x32_refresh_fixed();
        //lp32x32_refresh(demoGrid);
    }
}
