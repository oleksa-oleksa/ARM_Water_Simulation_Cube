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

typedef enum
    {
      VECTOR_ACCELEROMETER = BNO055_ACCEL_DATA_X_LSB_ADDR,
      VECTOR_MAGNETOMETER  = BNO055_MAG_DATA_X_LSB_ADDR,
      VECTOR_GYROSCOPE     = BNO055_GYRO_DATA_X_LSB_ADDR,
      VECTOR_EULER         = BNO055_EULER_H_LSB_ADDR,
      VECTOR_LINEARACCEL   = BNO055_LINEAR_ACCEL_DATA_X_LSB_ADDR,
      VECTOR_GRAVITY       = BNO055_GRAVITY_DATA_X_LSB_ADDR
    } adafruit_vector_type_t;

		

/** Sensor types */
typedef enum
{
  SENSOR_TYPE_ACCELEROMETER         = (1),   /**< Gravity + linear acceleration */
  SENSOR_TYPE_MAGNETIC_FIELD        = (2),
  SENSOR_TYPE_ORIENTATION           = (3),
  SENSOR_TYPE_GYROSCOPE             = (4),
  SENSOR_TYPE_LIGHT                 = (5),
  SENSOR_TYPE_PRESSURE              = (6),
  SENSOR_TYPE_PROXIMITY             = (8),
  SENSOR_TYPE_GRAVITY               = (9),
  SENSOR_TYPE_LINEAR_ACCELERATION   = (10),  /**< Acceleration not including gravity */
  SENSOR_TYPE_ROTATION_VECTOR       = (11),
  SENSOR_TYPE_RELATIVE_HUMIDITY     = (12),
  SENSOR_TYPE_AMBIENT_TEMPERATURE   = (13),
  SENSOR_TYPE_VOLTAGE               = (15),
  SENSOR_TYPE_CURRENT               = (16),
  SENSOR_TYPE_COLOR                 = (17)
} sensors_type_t;

/** struct sensors_vec_s is used to return a vector in a common format. */
typedef struct {
    union {
        float v[3];
        struct {
            float x;
            float y;
            float z;
        };
        /* Orientation sensors */
        struct {
            float roll;    /**< Rotation around the longitudinal axis (the plane body, 'X axis'). Roll is positive and increasing when moving downward. -90°<=roll<=90° */
            float pitch;   /**< Rotation around the lateral axis (the wing span, 'Y axis'). Pitch is positive and increasing when moving upwards. -180°<=pitch<=180°) */
            float heading; /**< Angle between the longitudinal axis (the plane body) and magnetic north, measured clockwise when viewing from the top of the device. 0-359° */
        };
    };
    int8_t status;
    uint8_t reserved[3];
} sensors_vec_t;

