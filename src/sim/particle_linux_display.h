#ifndef PARTICLE_LINUX_DISPLAY_H_
#define PARTICLE_LINUX_DISPLAY_H_

#include <particle.h>
#include <X11/Xlib.h>

typedef struct
{
    Display* display;   ///< pointer to X Display structure.
    Window win;         ///< pointer to the newly created window.
    GC gc;              ///< GC (graphics context) used for drawing
} x11_win_struct;

void particle_linux_display_init(x11_win_struct* handle, char* window_name);

void particle_linux_display_draw_pixels(x11_win_struct* handle, particle_grid_element_t grid[PARTICLE_GRID_X][PARTICLE_GRID_Y]);

void particle_linux_display_close(x11_win_struct* handle);



#endif
