#include "particle.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.141592653589793238
#endif

#define REST_DENS 1000.0    ///< rest density
#define GAS_CONST 2000.0    ///< gas constant for euation of state
#define VISCOSITY 250.0     ///< viscosity

#define X 0
#define Y 1

void _compute_density_pressure(particle_grid_element_t grid[PARTICLE_GRID_X][PARTICLE_GRID_Y]);
inline double _kernel_smoothing_density(double distance);
void _compute_forces(particle_grid_element_t grid[PARTICLE_GRID_X][PARTICLE_GRID_Y], double ex_force[2]);
inline double _kernel_smoothing_pressure(double distance);
inline double _kernel_smoothing_viscosity(double distance);
void _integrate(particle_grid_element_t grid[PARTICLE_GRID_X][PARTICLE_GRID_Y]);


static inline void _switch_list_elements(particle_list_element_t* part_a, particle_list_element_t* part_b)
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
static inline void _move_list_element(particle_list_element_t* move, particle_list_element_t* position)
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


static inline void _update_grid_position(particle_grid_element_t grid[PARTICLE_GRID_X][PARTICLE_GRID_Y], int orig_grid_x, int orig_grid_y, particle_list_element_t* element)
{
    int new_grid_x, new_grid_y;
    uint32_t i;
    particle_list_element_t* curr;

    new_grid_x = element->particle.position[X] * PARTICLE_GRID_CELL_WIDTH;   //c always rounds down when converding double to int
    new_grid_y = element->particle.position[Y] * PARTICLE_GRID_CELL_HEIGHT;  //c always rounds down when converding double to int

    if((new_grid_x != orig_grid_x) ||
       (new_grid_y != orig_grid_y))
    {
        //remove element from old grid-cell
        for(i = 0; i < grid[orig_grid_x][orig_grid_y].particle_count; ++i)
        {
            if(&grid[orig_grid_x][orig_grid_y].particle_list[i] == element)
            { //found the element to remove
                if(element->prev != NULL)
                {
                    element->prev->next = element->next;
                }
                if(element->next != NULL)
                {
                    element->next->prev = element->prev;
                }
            }
            grid[orig_grid_x][orig_grid_y].particle_count -= 1;
        }
        //add element to new grid-cell
        if(grid[new_grid_x][new_grid_y].particle_count != 0)
        {
            curr = grid[new_grid_x][new_grid_y].particle_list;
            while(curr->next != NULL)
            {
                curr = curr->next;
            }
            curr->next = element;
            element->prev = curr;
            element->next = NULL;
        }
        else
        {
            grid[new_grid_x][new_grid_y].particle_list = element;
            element->prev = element->next = NULL;
        }
        grid[orig_grid_x][orig_grid_y].particle_count += 1;
    }
}





