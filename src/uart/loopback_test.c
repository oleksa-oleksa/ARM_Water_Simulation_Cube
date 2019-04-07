
#include <stdint.h>

#include "uart.h"



extern volatile uint32_t uart0Count;
extern volatile uint8_t uart0RxBuffer[BUFSIZE];

int main(void)
{
    const uint8_t msg[11] = {"Hello Cube"};

    CCLKCFG = 0x3; // generates 72 MHz
    //lcd_init();
    //lcd_printer("Start!!");

    if (uart0_init(115200))
    {
        return 1;
    }

    while (1)
    {
        RBR_DISABLE;
            uart0_send((uint8_t *)msg, 11);
            uart0Count = 0;
        RBR_ENABLE;

        THRE_DISABLE;
            uart0_read(); // TODO this function will be deprecated
        THRE_ENABLE;
    }

    return 0;
}
