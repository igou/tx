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


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _tx_thread_priority_change                          PORTABLE C      */ 
/*                                                           4.0b         */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function changes the priority of the specified thread.  It     */ 
/*    also returns the old priority and handles preemption if the calling */ 
/*    thread is currently executing and the priority change results in a  */ 
/*    higher priority thread ready for execution.                         */ 
/*                                                                        */ 
/*    Note: the preemption threshold is automatically changed to the new  */ 
/*    priority.                                                           */
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    thread_ptr                            Pointer to thread to suspend  */ 
/*    new_priority                          New thread priority           */ 
/*    old_priority                          Old thread priority           */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    status                                Completion status             */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    _tx_thread_system_return              Return to system if self-     */ 
/*                                            suspension                  */ 
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
/*  07-04-1997     William E. Lamie         Added casting of local        */ 
/*                                            priority_group variable in  */ 
/*                                            order to avoid compiler     */ 
/*                                            warnings, resulting in      */ 
/*                                            version 3.0a.               */ 
/*  11-11-1997     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0b.  */ 
/*  03-01-1998     William E. Lamie         Corrected casting in making   */ 
/*                                            new priority bit map, and   */ 
/*                                            added clearing of bit in    */ 
/*                                            preempted map if thread is  */ 
/*                                            head of the priority list,  */ 
/*                                            resulting in version 3.0d.  */ 
/*  01-01-1999     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0e.  */ 
/*  11-01-1999     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0f.  */ 
/*  01-28-2001     William E. Lamie         Modified comment(s), fixed    */ 
/*                                            problem resetting the       */ 
/*                                            preemption-threshold with   */ 
/*                                            interrupts enabled, and     */ 
/*                                            added priority optimization,*/ 
/*                                            resulting in version 4.0.   */ 
/*  07-15-2002     William E. Lamie         Modified comment(s), moved    */ 
/*                                            check for valid priority    */ 
/*                                            to txe_trpc.c, resulting    */ 
/*                                            in version 4.0a.            */ 
/*  01-01-2003     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 4.0b.  */ 
/*                                                                        */ 
/**************************************************************************/ 
UINT    _tx_thread_priority_change(TX_THREAD *thread_ptr, UINT new_priority, UINT *old_priority)
{

TX_INTERRUPT_SAVE_AREA

REG_1 UINT      priority;               /* Thread priority          */
REG_2 ULONG     priority_map;           /* Working priority map     */
REG_3 UCHAR     priority_group;         /* Priority group           */

    /* Lockout interrupts while the thread is being suspended.  */
    TX_DISABLE

    /* Save the previous priority.  */
    *old_priority =  thread_ptr -> tx_priority;

    /* Determine if this thread is currently ready.  */
    if (thread_ptr -> tx_state != TX_READY)
    {

        /* Easy, just setup the priority and threshold in the thread's control
           block.  */
        thread_ptr -> tx_priority =           new_priority;
        thread_ptr -> tx_preempt_threshold =  new_priority;

        /* Setup the new priority bit.  */
        thread_ptr -> tx_priority_bit =       (((ULONG) 1) << new_priority);

        /* Restore interrupts.  */
        TX_RESTORE
    }
    else
    {

        /* The thread is ready and must first be removed from the list.  */

        /* Pickup priority of thread.  */
        priority =  thread_ptr -> tx_priority;

        /* Determine if there are other threads at this priority that are
           ready.  */
        if (thread_ptr -> tx_ready_next != thread_ptr)
        {

            /* Yes, there are other threads at this priority ready.  */
    
            /* Adjust the head pointer if this thread is at the front
               of the list.  */
            if (_tx_thread_priority_list[priority] == thread_ptr)
            {

                /* Setup a new head pointer at this priority.  */
                _tx_thread_priority_list[priority] =  thread_ptr -> tx_ready_next;

                /* Clear the corresponding bit in the preempted map.  */
                _tx_thread_preempted_map =  _tx_thread_preempted_map & ~(thread_ptr -> tx_priority_bit);
            }
    
            /* Just remove this thread from the priority list.  */
            (thread_ptr -> tx_ready_next) -> tx_ready_previous =    thread_ptr -> tx_ready_previous;
            (thread_ptr -> tx_ready_previous) -> tx_ready_next =    thread_ptr -> tx_ready_next;

            /* Setup the new priority bit.  */
            thread_ptr -> tx_priority_bit =     (((ULONG) 1) << new_priority);
        }
        else
        {

            /* This is the only thread at this priority ready to run.  Set the head 
               pointer to NULL.  */
            _tx_thread_priority_list[priority] =    TX_NULL;

            /* Clear this priority bit in the ready priority bit map.  */
            _tx_thread_priority_map =  _tx_thread_priority_map & ~(thread_ptr -> tx_priority_bit);

            /* Clear this priority bit in the preempted bit map just in case.  */
            _tx_thread_preempted_map =  _tx_thread_preempted_map & ~(thread_ptr -> tx_priority_bit);

            /* Setup the new priority bit.  */
            thread_ptr -> tx_priority_bit =  (((ULONG) 1) << new_priority);

            /* Put the priority map in a working copy.  */
            priority_map =  _tx_thread_priority_map;

            /* Find the next highest priority.  */
            if (priority_map & TX_THREAD_GROUP_0_MASK)
            {
                
                /* Setup the priority group to the priority group 0.  */
                priority_group =  (UINT) priority_map;

                /* Set the next highest priority thread.  */
                _tx_thread_highest_priority =  TX_THREAD_GROUP_0 + _tx_thread_lowest_bit[priority_group & TX_THREAD_PRIORITY_GROUP_MASK];
            }
            else if (priority_map & TX_THREAD_GROUP_1_MASK)
            {

                /* Setup the priority group to the priority group 1.  */
                priority_group =  (UINT) (priority_map >> TX_THREAD_GROUP_1);

                /* Set the next highest priority thread.  */
                _tx_thread_highest_priority =  TX_THREAD_GROUP_1 + _tx_thread_lowest_bit[priority_group & TX_THREAD_PRIORITY_GROUP_MASK];
            }
            else if (priority_map & TX_THREAD_GROUP_2_MASK)
            {

                /* Setup the priority group to the priority group 2.  */
                priority_group =  (UINT) (priority_map >> TX_THREAD_GROUP_2);

                /* Set the next highest priority thread.  */
                _tx_thread_highest_priority =  TX_THREAD_GROUP_2 + _tx_thread_lowest_bit[priority_group & TX_THREAD_PRIORITY_GROUP_MASK];
            }
            else if (priority_map & TX_THREAD_GROUP_3_MASK)
            {

                /* Setup the priority group to the priority group 3.  */
                priority_group =  (UINT) (priority_map >> TX_THREAD_GROUP_3);

                 /* Set the next highest priority thread.  */
                _tx_thread_highest_priority =  TX_THREAD_GROUP_3 + _tx_thread_lowest_bit[priority_group & TX_THREAD_PRIORITY_GROUP_MASK];
            }
            else
            {

                /* No more priorities, set highest priority accordingly.  */
                _tx_thread_highest_priority =  TX_MAX_PRIORITIES;
            }
        }

        /* Determine if this thread is the thread designated to execute.  */
        if (thread_ptr == _tx_thread_execute_ptr)
        {

            /* Pickup the highest priority thread to execute, if there is
               one.  */
            if (_tx_thread_highest_priority != TX_MAX_PRIORITIES)

                /* Setup the next thread to execute.  */
                _tx_thread_execute_ptr =  _tx_thread_priority_list[_tx_thread_highest_priority];
            else

                /* No more threads ready to execute, set execute pointer to NULL.  */
                _tx_thread_execute_ptr =  TX_NULL;
        }

        /* Setup the new thread priority.  */
        thread_ptr -> tx_priority =  new_priority;

        /* Setup the new preemption-threshold.  */
        thread_ptr -> tx_preempt_threshold =  new_priority;

        /* Set the state to suspended so the resume function will work. */
        thread_ptr -> tx_state =  TX_SUSPENDED;

        /* Temporarily disable preemption.  */
        _tx_thread_preempt_disable++;
        
        /* Restore interrupts temporarily. */
        TX_RESTORE

        /* Resume this thread.  */
        _tx_thread_resume(thread_ptr);

        /* Determine if a context switch is required.  */
        if ((_tx_thread_current_ptr != _tx_thread_execute_ptr) &&
            (_tx_thread_system_state == 0))

            /* Return control to the system.  */
            _tx_thread_system_return();
    }

    /* Return success if we get here!  */
    return(TX_SUCCESS);
}