/** struct sensors_color_s is used to return color data in a common format. */
typedef struct {
    union {
        float c[3];
        /* RGB color space */
        struct {
            float r;       /**< Red component */
            float g;       /**< Green component */
            float b;       /**< Blue component */
        };
    };
    uint32_t rgba;         /**< 24-bit RGBA value */
} sensors_color_t;

		
/* Sensor event (36 bytes) */
/** struct sensor_event_s is used to provide a single sensor event in a common format. */
typedef struct
{
    int32_t version;                          /**< must be sizeof(struct sensors_event_t) */
    int32_t sensor_id;                        /**< unique sensor identifier */
    int32_t type;                             /**< sensor type */
    int32_t reserved0;                        /**< reserved */
    int32_t timestamp;                        /**< time is in milliseconds */
    union
    {
        float           data[4];
        sensors_vec_t   acceleration;         /**< acceleration values are in meter per second per second (m/s^2) */
        sensors_vec_t   magnetic;             /**< magnetic vector values are in micro-Tesla (uT) */
        sensors_vec_t   orientation;          /**< orientation values are in degrees */
        sensors_vec_t   gyro;                 /**< gyroscope values are in rad/s */
        float           temperature;          /**< temperature is in degrees centigrade (Celsius) */
        float           distance;             /**< distance in centimeters */
        float           light;                /**< light in SI lux units */
        float           pressure;             /**< pressure in hectopascal (hPa) */
        float           relative_humidity;    /**< relative humidity in percent */
        float           current;              /**< current in milliamps (mA) */
        float           voltage;              /**< voltage in volts (V) */
        sensors_color_t color;                /**< color in RGB component values */
    };
} sensors_event_t;

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
			I21CONCLR = 0x20;    // Clear start bit       
		  I21DAT = GlobalI2CAddr; // Send address and write bit 
		break;  

		case (0x10): // A repeated START condition has been transmitted. Next: 0x18
			I21CONCLR = 0x20;    // Clear start bit       
		  I21DAT = GlobalI2CAddr; // Send address and write bit
			break;
		
		case (0x18): // SLA+W has been transmitted; ACK has been received. Next: 0x28       
			I21DAT = GlobalI2CReg; // Write data to TX register    
		break;      
		
		case (0x20): // SLA+W has been transmitted; NOT ACK has been received      
			GlobalI2CState = I2C_ERR;    
		break;      
		
		case (0x28): // Data byte in I2DAT has been transmitted; ACK has been received.
      switch(GlobalI2CState) {
				case I2C_REG:
					I21DAT = GlobalI2CReg;
				  GlobalI2CState = I2C_DAT;
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
		
		default   :      
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
	
	I21DAT = 0x00;

	I21CONCLR = 0x000000FF; // Clear all I2C settings    
	I21CONSET = 0x00000040; // Enable the I2C interface    
	I21CONSET = 0x00000020; // Start condition
	
	
	while((GlobalI2CState != I2C_ERR) && (GlobalI2CState != I2C_DONE)) {
	;
	}
}
/**************************************************************************/
/*!
    @brief  I2C Write Data Transfer
		Once the I2C peripheral is initialised in master mode we can start a write data transfer
*/
/**************************************************************************/
void I2CTransferByteWrite(unsigned char addr, unsigned char data) {    

	I21DAT = data;    
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
void I2CTransferByteRead(void) {    
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
	
	VICVectCntl19 = 0x0000001;       // select a priority slot for a given interrupt
	VICVectAddr19 = (unsigned)i2c_irq; //pass the address of the IRQ into the VIC slot 
	VICIntEnable = 0x00080000; // enable interrupt 	
	PINSEL1 |= 0x000003C0; //Switch GPIO to I2C pins
	I21SCLH = 0xF;  //Set bit rate to 400KHz (max BNO frequency from datasheet)
									// Fcco = 2 * M * Fin / N = 2 * (MSEL +1 ) * 12 MHz / 1 = 2 * 12 * 12 * 10^6 = 288 * 10^6
									// Devider CCLKCFG_Val = 0x00000005 ==> 288 /6 = 48 * 10^6
									// CCLKCFG_Val =  0x00000000 ===> 48 / 4 = 12 * 10^6 == Input Frequency
									// BNO055 max frequency 400 KHz = 4 * 10^5
									// SCLH + SCLL = 30
	I21SCLL  = 0xF;
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
	I2CWriteReg(0x50, 0x07, 0x00);
		//for(;;){;}
	I2CTransferByteWrite(I2CAddress, BNO055_CHIP_ID_ADDR);
  I2CTransferByteRead(I2CAddress);
	id = I2Cmessage;
	
	/*
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
	*/
	
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
	BNOMode = requestedMode;
  delay();
	
	return 1;
}

/**************************************************************************/
/*!
    @brief  Use the external 32.768KHz crystal
*/
/**************************************************************************/
void setExtCrystalUse(int usextal)
{
	unsigned char swpMode = BNOMode;

  /* Switch to config mode (just in case since this is the default) */
  setBNOMode(BNO055_OPERATION_MODE_CONFIG);
  delay();
  
	I2CTransferByteWrite(I2CAddress, BNO055_PAGE_ID_ADDR);
  I2CTransferByteWrite(I2CAddress, 0);
	
  if (usextal) {
		I2CTransferByteWrite(I2CAddress, BNO055_SYS_TRIGGER_ADDR);
		I2CTransferByteWrite(I2CAddress, 0x80);
  } else {
				I2CTransferByteWrite(I2CAddress, BNO055_SYS_TRIGGER_ADDR);
				I2CTransferByteWrite(I2CAddress, 0x00);
  }
  delay();
  setBNOMode(swpMode);
	BNOMode = swpMode;
  delay();
}

/**************************************************************************/
/*!
    @brief  Reads the specified number of bytes over I2C
*/
/**************************************************************************/
int readLen(unsigned char Addr, unsigned char reg, unsigned char * buffer, uint8_t len)
{
	int i;
	I2CAddress = Addr;
	I21DAT = I2Cmessage; // global variable    
	I21CONCLR = 0x000000FF; // Clear all I2C settings    
	I21CONSET = 0x00000040; // Enable the I2C interface    
	I21CONSET = 0x00000020; // Start condition }
	
  I2CTransferByteWrite(I2CAddress, reg);
 
  for (i = 0; i < len; i++)
  {
    I2CTransferByteRead(I2CAddress);
  }

  return 1;
}

/**************************************************************************/
/*!
    @brief  Gets a vector reading from the specified source
*/
/**************************************************************************/
void getBNOVector(unsigned char vector_type, unsigned char* xyz)
{
  int16_t x, y, z;
  uint8_t buffer[6];
  memset (buffer, 0, 6);

  x = y = z = 0;

  /* Read vector data (6 bytes) */
  readLen(I2CAddress, vector_type, buffer, 6);

  x = ((int16_t)buffer[0]) | (((int16_t)buffer[1]) << 8);
  y = ((int16_t)buffer[2]) | (((int16_t)buffer[3]) << 8);
  z = ((int16_t)buffer[4]) | (((int16_t)buffer[5]) << 8);

  /* Convert the value to an appropriate range (section 3.6.4) */
  /* and assign the value to the Vector type */
  switch(vector_type)
  {
    case VECTOR_MAGNETOMETER:
      /* 1uT = 16 LSB */
      xyz[0] = ((double)x)/16.0;
      xyz[1] = ((double)y)/16.0;
      xyz[2] = ((double)z)/16.0;
      break;
    case VECTOR_GYROSCOPE:
      /* 1dps = 16 LSB */
      xyz[0] = ((double)x)/16.0;
      xyz[1] = ((double)y)/16.0;
      xyz[2] = ((double)z)/16.0;
      break;
    case VECTOR_EULER:
      /* 1 degree = 16 LSB */
      xyz[0] = ((double)x)/16.0;
      xyz[1] = ((double)y)/16.0;
      xyz[2] = ((double)z)/16.0;
      break;
    case VECTOR_ACCELEROMETER:
    case VECTOR_LINEARACCEL:
    case VECTOR_GRAVITY:
      /* 1m/s^2 = 100 LSB */
      xyz[0] = ((double)x)/100.0;
      xyz[1] = ((double)y)/100.0;
      xyz[2] = ((double)z)/100.0;
      break;
  }
}


/**************************************************************************/
/*!
    @brief  Reads the sensor and returns the data as a sensors_event_t
*/
/**************************************************************************/
void getBNOEvent(sensors_event_t *event, unsigned char vector_type, unsigned char* xyz)
{
  /* Clear the event */
  memset(event, 0, sizeof(sensors_event_t));

  event->version   = sizeof(sensors_event_t);
  event->sensor_id = BNO055_ID;
  event->type      = SENSOR_TYPE_ORIENTATION;
  event->timestamp = 0; // TODO Adjust with MCB2300 Timer (?)

  /* Get a Euler angle sample for orientation */
  getBNOVector(vector_type, xyz);
  event->orientation.x = xyz[0];
  event->orientation.y = xyz[1];
  event->orientation.z = xyz[2];

}

/*************************************************************/

int main (void) {
	
  unsigned char message[100];
	
	// UART Init
	init_serial();

	// LCD Init
  lcd_init();
	delay();
	lcd_print_greeting();
	delay();

	// I2C Init 
	i2c_init();
	delay();
	
	
	// BNO055 Adafruit Init
	if(!accelerometer_init(BNO055_OPERATION_MODE_ACCGYRO))
  {
    lcd_print_message("NO BNO055 found");
    delay();
  }
	
	delay();
	
	setExtCrystalUse(1);		
	
	while (1) {
			sensors_event_t event; 
		
			lcd_print_message("ACCGYRO started");
			/* Get a new sensor event */ 
			getBNOEvent(&event, VECTOR_ACCELEROMETER, BNO_AccVector);
			getBNOEvent(&event, VECTOR_GYROSCOPE, BNO_GyrVector);

			sprintf(message, "%8.2f", event.orientation.x);
			lcd_print_message(message);
			delay();
			delay();

		
		  sprintf(message, "%i", (int)I2Cmessage);
		  //lcd_print_message(message); 
			delay();
  }  
}
