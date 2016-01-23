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
/**   Mutex (MUT)                                                         */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define TX_SOURCE_CODE


/* Include necessary system files.  */

#include    "tx_api.h"
#include    "tx_thr.h"
#include    "tx_tim.h"
#include    "tx_mut.h"


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _tx_mutex_get                                       PORTABLE C      */ 
/*                                                           4.0b         */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function gets the specified mutex.  If the calling thread      */ 
/*    already owns the mutex, an ownership count is simply increased.     */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    mutex_ptr                         Pointer to mutex control block    */ 
/*    wait_option                       Suspension option                 */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    status                            Completion status                 */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    _tx_timer_activate                Activate timer routine            */ 
/*    _tx_thread_suspend                Suspend thread service            */ 
/*    _tx_mutex_priority_change         Inherit thread priority           */ 
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    Application Code                                                    */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */ 
/*                                                                        */ 
/*  01-28-2001     William E. Lamie         Initial Version 4.0           */ 
/*  07-15-2002     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 4.0a.  */ 
/*  01-01-2003     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 4.0b.  */ 
/*                                                                        */ 
/**************************************************************************/ 
UINT    _tx_mutex_get(TX_MUTEX *mutex_ptr, ULONG wait_option)
{

TX_INTERRUPT_SAVE_AREA

REG_1   UINT        status;                 /* Return status           */
REG_2   TX_THREAD   *thread_ptr;            /* Working thread pointer  */


    /* Disable interrupts to get an instance from the mutex.  */
    TX_DISABLE

    /* Determine if there is an instance of the mutex.  */
    if (!mutex_ptr -> tx_mutex_ownership_count)
    {

        /* Set the ownership count to 1.  */
        mutex_ptr -> tx_mutex_ownership_count =  1;

        /* Remember that the calling thread owns the mutex.  */
        mutex_ptr -> tx_mutex_owner =  _tx_thread_current_ptr;

        /* Determine if priority inheritance is required.  */
        if ((mutex_ptr -> tx_mutex_inherit) && (_tx_thread_current_ptr))
        {

            /* Remember the current priority and threshold of thread.  */
            mutex_ptr -> tx_mutex_original_priority =  _tx_thread_current_ptr -> tx_priority;
            mutex_ptr -> tx_mutex_original_threshold = _tx_thread_current_ptr -> tx_preempt_threshold;
        }

        /* Set status to success.  */
        status =  TX_SUCCESS;
    }
    else if (mutex_ptr -> tx_mutex_owner == _tx_thread_current_ptr)
    {

        /* The owning thread is requesting the mutex again, just 
           increment the ownership count.  */
        mutex_ptr -> tx_mutex_ownership_count++;

        /* Set status to success.  */
        status =  TX_SUCCESS;
    }
    else
    {

        /* Determine if the request specifies suspension.  */
        if (wait_option)
        {

            /* Prepare for suspension of this thread.  */
            
            /* Pickup thread pointer.  */
            thread_ptr =  _tx_thread_current_ptr;

            /* Setup cleanup routine pointer.  */
            thread_ptr -> tx_suspend_cleanup =  _tx_mutex_cleanup;

            /* Setup cleanup information, i.e. this mutex control
               block.  */
            thread_ptr -> tx_suspend_control_block =  (VOID_PTR) mutex_ptr;

            /* Setup suspension list.  */
            if (mutex_ptr -> tx_mutex_suspension_list)
            {

                /* This list is not NULL, add current thread to the end. */
                thread_ptr -> tx_suspended_next =      
                        mutex_ptr -> tx_mutex_suspension_list;
                thread_ptr -> tx_suspended_previous =  
                        (mutex_ptr -> tx_mutex_suspension_list) -> tx_suspended_previous;
                ((mutex_ptr -> tx_mutex_suspension_list) -> tx_suspended_previous) -> tx_suspended_next =  
                        thread_ptr;
                (mutex_ptr -> tx_mutex_suspension_list) -> tx_suspended_previous =   thread_ptr;
            }
            else
            {

                /* No other threads are suspended.  Setup the head pointer and
                   just setup this threads pointers to itself.  */
                mutex_ptr -> tx_mutex_suspension_list =  thread_ptr;
                thread_ptr -> tx_suspended_next =        thread_ptr;
                thread_ptr -> tx_suspended_previous =    thread_ptr;
            }

            /* Increment the suspended thread count.  */
            mutex_ptr -> tx_mutex_suspended_count++;

            /* Set the state to suspended.  */
            thread_ptr -> tx_state =    TX_MUTEX_SUSP;

            /* Set the suspending flag.  */
            thread_ptr -> tx_suspending =  TX_TRUE;

            /* Temporarily disable preemption.  */
            _tx_thread_preempt_disable++;

            /* Save the timeout value.  */
            thread_ptr -> tx_thread_timer.tx_remaining_ticks =  wait_option;

            /* Restore interrupts.  */
            TX_RESTORE

            /* Determine if we need to raise the priority of the thread 
               owning the mutex.  */
            if ((mutex_ptr -> tx_mutex_inherit) && (mutex_ptr -> tx_mutex_owner) 
                                                            && (_tx_thread_current_ptr))
            {

                /* Priority inheritance is requested, check to see if the 
                   thread that owns the mutex is lower priority.  */
                if ((mutex_ptr -> tx_mutex_owner) -> tx_priority > 
                        _tx_thread_current_ptr -> tx_priority)
                {

                    /* Yes, raise the suspended, owning thread's priority to that
                       of the current thread.  */
                    _tx_mutex_priority_change(mutex_ptr -> tx_mutex_owner, _tx_thread_current_ptr -> tx_priority, _tx_thread_current_ptr -> tx_priority);
                }
            }

            /* See if we need to start a timer.  */
            if (wait_option != TX_WAIT_FOREVER)
            {

                /* A timeout is required.  */
                _tx_timer_activate(&(thread_ptr -> tx_thread_timer));
            }

            /* Call actual thread suspension routine.  */
            _tx_thread_suspend(thread_ptr);

            /* Return the completion status.  */
            return(thread_ptr -> tx_suspend_status);
        }
        else
    
            /* Immediate return, return error completion.  */
            status =  TX_NOT_AVAILABLE;
    }

    /* Restore interrupts.  */
    TX_RESTORE

    /* Return completion status.  */
    return(status);
}

