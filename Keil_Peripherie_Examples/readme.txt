/*****************************************************************************
 *   readme.txt:  Description of the LPC23xx/24xx Example Software Package
 *
 *   Copyright(C) 2009, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.09.01  ver 1.00    Preliminary version, first Release
 *   2006.11.30  ver 1.10    Release 1. In this release, the major change
 *                           including:
 *                           (1) "lpc230x.h" has been changed to "lpc23xx.h"
 *                           to match the description. 
 *                           (2) Tested on the latest Keil IDE 3.03a which has 
 *                           the LPC236x device in the database, the beta release 
 *                           used "LPC214x" for debugger and flash utility.
 *                           (3) Added I2S example.
 *                           (4) Hardware chage in Enable Control in port LCD.
 *                           (5) Fixed USB clock enable bit hangs caused by the 
 *                           bootloader change.
 *   2007.01.26  ver 1.20    (1) Added LPC24xx related External Memory Controller
 *                           (EMC) support such as External SDRAM, External NOR
 *                           Flash, external NAND flash for Embedded Artists's
 *                           LPC2400 OEM Base Board support, external QVGA
 *                           LCD controller. The original LCD driver is from
 *                           Embedded Artists                  
 *                           (2) Added UART1 example software,
 *                           (3) Power management for GPIO wakeup
 *                           (4) Modified LPC23xx.h file, Clarified name definition for 
 *                           USB Device configuration on both LPC23xx and LPC24xx such 
 *                           as USBClkCtrl, USBClkSt, and USBPortSel registers.
 *                           (5) Added USB2 configuration as USB device in HID example 
 *                           for Embedded Artists OEM board. Windows will recognize 
 *                           Embedded Artists's board as a USB device during enumeration.
 *                           However, Keil's USB Utility may not work as the LEDs don't
 *                           match between the Keil's MCB2300 board and Embedded Artists
 *                           LPC24xx OEM board.
 *                           (6) Added hex file of the release build for each subdirectory
 *                           that you can use FlashMagic Flash Utility to update to the 
 *                           internal flash.
 *   2007.02.27  ver 1.30    (1) Fixed a SDRAM initialization issue that the SDRAM test
 *                           will work when code is running from debugger but fails in
 *                           stand-alone. This is a timing issue in the SDRAM initialization.
 *                           Added SDRAM stand-alone debugging with UART and PC terminal
 *                           (2) Fixed a SD/MMC test software bug in mcitest.c. No change
 *                           in the driver, but error in the test comparison module.
 *                           (3) Fixed a EA_LCD driver initialization issue for the Embedded 
 *                           Artists' LCD module.
 *                           (4) Added UART1 driver for Embedded Artists' board.
 *                           (5) Added board selection for Keil's MCB2300 and Embedded Artists'
 *                           LPC24xx OEM board definition in the tartget.h. The default is
 *                           Keil's MCB2300. The modules under EA_LCD and EXTMEM directories
 *                           are for Embedded Artists' board only.
 *   2007.07.20  ver 1.40    (1) Added EMAC ID check for compatibility between rev "-" and "A".
 *                           (2) Added I2S DMA access example.
 *                           (3) Added USB Host initialization example.
 *                           (4) Modified USB HID example on rev. "A" and verified USB related
 *                           bugs from rev. "-".
 *                           (5) Modified timer initialization routine to deal with both timer 0
 *                           and timer 1.   
 *                           (6) Fixed some header file error, FIO1PIN1.
 *                           (7) Added Micrel PHY example to support PHY used on IAR KickStart Kit.
 *                           (8) Added SDRAM example for both Embedded Artists LPC2400 and Internal 
 *                           Engineering board.
 *                           (9) Optimization on the MCI data transfer.
 *   2007.09.07  ver 1.50    (1) Due to the MAM fix and User Manual Update, CCO divider with
 *                           the odd number for CCLK is no longer allowed. Update PLL setting
 *                           example accordingly.
 *                           (2) LPC23xx.h Header file update to be consistent with User Manual.  
 *                           (3) Due to the clock change in the library, from 57.6Mhz to 48Mhz,
 *                           CAN bit timing needs to be changed as well. All the timing related
 *                           modules including timer, UART, SDRAM, SSP, SPI are retested.
 *                           (4) Included latest Keil USB Video Class Driver example. The example
 *                           has passed USBCV UVC compliance test.
 *   2009.03.10  ver 1.60    (1) Added Keil USBCDC class driver example.
 *                           (2) Added USBMem example with USB DMA Support.
 *                           (3) Added FIQ support for timer example.
 *                           (4) Fixed an I2C driver issue that it can read only one byte after 
 *                           repeated start followed by a SA with RD(bit 0) set.
 *                           (5) In addition to loopback example, added serial EEPROM support 
 *                           on SSP driver.
 *                           (6) Fixed errors in header file definition in FIO, External memory 
 *                           controller modules. 
 *                                                        
 *		  
 *                           
 *
******************************************************************************/

