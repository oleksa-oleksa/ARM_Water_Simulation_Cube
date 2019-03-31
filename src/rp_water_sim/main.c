#include <gpio.h>
#include <util.h>
#include <ll_time.h>


int main(void)
{
    // uint32_t tim;


    led_setup();

    while(1)
    {
        // for(tim = 0; tim < 500000; tim++)
        // {
        //     foo(tim);
        // }   
        time_usec_wait(500 * 1000);

        led_set(true);

        // for(tim = 0; tim < 500000; tim++)
        // {
        //     foo(tim);
        // }
        time_usec_wait(500 * 1000);

        led_set(false);
    }
}