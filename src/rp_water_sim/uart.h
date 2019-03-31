#ifndef UART_T_
#define UART_T_

#include <stdint.h>

void uart_init(void);

void uart_putc(uint32_t c);

void hexstrings(uint32_t d);

void hexstring(uint32_t d);

uint8_t* get_rx_buf(void);

void reset_rx_buf(void);

uint32_t get_rx_buf_size(void);

#endif