bool particle_init_list(particle_list_element_t* list, uint32_t size)
{
    uint32_t i;
    for(i = 0; i < size; ++i)
    {
        list[i].particle.position[0] = i % PARTICLE_GRID_X;
        list[i].particle.position[1] = i % PARTICLE_GRID_Y;
        list[i].particle.velocity[0] = 0;
        list[i].particle.velocity[1] = 0;
        list[i].particle.mass        = PARTICLE_MASS;
        list[i].particle.density     = 0;
        list[i].particle.pressure    = 0;
        list[i].particle.force[0]    = 0;
        list[i].particle.force[1]    = 0;
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


bool particle_init_grid(particle_grid_element_t grid[PARTICLE_GRID_X][PARTICLE_GRID_Y], particle_list_element_t* list, uint32_t size)
{
    uint32_t i, j;
    for(i = 0; i < PARTICLE_GRID_X; ++i)
    {
        for(j = 0; j < PARTICLE_GRID_Y; ++j)
        {
            grid[i][j].particle_list  = NULL;
            grid[i][j].particle_count = 0;
        }
    }

    for(i = 0; i < size; ++i)
    {
        if(grid[(int)list[i].particle.position[X]][(int)list[i].particle.position[Y]].particle_count == 0)
        {
            grid[(int)list[i].particle.position[X]][(int)list[i].particle.position[Y]].particle_count = 1;
            grid[(int)list[i].particle.position[X]][(int)list[i].particle.position[Y]].particle_list = &list[i];
        }
        else
        {
            _move_list_element(&list[i], 
                               &grid[(int)list[i].particle.position[X]][(int)list[i].particle.position[Y]].particle_list[ 
                                grid[(int)list[i].particle.position[X]][(int)list[i].particle.position[Y]].particle_count - 1]);
            grid[(int)list[i].particle.position[X]][(int)list[i].particle.position[Y]].particle_count += 1;
        }
    }
    return true;
}


bool particle_move(particle_grid_element_t grid[PARTICLE_GRID_X][PARTICLE_GRID_Y], uint32_t time, double ex_force[2])
{
    (void)grid;
    (void)time;
    (void)ex_force;
    return true;
}



void _compute_density_pressure(particle_grid_element_t grid[PARTICLE_GRID_X][PARTICLE_GRID_Y])
{
    int i_x, i_y, d_x, d_y;
    uint32_t i_part, j_part;
    particle_list_element_t* curr;

    for(i_x = 0; i_x < PARTICLE_GRID_X; ++i_x)
    {
        for(i_y = 0; i_y < PARTICLE_GRID_Y; ++i_y)
        {
            //first calculate the density at all particles 
            //current particle i, all other particles j, W is the kernel function, d the distance between i and the current j
            //rho_i = SUM_j m_j * W(d)
            for(i_part = 0; i_part < grid[i_x][i_y].particle_count; ++i_part)
            {
                curr = &grid[i_x][i_y].particle_list[i_part];
                curr->particle.pressure = 0;
                //now use the particles within the smoothing-distance (surounding grid-cells) to calculate the density at the current particle
                for(d_x = pow(PARTICLE_GRID_SMOOTH_DISTANCE, 2) * (-1); d_x <= pow(PARTICLE_GRID_SMOOTH_DISTANCE, 2); ++d_x)
                {
                    if( (i_x + d_x > 0) && 
                        (i_x + d_x < PARTICLE_GRID_X) )
                    {
                        for(d_y = pow(PARTICLE_GRID_SMOOTH_DISTANCE, 2) * (-1); d_y <= pow(PARTICLE_GRID_SMOOTH_DISTANCE, 2); ++d_y)
                        {
                            if( (i_y + d_y > 0) && 
                                (i_y + d_y < PARTICLE_GRID_Y) )
                            {
                                for(j_part = 0; j_part < grid[i_x+d_x][i_y+d_y].particle_count; ++j_part)
                                {
                                    //density is sum of: particlemass * kernel-smoothing
                                    double distance = sqrt( pow(curr->particle.position[X] - grid[i_x+d_x][i_y+d_y].particle_list[j_part].particle.position[X], 2) +  
                                                            pow(curr->particle.position[Y] - grid[i_x+d_x][i_y+d_y].particle_list[j_part].particle.position[Y], 2) );
                                    curr->particle.density += grid[i_x+d_x][i_y+d_y].particle_list[j_part].particle.mass * _kernel_smoothing_density(distance) * pow(pow(PARTICLE_GRID_SMOOTH_DISTANCE, 2) - pow(distance, 2), 3.0);
                                }
                            }
                        }
                    }
                    
                }
            }
            //now we can use the density at the current particle to calculate the pressure at the current particle
            // P = k_p * (rho - rho_0)
            curr->particle.pressure = GAS_CONST * (curr->particle.density - REST_DENS);
        }
    }
}


double _kernel_smoothing_density(double distance)
{
    //poly6 kernel funciton
    return 315.0 / (65.0 * M_PI * pow(distance, 9.0));
}


void _compute_forces(particle_grid_element_t grid[PARTICLE_GRID_X][PARTICLE_GRID_Y], double ex_force[2])
{
    int i_x, i_y, d_x, d_y;
    uint32_t i_part, j_part;
    particle_list_element_t* curr_i;
    particle_list_element_t* curr_j;
    double f_pressure[2];
    double f_viscosity[2];
    double f_gravity[2];

    for(i_x = 0; i_x < PARTICLE_GRID_X; ++i_x)
    {
        for(i_y = 0; i_y < PARTICLE_GRID_Y; ++i_y)
        {
            //get a particle and calculate the force applied to it depending on it's neighbours
            for(i_part = 0; i_part < grid[i_x][i_y].particle_count; ++i_part)
            {
                curr_i = &grid[i_x][i_y].particle_list[i_part];
                f_pressure[X] = 0;
                f_pressure[Y] = 0;
                f_viscosity[X] = 0;
                f_viscosity[Y] = 0;
                f_gravity[X] = ex_force[X] * curr_i->particle.density;
                f_gravity[Y] = ex_force[Y] * curr_i->particle.density;
                for(d_x = PARTICLE_GRID_SMOOTH_DISTANCE * (-1); d_x <= PARTICLE_GRID_SMOOTH_DISTANCE; ++d_x)
                {
                    if( (i_x + d_x > 0) && 
                        (i_x + d_x < PARTICLE_GRID_X) )
                    {
                        for(d_y = PARTICLE_GRID_SMOOTH_DISTANCE * (-1); d_y <= PARTICLE_GRID_SMOOTH_DISTANCE; ++d_y)
                        {
                            if( (i_y + d_y > 0) && 
                                (i_y + d_y < PARTICLE_GRID_Y) )
                            {
                                for(j_part = 0; j_part < grid[i_x+d_x][i_y+d_y].particle_count; ++j_part)
                                {
                                    curr_j = &grid[i_x+d_x][i_y+d_y].particle_list[j_part];
                                    //density is sum of: particlemass * kernel-smoothing
                                    
                                    double v_ij[2]; //vector between i and j of i and j
                                    v_ij[X] = curr_j->particle.position[X] - curr_i->particle.position[X];
                                    v_ij[Y] = curr_j->particle.position[Y] - curr_i->particle.position[Y];
                                    double distance = sqrt( pow(curr_i->particle.position[X] - grid[i_x+d_x][i_y+d_y].particle_list[j_part].particle.position[X], 2) +  
                                                            pow(curr_i->particle.position[Y] - grid[i_x+d_x][i_y+d_y].particle_list[j_part].particle.position[Y], 2) );
                                    double x_normalized = 0;
                                    double y_normalized = 0;
                                    if(distance > 0)
                                    {
                                        x_normalized = v_ij[X] / distance;
                                        y_normalized = v_ij[Y] / distance;
                                    }
                                    double multiplicator = curr_j->particle.mass * (curr_i->particle.pressure + curr_j->particle.pressure) / (2.0 * curr_j->particle.density) * _kernel_smoothing_pressure(distance);
                                    f_pressure[X]  += x_normalized * multiplicator;
                                    f_pressure[Y]  += y_normalized * multiplicator;
                                    f_viscosity[X] += VISCOSITY * curr_j->particle.mass * (curr_j->particle.velocity[X] - curr_i->particle.velocity[X]) / curr_j->particle.density * _kernel_smoothing_viscosity(distance);
                                }
                            }
                        }
                    }
                }
                curr_i->particle.force[X] = f_pressure[X] + f_viscosity[X] + f_gravity[X];
                curr_i->particle.force[Y] = f_pressure[Y] + f_viscosity[Y] + f_gravity[Y];
            }
        }
    }
}


double _kernel_smoothing_pressure(double distance)
{
    //spiky gradient kernel (constant scalar)
    return (-45.0) / (M_PI * pow(PARTICLE_GRID_SMOOTH_DISTANCE, 6.0))  * pow(PARTICLE_GRID_SMOOTH_DISTANCE - distance, 2.0);
}


double _kernel_smoothing_viscosity(double distance)
{
    //viscosity kernel (laplacian scalar)
    return (45) / (M_PI * pow(PARTICLE_GRID_SMOOTH_DISTANCE, 6.0)) * (PARTICLE_GRID_SMOOTH_DISTANCE - distance);
}


void _integrate(particle_grid_element_t grid[PARTICLE_GRID_X][PARTICLE_GRID_Y])
{
    (void)grid;
}

