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
/**   Queue (QUE)                                                         */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define TX_SOURCE_CODE


/* Include necessary system files.  */

#include    "tx_api.h"
#include    "tx_thr.h"
#include    "tx_que.h"


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _tx_queue_prioritize                                PORTABLE C      */ 
/*                                                           4.0b         */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function places the highest priority suspended thread at the   */ 
/*    front of the suspension list.  All other threads remain in the same */ 
/*    FIFO suspension order.                                              */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    queue_ptr                         Pointer to queue control block    */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    status                            Completion status                 */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    None                                                                */ 
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
UINT    _tx_queue_prioritize(TX_QUEUE *queue_ptr)
{

TX_INTERRUPT_SAVE_AREA

REG_1   TX_THREAD   *thread_ptr;            /* Working thread pointer  */
REG_2   TX_THREAD   *priority_thread_ptr;   /* Highest priority thread */ 


    /* Disable interrupts to place message in the queue.  */
    TX_DISABLE

    /* Determine if there how many threads are suspended on this queue.  */
    if (queue_ptr -> tx_queue_suspended_count == 2)
    {

        /* Determine if the next suspended thread has a higher priority.  */
        if (((queue_ptr -> tx_queue_suspension_list) -> tx_suspended_next) -> tx_priority <
            ((queue_ptr -> tx_queue_suspension_list) -> tx_priority))
        {

            /* Yes, move the list head to the next thread.  */
            queue_ptr -> tx_queue_suspension_list =  
                            (queue_ptr -> tx_queue_suspension_list) -> tx_suspended_next;
        }
    }
    else if (queue_ptr -> tx_queue_suspended_count > 2)
    {

        /* Default the highest priority thread to the thread at the front of the list.  */
        priority_thread_ptr =  queue_ptr -> tx_queue_suspension_list;

        /* Setup search pointer.  */
        thread_ptr =  priority_thread_ptr -> tx_suspended_next;

        /* Search through the list to find the highest priority thread.  */
        do
        {

            /* Is the current thread higher priority?  */
            if (thread_ptr -> tx_priority < priority_thread_ptr -> tx_priority)
            {

                /* Yes, remember that this thread is the highest priority.  */
                priority_thread_ptr =  thread_ptr;
            }

            /* Move the thread pointer to the next thread.  */
            thread_ptr =  thread_ptr -> tx_suspended_next;

        } while (thread_ptr != queue_ptr -> tx_queue_suspension_list);

        /* Now determine if the highest priority thread is at the front 
           of the list.  */
        if (priority_thread_ptr != queue_ptr -> tx_queue_suspension_list)
        {

            /* No, we need to move the highest priority suspended thread to the 
               front of the list.  */

            /* First, remove the highest priority thread by updating the
               adjacent suspended threads.  */
            (priority_thread_ptr -> tx_suspended_next) -> tx_suspended_previous =  
                                                priority_thread_ptr -> tx_suspended_previous;
            (priority_thread_ptr -> tx_suspended_previous) -> tx_suspended_next =
                                                priority_thread_ptr -> tx_suspended_next;

            /* Now, link the highest priority thread at the front of the list.  */
            priority_thread_ptr -> tx_suspended_next =      
                                                queue_ptr -> tx_queue_suspension_list;
            priority_thread_ptr -> tx_suspended_previous =  
                        (queue_ptr -> tx_queue_suspension_list) -> tx_suspended_previous;
            ((queue_ptr -> tx_queue_suspension_list) -> tx_suspended_previous) -> tx_suspended_next =  
                        priority_thread_ptr;
            (queue_ptr -> tx_queue_suspension_list) -> tx_suspended_previous =   priority_thread_ptr;

            /* Move the list head pointer to the highest priority suspended thread.  */
            queue_ptr -> tx_queue_suspension_list =  priority_thread_ptr;
        }
    }

    /* Restore interrupts.  */
    TX_RESTORE

    /* Return completion status.  */
    return(TX_SUCCESS);
}

