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
    //uint32_t demoGrid[ROW_NUM][COL_NUM]; ///< Include information on particle density per LED pixel

    #if MODE == LED32X32_PWMMODE
        led32x32_pwmInit();

    #elif MODE == LED32X32
        led32x32_init();

    #elif MODE == LED_PWMMODE
        led_pwmInit();
        led_setPwmLvl(1000,500,200,100,70,50,10);
        led_setPwmLvl(1000,10,50,70,100,200,500);
        led_setPwmLvl(1000,10,500,10,500,10,500);

    #elif MODE == LED_DEFAULT
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
