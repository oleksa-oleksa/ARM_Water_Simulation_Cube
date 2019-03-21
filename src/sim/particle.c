/*
 * website: 
 */
#include "particle.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#ifndef M_PI
#define M_PI 3.141592653589793238
#endif

#if (PARTICLE_DEBUG == 0)
    #define DEBUG(...) fprintf(stdout, __VA_ARGS__)
#else
    #define DEBUG(...) {;}
#endif

#if (PARTICLE_DEBUG == 0)
    #define ERROR(...) fprintf(stderr, __VA_ARGS__)
#else
    #define ERROR(...) {;}
#endif

#define PARTICLE_MASS       6.5    ///< 
#define REST_DENS           1.0    ///< rest density
#define GAS_CONST           2.0    ///< gas constant for equation of state
#define VISCOSITY            2.5    ///< viscosity
#define BOUNDARY_DAMPENING  -0.5    ///< 
#define KERNEL_RADIUS 1
#define KERNEL_RADIUS_SQ (KERNEL_RADIUS * KERNEL_RADIUS)
#define UPPER_BOUNDS_DELTA 0.001    ///< Stop the position from becomming PARTICLE_GRID_X or PARTICLE_GRID_Y
#define MAX_X_POS ((PARTICLE_GRID_X * PARTICLE_GRID_CELL_WIDTH ) - UPPER_BOUNDS_DELTA)
#define MAX_Y_POS ((PARTICLE_GRID_Y * PARTICLE_GRID_CELL_HEIGHT) - UPPER_BOUNDS_DELTA)

#define X 0
#define Y 1
#define Z 2

void _compute_density_pressure(particle_grid_element_t grid[PARTICLE_GRID_X][PARTICLE_GRID_Y]);
inline double _kernel_smoothing_density(double distance);
void _compute_forces(particle_grid_element_t grid[PARTICLE_GRID_X][PARTICLE_GRID_Y], double ex_force[2]);
inline double _kernel_smoothing_pressure(double distance);
inline double _kernel_smoothing_viscosity(double distance);
void _integrate(particle_grid_element_t grid[PARTICLE_GRID_X][PARTICLE_GRID_Y], double d_time);
void _integrate_cube(particle_grid_element_t grid[PARTICLE_GRID_X][PARTICLE_GRID_Y], double d_time);
static inline void _update_grid_position(particle_grid_element_t grid[PARTICLE_GRID_X][PARTICLE_GRID_Y]);
static inline void _update_grid_position_particle(particle_grid_element_t grid[PARTICLE_GRID_X][PARTICLE_GRID_Y], int orig_grid_x, int orig_grid_y, particle_list_element_t* element);
static inline bool _remove_particle(particle_grid_element_t* cell, particle_list_element_t* element);
static inline void _add_particle(particle_grid_element_t* cell, particle_list_element_t* element);
static inline void _manage_rolloff(particle_grid_element_t top[PARTICLE_GRID_X][PARTICLE_GRID_Y], 
                                   particle_grid_element_t bottom[PARTICLE_GRID_X][PARTICLE_GRID_Y], 
                                   particle_grid_element_t front[PARTICLE_GRID_X][PARTICLE_GRID_Y], 
                                   particle_grid_element_t back[PARTICLE_GRID_X][PARTICLE_GRID_Y], 
                                   particle_grid_element_t left[PARTICLE_GRID_X][PARTICLE_GRID_Y], 
                                   particle_grid_element_t right[PARTICLE_GRID_X][PARTICLE_GRID_Y]);\






bool particle_init_list(particle_list_element_t* list, uint32_t size)
{
    uint32_t i;
    for(i = 0; i < size; ++i)
    {

        list[i].particle.position[X] = (2 + (i % (PARTICLE_GRID_X/2))) * PARTICLE_GRID_CELL_WIDTH;
        list[i].particle.position[Y] = (2 + (i / (PARTICLE_GRID_Y/2))) * PARTICLE_GRID_CELL_HEIGHT;
        list[i].particle.velocity[0] = 0;
        list[i].particle.velocity[1] = 0;
        list[i].particle.mass        = PARTICLE_MASS;
        list[i].particle.density     = 0;
        list[i].particle.pressure    = 0;
        list[i].particle.force[0]    = 0;
        list[i].particle.force[1]    = 0;
        list[i].next = list[i].prev = NULL;
    }
    return true;
}


bool particle_init_grid(particle_grid_element_t grid[PARTICLE_GRID_X][PARTICLE_GRID_Y], particle_list_element_t* list, uint32_t size)
{
    int particle_x, particle_y;
    uint32_t i, j;
    particle_list_element_t* curr;

    if(grid == NULL)
    {
        ERROR("grid == NULL\r\n");
        return false;
    }

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
        if(list == NULL)
        {
            ERROR("list == NULL\r\n");
            return false;
        }
        particle_x = (int)list[i].particle.position[X] / PARTICLE_GRID_CELL_WIDTH;
        particle_y = (int)list[i].particle.position[Y] / PARTICLE_GRID_CELL_HEIGHT;
        if(grid[particle_x][particle_y].particle_count == 0)
        {
            grid[particle_x][particle_y].particle_count = 1;
            list[i].prev = NULL;
            list[i].next = NULL;
            grid[particle_x][particle_y].particle_list = &list[i];
        }
        else
        {
            curr = grid[particle_x][particle_y].particle_list;
            for(j = 1; j < grid[particle_x][particle_y].particle_count; ++j)
            {
                curr = curr->next;
            }
            curr->next = &list[i];
            list[i].prev = curr;
            list[i].next = NULL;
            grid[particle_x][particle_y].particle_count += 1;
        }
    }
    return true;
}


