/******************************************************************************/
/* Accelerometer Adafruit BNO55                             */
/******************************************************************************/
/* This file is part of the project for university course   */
/* Advanced ARM Programming 	                              */
/* Wintersemester 2018/2019                                 */
/******************************************************************************/
#define BNO055_ID (0xA0)

#include <stdio.h>
#include <LPC23xx.H>                    /* LPC23xx definitions                */
#include "LCD.h"                        /* Graphic LCD function prototypes    */
#include <stdint.h>
#include "include/bno055.h"

/* Global variables */
unsigned char I2Cdata; // data to send into I2C
unsigned char I2Cmessage; // received data fron I2C
unsigned int I2CAddress = 0xE005C000;

/* Import external functions from Serial.c file                               */
extern       void init_serial    (void);

/**************************************************************************/
/*!
    @brief  delay
*/
/**************************************************************************/
void delay(void) {
	int i;
	for (i = 0; i < 200000; i++) {
		// just a hadrcore delay
	};
}

/**************************************************************************/
/*!
    @brief  Greeting message before I2C init
*/
/**************************************************************************/
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

/**************************************************************************/
/*!
    @brief  I2C Interrupt Service Routine
		State machine examines the status  register on  each  interrupt  
		and  performs  the  necessary  action 
*/
/**************************************************************************/
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

/**************************************************************************/
/*!
    @brief  I2C Write Data Transfer
		Once the I2C peripheral is initialised in master mode we can start a write data transfer
*/
/**************************************************************************/
void I2CTransferByteWrite(unsigned char Addr, unsigned char Data) {    
	I2CAddress = Addr;
	I21DAT = Data;    
	I21CONCLR = 0x000000FF; // Clear all I2C settings    
	I21CONSET = 0x00000040; // Enable the I2C interface    
	I21CONSET = 0x00000020; // Start condition }
} 

/**************************************************************************/
/*!
    @brief  I2C Read Data Transfer
		Once the I2C peripheral is initialised in master mode we can start a write data transfer
*/
/**************************************************************************/
void I2CTransferByteRead(unsigned char Addr) {    
	I2CAddress = Addr;
	I21DAT = I2Cmessage; // global variable    
	I21CONCLR = 0x000000FF; // Clear all I2C settings    
	I21CONSET = 0x00000040; // Enable the I2C interface    
	I21CONSET = 0x00000020; // Start condition }
} 

/**************************************************************************/
/*!
    @brief  I2C init
    1. configure the VIC to respond to a I2C interrupt
		2. configure pinselect block to connect the I2C data and clock lines to the external pins
		3. set the bit rate by programming I2SCLH and I2SCLL. Only the first 16 bits are used to hold the timing values
*/
/**************************************************************************/
void i2c_init(void) {
	
	VICVectCntl1 = 0x00000029;       // select a priority slot for a given interrupt
	VICVectAddr1 = (unsigned)i2c_isr; //pass the address of the IRQ into the VIC slot 
	VICIntEnable = 0x00000200; // enable interrupt 
	PINSEL1 = 0x50; //Switch GPIO to I2C pins
	I21SCLH = 0x08; //Set bit rate to 57.6KHz
	I21SCLL  = 0x08;
}

/**************************************************************************/
/*!
    @brief  Puts the chip in the specified operating mode
*/
/**************************************************************************/
void setBNOMode(unsigned char mode)
{
  I2CTransferByteWrite(I2CAddress, BNO055_OPR_MODE_ADDR);
	I2CTransferByteWrite(I2CAddress, mode);
  delay();
}


/**************************************************************************/
/*!
    @brief  Adafruit BNO055 Init
    Note: I2C should be already enabled
*/
/**************************************************************************/
int accelerometer_init(unsigned char requestedMode) {
	uint8_t id = 0; 
	 /* Make sure we have the right device */
	I2CTransferByteWrite(I2CAddress, BNO055_CHIP_ID_ADDR);
  I2CTransferByteRead(I2CAddress);
	id = I2Cmessage;
	
  if (id != BNO055_ID)
  {
    delay();
			
		// repeat 
    I2CTransferByteRead(I2CAddress);
		id = I2Cmessage;
    
		if(id != BNO055_ID) {
      return 0;  // still not? ok bail
    }
  }
	
  /* Switch to config mode (just in case since this is the default) */
  setBNOMode(BNO055_OPERATION_MODE_CONFIG);
	
	  /* Reset */
  I2CTransferByteWrite(I2CAddress, BNO055_SYS_TRIGGER_ADDR);
	I2CTransferByteWrite(I2CAddress, 0x20);

	do {
		
		I2CTransferByteRead(I2CAddress);
		id = I2Cmessage;
		delay();
		
	} while (id != BNO055_ID);
  
	delay();
		
  /* Set to normal power mode */
	I2CTransferByteWrite(I2CAddress, BNO055_PWR_MODE_ADDR);
  I2CTransferByteWrite(I2CAddress, BNO055_POWER_MODE_NORMAL);
 
	delay();
	
	I2CTransferByteWrite(I2CAddress, BNO055_PAGE_ID_ADDR);
  I2CTransferByteWrite(I2CAddress, 0);
	
	I2CTransferByteWrite(I2CAddress, BNO055_SYS_TRIGGER_ADDR);
  I2CTransferByteWrite(I2CAddress, 0x0);
	
  delay();
 
	 /* Set the requested operating mode */
  setBNOMode(requestedMode);
  delay();
	
	return 1;
}


int main (void) {
	
  unsigned char message[100];
	
	// UART Init
	init_serial();

	// LCD Init
  lcd_init();
	lcd_print_greeting();

	// I2C Init 
	i2c_init();

	// BNO055 Adafruit Init
	accelerometer_init(BNO055_OPERATION_MODE_ACCGYRO);
	  
	while (1) {
			sprintf(message, "%i", (int)I2Cmessage);
      lcd_print_message(message); 
			delay();
  }  
}
