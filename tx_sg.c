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
/**   Semaphore (SEM)                                                     */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define TX_SOURCE_CODE


/* Include necessary system files.  */

#include    "tx_api.h"
#include    "tx_thr.h"
#include    "tx_tim.h"
#include    "tx_sem.h"


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _tx_semaphore_get                                   PORTABLE C      */ 
/*                                                           4.0b         */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function gets an instance from the specified counting          */ 
/*    semaphore.                                                          */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    semaphore_ptr                     Pointer to semaphore control block*/ 
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
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    Application Code                                                    */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */ 
/*                                                                        */ 
/*  12-31-1996     William E. Lamie         Initial Version 3.0           */ 
/*  11-11-1997     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0b.  */ 
/*  03-01-1998     William E. Lamie         Optimized post RESTORE        */ 
/*                                            processing, resulting in    */ 
/*                                            version 3.0d.               */ 
/*  01-01-1999     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0e.  */ 
/*  11-01-1999     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0f.  */ 
/*  01-28-2001     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 4.0.   */ 
/*  07-15-2002     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 4.0a.  */ 
/*  01-01-2003     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 4.0b.  */ 
/*                                                                        */ 
/**************************************************************************/ 
UINT    _tx_semaphore_get(TX_SEMAPHORE *semaphore_ptr, ULONG wait_option)
{

TX_INTERRUPT_SAVE_AREA

REG_1   UINT        status;                 /* Return status           */
REG_2   TX_THREAD   *thread_ptr;            /* Working thread pointer  */


    /* Disable interrupts to get an instance from the semaphore.  */
    TX_DISABLE

    /* Determine if there is an instance of the semaphore.  */
    if (semaphore_ptr -> tx_semaphore_count)
    {

        /* Decrement the semaphore count.  */
        semaphore_ptr -> tx_semaphore_count--;

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
            thread_ptr -> tx_suspend_cleanup =  _tx_semaphore_cleanup;

            /* Setup cleanup information, i.e. this semaphore control
               block.  */
            thread_ptr -> tx_suspend_control_block =  (VOID_PTR) semaphore_ptr;

            /* Setup suspension list.  */
            if (semaphore_ptr -> tx_semaphore_suspension_list)
            {

                /* This list is not NULL, add current thread to the end. */
                thread_ptr -> tx_suspended_next =      
                        semaphore_ptr -> tx_semaphore_suspension_list;
                thread_ptr -> tx_suspended_previous =  
                        (semaphore_ptr -> tx_semaphore_suspension_list) -> tx_suspended_previous;
                ((semaphore_ptr -> tx_semaphore_suspension_list) -> tx_suspended_previous) -> tx_suspended_next =  
                        thread_ptr;
                (semaphore_ptr -> tx_semaphore_suspension_list) -> tx_suspended_previous =   thread_ptr;
            }
            else
            {

                /* No other threads are suspended.  Setup the head pointer and
                   just setup this threads pointers to itself.  */
                semaphore_ptr -> tx_semaphore_suspension_list =  thread_ptr;
                thread_ptr -> tx_suspended_next =                thread_ptr;
                thread_ptr -> tx_suspended_previous =            thread_ptr;
            }

            /* Increment the suspended thread count.  */
            semaphore_ptr -> tx_semaphore_suspended_count++;

            /* Set the state to suspended.  */
            thread_ptr -> tx_state =    TX_SEMAPHORE_SUSP;

            /* Set the suspending flag.  */
            thread_ptr -> tx_suspending =  TX_TRUE;

            /* Temporarily disable preemption.  */
            _tx_thread_preempt_disable++;

            /* Save the timeout value.  */
            thread_ptr -> tx_thread_timer.tx_remaining_ticks =  wait_option;

            /* Restore interrupts.  */
            TX_RESTORE

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
            status =  TX_NO_INSTANCE;
    }

    /* Restore interrupts.  */
    TX_RESTORE

    /* Return completion status.  */
    return(status);
}

