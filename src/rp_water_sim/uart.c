#include "uart.h"

#include <util.h>

#define GPFSEL1             0x20200004  ///< 
#define GPSET0              0x2020001C  ///< 
#define GPCLR0              0x20200028  ///< 
#define GPPUD               0x20200094  ///< 
#define GPPUDCLK0           0x20200098  ///< 

//mini UART

#define AUX_ENABLES         0x20215004  ///< 
#define AUX_MU_IO_REG       0x20215040  ///< Mini-UART FiFo read / write
#define AUX_MU_IER_REG      0x20215044  ///< Mini-UART interrupt enable register
#define AUX_MU_IIR_REG      0x20215048  ///< Mini-UART Interrupt status register
#define AUX_MU_LCR_REG      0x2021504C  ///< Mini-UART data format control register
#define AUX_MU_MCR_REG      0x20215050  ///< Mini-UART 'modem' signal control register
#define AUX_MU_LSR_REG      0x20215054  ///< Mini-UART data status register
#define AUX_MU_MSR_REG      0x20215058  ///< Mini-UART 'modem' status register
#define AUX_MU_SCRATCH      0x2021505C  ///< Mini-UART single bit storate register
#define AUX_MU_CNTL_REG     0x20215060  ///< Mini-UART extra feature register
#define AUX_MU_STAT_REG     0x20215064  ///< Mini-UART extra status register
#define AUX_MU_BAUD_REG     0x20215068  ///< Mini-UART baudrate register

//PL011 UART
#define UART0_DR            0x20201000  ///< UART0 data register
#define UART0_RSRECR        0x20201004  ///< UART0 receive status register
#define UART0_FR            0x20201018  ///< UART0 flag register
#define UART0_ILPR          0x20201020  ///< UART0 disabled IrDA register
#define UART0_IBRD          0x20201024  ///< UART0 baud rate divisor register
#define UART0_FBRD          0x20201028  ///< UART0 baud rate fractional value
#define UART0_LCRH          0x2020102C  ///< UART0 line control register
#define UART0_CR            0x20201030  ///< UART0 control register
#define UART0_IFLS          0x20201034  ///< UART0 interrupt FiFo level select register
#define UART0_IMSC          0x20201038  ///< UART0 interrupt mask register
#define UART0_RIS           0x2020103C  ///< UART0 raw interrupt status register
#define UART0_MIS           0x20201040  ///< UART0 masked interrupt register
#define UART0_ICR           0x20201044  ///< UART0 interrupt clear register
#define UART0_DMACR         0x20201048  ///< UART0 disabled DMA control register
#define UART0_ITCR          0x20201080  ///< UART0 test control register
#define UART0_ITIP          0x20201084  ///< UART0 test control register
#define UART0_ITOP          0x20201088  ///< UART0 test control register
#define UART0_TDR           0x2020108C  ///< UART0 test data register


#define IRQ_BASIC           0x2000B200  ///< 
#define IRQ_PEND1           0x2000B204  ///< 
#define IRQ_PEND2           0x2000B208  ///< 
#define IRQ_FIQ_CONTROL     0x2000B210  ///< 
#define IRQ_ENABLE1         0x2000B210  ///< 
#define IRQ_ENABLE2         0x2000B214  ///< 
#define IRQ_ENABLE_BASIC    0x2000B218  ///< 
#define IRQ_DISABLE1        0x2000B21C  ///< 
#define IRQ_DISABLE2        0x2000B220  ///< 
#define IRQ_DISABLE_BASIC   0x2000B224  ///< 




#define RXBUFMASK 0xFFF
rx_handler_fp_t _mini_uart_rx_handler = NULL;
rx_handler_fp_t _uart0_rx_handler = NULL;
volatile uint32_t mini_uart_rxhead;
volatile uint32_t uart0_rxhead;
volatile uint8_t mini_uart_rxbuffer[RXBUFMASK+1];
volatile uint8_t uart0_rxbuffer[RXBUFMASK+1];

//-------------------------------------------------------------------------

void c_irq_handler ( void )
{
    uint32_t rb,rc;

    //an interrupt has occurred, find out why
    while(1) //resolve all interrupts to mini uart
    {
        rb = GET32(AUX_MU_IIR_REG);
        if( (rb & 1) == 1) 
        {
            if(_mini_uart_rx_handler != NULL)
            {
                _mini_uart_rx_handler();
            }
            break; //no more interrupts
        }
        if((rb & 6) == 4)
        {
            //receiver holds a valid byte
            rc = GET32(AUX_MU_IO_REG); //read byte from rx fifo
            mini_uart_rxbuffer[mini_uart_rxhead] = rc & 0xFF;
            mini_uart_rxhead = (mini_uart_rxhead + 1) & RXBUFMASK;
        }
    }
    rb = GET32(UART0_MIS);
    if( (rb & 0x10) == 1)
    {//got UART0 Rx irq
        while(1)
        {
            rb = GET32(UART0_FR);
            if( (rb & 0x10) == 0) //Rx FiFo not empty
            {
                rc = GET32(UART0_DR);
                uart0_rxbuffer[uart0_rxhead] = rc & 0xFF;
                uart0_rxhead = (uart0_rxhead + 1) & RXBUFMASK;
            }
            else
            {
                break;
            }
        }
    }
}


