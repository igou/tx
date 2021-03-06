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
/*    _tx_mutex_delete                                    PORTABLE C      */ 
/*                                                           4.0b         */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function deletes the specified mutex.  All threads             */ 
/*    suspended on the mutex are resumed with the TX_DELETED status       */ 
/*    code.                                                               */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    mutex_ptr                         Pointer to mutex control block    */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    TX_SUCCESS                        Successful completion status      */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    _tx_timer_deactivate              Deactivate timer routine          */ 
/*    _tx_thread_resume                 Resume thread service             */ 
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
UINT    _tx_mutex_delete(TX_MUTEX *mutex_ptr)
{

TX_INTERRUPT_SAVE_AREA

TX_THREAD       *thread_ptr;                /* Working thread pointer  */


    /* Disable interrupts to remove the mutex from the created list.  */
    TX_DISABLE

    /* Decrement the number of mutexes created.  */
    _tx_mutex_created_count--;

    /* Clear the mutex ID to make it invalid.  */
    mutex_ptr -> tx_mutex_id =  0;

    /* See if the mutex is the only one on the list.  */
    if (mutex_ptr == mutex_ptr -> tx_mutex_created_next)
    {

        /* Only created mutex, just set the created list to NULL.  */
        _tx_mutex_created_ptr =  TX_NULL;
    }
    else
    {

        /* Link-up the neighbors.  */
        (mutex_ptr -> tx_mutex_created_next) -> tx_mutex_created_previous =
                                            mutex_ptr -> tx_mutex_created_previous;
        (mutex_ptr -> tx_mutex_created_previous) -> tx_mutex_created_next =
                                            mutex_ptr -> tx_mutex_created_next;

        /* See if we have to update the created list head pointer.  */
        if (_tx_mutex_created_ptr == mutex_ptr)
            
            /* Yes, move the head pointer to the next link. */
            _tx_mutex_created_ptr =  mutex_ptr -> tx_mutex_created_next; 
    }

    /* Temporarily disable preemption.  */
    _tx_thread_preempt_disable++;

    /* Restore interrupts.  */
    TX_RESTORE

    /* Walk through the mutex list to resume any and all threads suspended
       on this mutex.  */
    thread_ptr =  mutex_ptr -> tx_mutex_suspension_list;    
    while (mutex_ptr -> tx_mutex_suspended_count)
    {
        /* Lockout interrupts.  */
        TX_DISABLE

        /* Clear the cleanup pointer, this prevents the timeout from doing 
           anything.  */
        thread_ptr -> tx_suspend_cleanup =  TX_NULL;

        /* Temporarily disable preemption again.  */
        _tx_thread_preempt_disable++;

        /* Restore interrupts.  */
        TX_RESTORE
    
        /* Yes, deactivate the thread's timer just in case.  */
        _tx_timer_deactivate(&(thread_ptr -> tx_thread_timer));

        /* Set the return status in the thread to TX_DELETED.  */
        thread_ptr -> tx_suspend_status =  TX_DELETED;

        /* Move the thread pointer ahead.  */
        thread_ptr =  thread_ptr -> tx_suspended_next;

        /* Resume the thread.  */
        _tx_thread_resume(thread_ptr -> tx_suspended_previous);

        /* Decrease the suspended count.  */
        mutex_ptr -> tx_mutex_suspended_count--;
    }

    /* Disable interrupts.  */
    TX_DISABLE

    /* Release previous preempt disable.  */
    _tx_thread_preempt_disable--;

    /* Restore interrupts.  */
    TX_RESTORE

    /* Check for preemption.  */
    if (_tx_thread_current_ptr != _tx_thread_execute_ptr)

        /* Transfer control to system.  */
        _tx_thread_system_return();

    /* Return TX_SUCCESS.  */
    return(TX_SUCCESS);
}

