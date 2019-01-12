#ifndef PARTICLE_H_
#define PARTICLE_H_

#include <stdint.h>
#include <stdbool.h>

#define PARTICLE_GRID_X     32
#define PARTICLE_GRID_Y     32

#define PARTICLE_GRID_CELL_WIDTH  1
#define PARTICLE_GRID_CELL_HEIGHT 1



typedef struct
{
    double position[2];
    double velocity[2];
    double mass;
    double density;
    double pressure;
    double force[2];
    //color
} particle_t;


typedef struct particle_list_element_s
{
    particle_t particle;
    struct particle_list_element_s* prev;
    struct particle_list_element_s* next;
} particle_list_element_t;


typedef struct
{
    particle_list_element_t* particle_list;
    uint32_t particle_count;
} particle_grid_element_t;




bool particle_init_list(particle_list_element_t* list, uint32_t size);

bool particle_init_grid(particle_grid_element_t grid[PARTICLE_GRID_X][PARTICLE_GRID_Y], particle_list_element_t* list, uint32_t size);

bool particle_move_single_panel(particle_grid_element_t grid[PARTICLE_GRID_X][PARTICLE_GRID_Y], double d_time, double ex_force[2]);

bool particle_move_cube(particle_grid_element_t top[PARTICLE_GRID_X][PARTICLE_GRID_Y], 
                        particle_grid_element_t bottom[PARTICLE_GRID_X][PARTICLE_GRID_Y], 
                        particle_grid_element_t front[PARTICLE_GRID_X][PARTICLE_GRID_Y], 
                        particle_grid_element_t back[PARTICLE_GRID_X][PARTICLE_GRID_Y], 
                        particle_grid_element_t left[PARTICLE_GRID_X][PARTICLE_GRID_Y], 
                        particle_grid_element_t right[PARTICLE_GRID_X][PARTICLE_GRID_Y], 
                        double d_time, double ex_force[2]);





#endif
