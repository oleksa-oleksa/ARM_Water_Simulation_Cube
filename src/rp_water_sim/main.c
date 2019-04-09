#include <gpio.h>
#include <util.h>
#include <uart.h>
#include <ll_time.h>
#include <protocol.h>
#include <particle.h>

#define LED_BLINK_IVAL (500 * 1000)
#define DT_SIM 0.05

static panel_t panels[6];
// static particle_list_element_t particles[50];

void _particle_change_callback(enum side_e side, int x, int y)
{
    pixeldata_t pixel;
    pixel.panel_no = side;
    pixel.x_pos = x;
    pixel.y_pos = y;
    pixel.color = panels[side][x][y].particle_count;
    protocol_send_pixel(pixel);
}


void _echo_back(void)
{
    led_set(false);
    while(uart0_get_rx_buf_size() != 0)
    {
        uart0_putc(*uart0_get_rx_buf());
        uart0_remove_n_rx_buf(1);
    }
}


int main(void)
{
    // double force[3] = {0, 0, 0};
    
    led_setup();
    // protocol_init();

    /*Initialise simulation*/
    // particle_init_list(particles, 50);
    // particle_init_grid(panels[side_top], particles, 50);
    // particle_init_grid(panels[side_bottom], NULL, 0);
    // particle_init_grid(panels[side_front], NULL, 0);
    // particle_init_grid(panels[side_back], NULL, 0);
    // particle_init_grid(panels[side_left], NULL, 0);
    // particle_init_grid(panels[side_right], NULL, 0);
    // particle_add_grid_element_change_callback(_particle_change_callback);

    /*Initialise display*/
    // protocol_send_panel(panels[side_top], side_top);
    // protocol_send_panel(panels[side_bottom], side_bottom);
    // protocol_send_panel(panels[side_front], side_front);
    // protocol_send_panel(panels[side_back], side_back);
    // protocol_send_panel(panels[side_left], side_left);
    // protocol_send_panel(panels[side_right], side_right);
    
    uart0_init(_echo_back);

        led_set(true);
    while(1)
    {
        time_usec_wait(MSEC2USEC(250));
        // force[0] = protocol_get_last_acc_data()->force_x;
        // force[1] = protocol_get_last_acc_data()->force_y;
        // force[2] = protocol_get_last_acc_data()->force_z;

        // particle_move_cube(/*top=*/panels[0], /*bottom=*/panels[1], /*front=*/panels[2], /*back=*/panels[3], /*left=*/panels[4], /*right=*/panels[5], DT_SIM, force);
        time_usec_wait(MSEC2USEC(250));
    }
}