#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SEC_TO_USEC(sec) (sec * 1000000)
#define DT 0.005

#include <particle.h>
#include <particle_linux_display.h>


void _cube()
{
    double force[3] = {1, 9.8, 1};
    particle_list_element_t particles[50];

    particle_grid_element_t grid_top[PARTICLE_GRID_X][PARTICLE_GRID_Y];
    particle_grid_element_t grid_bottom[PARTICLE_GRID_X][PARTICLE_GRID_Y];
    particle_grid_element_t grid_front[PARTICLE_GRID_X][PARTICLE_GRID_Y];
    particle_grid_element_t grid_back[PARTICLE_GRID_X][PARTICLE_GRID_Y];
    particle_grid_element_t grid_left[PARTICLE_GRID_X][PARTICLE_GRID_Y];
    particle_grid_element_t grid_right[PARTICLE_GRID_X][PARTICLE_GRID_Y];
    x11_win_struct x11_top;
    x11_win_struct x11_bottom;
    x11_win_struct x11_front;
    x11_win_struct x11_back;
    x11_win_struct x11_left;
    x11_win_struct x11_right;

    
    particle_init_list(particles, 50);
    particle_init_grid(grid_top, NULL, 0);
    particle_init_grid(grid_bottom, NULL, 0);
    particle_init_grid(grid_front, particles, 50);
    particle_init_grid(grid_back, NULL, 0);
    particle_init_grid(grid_left, NULL, 0);
    particle_init_grid(grid_right, NULL, 0);

    particle_linux_display_init(&x11_top, "top");
    particle_linux_display_draw_pixels(&x11_top, grid_top);
    particle_linux_display_init(&x11_bottom, "bottom");
    particle_linux_display_draw_pixels(&x11_bottom, grid_bottom);
    particle_linux_display_init(&x11_front, "front");
    particle_linux_display_draw_pixels(&x11_front, grid_front);
    particle_linux_display_init(&x11_back, "back");
    particle_linux_display_draw_pixels(&x11_back, grid_back);
    particle_linux_display_init(&x11_left, "left");
    particle_linux_display_draw_pixels(&x11_left, grid_left);
    particle_linux_display_init(&x11_right, "right");
    particle_linux_display_draw_pixels(&x11_right, grid_right);
    

    while(1)
    {
        // usleep(SEC_TO_USEC(0.1));
        // particle_move_single_panel(grid_front, DT, force);
        particle_move_cube(grid_top, grid_bottom, grid_front, grid_back, grid_left, grid_right, DT, force);
        particle_linux_display_draw_pixels(&x11_top, grid_top);
        particle_linux_display_draw_pixels(&x11_bottom, grid_bottom);
        particle_linux_display_draw_pixels(&x11_front, grid_front);
        particle_linux_display_draw_pixels(&x11_back, grid_back);
        particle_linux_display_draw_pixels(&x11_left, grid_left);
        particle_linux_display_draw_pixels(&x11_right, grid_right);
        usleep(SEC_TO_USEC(0.015)); //X11 needs some tome to display everything
    }

    particle_linux_display_close(&x11_top);
    particle_linux_display_close(&x11_bottom);
    particle_linux_display_close(&x11_front);
    particle_linux_display_close(&x11_back);
    particle_linux_display_close(&x11_left);
    particle_linux_display_close(&x11_right);
}

void _single_panel()
{
    double force[3] = {1, 9.8, 1};
    particle_list_element_t particles[250];
    particle_grid_element_t grid_top[PARTICLE_GRID_X][PARTICLE_GRID_Y];
    x11_win_struct x11_top;

    
    particle_init_list(particles, 250);
    particle_init_grid(grid_top, particles, 250);

    particle_linux_display_init(&x11_top, "top");
    particle_linux_display_draw_pixels(&x11_top, grid_top);
    

    while(1)
    {
        // usleep(SEC_TO_USEC(0.1));
        // particle_move_single_panel(grid_front, DT, force);
        particle_move_single_panel(grid_top, DT, force);
        particle_linux_display_draw_pixels(&x11_top, grid_top);
        usleep(SEC_TO_USEC(0.015)); //X11 needs some tome to display everything
    }

    particle_linux_display_close(&x11_top);
}

int main(int argc, char const *argv[])
{
    (void) argc;
    (void) argv;

    if(argc > 1)
    {
        _cube();
    }
    else
    {
        _single_panel();
    }

    


    return 0;
}