bool particle_move_single_panel(particle_grid_element_t grid[PARTICLE_GRID_X][PARTICLE_GRID_Y], double d_time, double ex_force[2])
{
    _compute_density_pressure(grid);
    _compute_forces(grid, ex_force);
    _integrate(grid, d_time);
    _update_grid_position(grid);
    
    return true;
}


bool particle_move_cube(particle_grid_element_t top[PARTICLE_GRID_X][PARTICLE_GRID_Y], 
                        particle_grid_element_t bottom[PARTICLE_GRID_X][PARTICLE_GRID_Y], 
                        particle_grid_element_t front[PARTICLE_GRID_X][PARTICLE_GRID_Y], 
                        particle_grid_element_t back[PARTICLE_GRID_X][PARTICLE_GRID_Y], 
                        particle_grid_element_t left[PARTICLE_GRID_X][PARTICLE_GRID_Y], 
                        particle_grid_element_t right[PARTICLE_GRID_X][PARTICLE_GRID_Y], 
                        double d_time, double ex_force[3])
{
    static uint8_t no_called = 0;
    static double rel_force[2] = {0};

    ++no_called;
    switch(no_called)
    {
        //forces: global x, y
        case  1:
        {
            rel_force[X] = ex_force[X];
            rel_force[Y] = ex_force[Y];
            _compute_density_pressure(top);
            break;
        }
        case  2:
        {
            _compute_forces(top, rel_force);
            break;
        }

        //forces: x = -x, y = -y
        case  3:
        {
            rel_force[X] = -1 * ex_force[X];
            rel_force[Y] = -1 * ex_force[Y];
            _compute_density_pressure(bottom);
            break;
        }
        case  4:
        {
            _compute_forces(bottom, rel_force);
            break;
        }

        //forces: x = x, y = z
        case  5:
        {
            rel_force[X] = ex_force[X];
            rel_force[Y] = ex_force[Z];
            _compute_density_pressure(front);
            break;
        }
        case  6:
        {
            _compute_forces(front, rel_force);
            break;
        }

        //forces: x = -x, y = z
        case  7:
        {
            rel_force[X] = -1 * ex_force[X];
            rel_force[Y] = ex_force[Z];
            _compute_density_pressure(back);
            break;
        }
        case  8:
        {
            _compute_forces(back, rel_force);
            break;
        }

        //forces: x = -y, y = z
        case  9:
        {
            rel_force[X] = -1 * ex_force[Y];
            rel_force[Y] = ex_force[Z];
            _compute_density_pressure(left);
            break;
        }
        case 10:
        {
            _compute_forces(left, rel_force);
            break;
        }

        //forces: x = y, y = z
        case 11:
        {
            rel_force[X] = ex_force[Y];
            rel_force[Y] = ex_force[Z];
            _compute_density_pressure(right);
            break;
        }
        case 12:
        {
            _compute_forces(right, rel_force);
            break;
        }

        case 13:
        {
            _integrate_cube(top, d_time);
            break;
        }
        case 14:
        {
            _integrate_cube(bottom, d_time);
            break;
        }
        case 15:
        {
            _integrate_cube(front, d_time);
            break;
        }
        case 16:
        {
            _integrate_cube(back, d_time);
            break;
        }
        case 17:
        {
            _integrate_cube(left, d_time);
            break;
        }
        case 18:
        {
            _integrate_cube(right, d_time);
            break;
        }

        case 19:
        {
            _manage_rolloff(top, bottom, front, back, left, right);
            break;
        }

        case 20:
        {
            _update_grid_position(top);
            break;
        }
        case 21:
        {
            _update_grid_position(bottom);
            break;
        }
        case 22:
        {
            _update_grid_position(front);
            break;
        }
        case 23:
        {
            _update_grid_position(back);
            break;
        }
        case 24:
        {
            _update_grid_position(left);
            break;
        }
        case 25:
        {
            _update_grid_position(right);
            break;
        }
        default:
        {
            no_called = 0;
        }
    }
    
    return true;
}