//-------------------------------------------------------------------------


void mini_uart_init(rx_handler_fp_t rx_handler)
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

    _mini_uart_rx_handler = rx_handler;

    mini_uart_reset_rx_buf();
    PUT32(IRQ_ENABLE1, (1 << 29));
    enable_irq();
}

void uart0_init(rx_handler_fp_t rx_handler)
{
    unsigned int ra;

    PUT32(UART0_CR, 0);

    ra = GET32(GPFSEL1);
    ra &= ~(7 << 12); //gpio14
    ra |= 4 << 12;    //alt0
    ra&=~(7 << 15);   //gpio15
    ra |= 4 << 15;    //alt0
    PUT32(GPFSEL1, ra);

    PUT32(GPPUD, 0);
    for(ra = 0; ra < 150; ra++)
    {
        foo(ra);
    }
    PUT32(GPPUDCLK0, (1 << 14) | (1 << 15));
    for(ra = 0; ra < 150; ra++)
    {
        foo(ra);
    }
    PUT32(GPPUDCLK0, 0);

    PUT32(UART0_ICR, 0x7FF);    //0111 1111 1111 <- clear all interrupts
    //(3000000 / (16 * 115200) = 1.627
    //(0.627*64)+0.5 = 40
    //int 1 frac 40
    PUT32(UART0_IBRD, 1);       //set integer baud rate
    PUT32(UART0_FBRD, 40);      //set fractional baud rate
    PUT32(UART0_LCRH, 0x70);    //0111 0000 <- no sticky parity, 8bits word length, FiFo mode, one stopbit, no parity, no break
    PUT32(UART0_CR, 0x301);     //0011 0000 0001 <- Rx enable, Tx enable, UART enable
    PUT32(UART0_IMSC, 0x7EF);   //111 1110 1111 <- Unmask receive interrupt

    _mini_uart_rx_handler = rx_handler;

    uart0_reset_rx_buf();

    PUT32(IRQ_ENABLE2, (1 << 25));

    enable_irq();
}


void mini_uart_putc(uint8_t c)
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


void uart0_putc(uint8_t c)
{
    while(1)
    {
        if((GET32(UART0_FR) & 0x4) == 0) 
        {//wait while BUSY
            break;
        }
    }
    PUT32(UART0_DR, c);
}


void mini_uart_hexstrings(uint32_t d)
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
        mini_uart_putc(rc);
        if(rb == 0) 
        {
            break;
        }
    }
    mini_uart_putc(0x20);
}


void uart0_hexstrings(uint32_t d)
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
        uart0_putc(rc);
        if(rb == 0) 
        {
            break;
        }
    }
    uart0_putc(0x20);
}


void mini_uarthexstring(uint32_t d)
{
    mini_uart_hexstrings(d);
    mini_uart_putc(0x0D);
    mini_uart_putc(0x0A);
}


void uart0_hexstring(uint32_t d)
{
    uart0_hexstrings(d);
    uart0_putc(0x0D);
    uart0_putc(0x0A);
}


void mini_uart_reset_rx_buf(void)
{
    mini_uart_rxhead = 0;
}


void uart0_reset_rx_buf(void)
{
    uart0_rxhead = 0;
}


void mini_uart_remove_n_rx_buf(uint32_t n)
{
    if(n <= mini_uart_rxhead)
    {
        mini_uart_rxhead -= n;
        memcpy((void*)&mini_uart_rxbuffer[0], (void*)&mini_uart_rxbuffer[n], mini_uart_rxhead - n);
    }
    else
    {
        mini_uart_rxhead = 0;
    }
}


void uart0_remove_n_rx_buf(uint32_t n)
{
    if(n <= uart0_rxhead)
    {
        uart0_rxhead -= n;
        memcpy((void*)&uart0_rxbuffer[0], (void*)&uart0_rxbuffer[n], uart0_rxhead - n);
    }
    else
    {
        uart0_rxhead = 0;
    }
}


uint8_t* mini_uart_get_rx_buf(void)
{
    return (uint8_t*)mini_uart_rxbuffer;
}


uint8_t* uart0_get_rx_buf(void)
{
    return (uint8_t*)uart0_rxbuffer;
}


uint32_t mini_uart_get_rx_buf_size(void)
{
    return mini_uart_rxhead;
}


uint32_t uart0_get_rx_buf_size(void)
{
    return uart0_rxhead;
}

