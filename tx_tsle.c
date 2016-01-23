/**************************************************************************/ 
/*                                                                        */ 
/*            Copyright (c) 1996-2003 by Express Logic Inc.               */ 
/*                                                                        */ 
/*  This software is copyrighted by and is the sole property of Express   */ 
/*  Logic, Inc.  All rights, title, ownership, or other interests         */ 
/*  in the software remain the property of Express Logic, Inc.  This      */ 
/*  software may only be used in accordance with the corresponding        */ 
/*  license agreement.  Any unauthorized use, duplication, transmission,  */ 
/*  distribution, or disclosure of this software is expressly forbidden.  */ 
/*                                                                        */
/*  This Copyright notice may not be removed or modified without prior    */ 
/*  written consent of Express Logic, Inc.                                */ 
/*                                                                        */ 
/*  Express Logic, Inc. reserves the right to modify this software        */ 
/*  without notice.                                                       */ 
/*                                                                        */ 
/*  Express Logic, Inc.                     info@expresslogic.com         */
/*  11423 West Bernardo Court               http://www.expresslogic.com   */
/*  San Diego, CA  92127                                                  */
/*                                                                        */
/**************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */ 
/** ThreadX Component                                                     */ 
/**                                                                       */
/**   Thread Control (THR)                                                */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define TX_SOURCE_CODE


/* Include necessary system files.  */

#include    "tx_api.h"
#include    "tx_thr.h"
#include    "tx_tim.h"

/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _tx_thread_sleep                                    PORTABLE C      */ 
/*                                                           4.0b         */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function handles application thread sleep requests.  If the    */ 
/*    sleep request was called from a non-thread, an error is returned.   */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    timer_ticks                           Number of timer ticks to sleep*/ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    status                                Return completion status      */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    _tx_timer_activate                    Activate sleep timer          */ 
/*    _tx_thread_suspend                    Actual thread suspension      */ 
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    Application code                                                    */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */ 
/*                                                                        */ 
/*  12-31-1996     William E. Lamie         Initial Version 3.0           */ 
/*  11-11-1997     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0b.  */ 
/*  01-01-1999     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0e.  */ 
/*  11-01-1999     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0f.  */ 
/*  01-28-2001     William E. Lamie         Modified comment(s) and added */ 
/*                                            logic to detect wait abort  */ 
/*                                            status, resulting in        */ 
/*                                            version 4.0.                */ 
/*  07-15-2002     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 4.0a.  */ 
/*  01-01-2003     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 4.0b.  */ 
/*                                                                        */ 
/**************************************************************************/ 
UINT    _tx_thread_sleep(ULONG timer_ticks)
{

TX_INTERRUPT_SAVE_AREA


    /* Determine if this is a legal request.  */
    if ((!_tx_thread_current_ptr) || (_tx_thread_current_ptr == &_tx_timer_thread) ||
        (_tx_thread_system_state))
        
        /* Illegal caller of this service.  */
        return(TX_CALLER_ERROR);

    /* Determine if the requested number of ticks is zero.  */
    if (timer_ticks == 0)
        
        /* Just return with a successful status.  */
        return(TX_SUCCESS);

    /* Lockout interrupts while the thread is being resumed.  */
    TX_DISABLE

    /* Suspend the current thread.  */

    /* Set the state to suspended.  */
    _tx_thread_current_ptr -> tx_state =    TX_SLEEP;

    /* Set the suspending flag. */
    _tx_thread_current_ptr -> tx_suspending =  TX_TRUE;

    /* Initialize the status to successful.  */
    _tx_thread_current_ptr -> tx_suspend_status =  TX_SUCCESS;

    /* Temporarily disable preemption.  */
    _tx_thread_preempt_disable++;

    /* Restore interrupts.  */
    TX_RESTORE

    /* Activate the thread timer with the appropriate sleep value.  */
    _tx_thread_current_ptr -> tx_thread_timer.tx_remaining_ticks =  timer_ticks;  
    
    _tx_timer_activate(&(_tx_thread_current_ptr -> tx_thread_timer));

    /* Call actual thread suspension routine.  */
    _tx_thread_suspend(_tx_thread_current_ptr);

    /* Return status to the caller.  */
    return(_tx_thread_current_ptr -> tx_suspend_status);
}

