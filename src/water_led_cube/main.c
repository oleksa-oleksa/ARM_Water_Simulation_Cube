#include <particle.h>
#include <led32x32.h>
#include <utils.h>
#include <paint_tool.h>

static panel_t panels[CHAIN_LEN];
static particle_list_element_t particles[50];


int main(int argc, char const *argv[])
{
    (void) argc;
    (void) argv;

    particle_init_list(particles, 50);
    particle_init_grid(panels[0], particles, 50);


    double force[2] = {0, 9.8};

    while(1)
    {
        delay(SEC_TO_MSEC(0.1));
        for(int i = 1; i < CHAIN_LEN; ++i)
        {
            particle_move_single_panel(panels[0], 0.005, force);
        }
        lp32x32_refresh_chain(panels);
    }

    return 0;
}
