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
#include    "tx_tim.h"
#include    "tx_que.h"


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _tx_queue_front_send                                PORTABLE C      */ 
/*                                                           4.0b         */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function places a message at the front of the specified queue. */ 
/*    If there is no room in the queue, this function returns the         */ 
/*    queue full status.                                                  */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    queue_ptr                         Pointer to queue control block    */ 
/*    source_ptr                        Pointer to message source         */ 
/*    wait_option                       Suspension option                 */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    status                            Completion status                 */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    _tx_timer_activate                Activate timer routine            */ 
/*    _tx_timer_deactivate              Deactivate timer routine          */ 
/*    _tx_thread_resume                 Resume thread routine             */ 
/*    _tx_thread_suspend                Suspend thread routine            */ 
/*    _tx_thread_system_return          Return to system routine          */ 
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
/*  07-15-2002     William E. Lamie         Modified comment(s) and       */ 
/*                                            changed to ULONG_PTR type,  */ 
/*                                            resulting in version 4.0a.  */ 
/*  01-01-2003     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 4.0b.  */ 
/*                                                                        */ 
/**************************************************************************/ 
UINT    _tx_queue_front_send(TX_QUEUE *queue_ptr, VOID *source_ptr, ULONG wait_option)
{

TX_INTERRUPT_SAVE_AREA

UINT                requeue_flag =  TX_FALSE;
ULONG               oldest_message[TX_16_ULONG];    

REG_1   UINT        status;                 /* Return status           */
REG_2   TX_THREAD   *thread_ptr;            /* Working thread pointer  */
REG_3   ULONG_PTR   source;                 /* Source pointer          */
REG_4   ULONG_PTR   destination;            /* Destination pointer     */


    /* Disable interrupts to place message in the queue.  */
    TX_DISABLE

    /* Determine if there is room in the queue.  */
    if ((!queue_ptr -> tx_queue_available_storage) && (wait_option))
    {

        /* Pickup the last message in the queue and place it in the temporary
           local storage.  */

        /* See if the write pointer is at the beginning of the queue area.  */
        if (queue_ptr -> tx_queue_write == queue_ptr -> tx_queue_start)

             /* Adjust the write pointer to the last message at the end of the
                queue.  */
             queue_ptr -> tx_queue_write =  queue_ptr -> tx_queue_end -
                                                queue_ptr -> tx_queue_message_size;
        else

             /* Not at the beginning of the queue, just move back one message.  */
             queue_ptr -> tx_queue_write =  
                     queue_ptr -> tx_queue_write - queue_ptr -> tx_queue_message_size;

        /* Simply place the message in the queue.  */
            
        /* Reduce the amount of available storage.  */
        queue_ptr -> tx_queue_available_storage++;

        /* Increase the enqueued count.  */
        queue_ptr -> tx_queue_enqueued--;

        /* Setup source and destination pointers.  */
        source =  (ULONG_PTR) queue_ptr -> tx_queue_write;
        destination =  (ULONG_PTR) &oldest_message[0];

        /* Set the requeue flag to make sure this message gets back in the queue.  */
        requeue_flag = TX_TRUE;

        /* Copy the message from the queue.  */
        switch (queue_ptr -> tx_queue_message_size)
        {

        default:

            /* Copy a sixteen longword message from the queue.  */
            *destination++ =  *source++;
            *destination++ =  *source++;
            *destination++ =  *source++;
            *destination++ =  *source++;
            *destination++ =  *source++;
            *destination++ =  *source++;
            *destination++ =  *source++;
            *destination++ =  *source++;
            /* Fall through to copy the remaining eight longwords.  */

        case TX_8_ULONG:

            /* Copy an eight longword message from the queue.  */
            *destination++ =  *source++;
            *destination++ =  *source++;
            *destination++ =  *source++;
            *destination++ =  *source++;
            /* Fall through to copy the remaining four longwords.  */

        case TX_4_ULONG:

            /* Copy a four longword message from the queue.  */
            *destination++ =  *source++;
            *destination++ =  *source++;
            /* Fall through to copy the remaining two longwords.  */

        case TX_2_ULONG:

            /* Copy a two longword message from the queue.  */
            *destination++ =  *source++;
            /* Fall through to copy the remaining longword.  */

        case TX_1_ULONG:

            /* Copy single longword message from the queue.  */
            *destination =  *source;
        }
    }

    /* Now check for room in the queue for placing the new message in front.  */
    if (queue_ptr -> tx_queue_available_storage)
    {

        /* Now determine if there is a thread waiting for a message.  */
        if (!queue_ptr -> tx_queue_suspension_list)
        {

            /* No thread suspended while waiting for a message from
               this queue.  */

            /* Adjust the read pointer since we are adding to the front of the
               queue.  */

            /* See if the read pointer is at the beginning of the queue area.  */
            if (queue_ptr -> tx_queue_read == queue_ptr -> tx_queue_start)

                /* Adjust the read pointer to the last message at the end of the
                   queue.  */
                queue_ptr -> tx_queue_read =  queue_ptr -> tx_queue_end -
                                                queue_ptr -> tx_queue_message_size;
            else

                /* Not at the beginning of the queue, just move back one message.  */
                queue_ptr -> tx_queue_read =  
                     queue_ptr -> tx_queue_read - queue_ptr -> tx_queue_message_size;

            /* Simply place the message in the queue.  */
            
            /* Reduce the amount of available storage.  */
            queue_ptr -> tx_queue_available_storage--;

            /* Increase the enqueued count.  */
            queue_ptr -> tx_queue_enqueued++;

            /* Setup source and destination pointers.  */
            source =  (ULONG_PTR) source_ptr;
            destination =  (ULONG_PTR) queue_ptr -> tx_queue_read;

            /* Copy the message into the queue.  */
            switch (queue_ptr -> tx_queue_message_size)
            {

            default:

                /* Copy a sixteen longword message into the queue.  */
                *destination++ =  *source++;
                *destination++ =  *source++;
                *destination++ =  *source++;
                *destination++ =  *source++;
                *destination++ =  *source++;
                *destination++ =  *source++;
                *destination++ =  *source++;
                *destination++ =  *source++;
                /* Fall through to copy the remaining eight longwords.  */

            case TX_8_ULONG:

                /* Copy an eight longword message into the queue.  */
                *destination++ =  *source++;
                *destination++ =  *source++;
                *destination++ =  *source++;
                *destination++ =  *source++;
                /* Fall through to copy the remaining four longwords.  */

            case TX_4_ULONG:

                /* Copy a four longword message into the queue.  */
                *destination++ =  *source++;
                *destination++ =  *source++;
                /* Fall through to copy the remaining two longwords.  */

            case TX_2_ULONG:

                /* Copy a two longword message into the queue.  */
                *destination++ =  *source++;
                /* Fall through to copy the remaining longword.  */

            case TX_1_ULONG:

                /* Copy single longword message into the queue.  */
                *destination =  *source;
            }

            /* Determine if a requeue is necessary.  */
            if (requeue_flag)
            {

                /* Yes, room was made for the message by temporarily removing the 
                   oldest message in the queue.  We need to suspend the caller
                   in the front of the suspension list in order to get the message
                   placed back in the appropriate order.  */

                /* Prepare for suspension of this thread.  */
            
                /* Pickup thread pointer.  */
                thread_ptr =  _tx_thread_current_ptr;

                /* Setup cleanup routine pointer.  */
                thread_ptr -> tx_suspend_cleanup =  _tx_queue_cleanup;

                /* Setup cleanup information, i.e. this queue control
                   block and the source pointer.  */
                thread_ptr -> tx_suspend_control_block =    (VOID_PTR) queue_ptr;
                thread_ptr -> tx_additional_suspend_info =  (VOID_PTR) &oldest_message[0];

                /* Setup suspension list.  */
                if (queue_ptr -> tx_queue_suspension_list)
                {

                    /* This list is not NULL, add current thread to the end. */
                    thread_ptr -> tx_suspended_next =      
                            queue_ptr -> tx_queue_suspension_list;
                    thread_ptr -> tx_suspended_previous =  
                            (queue_ptr -> tx_queue_suspension_list) -> tx_suspended_previous;
                    ((queue_ptr -> tx_queue_suspension_list) -> tx_suspended_previous) -> tx_suspended_next =  
                            thread_ptr;
                    (queue_ptr -> tx_queue_suspension_list) -> tx_suspended_previous =   thread_ptr;

                    /* Update the suspension list to put this thread in front, which will put
                       the message that was removed in the proper relative order when room is 
                       made in the queue.  */
                    queue_ptr -> tx_queue_suspension_list =  thread_ptr;
                }
                else
                {

                    /* No other threads are suspended.  Setup the head pointer and
                       just setup this threads pointers to itself.  */
                    queue_ptr -> tx_queue_suspension_list =  thread_ptr;
                    thread_ptr -> tx_suspended_next =        thread_ptr;
                    thread_ptr -> tx_suspended_previous =    thread_ptr;
                }

                /* Increment the suspended thread count.  */
                queue_ptr -> tx_queue_suspended_count++;

                /* Set the state to suspended.  */
                thread_ptr -> tx_state =    TX_QUEUE_SUSP;

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

                    /* Activate the thread timer for timeout.  */
                    _tx_timer_activate(&(thread_ptr -> tx_thread_timer));
                }

                /* Call actual thread suspension routine.  */
                _tx_thread_suspend(thread_ptr);

                /* Return the completion status.  */
                return(thread_ptr -> tx_suspend_status);
            }
            else
            {

                /* Restore interrupts.  */
                TX_RESTORE

                /* Set status to success.  */
                status =  TX_SUCCESS;
            }
        }              
        else
        {

            /* Thread suspended waiting for a message.  Remove it and copy this message
               into its storage area.  */
            thread_ptr =  queue_ptr -> tx_queue_suspension_list;

            /* See if this is the only suspended thread on the list.  */
            if (thread_ptr == thread_ptr -> tx_suspended_next)
            {

                /* Yes, the only suspended thread.  */

                /* Update the head pointer.  */
                queue_ptr -> tx_queue_suspension_list =  TX_NULL;
            }
            else
            {

                /* At least one more thread is on the same expiration list.  */

                /* Update the list head pointer.  */
                queue_ptr -> tx_queue_suspension_list =  thread_ptr -> tx_suspended_next;

                /* Update the links of the adjacent threads.  */
                (thread_ptr -> tx_suspended_next) -> tx_suspended_previous =  
                                                    thread_ptr -> tx_suspended_previous;
                (thread_ptr -> tx_suspended_previous) -> tx_suspended_next =
                                                    thread_ptr -> tx_suspended_next;
            } 
 
            /* Decrement the suspension count.  */
            queue_ptr -> tx_queue_suspended_count--;

            /* Prepare for resumption of the thread.  */

            /* Clear cleanup routine to avoid timeout.  */
            thread_ptr -> tx_suspend_cleanup =  TX_NULL;

            /* Temporarily disable preemption.  */
            _tx_thread_preempt_disable++;

            /* Restore interrupts.  */
            TX_RESTORE

            /* Setup source and destination pointers.  */
            source =  (ULONG_PTR) source_ptr;
            destination =  (ULONG_PTR) thread_ptr -> tx_additional_suspend_info;

            /* Copy the message into the thread's destination.  */
            switch (queue_ptr -> tx_queue_message_size)
            {

            default:

                /* Copy a sixteen longword message into the thread's destination.  */
                *destination++ =  *source++;
                *destination++ =  *source++;
                *destination++ =  *source++;
                *destination++ =  *source++;
                *destination++ =  *source++;
                *destination++ =  *source++;
                *destination++ =  *source++;
                *destination++ =  *source++;
                /* Fall through to copy the remaining eight longwords.  */

            case TX_8_ULONG:

                /* Copy an eight longword message into the thread's destination.  */
                *destination++ =  *source++;
                *destination++ =  *source++;
                *destination++ =  *source++;
                *destination++ =  *source++;
                /* Fall through to copy the remaining four longwords.  */

            case TX_4_ULONG:

                /* Copy a four longword message into the thread's destination.  */
                *destination++ =  *source++;
                *destination++ =  *source++;
                /* Fall through to copy the remaining two longwords.  */

            case TX_2_ULONG:

                /* Copy a two longword message into the thread's destination.  */
                *destination++ =  *source++;
                /* Fall through to copy the remaining longword.  */

            case TX_1_ULONG:

                /* Copy single longword message into the thread's destination.  */
                *destination =  *source;
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

            /* Put return status into the thread control block.  */
            thread_ptr -> tx_suspend_status =  TX_SUCCESS;        

            /* Resume thread.  */
            if (_tx_thread_resume(thread_ptr))

                 /* Preemption is required, transfer control back to 
                   system.  */
                _tx_thread_system_return();
 
            /* Return successful status.  */
            return(TX_SUCCESS);    
        }
    }
    else
    {
        /* Restore interrupts.  */
        TX_RESTORE

        /* No room in queue, return error completion.  */
        status =  TX_QUEUE_FULL;
    }

    /* Return completion status.  */
    return(status);
}

