#ifndef UTIL_H_
#define UTIL_H_

#include <stdint.h>


extern void PUT32(uint32_t addr, uint32_t val);
extern uint32_t GET32(uint32_t addr);
extern void foo(uint32_t val);


#endif