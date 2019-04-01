#ifndef UART_T_
#define UART_T_

#include <stdint.h>

typedef void (*rx_handler_fp_t)(void);



void uart_init(rx_handler_fp_t rx_handler);

void uart_putc(uint8_t c);

void hexstrings(uint32_t d);

void hexstring(uint32_t d);

uint8_t* get_rx_buf(void);

void reset_rx_buf(void);

void remove_n_rx_buf(uint32_t n);

uint32_t get_rx_buf_size(void);

#endif