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
/*    _tx_thread_wait_abort                               PORTABLE C      */ 
/*                                                           4.0b         */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function aborts the wait condition that the specified thread   */ 
/*    is in - regardless of what object the thread is waiting on - and    */ 
/*    returns a TX_WAIT_ABORTED status to the specified thread.           */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    thread_ptr                            Thread to abort the wait on   */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    status                                Return completion status      */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    Suspension Cleanup Functions                                        */ 
/*    _tx_thread_resume                                                   */ 
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    Application code                                                    */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */ 
/*                                                                        */ 
/*  01-28-2001     William E. Lamie         Initial Version 4.0           */ 
/*  07-15-2002     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 4.0a.  */ 
/*  01-01-2003     William E. Lamie         Modified comment(s), and      */ 
/*                                            fixed problem relating to   */ 
/*                                            not decrementing the        */ 
/*                                            _tx_thread_preempt_disable  */ 
/*                                            flag, resulting in          */ 
/*                                            version 4.0b.               */ 
/*                                                                        */ 
/**************************************************************************/ 
UINT    _tx_thread_wait_abort(TX_THREAD  *thread_ptr)
{

TX_INTERRUPT_SAVE_AREA

VOID            (*suspend_cleanup)(struct TX_THREAD_STRUCT *);


    /* Disable interrupts.  */
    TX_DISABLE

    /* Determine if the thread is currently suspended.  */
    if (thread_ptr -> tx_state < TX_SLEEP)
    {
    
        /* Thread is either ready, completed, terminated, or in a pure 
           suspension condition.  */

        /* Restore interrupts.  */
        TX_RESTORE

        /* Just return with an error message to indicate that 
           nothing was done.  */
        return(TX_WAIT_ABORT_ERROR);
    }    

    /* Check for a sleep condition.  */
    else if (thread_ptr -> tx_state == TX_SLEEP)
    {

        /* Set the state to terminated.  */
        thread_ptr -> tx_state =    TX_SUSPENDED;

        /* Set the TX_WAIT_ABORTED status in the thread that is
           sleeping.  */
        thread_ptr -> tx_suspend_status =  TX_WAIT_ABORTED;

        /* Make sure there isn't a suspend cleanup routine.  */
        thread_ptr -> tx_suspend_cleanup =  TX_NULL;

        /* Increment the disable preemption flag.  */
        _tx_thread_preempt_disable++;

        /* Restore interrupts.  */
        TX_RESTORE

        /* Deactivate the timeout timer if necessary.  */
        if (thread_ptr -> tx_thread_timer.tx_list_head)
        {

            /* Deactivate the thread's timeout timer.  */
            _tx_timer_deactivate(&(thread_ptr -> tx_thread_timer));
        }
        else
        {

            /* Clear the remaining time to ensure timer doesn't get activated.  */
            thread_ptr -> tx_thread_timer.tx_remaining_ticks =  0;
        }
    }
    else
    {

        /* Process all other suspension timeouts.  */
    
        /* Set the state to suspended.  */
        thread_ptr -> tx_state =    TX_SUSPENDED;

        /* Pickup the cleanup routine address.  */
        suspend_cleanup =  thread_ptr -> tx_suspend_cleanup;

        /* Set the TX_WAIT_ABORTED status in the thread that was
           suspended.  */
        thread_ptr -> tx_suspend_status =  TX_WAIT_ABORTED;

        /* Increment the disable preemption flag.  */
        _tx_thread_preempt_disable++;

        /* Restore interrupts.  */
        TX_RESTORE

        /* Call any cleanup routines.  */
        if (suspend_cleanup)

            /* Yes, there is a function to call.  */
            (suspend_cleanup)(thread_ptr);
    }


    /* If the abort of the thread wait was successful, if so resume the thread.  */
    if (thread_ptr -> tx_suspend_status == TX_WAIT_ABORTED)
    {

        /* Lift the suspension on the previously waiting thread.  */
        if (_tx_thread_resume(thread_ptr))
        {

            /* Preemption is required, transfer control back to 
               system.  */
            _tx_thread_system_return();
        }
    }
    else
    {

        /* Disable interrupts.  */
        TX_DISABLE
        
        /* Decrement the disable preemption flag.  */
        _tx_thread_preempt_disable--;

        /* Restore interrupts.  */
        TX_RESTORE

        /* Return with an error message to indicate that 
           nothing was done.  */
        return(TX_WAIT_ABORT_ERROR);
    }

    /* Return a successful status.  */
    return(TX_SUCCESS);
}