/*****************************************************************************/
Software that is described herein is for illustrative purposes only which 
provides customers with programming information regarding the products.
This software is supplied "AS IS" without any warranties. NXP Semiconductors 
assumes no responsibility or liability for the use of the software, conveys no 
license or title under any patent, copyright, or mask work right to the 
product. NXP Semiconductors reserves the right to make changes in the 
software without notification. NXP Semiconductors also make no representation 
or warranty that such application will be suitable for the specified use without 
further testing or modification.
/*****************************************************************************/


The Description of the Example software
===================

This example demonstrates the use of build-in peripherals on the NXP
LPC23xx/LPC24xx family microcontrollers.

The Example software includes, common library, peripheral APIs, and test modules
for the APIs. The common library include startup file, standard definition and
header files, processor specific setup module, generic interrupt related APIs, 
timer routine. The peripheral directories include, ADC, DAC, DMA, GPIO, PWM, 
Real-time clock, timer, SPI, SSP, MCI, I2C, Watchdog timer, UART, external 
interrupt, CAN, Ethernet, USB Audio, USB HID, external memory controller, etc.
  
The development environment is Keil's, an ARM company now, uVision 3.x
and ULINK ICE. The target boards are Keil's MCB2300 with a NXP's LPC23xx MCU 
on it and Embedded Artists's LPC2400 OEM base board.

The project is created for both target option "MCB2300" and "MCB2300 Debug". 
For "MCB2300", the image code can be programmed into the flash and executed;
for "MCB2300 Debug", the image code can be loaded into internal SRAM(IRAM)
for easy debugging. The "RAM.ini" in each directory is a Keil uVision debugger
script file, once the debugger starts, the code will be loaded to the IRAM
first, then, "RAM.ini" will be executed and reset program counter(PC) to 
0x40000000(IRAM starting address) that code will be executed from.

Some external components, such as I2C temperature, SPI serial EEPROM, will be 
required to add on the board in order to complete the test below. For Embedded
Artists's LPC2400 OEM base board, external SDRAM from Samsung's K4S561632,
external NAND flash from Samsung's K9F1G08 and external NOR flash from ST's 
SST39LF160 are used for EMC testing. Some QVGA LCD software example from 
Embedded Artists is also included.

By default, the test program is for Keil MCB2300 board. Be aware that, in order to 
test Embedded Artists' LPC2400 board, such as EA_LCD, EXTMEM including SDRAM, ext. 
flash, ext. NAND flash, in target.h, the board definition needs to be changed.

The directory tree and content of the sample software
===================

common
    -- inc
	-- lpc23xx.h		Definition file of all the registers of LPC23xx
				The LPC24xx register definition is also included. 
	-- irq.h		Interrupt related definitions and API prototyping
	-- type.h		Type definition
	-- target.h		Target specific definition for Keil's MCB2300 board
	-- timer.h		Definition and header file for timer module
    -- src
	-- startup.s		Code start up entry
	-- swi_handler.s	SWI handler
				Note: not used, but a good example for future references. 
	-- irq.c		Generic interrupt handler related APIs, handle both IRQ and FIQ. 
	-- target.c		Target specific modules
	-- timer.c		Timer related APIs

GPIO
    -- fio.h			GPIO and fast I/O header
    -- fio.c			GPIO and fast I/O APIs
    -- fiotest.c		GPIO and Fast I/O test module
    -- fio.uv2			uVision project file


EXTINT
    -- extint.h			External interrupt header
    -- extint.c			External interrupt APIs
    -- einttest.c		External interrupt test module
    -- extint.uv2		uVision project file

Timer
    -- tmrtest.c		Timer test module, note: API modules
				are in the COMMON directory and 
				shared and used by some other peripheral testing.
    -- timer.uv2		uVision project file

PWM
    -- pwmc.h			PWM header
    -- pwm.c			PWM APIs
    -- pwmtest.c		PWM test module
    -- pwm.uv2			uVision project file

SPI
    -- spi.h			SPI header
    -- spi.c			SPI APIs
    -- spitest.c		SPI test module
    -- spi.uv2			uVision project file

SSP
    -- ssp.h			SSP(SPI1) header
    -- ssp.c			SSP(SPI1) APIs
    -- dma.h			SSP DMA header
    -- dma.c			SSP DMA APIs
    -- ssptest.c		SSP(SPI1) test module
    -- ssp.uv2			uVision project file

