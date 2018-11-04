#ifndef PARTICLE_H_
#define PARTICLE_H_

#include <stdint.h>
#include <stdbool.h>

#define PARTICLE_MASS       50
#define PARTICLE_DENSITY    12
#define PARTICLE_PREASURE   10

#define PARTICLE_GRID_X     32
#define PARTICLE_GRID_Y     32


typedef struct
{
    double position[2];
    double velocity[2];
    double mass;
    double density;
    double preasure;
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

bool particle_move(particle_grid_element_t grid[PARTICLE_GRID_X][PARTICLE_GRID_Y], uint32_t time, double ex_force[2]);






#endif