void _compute_density_pressure(particle_grid_element_t grid[PARTICLE_GRID_X][PARTICLE_GRID_Y])
{
    int i_x, i_y, d_x, d_y;
    uint32_t i_part, j_part;
    particle_list_element_t* curr;
    particle_list_element_t* partner;
    if(grid == NULL)
    {
        ERROR("grid == NULL\r\n");
        return;
    }

    for(i_x = 0; i_x < PARTICLE_GRID_X; ++i_x)
    {
        for(i_y = 0; i_y < PARTICLE_GRID_Y; ++i_y)
        {
            //first calculate the density at all particles 
            //current particle i, all other particles j, W is the kernel function, d the distance between i and the current j
            //rho_i = SUM_j m_j * W(d)
            curr = grid[i_x][i_y].particle_list;
            for(i_part = 0; i_part < grid[i_x][i_y].particle_count; ++i_part)
            {
                curr->particle.density = 0;
                //now use the particles within the smoothing-distance (surounding grid-cells) to calculate the density at the current particle
                for(d_x = KERNEL_RADIUS_SQ * (-1); d_x <= KERNEL_RADIUS_SQ; ++d_x)
                {
                    if( (i_x + d_x > 0) && 
                        (i_x + d_x < PARTICLE_GRID_X) )
                    {
                        for(d_y = KERNEL_RADIUS_SQ * (-1); d_y <= KERNEL_RADIUS_SQ; ++d_y)
                        {
                            if( (i_y + d_y > 0) && 
                                (i_y + d_y < PARTICLE_GRID_Y) )
                            {
                                partner = grid[i_x+d_x][i_y+d_y].particle_list;
                                for(j_part = 0; j_part < grid[i_x+d_x][i_y+d_y].particle_count; ++j_part)
                                {
                                    //density is sum of: particlemass * kernel-smoothing
                                    double distance = sqrt( pow(curr->particle.position[X] - partner->particle.position[X], 2) +  
                                                            pow(curr->particle.position[Y] - partner->particle.position[Y], 2) );
                                    if(pow(distance, 2) < KERNEL_RADIUS_SQ)
                                    {
                                        curr->particle.density += partner->particle.mass * _kernel_smoothing_density(distance);
                                    }
                                    partner = partner->next;
                                }
                            }
                        }
                    }
                }
                //now we can use the density at the current particle to calculate the pressure at the current particle
                // P = k_p * (rho - rho_0)
                curr->particle.pressure = GAS_CONST * (curr->particle.density - REST_DENS);
                curr = curr->next;
            }
        }
    }
}


double _kernel_smoothing_density(double distance)
{
    //poly6 kernel funciton
    return 315.0 / (65.0 * M_PI * pow(KERNEL_RADIUS, 9.0) * pow(KERNEL_RADIUS_SQ - pow(distance, 2), 3.0));
}


void _compute_forces(particle_grid_element_t grid[PARTICLE_GRID_X][PARTICLE_GRID_Y], double ex_force[2])
{
    int i_x, i_y, d_x, d_y;
    uint32_t i_part, j_part;
    particle_list_element_t* curr;
    particle_list_element_t* partner;
    double f_pressure[2];
    double f_viscosity[2];
    double f_gravity[2];
    if(grid == NULL)
    {
        ERROR("grid == NULL\r\n");
        return;
    }

    for(i_x = 0; i_x < PARTICLE_GRID_X; ++i_x)
    {
        for(i_y = 0; i_y < PARTICLE_GRID_Y; ++i_y)
        {
            //get a particle and calculate the force applied to it depending on it's neighbours
            curr = grid[i_x][i_y].particle_list;
            for(i_part = 0; i_part < grid[i_x][i_y].particle_count; ++i_part)
            {
                f_pressure[X] = 0;
                f_pressure[Y] = 0;
                f_viscosity[X] = 0;
                f_viscosity[Y] = 0;
                f_gravity[X] = ex_force[X] * curr->particle.density;
                f_gravity[Y] = ex_force[Y] * curr->particle.density;
                for(d_x = KERNEL_RADIUS * (-1); d_x <= KERNEL_RADIUS; ++d_x)
                {
                    if( (i_x + d_x > 0) && 
                        (i_x + d_x < PARTICLE_GRID_X) )
                    {
                        for(d_y = KERNEL_RADIUS * (-1); d_y <= KERNEL_RADIUS; ++d_y)
                        {
                            if( (i_y + d_y > 0) && 
                                (i_y + d_y < PARTICLE_GRID_Y) )
                            {
                                partner = grid[i_x+d_x][i_y+d_y].particle_list;
                                for(j_part = 0; j_part < grid[i_x+d_x][i_y+d_y].particle_count; ++j_part)
                                {
                                    //density is sum of: particlemass * kernel-smoothing
                                    
                                    double v_ij[2]; //vector between i and j of i and j
                                    v_ij[X] = partner->particle.position[X] - curr->particle.position[X];
                                    v_ij[Y] = partner->particle.position[Y] - curr->particle.position[Y];
                                    double distance = sqrt( pow(curr->particle.position[X] - partner->particle.position[X], 2) +  
                                                            pow(curr->particle.position[Y] - partner->particle.position[Y], 2) );
                                    double x_normalized = 0;
                                    double y_normalized = 0;
                                    if(distance > 0)
                                    {
                                        x_normalized = v_ij[X] / distance;
                                        y_normalized = v_ij[Y] / distance;
                                    }
                                    double multiplicator = partner->particle.mass * (curr->particle.pressure + partner->particle.pressure) / (2.0 * partner->particle.density) * _kernel_smoothing_pressure(distance);
                                    f_pressure[X]  += x_normalized * multiplicator;
                                    f_pressure[Y]  += y_normalized * multiplicator;
                                    f_viscosity[X] += VISCOSITY * partner->particle.mass * (partner->particle.velocity[X] - curr->particle.velocity[X]) / partner->particle.density * _kernel_smoothing_viscosity(distance);
                                    partner = partner->next;
                                }
                            }
                        }
                    }
                }
                curr->particle.force[X] = f_pressure[X] + f_viscosity[X] + f_gravity[X];
                curr->particle.force[Y] = f_pressure[Y] + f_viscosity[Y] + f_gravity[Y];
                curr = curr->next;
            }
        }
    }
}