UART
    -- uart.h			UART header
    -- uart.c			UART APIs
    -- uarttest.c		UART test module
    -- uart.uv2			uVision project file

RTC
    -- rtc.h			Real-time clock(RTC) header
    -- rtc.c			Real-time clock(RTC) APIs
    -- rtctest.c		Real-time clock(RTC) test module
    -- rtc.uv2			uVision project file

MCI
    -- mci.h			MCI header
    -- mci.c			MCI APIs for both SD and MMC card interface
    -- dma.h			MCI DMA header
    -- dma.c			MCI DMA APIs.
    -- mcitest.c		MCI test module
    -- mci.uv2			uVision project file

ADC
    -- adc.h			ADC header
    -- adc.c			ADC APIs
    -- adctest.c		ADC controller test module
    -- adc.uv2			uVision project file

DAC
    -- dac.h			DAC header
    -- dac.c			DAC APIs
    -- dactest.c		DAC test module
    -- dac.uv2			uVision project file

I2C
    -- i2c.h			I2C header
    -- i2c.c			I2C APIs
    -- i2cmst.c			I2C test module
    -- i2cmst.uv2		uVision project file

I2S
    -- i2s.h			I2S header
    -- i2s.c			I2S APIs
    -- dma.c			I2S DMA APIs
    -- dma.h			I2S DMA header
    -- i2stest.c		I2S test module
    -- i2s.uv2			uVision project file

CAN
    -- can.h			CAN header
    -- can.c			CAN APIs
    -- cantest.c		CAN test module
    -- can.uv2			uVision project file

DMA
    -- dma.h			DMA header
    -- dma.c			GPDMA APIs, memory to memory only.
				Note: memory to peripheral or peripheral
				to memory are in SSP, MCI, and I2S directories.
    -- dmatest.c		DMA test module
    -- dma.uv2			uVision project file

WDT
    -- wdt.h			Watchdog timer header
    -- wdt.c			Watchdog timer APIs
    -- wdttest.c		Watchdog timer test module
    -- wdt.uv2			uVision project file

PWRMAN
    -- pwrman.h			Power management header
    -- pwrman.c			Power management APIs 
    -- pmtest.c			Power management test module,
				test powerdown and external interrupt wakeup.
    -- pwrman.uv2		uVision project file

PortLCD
    -- portlcd.h		4-bit port to LCD driver header for Keil MCB2300
    -- portlcd.c		4-bit port to LCD driver APIs 
    -- lcdtest.c		4-bit port to LCD test module
    -- portlcd.uv2		uVision project file

EMAC
    -- crc32.h			CRC calculation header
    -- crc32.c			CRC calculation APIs
    -- emac.h			EMAC header
    -- emac.c			EMAC APIs, options include transmit only(TX_ONLY);
				receive packet, switch source and destination
				and transmit(BOUNCE_RX), WOL, etc. 
    -- emactest.c		EMAC test module
    -- emac.uv2			uVision project file
    -- ethpkt.txt		Ethernet packet example for transmit

USBDEV
    -- hid.h			USB HID related header
    -- hiduser.h		USB HID user specific header
    -- hiduser.c		USB HID user APIs
    -- usb.h			USB header
    -- usbcfg.h			USB configuration header
    -- usbcore.h		USB Core header
    -- usbcore.c		USB Core APIs
    -- usbdesc.h		USB descriptor header
    -- usbdesc.c		USB descriptor APIs
    -- usbhw.h			USB hardware header
    -- usbhw.c			USB hardware APIs
    -- usbreg.h			USB misc. register header
    -- usbuser.h		USB user header
    -- useuser.c		USB user APIs
    -- usbmain.c		USB HID test module, power management, USB 
				suspend to powerdown, and resume to USB
				wakeup. 
    -- usbdev.uv2		uVision project file

USBAudio
    -- audio.h			USB Audio related header
    -- adcuser.h		USB Audio Device Class user specific header
    -- adcuser.c		USB Audio Device Class user APIs
    -- usb.h			USB header
    -- usbaudio.h		USB Audio misc. definition header
    -- usbcfg.h			USB configuration header
    -- usbcore.h		USB Core header
    -- usbcore.c		USB Core APIs
    -- usbdesc.h		USB descriptor header
    -- usbdesc.c		USB descriptor APIs
    -- usbhw.h			USB hardware header
    -- usbhw.c			USB hardware APIs
    -- usbreg.h			USB misc. register header
    -- usbuser.h		USB user header
    -- useuser.c		USB user APIs
    -- usbmain.c		USB Audio test module
    -- usbaudio.uv2		uVision project file

