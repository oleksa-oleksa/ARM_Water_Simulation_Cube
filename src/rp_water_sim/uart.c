#include "uart.h"

#include <util.h>

#define GPFSEL1 0x20200004
#define GPSET0  0x2020001C
#define GPCLR0  0x20200028
#define GPPUD       0x20200094
#define GPPUDCLK0   0x20200098

#define AUX_ENABLES     0x20215004
#define AUX_MU_IO_REG   0x20215040
#define AUX_MU_IER_REG  0x20215044
#define AUX_MU_IIR_REG  0x20215048
#define AUX_MU_LCR_REG  0x2021504C
#define AUX_MU_MCR_REG  0x20215050
#define AUX_MU_LSR_REG  0x20215054
#define AUX_MU_MSR_REG  0x20215058
#define AUX_MU_SCRATCH  0x2021505C
#define AUX_MU_CNTL_REG 0x20215060
#define AUX_MU_STAT_REG 0x20215064
#define AUX_MU_BAUD_REG 0x20215068

#define IRQ_BASIC 0x2000B200
#define IRQ_PEND1 0x2000B204
#define IRQ_PEND2 0x2000B208
#define IRQ_FIQ_CONTROL 0x2000B210
#define IRQ_ENABLE1 0x2000B210
#define IRQ_ENABLE2 0x2000B214
#define IRQ_ENABLE_BASIC 0x2000B218
#define IRQ_DISABLE1 0x2000B21C
#define IRQ_DISABLE2 0x2000B220
#define IRQ_DISABLE_BASIC 0x2000B224

rx_handler_fp_t _rx_handler;

void uart_init(rx_handler_fp_t rx_handler)
{
    uint32_t ra;

    PUT32(AUX_ENABLES, 1);
    PUT32(AUX_MU_IER_REG, 0);
    PUT32(AUX_MU_CNTL_REG, 0);
    PUT32(AUX_MU_LCR_REG, 3);
    PUT32(AUX_MU_MCR_REG, 0);
    PUT32(AUX_MU_IER_REG, 0x5); //enable rx interrupts
    PUT32(AUX_MU_IIR_REG, 0xC6);
    PUT32(AUX_MU_BAUD_REG, 270);

    ra = GET32(GPFSEL1);
    ra &= ~(7 << 12); //gpio14
    ra |= 2 << 12;    //alt5
    ra &= ~(7 << 15); //gpio15
    ra |= 2 << 15;    //alt5
    PUT32(GPFSEL1, ra);

    PUT32(GPPUD, 0);
    for(ra = 0; ra < 150; ra++)
    {
        foo(ra);
    }
    PUT32(GPPUDCLK0, (1 << 14)|(1 << 15));
    for(ra = 0; ra < 150; ra++)
    {
        foo(ra);
    }
    PUT32(GPPUDCLK0, 0);

    PUT32(AUX_MU_CNTL_REG, 3);

    _rx_handler = rx_handler;

    reset_rx_buf();
    PUT32(IRQ_ENABLE1, (1 << 29));
    enable_irq();
}


void uart_putc(uint8_t c)
{
    while(1)
    {
        if(GET32(AUX_MU_LSR_REG) & 0x20) 
        {
            break;
        }
    }
    PUT32(AUX_MU_IO_REG, c);
}


void hexstrings(uint32_t d)
{
    uint32_t rb;
    uint32_t rc;

    rb = 32;
    while(1)
    {
        rb -= 4;
        rc = (d >> rb) & 0xF;
        if(rc > 9)
        {
            rc += 0x37; 
        }
        else
        { 
            rc += 0x30;
        }
        uart_putc(rc);
        if(rb == 0) 
        {
            break;
        }
    }
    uart_putc(0x20);
}


void hexstring(uint32_t d)
{
    hexstrings(d);
    uart_putc(0x0D);
    uart_putc(0x0A);
}




volatile uint32_t rxhead;
#define RXBUFMASK 0xFFF
volatile uint8_t rxbuffer[RXBUFMASK+1];
//-------------------------------------------------------------------------
void c_irq_handler ( void )
{
    uint32_t rb,rc;

    //an interrupt has occurred, find out why
    while(1) //resolve all interrupts to uart
    {
        rb = GET32(AUX_MU_IIR_REG);
        if( (rb & 1) == 1) 
        {
            break; //no more interrupts
        }
        if((rb & 6) == 4)
        {
            //receiver holds a valid byte
            rc = GET32(AUX_MU_IO_REG); //read byte from rx fifo
            rxbuffer[rxhead] = rc & 0xFF;
            rxhead = (rxhead + 1) & RXBUFMASK;
        }
    }
    if(_rx_handler != NULL)
    {
        _rx_handler();
    }
}


void reset_rx_buf(void)
{
    rxhead = 0;
}


void remove_n_rx_buf(uint32_t n)
{
    if(n <= rxhead)
    {
        rxhead -= n;
    }
    else
    {
        rxhead = 0;
    }
}

uint8_t* get_rx_buf(void)
{
    return (uint8_t*)rxbuffer;
}


uint32_t get_rx_buf_size(void)
{
    return rxhead;
}

