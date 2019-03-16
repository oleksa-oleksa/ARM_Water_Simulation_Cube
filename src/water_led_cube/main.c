#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define DISPLAY_IT_PER_SIM_IT 15
#define ENABLE_ACCELEROMETER 0
#define DT_SIM 0.005

#include <particle.h>
#include <led32x32.h>
#include <utils.h>
#include <paint_tool.h>
#if ENABLE_ACCELEROMETER
#include <accelerometer.h>
#endif

int main()
{
    static panel_t panels[CHAIN_LEN];
    static particle_list_element_t particles[50];
    double force[3] = {1, 9.8, 1};
    uint32_t iteration = 0;
    #if ENABLE_ACCELEROMETER
    volatile uint8_t id;
    char i2c_msg[16];
    int16_t x;
    int16_t y;
    int16_t z;
    #endif
    
    /* Initialize hardware */
    led32x32_init();
    #if ENABLE_ACCELEROMETER
    lcd_init();
    lcd_print_greeting();
    delay(ldelay);
    i2c_init(); // fixed, works properly
    lcd_print_message((unsigned char *)"I2C Init done...");
    delay(ldelay);
    if (accelerometer_init()) {
        lcd_print_message((unsigned char *)"ADXL345 found");
        delay(ldelay);
    } else {
        lcd_print_message((unsigned char *)"ADXL345 error");
        delay(ldelay);

        return 1;
    }
    lcd_print_message((unsigned char *)"ADXL345 started!");
    delay(ldelay);
    #endif

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
        ++iteration;
        if(iteration >= DISPLAY_IT_PER_SIM_IT)
        {
            #if ENABLE_ACCELEROMETER
            x = getX();
            y = getY();
            z = getZ();
            sprintf(i2c_msg, "%4d %4d %4d", x, y, z);
            lcd_print_coordinates((unsigned char *)i2c_msg);
            delay(sdelay);
            force[0] = x;
            force[1] = y;
            force[2] = z;
            #endif
            particle_move_cube(/*top=*/panels[0], /*bottom=*/panels[1], /*front=*/panels[2], /*back=*/panels[3], /*left=*/panels[4], /*right=*/panels[5], DT_SIM, force);
            iteration = 0;
        }
        lp32x32_refresh_chain(panels);
    }

    return 0;
}