EXTMEM ( LPC24xx only, tested on Embedded Artists LPC24xx OEM board)
    -- NOR_Flash
	-- ex_norflash.h	NOR flash configuration header for Embedded Artists LPC2400 
				OEM board
	-- ex_norflash.c	NOR flash configuration and access APIs 
	-- norflash_test.c	NOR flash test module
	-- ex_norflash.uv2	uVision project file
    -- NAND_Flash
	-- ex_nandflash.h	NAND flash configuration header for Embedded Artists LPC2400 
				OEM board
	-- ex_nandflash.c	NAND flash configuration and access APIs 
	-- nandflash_test.c	NAND flash test module
	-- ex_nandflash.uv2	uVision project file	
    -- SDRAM
	-- ex_sdram.h		SDRAM configuration header for Embedded Artists LPC2400 
				OEM board
	-- ex_sdram.c		SDRAM configuration and access APIs
	-- uart.h		UART debugging header file for stand-alone SDRAM test
	-- uart.c		UART debugging file for stand-alone SDRAM test 
	-- sdram_test.c		SDRAM test module
	-- ex_sdram.uv2		uVision project file

EA_LCD (Embedded Artists QVGA LCD controller OEM board )
    -- lcd_hw.h			16-bit port to LCD controller driver header for Embedded Artists
				LPC2400 OEM base board
    -- lcd_hw.c			16-bit port to LCD controller driver APIs
    -- lcd_grph.c		Graphic related APIs such as drawing an object such as a line,
				a circle, etc.
    -- lcd_grph.h		Header file to support lcd_graph.c
    -- font5x7.c		a 5x7 font for letters and numbers, etc.
    -- font5x7.h		font related header
    -- font_macro.h		font macro
    -- ealcd_test.c		16-bit port to LCD controller test module
    -- ea_lcd.uv2		uVision project file

USBHost
    -- usbhost.h		USB host header
    -- usbhost.c		USB host APIs 
    -- hcdtest.c		USB host test module,
				USB host initialization only, no queue setup and enumeration yet.
    -- usbhost.uv2		uVision project file

USBUVC
    -- lpc2300.s		Startup file for USBUVC project.
				(This stand-alone project doesn't use any of the library files 
				under the common directory.)
    -- uvc.h			USB Video related header
    -- uvcuser.h		USB Video Device Class user specific header
    -- uvcuser.c		USB Video Device Class user APIs
    -- usb.h			USB header
    -- usbcfg.h			USB configuration header
    -- usbcore.h		USB Core header
    -- usbcore.c		USB Core APIs
    -- usbdesc.h		USB descriptor header
    -- usbdesc.c		USB descriptor APIs
    -- usbhw.h			USB hardware header
    -- usbhw.c			USB hardware APIs
    -- usbreg.h			USB misc. register header
    -- usbuser.h		USB user header
    -- usbuser.c		USB user APIs
    -- type.h			Local type definition header
    -- lcd.h			display support module header
    -- lcd.c			Display support module
    -- uvcdemo.c		USB UVC test module
    -- uvc.uv2			uVision project file
    -- debug.ini		Debug configuration setting

USBCDC
    -- lpc2300.s		Startup file for USBUVC project.
				(This stand-alone project doesn't use any of the library files 
				under the common directory.)
    -- swi.s			SWI handler to handle USB_WriteEP().    
    -- cdc.h			USB CDC related header
    -- cdcuser.h		USB CDC Device Class user specific header
    -- cdcuser.c		USB CDC Device Class user APIs
    -- usb.h			USB header
    -- usbcfg.h			USB configuration header
    -- usbcore.h		USB Core header
    -- usbcore.c		USB Core APIs
    -- usbdesc.h		USB descriptor header
    -- usbdesc.c		USB descriptor APIs
    -- usbhw.h			USB hardware header
    -- usbhw.c			USB hardware APIs
    -- usbreg.h			USB misc. register header
    -- usbuser.h		USB user header
    -- usbuser.c		USB user APIs
    -- type.h			Local type definition header
    -- lcd.h			display support module header
    -- lcd.c			Display support module
    -- vcomdemo.h		header for VCOM demo
    -- vcomdemo.c		USB CDC(Virtual COM) test module, main entry
    -- virtualCom.uv2		uVision project file
    -- serial.h			Header for UART port
    -- serial.c			UART module APIs
    -- lcd.h			Header for LCD display
    -- lcd.c			APIs for LCD display
    -- mcb2300-vcom.inf		The host side driver installation file
 
