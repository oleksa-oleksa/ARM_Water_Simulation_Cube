#include <gpio.h>
#include <util.h>
#include <uart.h>
#include <protocol.h>
#include <ll_time.h>


int main(void)
{
    // uint32_t tim;


    led_setup();
    protocol_init();

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

        hexstring(0x12345678);

        pixeldata_t pixel = {.x_pos = 0x43, .y_pos = 0x44, .color = 0x57575757};
        protocol_send_pixel(pixel);
    }
}