double _kernel_smoothing_pressure(double distance)
{
    //spiky gradient kernel (constant scalar)
    return (-45.0) / (M_PI * pow(KERNEL_RADIUS, 6.0))  * pow(KERNEL_RADIUS - distance, 2.0);
}


double _kernel_smoothing_viscosity(double distance)
{
    //viscosity kernel (laplacian scalar)
    return (45) / (M_PI * pow(KERNEL_RADIUS, 6.0)) * (KERNEL_RADIUS - distance);
}


void _integrate(particle_grid_element_t grid[PARTICLE_GRID_X][PARTICLE_GRID_Y], double d_time)
{
    int i_x, i_y;
    particle_list_element_t* curr;
    if(grid == NULL)
    {
        ERROR("grid == NULL\r\n");
        return;
    }
    for(i_x = 0; i_x < PARTICLE_GRID_X; ++i_x)
    {
        for(i_y = 0; i_y < PARTICLE_GRID_Y; ++i_y)
        {
            curr = grid[i_x][i_y].particle_list;
            while(curr != NULL)
            {
                //forward Euler integration
                if(curr->particle.density != 0)
                {
                    curr->particle.velocity[X] += (d_time * curr->particle.force[X]) / curr->particle.density;
                    curr->particle.velocity[Y] += (d_time * curr->particle.force[Y]) / curr->particle.density;
                }
                else
                {
                    curr->particle.velocity[X] += d_time * curr->particle.force[X];
                    curr->particle.velocity[Y] += d_time * curr->particle.force[Y];
                }
                curr->particle.position[X] += d_time * curr->particle.velocity[X];
                curr->particle.position[Y] += d_time * curr->particle.velocity[Y];

                //enforce boundary conditions
                if(curr->particle.position[X] <= 0 + UPPER_BOUNDS_DELTA)
                {
                    curr->particle.position[X] = 0 + UPPER_BOUNDS_DELTA;
                    curr->particle.velocity[X] *= BOUNDARY_DAMPENING;
                }
                if(curr->particle.position[X] >= MAX_X_POS)
                {
                    curr->particle.position[X] = MAX_X_POS;
                    curr->particle.velocity[X] *= BOUNDARY_DAMPENING;
                }
                if(curr->particle.position[Y] <= 0 + UPPER_BOUNDS_DELTA)
                {
                    curr->particle.position[Y] = 0 + UPPER_BOUNDS_DELTA;
                    curr->particle.velocity[Y] *= BOUNDARY_DAMPENING;
                }
                if(curr->particle.position[Y] >= MAX_Y_POS)
                {
                    curr->particle.position[Y] = MAX_Y_POS;
                    curr->particle.velocity[Y] *= BOUNDARY_DAMPENING;
                }
                curr = curr->next;
            }
        }
    }
}


void _integrate_cube(particle_grid_element_t grid[PARTICLE_GRID_X][PARTICLE_GRID_Y], double d_time)
{
    int i_x, i_y;
    particle_list_element_t* curr;
    if(grid == NULL)
    {
        ERROR("grid == NULL\r\n");
        return;
    }
    for(i_x = 0; i_x < PARTICLE_GRID_X; ++i_x)
    {
        for(i_y = 0; i_y < PARTICLE_GRID_Y; ++i_y)
        {
            curr = grid[i_x][i_y].particle_list;
            while(curr != NULL)
            {
                //forward Euler integration
                if(curr->particle.density != 0)
                {
                    curr->particle.velocity[X] += (d_time * curr->particle.force[X]) / curr->particle.density;
                    curr->particle.velocity[Y] += (d_time * curr->particle.force[Y]) / curr->particle.density;
                }
                else
                {
                    curr->particle.velocity[X] += d_time * curr->particle.force[X];
                    curr->particle.velocity[Y] += d_time * curr->particle.force[Y];
                }
                curr->particle.position[X] += d_time * curr->particle.velocity[X];
                curr->particle.position[Y] += d_time * curr->particle.velocity[Y];

                //enforce boundary conditions done in _manage_rolloff()
                curr = curr->next;
            }
        }
    }
}


void _update_grid_position(particle_grid_element_t grid[PARTICLE_GRID_X][PARTICLE_GRID_Y])
{
    int i_x, i_y;
    uint32_t part;
    particle_list_element_t* curr;
    particle_list_element_t* following;
    if(grid == NULL)
    {
        ERROR("grid == NULL\r\n");
        return;
    }

    for(i_x = 0; i_x < PARTICLE_GRID_X; ++i_x)
    {
        for(i_y = 0; i_y < PARTICLE_GRID_Y; ++i_y)
        {
            curr = grid[i_x][i_y].particle_list;
            for(part = 0; part < grid[i_x][i_y].particle_count; ++part)
            {
                following = curr->next;
                _update_grid_position_particle(grid, i_x, i_y, curr);
                curr = following;
            }
        }
    }
}



