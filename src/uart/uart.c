/**
 * @file uart.c
 * @brief UART0 interface library
 * @date 2019-03-24
 * @author Kayoko Abe
 */
#include "uart.h"
#include <irq.h>

volatile uint32_t uart0Status; // TODO check for its usage and necessity
volatile uint8_t uart0TxEmpty = 1;
volatile uint8_t uart0RxBuffer[BUFSIZE];
volatile uint32_t uart0Count = 0;
rx_handler_fp_t _rx_handler = NULL;

static inline void _readRxFifo(void)
{
    while(!(U0LSR & 0x01))
    {
        ;
    }

    uart0RxBuffer[uart0Count] = U0RBR;
    ++uart0Count;
    if (uart0Count == BUFSIZE)
    {
        uart0Count = 0;
    }
    if(_rx_handler != NULL)
    {
        _rx_handler();
    }
}


/**
 * @brief Interrupt Service Routine for UART0
 * Three interrupt sources are available.
 * Check which interrupt is set: see U0IIR register, 3:1 bit.
 *   Int 1) Errors
 *   011: Receive Line Status (RLS)
 *
 *   Int 2) Rx FIFO
 *   010: Receive Data Available (RDA)
 *   110: Character Time-out Indicator (CTI)
 *
 *   Int 3) Tx FIFO
 *   001: THRE Interrupt
 */
void uart0_isr(void) __irq
{
    uint8_t intId;    ///< InterruptIdRegister
    uint8_t LSRValue; ///< LineStatusRegister
    uint8_t dummy = dummy;

    IENABLE; ///< Handles nested interrupt

    intId = (U0IIR >> 1) & 0x07;

    switch (intId)
    {
    case (0x03): /* Change occurred in Receive Line Status (RLS) */
        LSRValue = U0LSR;

        if ( LSRValue & (LSR_OE | LSR_PE | LSR_FE | LSR_RXFE | LSR_BI) )
        {
            /* There are errors or break interrupt */
            /* Read LSR will clear the interrupt */
            uart0Status = LSRValue;
            dummy = U0RBR;        /* Dummy read on RX (ReceiverBufferRegister) to clear interrupt, then bail out */
            IDISABLE;
            VICVectAddr = 0;      /* Acknowledge Interrupt */
            return;
        }
        if ( LSRValue & LSR_RDR ) /* Receive Data Ready */
        {
            /* If no error on RLS, normal ready, save into the data buffer. */
            /* Note: read RBR will clear the interrupt */
            _readRxFifo();
        }
        break;

    case (0x02): /* Receive Data Available (RDA) */
        _readRxFifo();
        break;

    case (0x06): /* Character Time-out Indicator (CTI) */
        uart0Status |= 0x100; /* Bit 9 as the CTI error */
        break;

    case (0x01): /* THRE Interrupt */
        LSRValue = U0LSR; /* Check status in the LSR to see if valid data in U0THR or not */
        if ( LSRValue & LSR_THRE )
        {
            uart0TxEmpty = 1;
        }
        else
        {
            uart0TxEmpty = 0;
        }
        break;
    }

    IDISABLE;
    VICVectAddr = 0; ///< Acknowledge Interrupt
}


/**
 * @brief Initialize UART0 by the following steps:
 * 1. Power in PCONP register, set bit PCUART0 (default enabled)
 * 2. Peripheral clock in PCLK_SEL0 register, select PCLK_UART0
 * 3. Baud rate in U0LCR register, set bit DLAB = 1.
 * 4. UART Fifo: user bit fifo enable (bit 0) in register U0FCR to enable fifo.
 * 5. Pins, select UART pins and pin modes in register PINSEL and PINMODE
 * 6. Interrupts: set DLAB = 0 in register U0LCR. VICIntEnable register in VIC.
 *
 * @param[in] baudrate Baud rate of UART communication
 * @warn      Current init seting is for baud rate 115200, Osc 12 MHz, Sysclk 72 MHz.
*/
int uart0_init(unsigned long baudrate, rx_handler_fp_t rx_handler)
{
    PCONP |= PCUART0; ///< UART0 power/clock control

    if (115200 != baudrate)
    {
        // TODO baudrate generator algorithm for other variations if needed
        return 1;
    }
    _rx_handler = rx_handler;

    /* Set 7:6 bit of PCLKSEL0
    * 00: PCLK = CCLK/4
    * 01: PCLK = CCLK
    * 10: PCLK = CCLK/2
    * 11: PCLK = CCLK/8
    */
    PCLKSEL0 |= 0x00; ///< PCLK_UART0

    U0LCR |= 0x83; ///< Enable access to Divisor Latches, 8 bits, 1 stop bit, no parity

    /*
    * UART0 Divisor Latch LSB register
    *
    * /f[
    *    UART0_baudrate = PCLK / [16 * (256 * U0DLM + U0DLL) * (1+ DivAddVal/MulVal)]
    * /f]
    *
    * See Fig 83. Algorithm for setting UART dividers in LPC23xx_um for details.
    */
    U0DLM = 0; // Higher 8 bits of Divisor
    U0DLL = 6; // Lower 8 bits of Divisor
    /* UART0 Fractional Divider Register controls prescaler for baud rate generator */
    // MULVAL = 8; DIVADDVAL = 5;
    U0FDR |= 0x05;
    U0FDR &= 0xFFFFFF0F;
    U0FDR |= 0x80;

    U0LCR &= 0x7F; ///< Disable access to Divisor Latches
    U0FCR |= 0x07; ///< Enable Rx and Tx FIFOs and clear them

    PINSEL0 |= 0x50; ///< Set Pins P0.2, P0.3 in TXD0, RXD0 mode (PMODE has to 00 (pull-up))

    if (0 == install_irq(UART0_INT, (void *)uart0_isr, HIGHEST_PRIORITY))
    {
        return 1;
    }
    
    U0IER |= (IER_RBR | IER_THRE | IER_RLS); ///< Enable three UART0 interrupt sources: RBR, THRE, Rx Line Status

    return 0;
}


void uart0_send(uint8_t *bufptr, uint32_t len)
{
    while (0 != len)
    {
        while (0 == uart0TxEmpty)
        {
            ;
        }

        U0THR = *bufptr;
        
        uart0TxEmpty = 0;
        ++bufptr;
        --len;
    }
}


uint8_t* uart0_buf(void)
{
    return (uint8_t*)uart0RxBuffer;
}


uint32_t* uart0_buf_size(void)
{
    return (uint32_t*)&uart0Count;
}




// TODO deprecate this function as RxBuffer can directly be read in main function.
void uart0_read(void)
{
    uint8_t* ptr;

    for (ptr = (uint8_t*)uart0RxBuffer; *ptr != '\0'; ++ptr)
    {
        //Read ptr
    }
    uart0RxBuffer[0] = 0;
}
