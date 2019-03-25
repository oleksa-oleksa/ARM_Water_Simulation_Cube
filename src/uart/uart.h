/**
 * @file uart.h
 * @brief UART0 interface library
 * @date 2019-03-24
 * @author Kayoko Abe
 */

#ifndef UART_H_
#define UART_H_

#include "LPC23xx.h"

#include <lcd.h>

#define BUFSIZE 0x40 // = 64

typedef unsigned long DWORD;
typedef unsigned char BYTE;

/* Symbol of Power Control for eripherals register (PCONP) */
#define PCUART0 0x08
/* Symbols of UART0 Interrupt Enable Register (U0IER) */
#define IER_RBR  0x01
#define IER_THRE 0x02
#define IER_RLS  0x04
/* Symbols of UART0 Line Status Register (U0LSR) */
#define LSR_RDR  0x01 ///< Receiver Data Ready
#define LSR_OE   0x02 ///< Overrun Error
#define LSR_PE   0x04 ///< Parity Error
#define LSR_FE   0x08 ///< Framing Error
#define LSR_BI   0x10 ///< Break Interrupt
#define LSR_THRE 0x20 ///< Transmitter Holding Register Empty
#define LSR_TEMT 0x40 ///< Transmitter Empty
#define LSR_RXFE 0x80 ///< Error in Rx FIFO

void delay(int delay);
void lcd_printer(BYTE *msg);

/**
 * @brief 	  Initialize UART0 interface
 * @param[in] baudrate Baud rate of UART communication
 */
int uart0_init(unsigned long baudrate);
/**
 * @brief 	  Send data through UART0
 * @param[in] bufptr Pointer to datatype
 * @param[in] len    Data length
 */
void uart0_send(BYTE *bufptr, DWORD len);
void uart0_read(void);

#endif
