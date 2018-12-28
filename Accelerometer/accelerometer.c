/******************************************************************************/
/* Accelerometer Adafruit BNO55                             */
/******************************************************************************/
/* This file is part of the project for university course   */
/* Advanced ARM Programming 	                              */
/* Wintersemester 2018/2019                                 */
/******************************************************************************/
#pragma anon_unions

#define BNO055_ID (0xA0)

#include <stdio.h>
#include <LPC23xx.H>                    /* LPC23xx definitions                */
#include "LCD.h"                        /* Graphic LCD function prototypes    */
#include <stdint.h>
#include <string.h>
#include <time.h>
#include "include/ada_sensor.h"

/* Global variables */
unsigned char I2Cdata; // data to send into I2C
unsigned char I2Cmessage; // received data fron I2C
unsigned int BNOI2CAddress = 0x50; //01010000 Device (Slave) Address (7 bits) shifted BNO055_I2C_ADDR1 (0x28) 
unsigned int ADXLI2CAdresss = 0x3A; // With the ALT ADDRESS pin high, the 7-bit I2C address for the device is 0x1D, followed by the R/W bit. 
																		// This translates to 0x3A for a write and 0x3B for a read.
																		// But 0x3A is used for read and write, the i2c_irq will add read bit during read transfer 

unsigned char GlobalI2CAddr;
unsigned char GlobalI2CReg;
unsigned char GlobalI2CData;
unsigned char GlobalI2CRead;
volatile enum {I2C_ADR, I2C_REG, I2C_DAT, I2C_READ, I2C_ERR, I2C_LOST, I2C_DONE} GlobalI2CState;

volatile uint8_t DebugI2CState;


