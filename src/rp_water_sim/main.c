#include <gpio.h>
#include <util.h>
#include <uart.h>
#include <ll_time.h>


int main(void)
{
    // uint32_t tim;


    led_setup();
    uart_init();

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

        for(uint32_t i = 0; i < get_rx_buf_size(); ++i)
        {
            uart_putc(get_rx_buf()[i]);
        }
        reset_rx_buf();
    }
}