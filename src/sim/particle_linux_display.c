#include "particle_linux_display.h"

#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>

#define DISPLAY_GRID_WIDTH 21
#define DISPLAY_GRID_BORDER_OFFSET 10
#define DISPLAY_PARTICLE_SIZE 4
#define X 0
#define Y 1
#define SEC_TO_USEC(sec) (sec * 1000000)


Display* display;   ///< pointer to X Display structure.
Window win;         ///< pointer to the newly created window.
GC gc;              ///< GC (graphics context) used for drawing


/*
 * function: create_simple_window. Creates a window with a white background
 *           in the given size.
 * input:    display, size of the window (in pixels), and location of the window
 *           (in pixels).
 * output:   the window's ID.
 * notes:    window is created with a black border, 2 pixels wide.
 *           the window is automatically mapped after its creation.
 */
Window _create_simple_window(Display* display, int width, int height, int x, int y)
{
    int screen_num = DefaultScreen(display);
    int win_border_width = 2;
    Window win;

    /* create a simple window, as a direct child of the screen's */
    /* root window. Use the screen's black and white colors as   */
    /* the foreground and background colors of the window,       */
    /* respectively. Place the new window's top-left corner at   */
    /* the given 'x,y' coordinates.                              */
    win = XCreateSimpleWindow(display, RootWindow(display, screen_num),
                            x, y, width, height, win_border_width,
                            BlackPixel(display, screen_num),
                            WhitePixel(display, screen_num));

    /* make the window actually appear on the screen. */
    XMapWindow(display, win);

    /* flush all pending requests to the X server. */
    XFlush(display);

    return win;
}

GC _create_gc(Display* display, Window win, int reverse_video)
{
    GC gc;                /* handle of newly created GC.  */
    unsigned long valuemask = 0;      /* which values in 'values' to  */
                    /* check when creating the GC.  */
    XGCValues values;         /* initial values for the GC.   */
    unsigned int line_width = 2;      /* line width for the GC.       */
    int line_style = LineSolid;       /* style for lines drawing and  */
    int cap_style = CapButt;      /* style of the line's edje and */
    int join_style = JoinBevel;       /*  joined lines.       */
    int screen_num = DefaultScreen(display);

    gc = XCreateGC(display, win, valuemask, &values);
    if (gc == NULL) 
    {
        fprintf(stderr, "XCreateGC: \n");
    }

    /* allocate foreground and background colors for this GC. */
    if (reverse_video) 
    {
        XSetForeground(display, gc, WhitePixel(display, screen_num));
        XSetBackground(display, gc, BlackPixel(display, screen_num));
    }
    else 
    {
        XSetForeground(display, gc, BlackPixel(display, screen_num));
        XSetBackground(display, gc, WhitePixel(display, screen_num));
    }

    /* define the style of lines that will be drawn using this GC. */
    XSetLineAttributes(display, gc,
                     line_width, line_style, cap_style, join_style);

    /* define the fill style for the GC. to be 'solid filling'. */
    XSetFillStyle(display, gc, FillSolid);

    return gc;
}



void particle_linux_display_init(void)
{
    unsigned int width, height;   /* height and width for the new window.      */
    char *display_name = getenv("DISPLAY");  /* address of the X display.      */
                /*  in our window.               */

    /* open connection with the X server. */
    display = XOpenDisplay(display_name);
    if (display == NULL) {
        fprintf(stderr, "cannot connect to X server '%s'\n", display_name);
        return;
    }

    /* get the geometry of the default screen for our display. */
    // screen_num = DefaultScreen(display);
    // display_width = DisplayWidth(display, screen_num);
    // display_height = DisplayHeight(display, screen_num);

    /* make the new window occupy 1/9 of the screen's size. */
    // width = (display_width / 3);
    // height = (display_height / 3);
    width  = (PARTICLE_GRID_X * DISPLAY_GRID_WIDTH) + (2 * DISPLAY_GRID_BORDER_OFFSET);
    height = (PARTICLE_GRID_Y * DISPLAY_GRID_WIDTH) + (2 * DISPLAY_GRID_BORDER_OFFSET);
    printf("window width - '%d'; height - '%d'\n", width, height);

    /* create a simple window, as a direct child of the screen's   */
    /* root window. Use the screen's white color as the background */
    /* color of the window. Place the new window's top-left corner */
    /* at the given 'x,y' coordinates.                             */
    win = _create_simple_window(display, width, height, 0, 0);

    /* allocate a new GC (graphics context) for drawing in the window. */
    gc = _create_gc(display, win, 0);
    XSync(display, False);

    XFlush(display);
    // usleep(SEC_TO_USEC(0.05));  //20ms needed to let X11 initialise everyting
    sleep(1);
}