void _update_grid_position_particle(particle_grid_element_t grid[PARTICLE_GRID_X][PARTICLE_GRID_Y], int orig_grid_x, int orig_grid_y, particle_list_element_t* element)
{
    int new_grid_x, new_grid_y;
    if(grid == NULL)
    {
        ERROR("grid == NULL\r\n");
        return;
    }

    new_grid_x = element->particle.position[X] / PARTICLE_GRID_CELL_WIDTH;   //c always cuts off decimal places double to int
    new_grid_y = element->particle.position[Y] / PARTICLE_GRID_CELL_HEIGHT;  //c always cuts off decimal places double to int


    if((new_grid_x != orig_grid_x) ||
       (new_grid_y != orig_grid_y))
    {
        //remove element from old grid-cell
        _remove_particle(&grid[orig_grid_x][orig_grid_y], element);

        //add element to new grid-cell
        _add_particle(&grid[new_grid_x][new_grid_y], element);
    }
}


bool _remove_particle(particle_grid_element_t* cell, particle_list_element_t* element)
{
    uint32_t i;
    particle_list_element_t* curr = cell->particle_list;
    if(cell == NULL)
    {
        ERROR("cell == NULL\r\n");
        return false;
    }

    for(i = 0; i < cell->particle_count; ++i)
    {
        if(curr == element)
        { //found the element to remove
            cell->particle_count -= 1;
            if(cell->particle_count == 0)
            {
                cell->particle_list = NULL;
            }
            else
            {
                if(element->prev != NULL)
                {
                    element->prev->next = element->next;
                }
                else
                {//element was the first of the particle_list
                    cell->particle_list = element->next;
                }
                if(element->next != NULL)
                {
                    element->next->prev = element->prev;
                }
                else
                {//element was the last of the particle_list
                    ;
                }
            }
        }
        curr = curr->next;
    }
    if(i == cell->particle_count)
    {//element not in cell
        ERROR("element %p not in cell\r\n", (void*)element);
        return false;
    }
    element->next = NULL;
    element->prev = NULL;
    DEBUG("Removed element %p\r\n", (void*)element);
    return true;
}


void _add_particle(particle_grid_element_t* cell, particle_list_element_t* element)
{
    uint32_t i;
    particle_list_element_t* curr;
    if(cell == NULL)
    {
        ERROR("cell == NULL\r\n");
        return;
    }

    if(cell->particle_count != 0)
    {
        curr = cell->particle_list;
        for(i = 1; i < cell->particle_count; ++i)
        {
            curr = curr->next;
        }
        curr->next = element;
        element->prev = curr;
        element->next = NULL;
    }
    else
    {
        cell->particle_list = element;
        element->prev = element->next = NULL;
    }
    cell->particle_count += 1;
}


