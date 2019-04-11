#ifndef LL_TIME_H_
#define LL_TIME_H_

#include <stdint.h>

#define MSEC2USEC(msec) (msec * 1000)

uint64_t time_usec_get(void);

void time_usec_wait(uint64_t dt_usec);



#endif