void particle_linux_display_draw_pixels(particle_grid_element_t grid[PARTICLE_GRID_X][PARTICLE_GRID_Y])
{
    unsigned int i, j, k;
    XColor grey;
    Status rc;            /* return status of various Xlib functions.  */
    Colormap screen_colormap;     /* color map to use for allocating colors.   */
    XClearWindow(display, win);
    screen_colormap = DefaultColormap(display, DefaultScreen(display));
    rc = XAllocNamedColor(display, screen_colormap, "grey", &grey, &grey);
    XSetForeground(display, gc, grey.pixel);
    XFlush(display);
    for(i = 0; i <= PARTICLE_GRID_X; ++i)
    {
        XDrawLine(display, win, gc, 
                    /*x1*/DISPLAY_GRID_BORDER_OFFSET + (DISPLAY_GRID_WIDTH * i), /*y1*/DISPLAY_GRID_BORDER_OFFSET, 
                    /*x1*/DISPLAY_GRID_BORDER_OFFSET + (DISPLAY_GRID_WIDTH * i), /*y1*/DISPLAY_GRID_BORDER_OFFSET + (DISPLAY_GRID_WIDTH * PARTICLE_GRID_Y));
    }
    for(i = 0; i <= PARTICLE_GRID_Y; ++i)
    {
        XDrawLine(display, win, gc, 
                    /*x1*/DISPLAY_GRID_BORDER_OFFSET, /*y1*/DISPLAY_GRID_BORDER_OFFSET + (DISPLAY_GRID_WIDTH * i), 
                    /*x1*/DISPLAY_GRID_BORDER_OFFSET + (DISPLAY_GRID_WIDTH * PARTICLE_GRID_Y), /*y1*/DISPLAY_GRID_BORDER_OFFSET + (DISPLAY_GRID_WIDTH * i));
    }
    XColor blue;
    screen_colormap = DefaultColormap(display, DefaultScreen(display));
    rc = XAllocNamedColor(display, screen_colormap, "blue", &blue, &blue);
    if (rc == 0) {
        fprintf(stderr, "XAllocNamedColor - failed to allocated 'blue' color.\n");
        exit(1);
    }
    XSetForeground(display, gc, blue.pixel);
    XFlush(display);

    // printf("start printing of particles\r\n");
    for(i = 0; i < PARTICLE_GRID_X; ++i)
    {
        for(j = 0; j < PARTICLE_GRID_Y; ++j)
        {
            if(grid[i][j].particle_count > 0)
            {
                particle_list_element_t* curr = grid[i][j].particle_list;
                for(k = 0; k < grid[i][j].particle_count; ++k)
                {
                    int posx =  DISPLAY_GRID_BORDER_OFFSET + //(i * DISPLAY_GRID_WIDTH) + 
                                ((curr->particle.position[X] / PARTICLE_GRID_CELL_WIDTH)  * DISPLAY_GRID_WIDTH);
                    // printf("[%d][%d] %d %lf \r\n", i, j, posx, curr->particle.position[X]);
                    int posy =  DISPLAY_GRID_BORDER_OFFSET + //(j * DISPLAY_GRID_WIDTH) + 
                                ((curr->particle.position[Y] / PARTICLE_GRID_CELL_HEIGHT) * DISPLAY_GRID_WIDTH);
                    XFillRectangle(display, win, gc, posx, posy, DISPLAY_PARTICLE_SIZE, DISPLAY_PARTICLE_SIZE);
                    curr = curr->next;
                }
            }
        }
    }
    // flush all pending requests to the X server.
    XFlush(display);
    usleep(SEC_TO_USEC(0.015)); //X11 needs some tome to display everything
    // sleep(1);
}


void particle_linux_display_close(void)
{
    XCloseDisplay(display);
}