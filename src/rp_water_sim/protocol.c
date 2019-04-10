#include "protocol.h"

#include <util.h>
#include <uart.h>
#include <ll_time.h>

acc_data_t _last_acc_data  = {.force_x = 0, \
                              .force_y = 0, \
                              .force_z = 0};
sync_marker_t _exp_marker = STD_VAL_SYNC_MARKER_T;

void _uart_rx_handle(void)
{
    acc_data_t rec_acc_data;
    while(uart0_get_rx_buf_size() >= sizeof(acc_data_t))
    {
        memcpy(&rec_acc_data, uart0_get_rx_buf(), sizeof(acc_data_t));
        if((rec_acc_data.sync_marker.a == _exp_marker.a) && 
           (rec_acc_data.sync_marker.b == _exp_marker.b) &&
           (rec_acc_data.sync_marker.c == _exp_marker.c) &&
           (rec_acc_data.sync_marker.d == _exp_marker.d))
        {
            memcpy(&_last_acc_data, &rec_acc_data, sizeof(acc_data_t));
            uart0_remove_n_rx_buf(sizeof(acc_data_t));
        }
        else
        {
            uart0_remove_n_rx_buf(1); //resync by checking byte-for-byte untill sync-marker has been found
        }
    }
}


void protocol_init(void)
{
    uart0_init(_uart_rx_handle);
}


const acc_data_t* protocol_get_last_acc_data(void)
{
    return &_last_acc_data;
}


void protocol_send_pixel(pixeldata_t pixel)
{
    uint8_t buff[sizeof(pixeldata_t)];

    memcpy(&pixel.sync_marker, &_exp_marker, sizeof(sync_marker_t));
    memcpy(buff, &pixel, sizeof(pixeldata_t));
    for(uint32_t i = 0; i < sizeof(pixeldata_t); ++i)
    {
        uart0_putc(buff[i]);
    }
    time_usec_wait(100);
}


void protocol_send_panel(particle_grid_element_t panel[PARTICLE_GRID_X][PARTICLE_GRID_Y], enum side_e side)
{
    uint32_t x, y;
    for(x = 0; x < PARTICLE_GRID_X; ++x)
    {
        for(y = 0; y < PARTICLE_GRID_Y; ++y)
        {
            pixeldata_t pixel;
            pixel.panel_no = side;
            pixel.x_pos = x;
            pixel.y_pos = y;
            pixel.color = panel[x][y].particle_count;
            protocol_send_pixel(pixel);
        }
    }
}


