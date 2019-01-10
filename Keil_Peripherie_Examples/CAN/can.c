/*****************************************************************************
 *  can.c:  CAN module API file for NXP LPC23xx/24xx Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.09.13  ver 1.00    Prelimnary version, first Release
 *
*****************************************************************************/
#include "LPC23xx.h"					/* LPC23xx definitions */
#include "type.h"
#include "irq.h"
#include "can.h"

// Receive Queue: one queue for each CAN port
extern CAN_MSG MsgBuf_RX1, MsgBuf_RX2;
extern volatile DWORD CAN1RxDone, CAN2RxDone;

DWORD CANStatus;
DWORD CAN1RxCount = 0, CAN2RxCount = 0;
DWORD CAN1ErrCount = 0, CAN2ErrCount = 0;

/******************************************************************************
** Function name:		CAN_ISR_Rx1
**
** Descriptions:		CAN Rx1 interrupt handler
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void CAN_ISR_Rx1( void )
{
  DWORD * pDest;

  // initialize destination pointer
  pDest = (DWORD *)&MsgBuf_RX1;
  *pDest = CAN1RFS;  // Frame

  pDest++;
  *pDest = CAN1RID; // ID		//change by gongjun

  pDest++;
  *pDest = CAN1RDA; // Data A

  pDest++;
  *pDest = CAN1RDB; // Data B
	
  CAN1RxDone = TRUE;
  CAN1CMR = 0x04; // release receive buffer
  return;
}

/******************************************************************************
** Function name:		CAN_ISR_Rx2
**
** Descriptions:		CAN Rx2 interrupt handler
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void CAN_ISR_Rx2( void )
{
  DWORD *pDest;

  // initialize destination pointer
  pDest = (DWORD *)&MsgBuf_RX2;
  *pDest = CAN2RFS;  // Frame

  pDest++;
  *pDest = CAN2RID; // ID

  pDest++;
  *pDest = CAN2RDA; // Data A

  pDest++;
  *pDest = CAN2RDB; // Data B

  CAN2RxDone = TRUE;
  CAN2CMR = 0x04; // release receive buffer
  return;
}

/*****************************************************************************
** Function name:		CAN_Handler
**
** Descriptions:		CAN interrupt handler
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void CAN_Handler(void) __irq 
{		
  IENABLE;			/* handles nested interrupt */

  CANStatus = CAN_RX_SR;
  if ( CANStatus & (1 << 8) )
  {
	CAN1RxCount++;
	CAN_ISR_Rx1();
  }
  if ( CANStatus & (1 << 9) )
  {
	CAN2RxCount++;
	CAN_ISR_Rx2();
  }
  if ( CAN1GSR & (1 << 6 ) )
  {
	/* The error count includes both TX and RX */
	CAN1ErrCount = (CAN1GSR >> 16 );
  }
  if ( CAN2GSR & (1 << 6 ) )
  {
	/* The error count includes both TX and RX */
	CAN2ErrCount = (CAN2GSR >> 16 );
  }
  IDISABLE;
  VICVectAddr = 0;		/* Acknowledge Interrupt */
  return;
}

/******************************************************************************
** Function name:		CAN_Init
**
** Descriptions:		Initialize CAN, install CAN interrupt handler
**
** parameters:			bitrate
** Returned value:		true or false, false if initialization failed.
** 
******************************************************************************/
DWORD CAN_Init( DWORD can_btr )
{
  CAN1RxDone = CAN2RxDone = FALSE;

  PCONP |= (1 << 13) | (1 << 14);	// Enable clock to the peripheral

  PINSEL0	&= ~0x00000F0F;
  PINSEL0 |= 0x0000A05;	// port0.0~1, function 0x01, port0.4~5, function 0x10

  CAN1MOD = CAN2MOD = 1;	// Reset CAN
  CAN1IER = CAN2IER = 0;	// Disable Receive Interrupt
  CAN1GSR = CAN2GSR = 0;	// Reset error counter when CANxMOD is in reset

  CAN1BTR = CAN2BTR = can_btr;
  CAN1MOD = CAN2MOD = 0x0;	// CAN in normal operation mode

  // Install CAN interrupt handler
  install_irq( 23, (void *)CAN_Handler, HIGHEST_PRIORITY );
  CAN1IER = CAN2IER = 0x01;		// Enable receive interrupts
  return( TRUE );
}

