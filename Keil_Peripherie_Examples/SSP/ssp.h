/*****************************************************************************
 *   ssp.h:  Header file for NXP LPC23xx/24xx Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.07.19  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#ifndef __SSP_H__
#define __SSP_H__

#define DMA_ENABLED		0

/* if USE_CS is zero, set SSEL as GPIO that you have total control of the sequence */
/* When in loopback mode, set USE_CS to 1. 
   When test serial SEEPROM, set USE_CS to 0. */
#define USE_CS			1
#define LOOPBACK_MODE	1		/* When 0, test Serial EEPROM */

/* SPI read and write buffer size */
#if LOOPBACK_MODE
#define SSP_BUFSIZE		256
#else
#define SSP_BUFSIZE		16
#endif
#define FIFOSIZE		8

#define DELAY_COUNT		10
#define MAX_TIMEOUT		0xFF

/* SSP select pin */
#define SSP0_SEL		(1 <<16)
	
/* SSP Status register */
#define SSPSR_TFE		(1 << 0)
#define SSPSR_TNF		(1 << 1) 
#define SSPSR_RNE		(1 << 2)
#define SSPSR_RFF		(1 << 3) 
#define SSPSR_BSY		(1 << 4)

/* SSP0 CR0 register */
#define SSPCR0_DSS		(1 << 0)
#define SSPCR0_FRF		(1 << 4)
#define SSPCR0_SPO		(1 << 6)
#define SSPCR0_SPH		(1 << 7)
#define SSPCR0_SCR		(1 << 8)

/* SSP0 CR1 register */
#define SSPCR1_LBM		(1 << 0)
#define SSPCR1_SSE		(1 << 1)
#define SSPCR1_MS		(1 << 2)
#define SSPCR1_SOD		(1 << 3)

/* SSP0 Interrupt Mask Set/Clear register */
#define SSPIMSC_RORIM	(1 << 0)
#define SSPIMSC_RTIM	(1 << 1)
#define SSPIMSC_RXIM	(1 << 2)
#define SSPIMSC_TXIM	(1 << 3)

/* SSP0 Interrupt Status register */
#define SSPRIS_RORRIS	(1 << 0)
#define SSPRIS_RTRIS	(1 << 1)
#define SSPRIS_RXRIS	(1 << 2)
#define SSPRIS_TXRIS	(1 << 3)

/* SSP0 Masked Interrupt register */
#define SSPMIS_RORMIS	(1 << 0)
#define SSPMIS_RTMIS	(1 << 1)
#define SSPMIS_RXMIS	(1 << 2)
#define SSPMIS_TXMIS	(1 << 3)

/* SSP0 Interrupt clear register */
#define SSPICR_RORIC	(1 << 0)
#define SSPICR_RTIC		(1 << 1)

/* ATMEL SEEPROM command set */
#define WREN		0x06		/* MSB A8 is set to 0, simplifying test */
#define WRDI		0x04
#define RDSR		0x05
#define WRSR		0x01
#define READ		0x03
#define WRITE		0x02

/* RDSR status bit definition */
#define RDSR_RDY	0x01
#define RDSR_WEN	0x02

/* If RX_INTERRUPT is enabled, the SSP RX will be handled in the ISR
SSP0Receive() will not be needed. */
extern void SSSP0Handler (void) __irq;
extern DWORD SSP0Init( void );
extern void SSP0Send( BYTE *Buf, DWORD Length );
extern void SSP0Receive( BYTE *buf, DWORD Length );

#endif  /* __SSP_H__ */
/*****************************************************************************
**                            End Of File
******************************************************************************/

