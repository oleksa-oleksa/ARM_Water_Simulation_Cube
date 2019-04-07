#include "protocol.h"

#include <util.h>
#include <uart.h>

acc_data_t _last_acc_data  = {.force_x = 0, \
                              .force_y = 0, \
                              .force_z = 0};
sync_marker_t _exp_marker = STD_VAL_SYNC_MARKER_T;

void _uart_rx_handle(void)
{
    acc_data_t rec_acc_data;
    while(get_rx_buf_size() >= sizeof(acc_data_t))
    {
        memcpy(&rec_acc_data, get_rx_buf(), sizeof(acc_data_t));
        if((rec_acc_data.sync_marker.a == _exp_marker.a) && 
           (rec_acc_data.sync_marker.b == _exp_marker.b) &&
           (rec_acc_data.sync_marker.c == _exp_marker.c) &&
           (rec_acc_data.sync_marker.d == _exp_marker.d))
        {
            memcpy(&_last_acc_data, &rec_acc_data, sizeof(acc_data_t));
            remove_n_rx_buf(sizeof(acc_data_t));
        }
        else
        {
            remove_n_rx_buf(1); //resync by checking byte-for-byte untill sync-marker has been found
        }
    }
}


void protocol_init(void)
{
    uart_init(_uart_rx_handle);
}


acc_data_t protocol_get_last_acc_data(void)
{
    return _last_acc_data;
}


void protocol_send_pixel(pixeldata_t pixel)
{
    uint8_t buff[sizeof(pixeldata_t)];

    memcpy(&pixel.sync_marker, &_exp_marker, sizeof(sync_marker_t));
    memcpy(buff, &pixel, sizeof(pixeldata_t));
    for(uint32_t i = 0; i < sizeof(pixeldata_t); ++i)
    {
        uart_putc(buff[i]);
    }
}


