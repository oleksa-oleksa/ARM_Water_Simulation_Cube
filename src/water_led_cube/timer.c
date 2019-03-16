/*****************************************************************************
 *   timer.c:  Timer C file for NXP LPC23xx/24xx Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.09.01  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#include "LPC23xx.h"		/* LPC23xx/24xx Peripheral Registers	*/
#include "irq.h"
#include "timer.h"

timer_isr_t _timer_0_isr;
timer_isr_t _timer_1_isr;

extern void Timer0FIQHandler(void);
extern void Timer1FIQHandler(void);

#if !FIQ
/******************************************************************************
** Function name:		Timer0Handler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**						executes each 10ms @ 60 MHz CPU Clock
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void Timer0Handler (void) __irq 
{  
  T0IR = 1;			/* clear interrupt flag */
  IENABLE;			/* handles nested interrupt */

  _timer_0_isr();

  IDISABLE;
  VICVectAddr = 0;	/* Acknowledge Interrupt */
}

#else
/******************************************************************************
** Function name:		Timer0FIQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**						executes each 10ms @ 60 MHz CPU Clock
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void Timer0FIQHandler(void) 
{  
  T0IR = 1;			/* clear interrupt flag */
  timer0_counter++;
//  VICVectAddr = 0;	/* Acknowledge Interrupt */
}
#endif

#if !FIQ
/******************************************************************************
** Function name:		Timer1Handler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**						executes each 10ms @ 60 MHz CPU Clock
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void Timer1Handler (void) __irq 
{  
  T1IR = 1;			/* clear interrupt flag */
  IENABLE;			/* handles nested interrupt */

  _timer_1_isr();

  IDISABLE;
  VICVectAddr = 0;	/* Acknowledge Interrupt */
}

#else
/******************************************************************************
** Function name:		Timer1Handler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**						executes each 10ms @ 60 MHz CPU Clock
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void Timer1FIQHandler (void)
{  
  T1IR = 1;			/* clear interrupt flag */
  timer1_counter++;
//  VICVectAddr = 0;	/* Acknowledge Interrupt */
}
#endif



/******************************************************************************
** Function name:		init_timer
**
** Descriptions:		Initialize timer, set timer interval, reset timer,
**						install timer interrupt handler
**
** parameters:			timer number and timer interval
** Returned value:		true or false, if the interrupt handler can't be
**						installed, return false.
** 
******************************************************************************/

#define MR0_VALUE TIME_INTERVAL / 2 ///< 250us

bool init_timer (uint8_t timer_num, timer_isr_t timer_isr) 
{
    if(timer_num == 0)
    {
        _timer_0_isr = timer_isr;
        #if FIQ
        /* FIQ is always installed. */
        VICIntSelect |= (0x1<<4);
        VICIntEnable = (0x1<<4);
        return (TRUE);
        #else
        if(install_irq( TIMER0_INT, (void*)Timer0Handler, HIGHEST_PRIORITY ) == 0 )
        {
            return true;
        }  
        else
        {
            T0TCR = 0x02;		/* reset timer */
            T0PR  = 0x00;		/* set prescaler to zero */
            T0IR  = 0xff;		/* reset all interrrupts */
            T0MR0 = MR0_VALUE;
            T0MCR = 3;		    /* Interrupt and Reset on MR0 */
            T0TCR = 0x01;		/* start timer */
            return false;
        }
        #endif
    }
    else if(timer_num == 1)
    {
        _timer_1_isr = timer_isr;
        #if FIQ
        VICIntSelect |= (0x1<<5);
        VICIntEnable = (0x1<<5);
        return (TRUE);
        #else
        if(install_irq( TIMER1_INT, (void *)Timer1Handler, HIGHEST_PRIORITY) == 0)
        {
            return false;
        }  
        else
        {
            T1TCR = 0x02;		/* reset timer */
            T1PR  = 0x00;		/* set prescaler to zero */
            T1IR  = 0xff;		/* reset all interrrupts */
            T1MR0 = MR0_VALUE;
            T1MCR = 3;		    /* Interrupt and Reset on MR0 */
            T1TCR = 0x01;		/* start timer */
            return true;
        }
    #endif
    }
    return false;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
