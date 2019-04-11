#ifndef UART_T_
#define UART_T_

#include <stdint.h>

typedef void (*rx_handler_fp_t)(void);



void mini_uart_init(rx_handler_fp_t rx_handler);

void mini_uart_putc(uint8_t c);

void mini_uart_hexstrings(uint32_t d);

void mini_uart_hexstring(uint32_t d);

uint8_t* mini_uart_get_rx_buf(void);

void mini_uart_reset_rx_buf(void);

void mini_uart_remove_n_rx_buf(uint32_t n);

uint32_t mini_uart_get_rx_buf_size(void);


void uart0_init(rx_handler_fp_t rx_handler);

void uart0_putc(uint8_t c);

void uart0_hexstrings(uint32_t d);

void uart0_hexstring(uint32_t d);

uint8_t* uart0_get_rx_buf(void);

void uart0_reset_rx_buf(void);

void uart0_remove_n_rx_buf(uint32_t n);

uint32_t uart0_get_rx_buf_size(void);

uint8_t uart0_getc(void);

#endif