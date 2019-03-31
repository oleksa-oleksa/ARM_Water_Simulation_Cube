#include <gpio.h>
#include <util.h>


int main(void)
{
    uint32_t tim;

    
    led_setup();

    while(1)
    {
        for(tim = 0; tim < 500000; tim++)
        {
            foo(tim);
        }

        led_set(true);

        for(tim = 0; tim < 500000; tim++)
        {
            foo(tim);
        }

        led_set(false);
    }
}