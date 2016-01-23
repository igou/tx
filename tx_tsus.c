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
/*    _tx_thread_suspend                                  PORTABLE C      */ 
/*                                                           4.0b         */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function suspends the specified thread and changes the thread  */ 
/*    state to the value specified.  Note: delayed suspension processing  */ 
/*    is handled outside of this routine.                                 */
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    thread_ptr                            Pointer to thread to suspend  */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    None                                                                */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    _tx_thread_system_return              Return to system if self-     */ 
/*                                            suspension                  */ 
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    ThreadX Components                                                  */ 
/*    _tx_thread_suspend_api                Application thread suspend    */ 
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
/*  03-01-1998     William E. Lamie         Added check to update list    */ 
/*                                            head pointer only when the  */ 
/*                                            first thread is suspending, */ 
/*                                            and added logic to update   */ 
/*                                            and use the preempted map,  */ 
/*                                            resulting in version 3.0d.  */ 
/*  01-01-1999     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0e.  */ 
/*  11-01-1999     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0f.  */ 
/*  01-28-2001     William E. Lamie         Modified comment(s) and       */ 
/*                                            optimized suspension code,  */ 
/*                                            resulting in version 4.0.   */ 
/*  07-15-2002     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 4.0a.  */ 
/*  01-01-2003     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 4.0b.  */ 
/*                                                                        */ 
/**************************************************************************/ 
VOID    _tx_thread_suspend(TX_THREAD *thread_ptr)
{

TX_INTERRUPT_SAVE_AREA
REG_1 UINT      priority;               /* Thread priority          */
REG_2 ULONG     priority_map;           /* Working priority map     */ 
REG_2 UINT      priority_group;         /* Priority group           */


    /* Lockout interrupts while the thread is being suspended.  */
    TX_DISABLE

    /* Decrement the preemption disable variable.  */
    _tx_thread_preempt_disable--;

    /* Check to make sure the thread suspending flag is still set.  If not, it
       has already been resumed.  */
    if (thread_ptr -> tx_suspending)
    {

        /* Actually suspend this thread.  But first, clear the suspending flag.  */
        thread_ptr -> tx_suspending =  TX_FALSE;

        /* Pickup priority of thread.  */
        priority =  thread_ptr -> tx_priority;

        /* Determine if there are other threads at this priority that are
           ready.  */
        if (thread_ptr -> tx_ready_next != thread_ptr)
        {

            /* Yes, there are other threads at this priority ready.  */

            /* Just remove this thread from the priority list.  */
            (thread_ptr -> tx_ready_next) -> tx_ready_previous =    thread_ptr -> tx_ready_previous;
            (thread_ptr -> tx_ready_previous) -> tx_ready_next =    thread_ptr -> tx_ready_next;

            /* Determine if this is the head of the priority list.  */
            if (_tx_thread_priority_list[priority] == thread_ptr)
            {

                /* Update the head pointer of this priority list.  */
                _tx_thread_priority_list[priority] =  thread_ptr -> tx_ready_next;

                /* Check for a thread preempted that had preemption threshold set.  */
                if (_tx_thread_preempted_map)
                {

                    /* Ensure that this thread's priority is clear in the preempt
                       map.  */
                    _tx_thread_preempted_map =  _tx_thread_preempted_map & 
                                                        ~(thread_ptr -> tx_priority_bit);
                }

                /* Determine if this thread is the thread designated to execute.  */
                if (thread_ptr == _tx_thread_execute_ptr)
                {

                    /* Pickup the highest priority thread to execute.  */
                    _tx_thread_execute_ptr =  _tx_thread_priority_list[_tx_thread_highest_priority];
                }
            }
        }
        else
        {

            /* This is the only thread at this priority ready to run.  Set the head 
               pointer to NULL.  */
            _tx_thread_priority_list[priority] =    TX_NULL;

            /* Clear this priority bit in the ready priority bit map.  */
            _tx_thread_priority_map =  _tx_thread_priority_map & ~(thread_ptr -> tx_priority_bit);

            /* Check for a thread preempted that had preemption threshold set.  */
            if (_tx_thread_preempted_map)
            {

                /* Ensure that this thread's priority is clear in the preempt
                   map.  */
                _tx_thread_preempted_map =  _tx_thread_preempted_map & 
                                                ~(thread_ptr -> tx_priority_bit);
            }

            /* Setup working variable.  */
            priority_map =  _tx_thread_priority_map;

            /* Find the next highest priority.  */
            if (priority_map & TX_THREAD_GROUP_0_MASK)
            {
                
                /* Setup the priority group to the priority group 0.  */
                priority_group =  (UINT) priority_map;

                /* Set the next highest priority thread to the group 0 base.  */
                priority =  TX_THREAD_GROUP_0;
            }
            else if (priority_map & TX_THREAD_GROUP_1_MASK)
            {

                /* Setup the priority group to the priority group 1.  */
                priority_group =  (UINT) (priority_map >> TX_THREAD_GROUP_1);

                /* Set the next highest priority thread to the group 1 base.  */
                priority =  TX_THREAD_GROUP_1;
            }
            else if (priority_map & TX_THREAD_GROUP_2_MASK)
            {

                /* Setup the priority group to the priority group 2.  */
                priority_group =  (UINT) (priority_map >> TX_THREAD_GROUP_2);

                /* Set the next highest priority thread to the group 2 base.  */
                priority =  TX_THREAD_GROUP_2;
            }
            else if (priority_map & TX_THREAD_GROUP_3_MASK)
            {

                /* Setup the priority group to the priority group 3.  */
                priority_group =  (UINT) (priority_map >> TX_THREAD_GROUP_3);

                /* Set the next highest priority thread to the group 3 base.  */
                priority =  TX_THREAD_GROUP_3;
            }
            else
            {

                /* Nothing else is ready.  Set highest priority and execute thread
                   accordingly.  */
                _tx_thread_highest_priority =  TX_MAX_PRIORITIES;
                _tx_thread_execute_ptr =       TX_NULL;

                /* Restore interrupts.  */
                TX_RESTORE

                /* Check to see if the caller is a thread.  */
                if (_tx_thread_system_state == 0)
                {

                    /* If so, return control to the system.  */
                    _tx_thread_system_return();
                }

                /* Return to caller.  */
                return;
            }

            /* Setup the next highest priority variable.  */
            _tx_thread_highest_priority =  priority + _tx_thread_lowest_bit[priority_group & TX_THREAD_PRIORITY_GROUP_MASK];        

            /* Determine if this thread is the thread designated to execute.  */
            if (thread_ptr == _tx_thread_execute_ptr)
            {

                /* Pickup the highest priority thread to execute.  */
                _tx_thread_execute_ptr =  _tx_thread_priority_list[_tx_thread_highest_priority];

                /* Determine if a previous thread with preemption threshold was 
                   preempted.  */
                if (_tx_thread_preempted_map)
                {

                    /* Disable preemption.  */
                    _tx_thread_preempt_disable++;

                    /* Restore interrupts.  */
                    TX_RESTORE

                    /* Interrupts are enabled briefly here to keep the interrupt
                       lockout time deterministic.  */

                    /* Disable interrupts again.  */
                    TX_DISABLE

                    /* Decrement the preemption disable variable.  */
                    _tx_thread_preempt_disable--;

                    /* Calculate the thread with preemption threshold set that
                       was interrupted by a thread above the preemption level.  */

                    /* Setup temporary preempted map.  */
                    priority_map =  _tx_thread_preempted_map;

                    /* Find the highest priority preempted thread.  */
                    if (priority_map & TX_THREAD_GROUP_0_MASK)
                    {
                
                        /* Setup the priority group to the priority group 0.  */
                        priority_group =  (UINT) priority_map;

                        /* Set the next highest priority thread to the group 0 base.  */
                        priority =  TX_THREAD_GROUP_0;
                    }
                    else if (priority_map & TX_THREAD_GROUP_1_MASK)
                    {

                        /* Setup the priority group to the priority group 1.  */
                        priority_group =  (UINT) (priority_map >> TX_THREAD_GROUP_1);

                        /* Set the next highest priority thread to the group 1 base.  */
                        priority =  TX_THREAD_GROUP_1;
                    }
                    else if (priority_map & TX_THREAD_GROUP_2_MASK)
                    {

                        /* Setup the priority group to the priority group 2.  */
                        priority_group =  (UINT) (priority_map >> TX_THREAD_GROUP_2);

                        /* Set the next highest priority thread to the group 2 base.  */
                        priority =  TX_THREAD_GROUP_2;
                    }
                    else
                    {

                        /* Setup the priority group to the priority group 3.  */
                        priority_group =  (UINT) (priority_map >> TX_THREAD_GROUP_3);

                        /* Set the next highest priority thread to the group 3 base.  */
                        priority =  TX_THREAD_GROUP_3;
                    }
            
                    /* Setup the highest priority preempted thread.  */
                    priority =  priority + _tx_thread_lowest_bit[priority_group & TX_THREAD_PRIORITY_GROUP_MASK];       

                    /* Determine if the next highest priority thread is above the highest
                       priority threshold value.  */
                    if (_tx_thread_highest_priority >= 
                                    (_tx_thread_priority_list[priority] -> tx_preempt_threshold))
                    {

                        /* Thread not allowed to execute until earlier preempted thread 
                           finishes or lowers its preemption threshold.  */
                        _tx_thread_execute_ptr =  _tx_thread_priority_list[priority];

                        /* Clear the corresponding bit in the preempted map, since the
                           preemption has been restored.  */
                        _tx_thread_preempted_map =  
                                _tx_thread_preempted_map & ~(_tx_thread_execute_ptr -> tx_priority_bit);
                    }
                }
            }
        }
    }

    /* Restore interrupts.  */
    TX_RESTORE

    /* Check for a preemption condition that might have occurred from an ISR.  */
    if ((_tx_thread_current_ptr != _tx_thread_execute_ptr) && (_tx_thread_system_state == 0))
    {
        /* Return control to the system.  */
        _tx_thread_system_return();
    }
}

