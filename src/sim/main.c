#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>



#include <particle.h>
#include <particle_linux_display.h>


int main(int argc, char const *argv[])
{
    // int i;
    (void) argc;
    (void) argv;

    particle_list_element_t particles[20];
    particle_grid_element_t grid[PARTICLE_GRID_X][PARTICLE_GRID_Y];
    particle_init_list(particles, 20);
    particle_init_grid(grid, particles, 20);

    particle_linux_display_init();
    particle_linux_display_draw_pixels(grid);
    

    while(1)
    {
        ;
    }

    particle_linux_display_close();


    return 0;
}