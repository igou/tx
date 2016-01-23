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
/*    _tx_mutex_cleanup                                   PORTABLE C      */ 
/*                                                           4.0b         */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function processes mutex timeout and thread terminate          */ 
/*    actions that require the mutex data structures to be cleaned        */ 
/*    up.                                                                 */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    thread_ptr                        Pointer to suspended thread's     */ 
/*                                        control block                   */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    None                                                                */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    _tx_timer_deactivate              Deactivate timer routine          */ 
/*    _tx_thread_resume                 Resume thread service             */ 
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    _tx_thread_timeout                Thread timeout processing         */ 
/*    _tx_thread_terminate              Thread terminate processing       */ 
/*    _tx_thread_wait_abort             Thread wait abort processing      */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */ 
/*                                                                        */ 
/*  01-28-2001     William E. Lamie         Initial Version 4.0           */ 
/*  07-15-2002     William E. Lamie         Modified comment(s) and       */ 
/*                                            optimized out extra disable */ 
/*                                            interrupts call, resulting  */ 
/*                                            in version 4.0a.            */ 
/*  01-01-2003     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 4.0b.  */ 
/*                                                                        */ 
/**************************************************************************/ 
VOID    _tx_mutex_cleanup(TX_THREAD  *thread_ptr)
{

TX_INTERRUPT_SAVE_AREA

REG_1   TX_MUTEX *mutex_ptr;            /* Working mutex pointer  */

    
    /* Setup pointer to mutex control block.  */
    mutex_ptr =  (TX_MUTEX *) thread_ptr -> tx_suspend_control_block;

    /* Disable interrupts to remove the suspended thread from the mutex.  */
    TX_DISABLE

    /* Determine if the cleanup is still required.  */
    if ((thread_ptr -> tx_suspend_cleanup) && (mutex_ptr) &&
        (mutex_ptr -> tx_mutex_id == TX_MUTEX_ID))
    {

        /* Yes, we still have thread suspension!  */

        /* Clear the suspension cleanup flag.  */
        thread_ptr -> tx_suspend_cleanup =  TX_NULL;

        /* Remove the suspended thread from the list.  */

        /* See if this is the only suspended thread on the list.  */
        if (thread_ptr == thread_ptr -> tx_suspended_next)
        {

            /* Yes, the only suspended thread.  */

            /* Update the head pointer.  */
            mutex_ptr -> tx_mutex_suspension_list =  TX_NULL;
        }
        else
        {

            /* At least one more thread is on the same suspension list.  */

            /* Update the list head pointer.  */
            mutex_ptr -> tx_mutex_suspension_list =  thread_ptr -> tx_suspended_next;

            /* Update the links of the adjacent threads.  */
            (thread_ptr -> tx_suspended_next) -> tx_suspended_previous =  
                                                    thread_ptr -> tx_suspended_previous;
            (thread_ptr -> tx_suspended_previous) -> tx_suspended_next =
                                                    thread_ptr -> tx_suspended_next;
        } 
 
        /* Decrement the suspension count.  */
        mutex_ptr -> tx_mutex_suspended_count--;

        /* Now we need to determine if this cleanup is from a terminate, timeout,
           or from a wait abort.  */
        if (thread_ptr -> tx_state == TX_MUTEX_SUSP)
        {

            /* Thread still suspended on the mutex.  Setup return error status and
               resume the thread.  */

            /* Setup return status.  */
            thread_ptr -> tx_suspend_status =  TX_NOT_AVAILABLE;

            /* Temporarily disable preemption.  */
            _tx_thread_preempt_disable++;

            /* Restore interrupts.  */
            TX_RESTORE

            /* Resume the thread!  Check for preemption even though we are executing 
               from the system timer thread right now which normally executes at the 
               highest priority.  */
            if (_tx_thread_resume(thread_ptr))
            {

                /* Preemption is required, transfer control back to 
                   system.  */
                _tx_thread_system_return();
            }
        }
        else
        {
    
            /* Restore interrupts.  */
            TX_RESTORE
        }
        
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
       
        /* Finished, just return.  */
        return;
    }

    /* Restore interrupts.  */
    TX_RESTORE
}