void _manage_rolloff(particle_grid_element_t top[PARTICLE_GRID_X][PARTICLE_GRID_Y], 
                     particle_grid_element_t bottom[PARTICLE_GRID_X][PARTICLE_GRID_Y], 
                     particle_grid_element_t front[PARTICLE_GRID_X][PARTICLE_GRID_Y], 
                     particle_grid_element_t back[PARTICLE_GRID_X][PARTICLE_GRID_Y], 
                     particle_grid_element_t left[PARTICLE_GRID_X][PARTICLE_GRID_Y], 
                     particle_grid_element_t right[PARTICLE_GRID_X][PARTICLE_GRID_Y])
{
    int i_x, i_y;
    particle_list_element_t* curr;

    DEBUG("\r\n");
    DEBUG("Manage rolloff top:\r\n");
    //top
    for(i_x = 0; i_x < PARTICLE_GRID_X; ++i_x)
    {
        for(i_y = 0; i_y < PARTICLE_GRID_Y; ++i_y)
        {
            curr = top[i_x][i_y].particle_list;
            while(curr != NULL)
            {
                //enforce boundary conditions
                if(curr->particle.position[X] < 0 + UPPER_BOUNDS_DELTA)
                {//rolloff left
                    DEBUG("particle %p [%2d][%2d] to left\r\n", (void*)&curr->particle, i_x, i_y);
                    if(_remove_particle(&top[i_x][i_y], curr) == true)
                    {
                        _add_particle(&left[0][0], curr);
                    }
                    curr->particle.velocity[X] = -1 * curr->particle.velocity[Y];
                    curr->particle.velocity[Y] = 0;
                    curr->particle.position[X] = -1 * curr->particle.position[Y];
                    curr->particle.position[Y] = MAX_Y_POS;
                }
                if(curr->particle.position[X] > MAX_X_POS)
                {//rolloff right
                    DEBUG("particle %p [%2d][%2d] to right\r\n", (void*)&curr->particle, i_x, i_y);
                    if(_remove_particle(&top[i_x][i_y], curr) == true)
                    {
                        _add_particle(&right[0][0], curr);
                    }
                    curr->particle.velocity[X] = curr->particle.velocity[Y];
                    curr->particle.velocity[Y] = 0;
                    curr->particle.position[X] = curr->particle.position[Y];
                    curr->particle.position[Y] = MAX_Y_POS;
                }
                if(curr->particle.position[Y] < 0 + UPPER_BOUNDS_DELTA)
                {//rolloff bottom
                    DEBUG("particle %p [%2d][%2d] to bottom\r\n", (void*)&curr->particle, i_x, i_y);
                    if(_remove_particle(&top[i_x][i_y], curr) == true)
                    {
                        _add_particle(&front[0][0], curr);
                    }
                    //vel X = vel X
                    curr->particle.velocity[Y] = 0;
                    //pos X = pos X
                    curr->particle.position[Y] = MAX_Y_POS;
                }
                if(curr->particle.position[Y] > MAX_Y_POS)
                {//rolloff top
                    DEBUG("particle %p [%2d][%2d] to top\r\n", (void*)&curr->particle, i_x, i_y);
                    if(_remove_particle(&top[i_x][i_y], curr) == true)
                    {
                        _add_particle(&back[0][0], curr);
                    }
                    curr->particle.velocity[X] *= -1;
                    curr->particle.velocity[Y]  = 0;
                    curr->particle.position[X] *= -1;
                    curr->particle.position[X] *= MAX_Y_POS;
                }
                curr = curr->next;
            }
        }
    }

    DEBUG("\r\n");
    DEBUG("Manage rolloff bottom:\r\n");
    //bottom
    for(i_x = 0; i_x < PARTICLE_GRID_X; ++i_x)
    {
        for(i_y = 0; i_y < PARTICLE_GRID_Y; ++i_y)
        {
            curr = bottom[i_x][i_y].particle_list;
            while(curr != NULL)
            {
                //enforce boundary conditions
                if(curr->particle.position[X] < 0 + UPPER_BOUNDS_DELTA)
                {//rolloff left
                    DEBUG("particle %p [%2d][%2d] to left\r\n", (void*)&curr->particle, i_x, i_y);
                    if(_remove_particle(&bottom[i_x][i_y], curr) == true)
                    {
                        _add_particle(&right[0][0], curr);
                    }
                    curr->particle.velocity[X] = curr->particle.velocity[Y];
                    curr->particle.velocity[Y] = 0;
                    curr->particle.position[X] = curr->particle.position[Y];
                    curr->particle.position[Y] = MAX_Y_POS;
                }
                if(curr->particle.position[X] > MAX_X_POS)
                {//rolloff right
                    DEBUG("particle %p [%2d][%2d] to right\r\n", (void*)&curr->particle, i_x, i_y);
                    if(_remove_particle(&bottom[i_x][i_y], curr) == true)
                    {
                        _add_particle(&left[0][0], curr);
                    }
                    curr->particle.velocity[X] = -1 * curr->particle.velocity[Y];
                    curr->particle.velocity[Y] = 0;
                    curr->particle.position[X] = curr->particle.position[Y];
                    curr->particle.position[Y] = 0 + UPPER_BOUNDS_DELTA;
                }
                if(curr->particle.position[Y] < 0 + UPPER_BOUNDS_DELTA)
                {//rolloff bottom
                    DEBUG("particle %p [%2d][%2d] to bottom\r\n", (void*)&curr->particle, i_x, i_y);
                    if(_remove_particle(&bottom[i_x][i_y], curr) == true)
                    {
                        _add_particle(&front[0][0], curr);
                    }
                    curr->particle.velocity[X] *= -1;
                    curr->particle.velocity[Y] = 0;
                    curr->particle.position[X] *= -1;
                    curr->particle.position[Y] = 0 + UPPER_BOUNDS_DELTA;
                }
                if(curr->particle.position[Y] > MAX_Y_POS)
                {//rolloff top
                    DEBUG("particle %p [%2d][%2d] to top\r\n", (void*)&curr->particle, i_x, i_y);
                    if(_remove_particle(&bottom[i_x][i_y], curr) == true)
                    {
                        _add_particle(&back[0][0], curr);
                    }
                    //vel X = vel X
                    curr->particle.velocity[Y] = 0;
                   //pos X = pos X
                    curr->particle.position[X] = 0 + UPPER_BOUNDS_DELTA;
                }
                curr = curr->next;
            }
        }
    }

    DEBUG("\r\n");
    DEBUG("Manage rolloff front:\r\n");
    //front
    for(i_x = 0; i_x < PARTICLE_GRID_X; ++i_x)
    {
        for(i_y = 0; i_y < PARTICLE_GRID_Y; ++i_y)
        {
            curr = front[i_x][i_y].particle_list;
            while(curr != NULL)
            {
                //enforce boundary conditions
                if(curr->particle.position[X] < 0 + UPPER_BOUNDS_DELTA)
                {//rolloff left
                    DEBUG("particle %p [%2d][%2d] to left\r\n", (void*)&curr->particle, i_x, i_y);
                    if(_remove_particle(&front[i_x][i_y], curr) == true)
                    {
                        _add_particle(&left[0][0], curr);
                    }
                    curr->particle.velocity[X] = 0;
                    //vel Y = vel Y
                    curr->particle.position[X] = MAX_X_POS;
                    //pos Y = pos Y
                }
                if(curr->particle.position[X] > MAX_X_POS)
                {//rolloff right
                    DEBUG("particle %p [%2d][%2d] to right\r\n", (void*)&curr->particle, i_x, i_y);
                    if(_remove_particle(&front[i_x][i_y], curr) == true)
                    {
                        _add_particle(&right[0][0], curr);
                    }
                    curr->particle.velocity[X] = 0;
                    //vel Y = vel Y
                    curr->particle.position[X] = 0 + UPPER_BOUNDS_DELTA;
                    //pos Y = pos Y
                }
                if(curr->particle.position[Y] < 0 + UPPER_BOUNDS_DELTA)
                {//rolloff bottom
                    DEBUG("particle %p [%2d][%2d] to bottom\r\n", (void*)&curr->particle, i_x, i_y);
                    if(_remove_particle(&front[i_x][i_y], curr) == true)
                    {
                        _add_particle(&bottom[0][0], curr);
                    }
                    curr->particle.velocity[X] *= -1;
                    curr->particle.velocity[Y] = 0;
                    curr->particle.position[X] *= -1;
                    curr->particle.position[Y] = 0 + UPPER_BOUNDS_DELTA;
                }
                if(curr->particle.position[Y] > MAX_Y_POS)
                {//rolloff top
                    DEBUG("particle %p [%2d][%2d] to top\r\n", (void*)&curr->particle, i_x, i_y);
                    if(_remove_particle(&front[i_x][i_y], curr) == true)
                    {
                        _add_particle(&top[0][0], curr);
                    }
                    //vel X = vel X
                    curr->particle.velocity[Y] = 0;
                    //pos X = pos X
                    curr->particle.position[Y] = 0 + UPPER_BOUNDS_DELTA;
                }
                curr = curr->next;
            }
        }
    }

    DEBUG("\r\n");
    DEBUG("Manage rolloff back:\r\n");
    //back
    for(i_x = 0; i_x < PARTICLE_GRID_X; ++i_x)
    {
        for(i_y = 0; i_y < PARTICLE_GRID_Y; ++i_y)
        {
            curr = back[i_x][i_y].particle_list;
            while(curr != NULL)
            {
                //enforce boundary conditions
                if(curr->particle.position[X] < 0 + UPPER_BOUNDS_DELTA)
                {//rolloff left
                    DEBUG("particle %p [%2d][%2d] to left\r\n", (void*)&curr->particle, i_x, i_y);
                    if(_remove_particle(&back[i_x][i_y], curr) == true)
                    {
                        _add_particle(&right[0][0], curr);
                    }
                    curr->particle.velocity[X] = 0;
                    //vel Y = vel Y
                    curr->particle.position[X] = MAX_X_POS;
                    //pos Y = pos Y
                }
                if(curr->particle.position[X] > MAX_X_POS)
                {//rolloff right
                    DEBUG("particle %p [%2d][%2d] to right\r\n", (void*)&curr->particle, i_x, i_y);
                    if(_remove_particle(&back[i_x][i_y], curr) == true)
                    {
                        _add_particle(&left[0][0], curr);
                    }
                    curr->particle.velocity[X] = 0;
                    //vel Y = vel Y
                    curr->particle.position[X] = 0 + UPPER_BOUNDS_DELTA;
                    //pos Y = pos Y
                }
                if(curr->particle.position[Y] < 0 + UPPER_BOUNDS_DELTA)
                {//rolloff bottom
                    DEBUG("particle %p [%2d][%2d] to bottom\r\n", (void*)&curr->particle, i_x, i_y);
                    if(_remove_particle(&back[i_x][i_y], curr) == true)
                    {
                        _add_particle(&bottom[0][0], curr);
                    }
                    //vel X = vel X
                    curr->particle.velocity[Y] = 0;
                    //pos X = pos X
                    curr->particle.position[Y] = MAX_Y_POS;
                }
                if(curr->particle.position[Y] > MAX_Y_POS)
                {//rolloff top
                    DEBUG("particle %p [%2d][%2d] to top\r\n", (void*)&curr->particle, i_x, i_y);
                    if(_remove_particle(&back[i_x][i_y], curr) == true)
                    {
                        _add_particle(&top[0][0], curr);
                    }
                    //vel X = vel X
                    curr->particle.velocity[Y] = 0;
                    //pos X = pos X
                    curr->particle.position[Y] = MAX_Y_POS;
                }
                curr = curr->next;
            }
        }
    }

    DEBUG("\r\n");
    DEBUG("Manage rolloff left:\r\n");
    //left
    for(i_x = 0; i_x < PARTICLE_GRID_X; ++i_x)
    {
        for(i_y = 0; i_y < PARTICLE_GRID_Y; ++i_y)
        {
            curr = left[i_x][i_y].particle_list;
            while(curr != NULL)
            {
                //enforce boundary conditions
                if(curr->particle.position[X] < 0 + UPPER_BOUNDS_DELTA)
                {//rolloff left
                    DEBUG("particle %p [%2d][%2d] to left\r\n", (void*)&curr->particle, i_x, i_y);
                    if(_remove_particle(&left[i_x][i_y], curr) == true)
                    {
                        _add_particle(&back[0][0], curr);
                    }
                    curr->particle.velocity[X] = 0;
                    //vel Y = vel Y
                    curr->particle.position[X] = MAX_X_POS;
                    //pos Y = pos Y
                }
                if(curr->particle.position[X] > MAX_X_POS)
                {//rolloff right
                    DEBUG("particle %p [%2d][%2d] to right\r\n", (void*)&curr->particle, i_x, i_y);
                    if(_remove_particle(&left[i_x][i_y], curr) == true)
                    {
                        _add_particle(&front[0][0], curr);
                    }
                    curr->particle.velocity[X] = 0;
                    //vel Y = vel Y
                    curr->particle.position[X] = 0 + UPPER_BOUNDS_DELTA;
                    //pos Y = pos Y
                }
                if(curr->particle.position[Y] < 0 + UPPER_BOUNDS_DELTA)
                {//rolloff bottom
                    DEBUG("particle %p [%2d][%2d] to bottom\r\n", (void*)&curr->particle, i_x, i_y);
                    if(_remove_particle(&left[i_x][i_y], curr) == true)
                    {
                        _add_particle(&bottom[0][0], curr);
                    }
                    curr->particle.velocity[Y] = -1 * curr->particle.velocity[X];
                    curr->particle.velocity[X] = 0;
                    curr->particle.position[Y] = -1 * curr->particle.position[X];
                    curr->particle.position[X] = MAX_X_POS;
                }
                if(curr->particle.position[Y] > MAX_Y_POS)
                {//rolloff top
                    DEBUG("particle %p [%2d][%2d] to top\r\n", (void*)&curr->particle, i_x, i_y);
                    if(_remove_particle(&left[i_x][i_y], curr) == true)
                    {
                        _add_particle(&top[0][0], curr);
                    }
                    curr->particle.velocity[Y] = curr->particle.velocity[X];
                    curr->particle.velocity[X] = 0;
                    curr->particle.position[Y] = curr->particle.position[X];
                    curr->particle.position[X] = 0 + UPPER_BOUNDS_DELTA;
                }
                curr = curr->next;
            }
        }
    }

    DEBUG("\r\n");
    DEBUG("Manage rolloff right:\r\n");
    //right
    for(i_x = 0; i_x < PARTICLE_GRID_X; ++i_x)
    {
        for(i_y = 0; i_y < PARTICLE_GRID_Y; ++i_y)
        {
            curr = right[i_x][i_y].particle_list;
            while(curr != NULL)
            {
                //enforce boundary conditions
                if(curr->particle.position[X] < 0 + UPPER_BOUNDS_DELTA)
                {//rolloff left
                    DEBUG("particle %p [%2d][%2d] to left\r\n", (void*)&curr->particle, i_x, i_y);
                    if(_remove_particle(&right[i_x][i_y], curr) == true)
                    {
                        _add_particle(&front[0][0], curr);
                    }
                    curr->particle.velocity[X] = 0;
                    //vel Y = vel Y
                    curr->particle.position[X] = MAX_X_POS;
                    //pos Y = pos Y
                }
                if(curr->particle.position[X] > MAX_X_POS)
                {//rolloff right
                    DEBUG("particle %p [%2d][%2d] to right\r\n", (void*)&curr->particle, i_x, i_y);
                    if(_remove_particle(&right[i_x][i_y], curr) == true)
                    {
                        _add_particle(&back[0][0], curr);
                    }
                    curr->particle.velocity[X] = 0;
                    //vel Y = vel Y
                    curr->particle.position[X] = 0 + UPPER_BOUNDS_DELTA;
                    //pos Y = pos Y
                }
                if(curr->particle.position[Y] < 0 + UPPER_BOUNDS_DELTA)
                {//rolloff bottom
                    DEBUG("particle %p [%2d][%2d] to bottom\r\n", (void*)&curr->particle, i_x, i_y);
                    if(_remove_particle(&right[i_x][i_y], curr) == true)
                    {
                        _add_particle(&bottom[0][0], curr);
                    }
                    curr->particle.velocity[Y] = curr->particle.velocity[X];
                    curr->particle.velocity[X] = 0;
                    curr->particle.position[Y] = curr->particle.position[X];
                    curr->particle.position[X] = 0 + UPPER_BOUNDS_DELTA;
                }
                if(curr->particle.position[Y] > MAX_Y_POS)
                {//rolloff top
                    DEBUG("particle %p [%2d][%2d] to top\r\n", (void*)&curr->particle, i_x, i_y);
                    if(_remove_particle(&right[i_x][i_y], curr) == true)
                    {
                        _add_particle(&top[0][0], curr);
                    }
                    curr->particle.velocity[Y] = curr->particle.velocity[X];
                    curr->particle.velocity[X] = 0;
                    curr->particle.position[Y] = curr->particle.position[X];
                    curr->particle.position[X] = MAX_X_POS;
                }
                curr = curr->next;
            }
        }
    }
}

