#include "protocol.h"

#include <stddef.h>
#include <string.h>

#include <uart.h>

#include <lcd.h>
#include <stdio.h>


pixeldata_t _last_pixel;
sync_marker_t _exp_marker = STD_VAL_SYNC_MARKER_T;
panel_t* _panels = NULL;



void _uart_rx_handle(void) __irq
{
    pixeldata_t rec_pixel;
    while(*uart0_buf_size() >= sizeof(pixeldata_t))
    {
        memcpy(&rec_pixel, uart0_buf(), sizeof(pixeldata_t));
        if((rec_pixel.sync_marker.a == _exp_marker.a) && 
           (rec_pixel.sync_marker.b == _exp_marker.b) &&
           (rec_pixel.sync_marker.c == _exp_marker.c) &&
           (rec_pixel.sync_marker.d == _exp_marker.d))
        {
            memcpy(&_last_pixel, &rec_pixel, sizeof(pixeldata_t));
            *uart0_buf_size() -= sizeof(pixeldata_t);

            //update pixelmatrix
            _panels[rec_pixel.panel_no][rec_pixel.x_pos][rec_pixel.y_pos].particle_count = rec_pixel.color;
        }
        else
        {
            *uart0_buf_size() -= 1; //resync by checking byte-for-byte untill sync-marker has been found
        }
    }
}



int protocol_init(panel_t panels[6])
{
    _panels = panels;
    return uart0_init(115200, _uart_rx_handle);
}


pixeldata_t protocol_get_last_pixel(void)
{
    return _last_pixel;
}


void protocol_send_acc_data(acc_data_t acc_data)
{
    uint8_t buff[sizeof(acc_data_t)];

    memcpy(&acc_data.sync_marker, &_exp_marker, sizeof(sync_marker_t));
    memcpy(buff, &acc_data, sizeof(acc_data_t));

    RBR_DISABLE;
    uart0_send(buff, sizeof(acc_data_t));
    RBR_ENABLE;
}
