#include "particle.h"

#include <stdlib.h>

#define X 0
#define Y 1

inline void _switch_list_elements(particle_list_element_t* part_a, particle_list_element_t* part_b)
{
    particle_list_element_t hand = *part_a;
    *part_a = *part_b;
    *part_b = hand;
}


/**
 * @brief                       Move an element of the list to a new position.
 * @details                     Inserts the 'move'-element after the 'position'-element.
 * @param[inout]    move        Element to move.
 * @param[inout]    position    Element after which the move-element should be inserted
 */
inline void _move_list_element(particle_list_element_t* move, particle_list_element_t* position)
{
    if(move->next != NULL)
    {
        move->next->prev = move->prev;
    }
    if(move->prev != NULL)
    {
        move->prev->next = move->next;
    }
    move->next = position->next;
    move->prev = position;
    if(position->next != NULL)
    {
        position->next->prev = move;
    }
    position->next = move;
}




bool init_particle_list(particle_list_element_t* list, particle_t* particle_array, uint32_t size)
{
    uint32_t i;
    for(i = 0; i < size; ++i)
    {
        particle_array[i].position[0] = i % PARTICLE_GRID_X;
        particle_array[i].position[1] = i % PARTICLE_GRID_Y;
        particle_array[i].velocity[0] = 0;
        particle_array[i].velocity[1] = 0;
        particle_array[i].mass        = PARTICLE_MASS;
        particle_array[i].density     = PARTICLE_DENSITY;
        particle_array[i].preasure    = PARTICLE_PREASURE;
        particle_array[i].force[0]    = 0;
        particle_array[i].force[1]    = 0;
        list[i].particle = &particle_array[i];
        if(i > 0)
        {
            list[i].prev = NULL;
            list[i].next = &list[i+1];
        }
        else if(i == size - 1)
        {
            list[i].prev = &list[i-1];
            list[i].next = NULL;
        }
        else
        {
            list[i].prev = &list[i-1];
            list[i].next = &list[i+1];
        }
    }
    return true;
}


bool init_particle_grid(particle_grid_element_t grid[PARTICLE_GRID_X][PARTICLE_GRID_Y], particle_list_element_t* list, uint32_t size)
{
    uint32_t i, j;
    for(i = 0; i < PARTICLE_GRID_X; ++i)
    {
        for(j = 0; j < PARTICLE_GRID_Y; ++i)
        {
            grid[i][j].particle_list  = NULL;
            grid[i][j].particle_count = 0;
        }
    }

    for(i = 0; i < size; ++i)
    {
        if(grid[(int)list[i].particle->position[X]][(int)list[i].particle->position[Y]].particle_count == 0)
        {
            grid[(int)list[i].particle->position[X]][(int)list[i].particle->position[Y]].particle_count = 1;
            grid[(int)list[i].particle->position[X]][(int)list[i].particle->position[Y]].particle_list = &list[i];
        }
        else
        {
            _move_list_element(&list[i], 
                               &grid[(int)list[i].particle->position[X]][(int)list[i].particle->position[Y]].particle_list[ 
                                grid[(int)list[i].particle->position[X]][(int)list[i].particle->position[Y]].particle_count - 1]);
            grid[(int)list[i].particle->position[X]][(int)list[i].particle->position[Y]].particle_count += 1;
        }
    }
    return true;
}



