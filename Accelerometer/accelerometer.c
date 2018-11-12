/******************************************************************************/
/* Accelerometer Adafruit BNO55                             */
/******************************************************************************/
/* This file is part of the project for university course   */
/* Advanced ARM Programming 	                              */
/* Wintersemester 2018/2019                                 */
/******************************************************************************/
                  
#include <stdio.h>
#include <LPC23xx.H>                    /* LPC23xx definitions                */
#include "LCD.h"                        /* Graphic LCD function prototypes    */
//#include "include/Adafruit_BNO055.h"

/* Global variables */
unsigned char I2Cdata; // data to send into I2C
unsigned char I2Cmessage; // received data fron I2C
unsigned int I2CAddress = 0xE005C000;

/* Import external functions from Serial.c file                               */
extern       void init_serial    (void);

/* Greeting message before I2C init */
void lcd_print_greeting(void) {
	
	lcd_clear();
  lcd_print ("Accelerometer");
  set_cursor (0, 1);
  lcd_print ("Beuth Hochschule");
}

 void lcd_print_message(unsigned char *msg) {
	lcd_clear();
  lcd_print(msg);
}


/* I2C Interrupt Service Routine
 state machine examines the status  register on  each  interrupt  
and  performs  the  necessary  action 
*/
void i2c_isr(void) {

	switch (I21STAT) // Read result code and switch to next action 
		{    
		case ( 0x08): // Start bit       
			I21CONCLR = 0x20;    // Clear start bit       
		  I21DAT = I2CAddress; // Send address and write bit 
		break;      
		
		case (0x18): // Slave address+W, ACK       
			I21DAT = I2Cdata; // Write data to TX register    
		break;      
		
		case (0x20): // Slave address +W, Not ACK       
			I21DAT = I2CAddress; // Resend address and write bit    
		break;      
		
		case (0x28): // Data sent, Ack       
			I21CONSET = 0x10; // Stop condition    
		break; 

		case (0x40) : // Slave Address +R, ACK    
			I21CONSET = 0x04; // Enable ACK for data byte 
		break; 
	
		case (0x48) : // Slave Address +R, Not Ack    
			I21CONSET = 0x20; // Resend Start condition 
		break; 
	
		case (0x50) : // Data Received, ACK     
			I2Cmessage = I21DAT;    
			I21CONSET = 0x10; // Stop condition    
			//lock = 0;         // Signal end of I2C activity 
		break; 
	
	case (0x58): // Data Received, Not Ack    
			I21CONSET = 0x20; // Resend Start condition 
	break;		
		
		default   :      
			break;      
		}      
		
		I21CONCLR = 0x08;   // Clear I2C interrupt flag    
		VICVectAddr = 0x00000000; // Clear interrupt in  } 

}

void I2CTransferByte(unsigned Addr, unsigned Data) {    
	I2CAddress = Addr;
	I21DAT = Data;    
	I21CONCLR = 0x000000FF; // Clear all I2C settings    
	I21CONSET = 0x00000040; // Enable the I2C interface    
	I21CONSET = 0x00000020; // Start condition }
} 

/* I2C init 
1. configure the VIC to respond to a I2C interrupt
2. configure pinselect block to connect the I2C data and clock lines to the external pins
3. set the bit rate by programming I2SCLH and I2SCLL. Only the first 16 bits are used to hold the timing values
*/
void i2c_init(void) {
	
	VICVectCntl1 = 0x00000029;       // select a priority slot for a given interrupt
	VICVectAddr1 = (unsigned)i2c_isr; //pass the address of the IRQ into the VIC slot 
	VICIntEnable = 0x00000200; // enable interrupt 
	PINSEL1 = 0x50; //Switch GPIO to I2C pins
	I21SCLH = 0x08; //Set bit rate to 57.6KHz
	I21SCLL  = 0x08;
}



int main (void) {
	int i = 0;
  unsigned char message[100];
  init_serial();

  lcd_init();
	lcd_print_greeting();

	i2c_init();
	  
	while (1) {
			sprintf(message, "%i", (int)I2Cmessage);
      lcd_print_message(message); 
			for(i = 0; i < 200000; i++){};
  }  
}
