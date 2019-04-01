#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include <stdint.h>

typedef struct __attribute__((packed))
{
    uint8_t a;
    uint8_t b;
    uint8_t c;
    uint8_t d;
} sync_marker_t;

#define STD_VAL_SYNC_MARKER_T {.a = 0xF3, \
                               .b = 0xCF, \
                               .c = 0xC3, \
                               .d = 0xCF}


typedef struct __attribute__((packed))
{
    sync_marker_t sync_marker;
    uint8_t  panel_no;
    uint8_t  x_pos;
    uint8_t  y_pos;
    uint32_t color;
} pixeldata_t;

typedef struct __attribute__((packed))
{
    sync_marker_t sync_marker;
    double force_x;
    double force_y;
    double force_z;
} acc_data_t;


void protocol_init(void);

acc_data_t protocol_get_last_acc_data(void);

void protocol_send_pixel(pixeldata_t pixel);

#endif