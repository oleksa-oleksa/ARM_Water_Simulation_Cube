#ifndef PARTICLE_LINUX_DISPLAY_H_
#define PARTICLE_LINUX_DISPLAY_H_

#include <particle.h>

void particle_linux_display_init(void);

void particle_linux_display_draw_pixels(particle_grid_element_t grid[PARTICLE_GRID_X][PARTICLE_GRID_Y]);

void particle_linux_display_close(void);



#endif
