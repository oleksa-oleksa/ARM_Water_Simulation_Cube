#ifndef GPIO_H_
#define GPIO_H_

#include <stdint.h>
#include <stdbool.h>

#define GPIO_BASE       0x20200000UL

#define GPIO_OFFSET_GPFSEL0    0
#define GPIO_OFFSET_GPFSEL1    1
#define GPIO_OFFSET_GPFSEL2    2
#define GPIO_OFFSET_GPFSEL3    3
#define GPIO_OFFSET_GPFSEL4    4
#define GPIO_OFFSET_GPFSEL5    5

#define GPIO_OFFSET_GPSET0     7
#define GPIO_OFFSET_GPSET1     8

#define GPIO_OFFSET_GPCLR0     10
#define GPIO_OFFSET_GPCLR1     11

#define GPIO_OFFSET_GPLEV0     13
#define GPIO_OFFSET_GPLEV1     14

#define GPIO_OFFSET_GPEDS0     16
#define GPIO_OFFSET_GPEDS1     17

#define GPIO_OFFSET_GPREN0     19
#define GPIO_OFFSET_GPREN1     20

#define GPIO_OFFSET_GPFEN0     22
#define GPIO_OFFSET_GPFEN1     23

#define GPIO_OFFSET_GPHEN0     25
#define GPIO_OFFSET_GPHEN1     26

#define GPIO_OFFSET_GPLEN0     28
#define GPIO_OFFSET_GPLEN1     29

#define GPIO_OFFSET_GPAREN0    31
#define GPIO_OFFSET_GPAREN1    32

#define GPIO_OFFSET_GPAFEN0    34
#define GPIO_OFFSET_GPAFEN1    35

#define GPIO_OFFSET_GPPUD      37
#define GPIO_OFFSET_GPPUDCLK0  38
#define GPIO_OFFSET_GPPUDCLK1  39



void gpio_reg_write(uint32_t offset, uint32_t val);

uint32_t gpio_reg_read(uint32_t offset);

void gpio_bit_set(uint32_t offset, uint8_t bit);

void gpio_bit_reset(uint32_t offset, uint8_t bit);

void led_setup(void);

void led_set(bool enable);



#endif