/**************************************************************************/
/*!
    @brief  delay
*/
/**************************************************************************/
void delay(void) {
	int i;
	for (i = 0; i < 2000000; i++) {
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
	set_cursor (0, 0);
}

 void lcd_print_message(unsigned char *msg) {
	lcd_clear();
	set_cursor (0, 0);
  lcd_print(msg);
}

/**************************************************************************/
/*!
    @brief  I2C Interrupt Service Routine
		State machine examines the status  register on  each  interrupt  
		and  performs  the  necessary  action 
*/
/**************************************************************************/
__irq void i2c_irq(void) {

	DebugI2CState = I21STAT;
	switch (I21STAT) // Read result code and switch to next action 
		{    
		case ( 0x08): // A START condition has been transmitted. Next: 0x18    
      if(GlobalI2CState == I2C_READ) {
				// Write Slave Address with R/W bit to I2DAT
			  I21DAT = GlobalI2CAddr | 1; // Send address and read bit R/W = 1
			} else {
				I21DAT = GlobalI2CAddr; // Send address and write bit R/W = 0.
				GlobalI2CState = I2C_REG;
			}
			I21CONSET = 0x04; // Write 0x04 to I2CONSET to set the AA bit
			I21CONCLR = 0x28; // Clear start bit and SI flag			
			
		break;  

		case (0x10): // A repeated START condition has been transmitted. Next: 0x18
			if(GlobalI2CState == I2C_READ) {
				// Write Slave Address with R/W bit to I2DAT
			  I21DAT = GlobalI2CAddr | 1; // Send address and read bit R/W = 1
			} else {
				I21DAT = GlobalI2CAddr; // Send address and write bit R/W = 0.
				GlobalI2CState = I2C_REG;
			}
			I21CONSET = 0x04; // Write 0x04 to I2CONSET to set the AA bit
			I21CONCLR = 0x28; // Clear start bit and SI flag			
			break;
		
		// Previous state was State 8 or State 10, Slave Address + Write has been transmitted, ACK has been received. 
		// The first data byte will be transmitted, an ACK bit will be received.
		case (0x18): // Next: 0x28
			I21DAT = GlobalI2CReg; // Write data to TX register 
			if (GlobalI2CRead) {
				GlobalI2CState = I2C_READ;
			}  
			else {	
				GlobalI2CState = I2C_DAT;
				 
			 }
			I21CONSET = 0x04;
			I21CONCLR = 0x08; // clear SI flag and STA
		break;      
		
		case (0x20): // SLA+W has been transmitted; NOT ACK has been received      
			I21CONSET = 0x14; // set the STO and AA bits
			I21CONCLR = 0x08; // clear SI flag
			GlobalI2CState = I2C_ERR;    
		break;      
				
		case (0x28): // Data byte in I2DAT has been transmitted; ACK has been received.
				if (GlobalI2CRead) {
					I21CONSET = 0x24; // Repeated start condition for Read Access
					I21CONCLR = 0x08; // clear SI flag
					GlobalI2CState = I2C_READ;
				}
				
				else if (GlobalI2CState == I2C_DAT) {
					I21DAT = GlobalI2CData;
					I21CONSET = 0x14; // STO and AA
					I21CONCLR = 0x08; // clear SI flag
					GlobalI2CState = I2C_DONE;
				}
		break; 
		
		case (0x30): // Data sent, Not Ack 
			I21CONSET = 0x14; // set the STO and AA bits
			I21CONCLR = 0x08; // clear SI flag
			GlobalI2CState = I2C_ERR;          
		break;

    case (0x38): // Arbitration lost 
			I21CONSET = 0x24; // to set the STA and AA bits
			I21CONCLR = 0x08; // clear SI flag
			GlobalI2CState = I2C_LOST;          
		break;		

		case (0x40) : // SLA+R has been transmitted; ACK has been received. Next: 0x50   
			I21CONSET = 0x04; // Enable ACK for data byte
			I21CONCLR = 0x08; // clear SI flag
		break; 
	
		case (0x48) : // Slave Address +R, Not Ack    
			I21CONSET = 0x14; // set the STO and AA bits
			I21CONCLR = 0x08; // clear SI flag
			GlobalI2CState = I2C_ERR;          
		break; 
	
		/* Data has been received, ACK has been returned. Data will be read from I2DAT. 
		Additional data will be received. If this is the last data byte then NOT ACK will be returned, 
		otherwise ACK will be returned */
		case (0x50) :      
			GlobalI2CData = I21DAT;    
			I21CONCLR = 0x0C; //clear the SI flag and the AA bit
			//I21CONSET = 0x04; // set the AA bit
			I21CONCLR = 0x08; // clear SI flag
			//I21CONSET = 0x10; //  set STO
			GlobalI2CState = I2C_DONE;   
		 break; 
	
	   case (0x58): // Data Received, Not Ack    
			//GlobalI2CData = I21DAT;    
			//I21CONSET = 0x14; // set STO and AA bit
		  I21CONSET = 0x10; // set STO and AA bit
			I21CONCLR = 0x08; // clear SI flag 
			GlobalI2CState = I2C_DONE;  
	   break;		
		
		default:      
			break;      
		}      

		VICVectAddr = 0x00000000; // Clear interrupt in 
}

void I2CWriteReg(unsigned char addr, unsigned char reg, unsigned char data) {
  GlobalI2CAddr = addr;
	GlobalI2CReg = reg;
	GlobalI2CData = data;
	GlobalI2CRead = 0;
	GlobalI2CState = I2C_ADR;
	   
	I21CONSET = 0x20; // Start condition
	
	while((GlobalI2CState != I2C_ERR) && (GlobalI2CState != I2C_DONE)) {
	;
	}
} 

uint8_t I2CReadReg(unsigned char addr, unsigned char reg) {
  GlobalI2CAddr = addr;
	GlobalI2CReg = reg;
	GlobalI2CRead = 1;
	GlobalI2CState = I2C_ADR;
	
	I21CONSET = 0x20; // Start condition
	
	while((GlobalI2CState != I2C_ERR) && (GlobalI2CState != I2C_DONE)) {
	;
	}
	
	return GlobalI2CData;
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
	PCONP |= 0x00080000;
	delay();
	
	VICVectCntl19 = 0x0000001;       // select a priority slot for a given interrupt
	VICVectAddr19 = (unsigned)i2c_irq; //pass the address of the IRQ into the VIC slot 
	VICIntEnable |= 0x00080000; // enable interrupt 	
	PINSEL1 |= 0x000003C0; //Switch GPIO to I2C pins
	//I21SCLH = 0xF; 
	//I21SCLL  = 0xF;
	I21SCLL  = 60; //Set bit rate to 200KHz (two time slower as max frequency from datasheet)
			
	// Fcco = 2 * M * Fin / N = 2 * (MSEL +1 ) * 12 MHz / 1 = 2 * 12 * 12 * 10^6 = 288 * 10^6
									// Devider CCLKCFG_Val = 0x00000005 ==> 288 /6 = 48 * 10^6
									// CCLKCFG_Val =  0x00000000 ===> 48 / 4 = 12 * 10^6 == Input Frequency
									// BNO055 max frequency 400 KHz = 4 * 10^5
									// SCLH + SCLL = 60
	I21SCLH  = 60;
	
	I21CONCLR = 0x000000FF; // Clear all I2C settings
  delay();
	// Before the master transmitter mode can be entered, I2CONSET must be initialized with 0100 0000
	I21CONSET = 0x00000040; // Enable the I2C interface 
}

/**************************************************************************/
/*!
    @brief  ADXL345 Init
    Note: I2C should be already enabled
*/
/**************************************************************************/
int accelerometer_init(unsigned char requestedMode) {
	volatile uint8_t id;

	return 1;
}

/*************************************************************/

int main (void) {
	volatile uint8_t id;
	char i2c_msg[10];

	// LCD Init
  lcd_init();
	lcd_print_greeting();
	delay();

	// I2C Init 
	i2c_init(); // fixed, works properly
	lcd_print_message("I2C Init done...");
	delay();
		
	while (1) {
			//I2CWriteReg(0x3A, 0x1E, 0xAB);
		delay();
			id = I2CReadReg(0x3A, 0x1E);
	
			sprintf(i2c_msg, "0x%x", id);
			lcd_print_message(i2c_msg);
			delay();
			delay();
			delay();
			delay();
			delay();
			lcd_print_message("Restart");
			delay();
			delay();

			/*sensors_event_t event; 
		
			lcd_print_message("ACCGYRO started");
			// Get a new sensor event
			getBNOEvent(&event, VECTOR_ACCELEROMETER, BNO_AccVector);
			getBNOEvent(&event, VECTOR_GYROSCOPE, BNO_GyrVector);

			sprintf(message, "%8.2f", event.orientation.x);
			lcd_print_message(message);
			delay();
			delay();

		
		  sprintf(message, "%i", (int)I2Cmessage);
		  //lcd_print_message(message); 
			delay();*/
  }  
}
