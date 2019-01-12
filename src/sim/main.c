#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SEC_TO_USEC(sec) (sec * 1000000)

#include <particle.h>
#include <particle_linux_display.h>


int main(int argc, char const *argv[])
{
    // int i;
    (void) argc;
    (void) argv;

    particle_list_element_t particles[50];
    particle_grid_element_t grid[PARTICLE_GRID_X][PARTICLE_GRID_Y];
    particle_init_list(particles, 50);
    particle_init_grid(grid, particles, 50);

    particle_linux_display_init();
    particle_linux_display_draw_pixels(grid);
    
    double force[2] = {0, 9.8};

    while(1)
    {
        // usleep(SEC_TO_USEC(0.1));
        particle_move_single_panel(grid, 0.005, force);
        particle_linux_display_draw_pixels(grid);
    }

    particle_linux_display_close();


    return 0;
}