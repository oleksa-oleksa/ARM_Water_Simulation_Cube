/**
 * @file   demo_accelerometer.c
 * @brief  Demonstration of accelerometer
 * @date   2019-03-13
 * @author Oleksandra Baga, Kayoko Abe
 */
#include "accelerometer.h"

#include <stdio.h>

int main (void) {
    volatile uint8_t id;
    char i2c_msg[16];
    int16_t x;
    int16_t y;
    int16_t z;

    // LCD Init
    lcd_init();
    lcd_print_greeting();
    delay(ldelay);

    // I2C Init
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

    while (1) {
        // Debug
        //sprintf(i2c_msg, "0x%x", I2Cmessage);
        //lcd_print_message(i2c_msg);

        x = getX();
        y = getY();
        z = getZ();
        sprintf(i2c_msg, "%4d %4d %4d", x, y, z);
        lcd_print_coordinates((unsigned char *)i2c_msg);
        delay(sdelay);
    }

}
