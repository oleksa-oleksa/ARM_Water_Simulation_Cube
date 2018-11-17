#include "utils.h"
#include "led32x32.h"

//#define LED_DEFAULT 1
#define LED_PWMMODE 1
#define LED32X32 1

int main(void)
{
#ifdef LED32X32
    led32x32_init();
    lp32x32_refresh_demo();

#elif LED_DEFAULT
    led_init();
    delay();
    led_on(0);
    delay();
    led_on(1);
    delay();
    led_all_off();
    led_select_on(0xF1);
#elif LED_PWMMODE
    led_pwmInit();
#endif

    while (1)
    {
        lp32x32_refresh_demo();
    }

}
