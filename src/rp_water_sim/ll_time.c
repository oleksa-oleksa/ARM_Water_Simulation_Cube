#include "ll_time.h"


#define RPI_SYSTIMER_BASE       0x20003000


typedef struct __attribute__((packed))
{
    volatile uint32_t control_status;
    volatile uint32_t counter_lo;
    volatile uint32_t counter_hi;
    volatile uint32_t compare0;
    volatile uint32_t compare1;
    volatile uint32_t compare2;
    volatile uint32_t compare3;
} rpi_sys_timer_t;

static rpi_sys_timer_t* rpiSystemTimer = (rpi_sys_timer_t*)RPI_SYSTIMER_BASE;



uint64_t time_usec_get(void)
{
    return (((uint64_t)rpiSystemTimer->counter_hi) << 32) | rpiSystemTimer->counter_lo;
}


void time_usec_wait(uint64_t dt_usec)
{
    uint64_t finish = time_usec_get() + dt_usec;
    while(time_usec_get() < finish)
    {
        ;
    }
}

