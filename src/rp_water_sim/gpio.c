#include "gpio.h"

#include <util.h>

#define LED_GPFSEL      GPIO_OFFSET_GPFSEL1
#define LED_GPFBIT      18
#define LED_GPSET       GPIO_OFFSET_GPSET0
#define LED_GPCLR       GPIO_OFFSET_GPCLR0
#define LED_GPIO_BIT    16


void gpio_reg_write(uint32_t offset, uint32_t val)
{
    PUT32(GPIO_BASE + (sizeof(uint32_t) * offset), val);
}


uint32_t gpio_reg_read(uint32_t offset)
{
    return GET32(GPIO_BASE + (sizeof(uint32_t) * offset));
}


void gpio_bit_set(uint32_t offset, uint8_t bit)
{
    uint32_t val = GET32(GPIO_BASE + (sizeof(uint32_t) * offset));
    val |= 1 << bit;
    PUT32(GPIO_BASE + (sizeof(uint32_t) * offset), val);
}


void gpio_bit_reset(uint32_t offset, uint8_t bit)
{
    uint32_t val = GET32(GPIO_BASE + (sizeof(uint32_t) * offset));
    val &= ~(1 << bit);
    PUT32(GPIO_BASE + (sizeof(uint32_t) * offset), val);
}


void led_setup(void)
{
    gpio_bit_set(LED_GPFSEL, LED_GPFBIT);
}


void led_set(bool enable)
{
    //LED low-active
    if(enable == true)
    {
        gpio_reg_write(LED_GPCLR, (1 << LED_GPIO_BIT));
    }
    else
    {
        gpio_reg_write(LED_GPSET, (1 << LED_GPIO_BIT));
    }
}

