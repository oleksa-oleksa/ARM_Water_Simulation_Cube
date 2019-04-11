/**@file    particle.h
 * @brief   SPH simulation implementation
 */
#ifndef PARTICLE_H_
#define PARTICLE_H_

#include <stdint.h>
#include <stdbool.h>

#define PARTICLE_GRID_X     32      ///< Grid X dimension.
#define PARTICLE_GRID_Y     32      ///< Grid Y dimension.

#define PARTICLE_GRID_CELL_WIDTH  1 ///< Width of a grid-cell in meter.
#define PARTICLE_GRID_CELL_HEIGHT 1 ///< Height of a grid-cell in meter.

#define PARTICLE_DEBUG  0           ///< Enable / disable debug-messages.
#define PARTICLE_ERROR  0           ///< Enable / disable error-messages.


typedef struct
{
    double position[2]; ///< Position on the grid in meter (from top-left corner, (X, Y)).
    double velocity[2]; ///< Velocity in m/s, (X, Y).
    double mass;        ///< Mass of the particle in kg.
    double density;     ///< Density of the particle in kg/m^2.
    double pressure;    ///< Preasure on the particle.
    double force[2];    ///< Forces on the particle in N.
} particle_t;


typedef struct particle_list_element_s
{
    particle_t particle;                    ///< Particle in the list.
    struct particle_list_element_s* prev;   ///< Previous particle. Can be NULL.
    struct particle_list_element_s* next;   ///< Next particle. Can be NULL.
} particle_list_element_t;


typedef struct
{
    particle_list_element_t* particle_list; ///< List of particles in this cell of the grid. Can be NULL.
    uint32_t particle_count;                ///< Number of elementes in the linked list of this grid-cell.
} particle_grid_element_t;

enum side_e
{
    side_top    = 0,    ///< 
    side_bottom = 1,    ///< 
    side_front  = 2,    ///< 
    side_back   = 3,    ///< 
    side_left   = 4,    ///< 
    side_right  = 5     ///< 
};

typedef void (*element_change_callback_fp_t)(enum side_e side, int x, int y);

typedef particle_grid_element_t panel_t[PARTICLE_GRID_X][PARTICLE_GRID_Y];

/**
 * @brief                   Initialise a particle-list.
 * @details                 The particles will be aranged in a rectangle.
 * @param[inout]    list    The particle-list.
 * @param[in]       size    Size of the list.
 * @return                  True on success.
 */
bool particle_init_list(particle_list_element_t* list, uint32_t size);

/**
 * @brief                   Initialise a grid with a given particle-list
 * @param[inout]    grid    The grid to initialise.
 * @param[in]       list    The list to initialise with.
 * @param[in]       size    The size of the list.
 * @return                  True on success.
 */
bool particle_init_grid(particle_grid_element_t grid[PARTICLE_GRID_X][PARTICLE_GRID_Y], particle_list_element_t* list, uint32_t size);

/**
 * @brief                       Move the particles on a singel panel.
 * @details                     Simulation only with a single panel. When the particles get near a boundary of the panel they
 *                              will bounce back.
 * @param[inout]    grid        The grid / panel to simulate.
 * @param[in]       d_time      The timestep.
 * @param[in]       ex_force    External force formatted ad 2D-vector (X, Y)
 * @return                      True on success
 */
bool particle_move_single_panel(particle_grid_element_t grid[PARTICLE_GRID_X][PARTICLE_GRID_Y], double d_time, double ex_force[2]);

/**
 * @brief                   Add a callback-function called when a particle in the grid has moved to another cell.
 * @details                 Optional. Used by the pi to trigger an update-message to the MCB2388.
 * @param[in]   callback    Callback-function.
 */
void particle_add_grid_element_change_callback(element_change_callback_fp_t callback);

/**
 * @brief                       Move the particles on a cube.
 * @details                     To help with spreading the load only a part of the simulation will be done per call. To do one 
 *                              complete simulation-step this function needs to be called 26 times.
 * @param[inout]    top         Top-panel.
 * @param[inout]    bottom      Bottom-panel.
 * @param[inout]    front       Front-panel.
 * @param[inout]    back        Bakc-panel.
 * @param[inout]    left        Left-panel.
 * @param[inout]    right       Right-panel.
 * @param[in]       d_time      The timestep.
 * @param[in]       ex_force    External force formatted ad 2D-vector (X, Y)
 * @return                      True on success.
 */
bool particle_move_cube(particle_grid_element_t top[PARTICLE_GRID_X][PARTICLE_GRID_Y], 
                        particle_grid_element_t bottom[PARTICLE_GRID_X][PARTICLE_GRID_Y], 
                        particle_grid_element_t front[PARTICLE_GRID_X][PARTICLE_GRID_Y], 
                        particle_grid_element_t back[PARTICLE_GRID_X][PARTICLE_GRID_Y], 
                        particle_grid_element_t left[PARTICLE_GRID_X][PARTICLE_GRID_Y], 
                        particle_grid_element_t right[PARTICLE_GRID_X][PARTICLE_GRID_Y], 
                        double d_time, double ex_force[3]);





#endif
