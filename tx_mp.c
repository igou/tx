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
/*    _tx_mutex_put                                       PORTABLE C      */ 
/*                                                           4.0b         */  
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function puts back an instance of the specified mutex.         */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    mutex_ptr                         Pointer to mutex control block    */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    TX_SUCCESS                        Success completion status         */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    _tx_timer_deactivate              Deactivate timer routine          */ 
/*    _tx_thread_resume                 Resume thread service             */ 
/*    _tx_thread_system_return          Return to system routine          */ 
/*    _tx_mutex_priority_change         Restore previous thread priority  */ 
/*    _tx_mutex_prioritize              Prioritize the mutex suspension   */ 
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
UINT    _tx_mutex_put(TX_MUTEX *mutex_ptr)
{

TX_INTERRUPT_SAVE_AREA

REG_1   TX_THREAD   *thread_ptr;            /* Working thread pointer          */
REG_2   TX_THREAD   *old_owner;             /* Remember previous mutex owner   */
REG_3   UINT        old_priority;           /* Original thread priority        */ 
REG_3   UINT        old_threshold;          /* Original thread threshold       */ 


    /* Disable interrupts to put an instance back to the mutex.  */
    TX_DISABLE

    /* Determine if this thread owns the mutex.  */
    if ((mutex_ptr -> tx_mutex_ownership_count) && 
            (mutex_ptr -> tx_mutex_owner == _tx_thread_current_ptr))
    {

        /* Yes, this thread does own the mutex.  */

        /* Decrement the mutex ownership count.  */
        mutex_ptr -> tx_mutex_ownership_count--;

        /* Determine if the mutex is still owned by the current thread.  */
        if (mutex_ptr -> tx_mutex_ownership_count)
        {

            /* Restore interrupts.  */
            TX_RESTORE

            /* Mutex is still owned, just return successful status.  */
            return(TX_SUCCESS);
        }
        else
        {
         
            /* Yes, the mutex is available we now need to check for a waiting thread.  */

            /* Determine if priority inheritance is in effect.  */
            if ((mutex_ptr -> tx_mutex_inherit) && (mutex_ptr -> tx_mutex_suspension_list))
            {

                /* Temporarily disable preemption.  */
                _tx_thread_preempt_disable++;

                /* Restore interrupts.  */
                TX_RESTORE

                /* Call the mutex prioritize processing to ensure the 
                   highest priority thread is resumed.  */
                _tx_mutex_prioritize(mutex_ptr);

                /* At this point, the highest priority thread is at the
                   front of the suspension list.  */

                /* Disable interrupts.  */
                TX_DISABLE

                /* Back off the preemption disable.  */
                _tx_thread_preempt_disable--;
            }

            if (mutex_ptr -> tx_mutex_suspension_list == TX_NULL)
            {           

                /* Temporarily disable preemption.  */
                _tx_thread_preempt_disable++;
 
                /* Restore interrupts.  */
                TX_RESTORE

                /* Mutex is not owned, but it is possible that a thread that 
                   caused a priority inheritance to occur is no longer waiting
                   on the mutex.  */
                if ((mutex_ptr -> tx_mutex_inherit) && (_tx_thread_current_ptr) &&
                    (_tx_thread_current_ptr -> tx_priority != mutex_ptr -> tx_mutex_original_priority))
                {

                    /* Restore the current priority and threshold of thread.  */
                    _tx_mutex_priority_change(mutex_ptr -> tx_mutex_owner, mutex_ptr -> tx_mutex_original_priority, 
                                                  mutex_ptr -> tx_mutex_original_threshold);
                }

                /* Disable interrupts again.  */
                TX_DISABLE

                /* Back off the preemption disable.  */
                _tx_thread_preempt_disable--;

                /* Restore interrupts.  */
                TX_RESTORE

                /* Check for a preemption condition that might be present.  */
                if ((_tx_thread_current_ptr != _tx_thread_execute_ptr) && 
                                                (_tx_thread_system_state == 0))
                {

                    /* Return control to the system.  */
                    _tx_thread_system_return();
                }

                /* Return success.  */
                return(TX_SUCCESS);
            }
            else
            {

                /* Pickup the thread a the front of the suspension list.  */
                thread_ptr =  mutex_ptr -> tx_mutex_suspension_list;

                /* Save the previous ownership information, if inheritance is
                   in effect.  */
                if (mutex_ptr -> tx_mutex_inherit)
                {
                    old_owner =     mutex_ptr -> tx_mutex_owner;
                    old_priority =  mutex_ptr -> tx_mutex_original_priority;
                    old_threshold = mutex_ptr -> tx_mutex_original_threshold;
        
                    /* Setup owner thread priority information.  */
                    mutex_ptr -> tx_mutex_original_priority =   thread_ptr -> tx_priority;
                    mutex_ptr -> tx_mutex_original_threshold =  thread_ptr -> tx_preempt_threshold;
                }

                /* Mark the Mutex as owned and fill in the corresponding information.  */
                mutex_ptr -> tx_mutex_ownership_count =  1;
                mutex_ptr -> tx_mutex_owner =            thread_ptr;

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

                    /* At least one more thread is on the same expiration list.  */

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

                /* Prepare for resumption of the first thread.  */

                /* Clear cleanup routine to avoid timeout.  */
                thread_ptr -> tx_suspend_cleanup =  TX_NULL;

                /* Temporarily disable preemption.  */
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

                /* Put return status into the thread control block.  */
                thread_ptr -> tx_suspend_status =  TX_SUCCESS;        

                /* Restore previous priority needs to be restored after priority
                   inheritance.  */
                if ((mutex_ptr -> tx_mutex_inherit) && (old_owner) &&
                    (old_owner -> tx_priority != old_priority))
                {

                    /* Restore the current priority and threshold of thread.  */
                    _tx_mutex_priority_change(old_owner, old_priority, old_threshold);
                }

                /* Resume thread.  */
                if (_tx_thread_resume(thread_ptr))

                    /* Return control to the system.  */
                    _tx_thread_system_return();
                 
                /* Return a successful status.  */
                return(TX_SUCCESS);
            }
        }
    }

    /* Restore interrupts.  */
    TX_RESTORE

    /* Caller does not own the mutex.  */
    return(TX_NOT_OWNED);
}

