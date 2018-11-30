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
#include "include/bno055.h"
//#include "include/Adafruit_Sensor.h"

/* Global variables */
unsigned char I2Cdata; // data to send into I2C
unsigned char I2Cmessage; // received data fron I2C
unsigned int I2CAddress = 0x50; //01010000
unsigned char BNOMode;
unsigned char *BNO_AccVector;
unsigned char *BNO_GyrVector;

unsigned char GlobalI2CAddr;
unsigned char GlobalI2CReg;
unsigned char GlobalI2CData;
unsigned char GlobalI2CRead;
volatile enum {I2C_REG, I2C_DAT, I2C_ERR, I2C_DONE} GlobalI2CState;

/* Import external functions from Serial.c file                               */
extern       void init_serial    (void);

/**************************************************************************/
/*!
    @brief  delay
*/
/**************************************************************************/
void delay(void) {
	int i;
	for (i = 0; i < 2000008; i++) {
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
	delay();

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

	switch (I21STAT) // Read result code and switch to next action 
		{    
		case ( 0x08): // A START condition has been transmitted. Next: 0x18    
      I21DAT = GlobalI2CAddr; // Send address and write bit
			I21CONCLR = 0x28;    // Clear start bit       
		break;  

		case (0x10): // A repeated START condition has been transmitted. Next: 0x18
			I21DAT = GlobalI2CAddr; // Send address and write bit
			I21CONCLR = 0x28;    // Clear start bit       
			break;
		
		case (0x18): // SLA+W has been transmitted; ACK has been received. Next: 0x28 
			I21DAT = GlobalI2CReg; // Write data to TX register 
      I21CONCLR = 0x08;		
		break;      
		
		case (0x20): // SLA+W has been transmitted; NOT ACK has been received      
			GlobalI2CState = I2C_ERR;    
		break;      
		
		case (0x28): // Data byte in I2DAT has been transmitted; ACK has been received.
      switch(GlobalI2CState) {
				case I2C_REG:
					I21DAT = GlobalI2CReg;
				  GlobalI2CState = I2C_DAT;
				  //I21CONSET = 0x04; // AA
					break;
				case I2C_DAT:
					I21CONSET = 0x10; // Stop condition
				  GlobalI2CState = I2C_DONE;
					break;
				default:
					GlobalI2CState = I2C_ERR;
					break;
			}				
		break; 
		
		case (0x30): // Data sent, Not Ack 
			GlobalI2CState = I2C_ERR;          
		break;

    case (0x38): // Arbitration lost 
			GlobalI2CState = I2C_ERR;          
		break;		

		case (0x40) : // SLA+R has been transmitted; ACK has been received. Next: 0x50   
			I21CONSET = 0x04; // Enable ACK for data byte 
		break; 
	
		case (0x48) : // Slave Address +R, Not Ack    
			I21CONSET = 0x20; // Resend Start condition 
		break; 
	
		case (0x50) : // Data Received, ACK     
			I2Cmessage = I21DAT;    
			I21CONSET = 0x10; // Stop condition
			switch(GlobalI2CState) {
				case I2C_DAT:
					I21CONSET = 0x10; // Stop condition
				  GlobalI2CState = I2C_DONE;
				  I2Cmessage = I21DAT; // saving received data to a global variable
					break;
				default:
					GlobalI2CState = I2C_ERR;
					break;
			}    
		 break; 
	
	   case (0x58): // Data Received, Not Ack    
			 I21CONSET = 0x20; // Resend Start condition 
	   break;		
		
		default:      
			break;      
		}      
		
		I21CONCLR = 0x08;   // Clear I2C interrupt flag    
		VICVectAddr = 0x00000000; // Clear interrupt in 
}

void I2CWriteReg(unsigned char addr, unsigned char reg, unsigned char data) {
  GlobalI2CAddr = addr;
	GlobalI2CReg = reg;
	GlobalI2CData = data;
	GlobalI2CRead = 0;
	GlobalI2CState = I2C_REG;
	
	//I21DAT = 0x00;
   
	I21CONSET = 0x20; // Start condition
	

	while((GlobalI2CState != I2C_ERR) && (GlobalI2CState != I2C_DONE)) {
	;
	}
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
	I21SCLH = 0xF;  //Set bit rate to 400KHz (max BNO frequency from datasheet)
									// Fcco = 2 * M * Fin / N = 2 * (MSEL +1 ) * 12 MHz / 1 = 2 * 12 * 12 * 10^6 = 288 * 10^6
									// Devider CCLKCFG_Val = 0x00000005 ==> 288 /6 = 48 * 10^6
									// CCLKCFG_Val =  0x00000000 ===> 48 / 4 = 12 * 10^6 == Input Frequency
									// BNO055 max frequency 400 KHz = 4 * 10^5
									// SCLH + SCLL = 30
	I21SCLL  = 0xF;
	
	I21CONCLR = 0x000000FF; // Clear all I2C settings
  delay();	
	I21CONSET = 0x00000040; // Enable the I2C interface 
}

/**************************************************************************/
/*!
    @brief  Adafruit BNO055 Init
    Note: I2C should be already enabled
*/
/**************************************************************************/
/*int accelerometer_init(unsigned char requestedMode) {
	uint8_t id = 0; 
	 // Make sure we have the right device
	I2CWriteReg(0x50, 0x07, 0x00);
	delay();
  I2CTransferByteRead();
	id = I2Cmessage;
	
  if (id != BNO055_ID)
  {
    delay();
			
		// repeat 
    I2CTransferByteRead();
		id = I2Cmessage;
   	
		if(id != BNO055_ID) {
      return 0;  // still not? ok bail
    }
  }
	
  // Switch to config mode (just in case since this is the default)
  setBNOMode(BNO055_OPERATION_MODE_CONFIG);
	
	// Reset
  I2CTransferByteWrite(I2CAddress, BNO055_SYS_TRIGGER_ADDR);
	I2CTransferByteWrite(I2CAddress, 0x20);

	do {
		
		I2CTransferByteRead();
		id = I2Cmessage;
		delay();
		
	} while (id != BNO055_ID);
  
	delay();
		
  // Set to normal power mode
	I2CTransferByteWrite(I2CAddress, BNO055_PWR_MODE_ADDR);
  I2CTransferByteWrite(I2CAddress, BNO055_POWER_MODE_NORMAL);
 
	delay();
	
	I2CTransferByteWrite(I2CAddress, BNO055_PAGE_ID_ADDR);
  I2CTransferByteWrite(I2CAddress, 0);
	
	I2CTransferByteWrite(I2CAddress, BNO055_SYS_TRIGGER_ADDR);
  I2CTransferByteWrite(I2CAddress, 0x0);
	
  delay();
 
	 // Set the requested operating mode
  setBNOMode(requestedMode);
	BNOMode = requestedMode;
  delay();
	
	return 1;
}*/


/*************************************************************/

int main (void) {
	
  //unsigned char message[100];
	
	// UART Init
	//init_serial();

	// LCD Init
  //lcd_init();
	//delay();
	//lcd_print_greeting();
	//delay();

	// I2C Init 
	//enable_isr();
	i2c_init();
	delay();
	
	
	I2CWriteReg(0x50, 0x3e, 0x00);
	// BNO055 Adafruit Init
	//if(!accelerometer_init(BNO055_OPERATION_MODE_ACCGYRO))
  //{
    //lcd_print_message("NO BNO055 found");
  //  delay();
  //}
	
	//delay();
	
	//setExtCrystalUse(1);		
	
	while (1) {
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
