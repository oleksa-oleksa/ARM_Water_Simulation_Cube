#include <gpio.h>
#include <util.h>
#include <uart.h>
#include <ll_time.h>
#include <protocol.h>
#include <particle.h>

#define LED_BLINK_IVAL (500 * 1000)
#define DT_SIM 0.05

static panel_t panels[6];
static particle_list_element_t particles[50];

void _particle_change_callback(enum side_e side, int x, int y)
{
    pixeldata_t pixel;
    pixel.panel_no = side;
    pixel.x_pos = x;
    pixel.y_pos = y;
    pixel.color = panels[side][x][y].particle_count;
    protocol_send_pixel(pixel);
}



int main(void)
{
    double force[3] = {1, 9.8, 1};
    led_setup();
    protocol_init();

    /*Initialise simulation*/
    particle_init_list(particles, 50);
    particle_init_grid(panels[0], particles, 50);
    particle_init_grid(panels[1], NULL, 0);
    particle_init_grid(panels[2], NULL, 0);
    particle_init_grid(panels[3], NULL, 0);
    particle_init_grid(panels[4], NULL, 0);
    particle_init_grid(panels[5], NULL, 0);
    particle_add_grid_element_change_callback(_particle_change_callback);

    while(1)
    {
        // for(tim = 0; tim < 500000; tim++)
        // {
        //     foo(tim);
        // }   
        led_set(true);

        particle_move_cube(/*top=*/panels[0], /*bottom=*/panels[1], /*front=*/panels[2], /*back=*/panels[3], /*left=*/panels[4], /*right=*/panels[5], DT_SIM, force);

        led_set(false);
    }
}