#ifndef UTIL_H_
#define UTIL_H_

#include <stdint.h>
#include <stddef.h>


extern void PUT32(uint32_t addr, uint32_t val);
extern uint32_t GET32(uint32_t addr);
extern void foo(uint32_t val);
extern void enable_irq(void);
extern void enable_fiq(void);

extern void memcpy(void *dest, const void *source, uint32_t count);

#endif