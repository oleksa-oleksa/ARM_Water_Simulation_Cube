#include <particle.h>
#include <led32x32.h>
#include <utils.h>
#include <paint_tool.h>
#include <stdint.h>
#include <stdlib.h>




int main()
{
    double force[3] = {1, 9.8, 1};
    panel_t panels[CHAIN_LEN];
    particle_list_element_t particles[50];
    
    /* Initialize hardware */
    led32x32_init();

    /*Initialise simulation*/
    particle_init_list(particles, 50);
    particle_init_grid(panels[0], particles, 50);
    particle_init_grid(panels[1], NULL, 0);
    particle_init_grid(panels[2], NULL, 0);
    particle_init_grid(panels[3], NULL, 0);
    particle_init_grid(panels[4], NULL, 0);
    particle_init_grid(panels[5], NULL, 0);

    while(1)
    {
        delay(SEC_TO_MSEC(0.1));
        for(int i = 1; i < CHAIN_LEN; ++i)
        {
            particle_move_cube(/*top=*/panels[0], /*bottom=*/panels[1], /*front=*/panels[2], /*back=*/panels[3], /*left=*/panels[4], /*right=*/panels[5], 0.005, force);
        }
        lp32x32_refresh_chain(panels);
    }

    return 0;
}