/******************************************************************************
** Function name:		CAN_SetACCF_Lookup
**
** Descriptions:		Initialize CAN, install CAN interrupt handler
**
** parameters:			bitrate
** Returned value:		true or false, false if initialization failed.
** 
******************************************************************************/
void CAN_SetACCF_Lookup( void )
{
  DWORD address = 0;
  DWORD i;
  DWORD ID_high, ID_low;

  // Set explicit standard Frame  
  CAN_SFF_SA = address;
  for ( i = 0; i < ACCF_IDEN_NUM; i += 2 )
  {
	ID_low = (i << 29) | (EXP_STD_ID << 16);
	ID_high = ((i+1) << 13) | (EXP_STD_ID << 0);
	*((volatile DWORD *)(CAN_MEM_BASE + address)) = ID_low | ID_high;
	address += 4; 
  }
		
  // Set group standard Frame 
  CAN_SFF_GRP_SA = address;
  for ( i = 0; i < ACCF_IDEN_NUM; i += 2 )
  {
	ID_low = (i << 29) | (GRP_STD_ID << 16);
	ID_high = ((i+1) << 13) | (GRP_STD_ID << 0);
	*((volatile DWORD *)(CAN_MEM_BASE + address)) = ID_low | ID_high;
	address += 4; 
  }
 
  // Set explicit extended Frame 
  CAN_EFF_SA = address;
  for ( i = 0; i < ACCF_IDEN_NUM; i++  )
  {
	ID_low = (i << 29) | (EXP_EXT_ID << 0);
	*((volatile DWORD *)(CAN_MEM_BASE + address)) = ID_low;
	address += 4; 
  }

  // Set group extended Frame 
  CAN_EFF_GRP_SA = address;
  for ( i = 0; i < ACCF_IDEN_NUM; i++  )
  {
	ID_low = (i << 29) | (GRP_EXT_ID << 0);
	*((volatile DWORD *)(CAN_MEM_BASE + address)) = ID_low;
	address += 4; 
  }
   
  // Set End of Table 
  CAN_EOT = address;
  return;
}

/******************************************************************************
** Function name:		CAN_SetACCF
**
** Descriptions:		Set acceptance filter and SRAM associated with	
**
** parameters:			ACMF mode
** Returned value:		None
**
** 
******************************************************************************/
void CAN_SetACCF( DWORD ACCFMode )
{
  switch ( ACCFMode )
  {
	case ACCF_OFF:
	  CAN_AFMR = ACCFMode;
	  CAN1MOD = CAN2MOD = 1;	// Reset CAN
	  CAN1IER = CAN2IER = 0;	// Disable Receive Interrupt
	  CAN1GSR = CAN2GSR = 0;	// Reset error counter when CANxMOD is in reset
	break;

	case ACCF_BYPASS:
	  CAN_AFMR = ACCFMode;
	break;

	case ACCF_ON:
	case ACCF_FULLCAN:
	  CAN_AFMR = ACCF_OFF;
	  CAN_SetACCF_Lookup();
	  CAN_AFMR = ACCFMode;
	break;

	default:
	break;
  }
  return;
}

/******************************************************************************
** Function name:		CAN1_SendMessage
**
** Descriptions:		Send message block to CAN1	
**
** parameters:			pointer to the CAN message
** Returned value:		true or false, if message buffer is available,
**						message can be sent successfully, return TRUE,
**						otherwise, return FALSE.
** 
******************************************************************************/
DWORD CAN1_SendMessage( CAN_MSG *pTxBuf )
{
  DWORD CANStatus;

  CANStatus = CAN1SR;
  if ( CANStatus & 0x00000004 )
  {
	CAN1TFI1 = pTxBuf->Frame & 0xC00F0000;
	CAN1TID1 = pTxBuf->MsgID;
	CAN1TDA1 = pTxBuf->DataA;
	CAN1TDB1 = pTxBuf->DataB;
	CAN1CMR = 0x21;
	return ( TRUE );
  }
  else if ( CANStatus & 0x00000400 )
  {
	CAN1TFI2 = pTxBuf->Frame & 0xC00F0000;
	CAN1TID2 = pTxBuf->MsgID;
	CAN1TDA2 = pTxBuf->DataA;
	CAN1TDB2 = pTxBuf->DataB;
	CAN1CMR = 0x41;
	return ( TRUE );
  }
  else if ( CANStatus & 0x00040000 )
  {	
	CAN1TFI3 = pTxBuf->Frame & 0xC00F0000;
	CAN1TID3 = pTxBuf->MsgID;
	CAN1TDA3 = pTxBuf->DataA;
	CAN1TDB3 = pTxBuf->DataB;
	CAN1CMR = 0x81;
	return ( TRUE );
  }
  return ( FALSE );
}

/******************************************************************************
**                            End Of File
******************************************************************************/
