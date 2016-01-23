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
/**   Timer Management (TIM)                                              */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define TX_SOURCE_CODE


/* Include necessary system files.  */

#include    "tx_api.h"
#include    "tx_tim.h"


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _tx_timer_deactivate_api                            PORTABLE C      */ 
/*                                                           4.0b         */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function deactivates the specified application timer.          */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    timer_ptr                         Pointer to timer control block    */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    TX_SUCCESS                        Always returns success            */ 
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
/*  12-31-1996     William E. Lamie         Initial Version 3.0           */ 
/*  11-11-1997     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0b.  */ 
/*  12-25-1997     William E. Lamie         Added logic to calculate and  */ 
/*                                            save the remaining time     */ 
/*                                            left before expiration,     */ 
/*                                            resulting in version 3.0c.  */ 
/*  03-01-1998     William E. Lamie         Replaced the timer deactivate */ 
/*                                            call with in-line logic,    */ 
/*                                            resulting in version 3.0d.  */ 
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
UINT    _tx_timer_deactivate_api(TX_TIMER *timer_ptr)
{
TX_INTERRUPT_SAVE_AREA

REG_1  TX_INTERNAL_TIMER    *internal_ptr;              /* Internal timer pointer       */ 
REG_2  ULONG                ticks_left;                 /* Ticks left before expiration */ 

    
    /* Setup internal timer pointer.  */
    internal_ptr =  &(timer_ptr -> tx_timer_internal);

    /* Disable interrupts while the remaining time before expiration is
       calculated.  */
    TX_DISABLE

    /* Determine if the head pointer is within the timer expiration list.  */
    if ((internal_ptr -> tx_list_head >= _tx_timer_list_start) &&
        (internal_ptr -> tx_list_head < _tx_timer_list_end))
    {

        /* This timer is active and has not yet expired.  */

        /* Calculate the amount of time that has elapsed since the timer
           was activated.  */

        /* Is this timer's entry after the current timer pointer?  */
        if (internal_ptr -> tx_list_head >= _tx_timer_current_ptr)
        {

            /* Calculate ticks left to expiration - just the difference between this 
               timer's entry and the current timer pointer.  */
            ticks_left =  (internal_ptr -> tx_list_head - _tx_timer_current_ptr) + 1;
        }
        else
        {

            /* Calculate the ticks left with a wrapped list condition.  */
            ticks_left =  (internal_ptr -> tx_list_head - _tx_timer_list_start);
            ticks_left =  ticks_left + (_tx_timer_list_end - _tx_timer_current_ptr) + 1;
        }

        /* Adjust the remaining ticks accordingly.  */
        if (internal_ptr -> tx_remaining_ticks > TX_TIMER_ENTRIES)
        {
            
            /* Subtract off the last full pass through the timer list and add the
               time left.  */
            internal_ptr -> tx_remaining_ticks =  
                    (internal_ptr -> tx_remaining_ticks - TX_TIMER_ENTRIES) + ticks_left;
        }
        else
        {

            /* Just put the ticks left into the timer's remaining ticks.  */
            internal_ptr -> tx_remaining_ticks =  ticks_left;
        }

    }

    /* Determine if the timer still needs deactivation.  */
    if (internal_ptr -> tx_list_head)
    {

        /* See if this is the only timer in the list.  */
        if (internal_ptr == internal_ptr -> tx_active_next)
        {

            /* Yes, the only timer on the list.  */

            /* Determine if the head pointer needs to be updated.  */
            if (*(internal_ptr -> tx_list_head) == internal_ptr)
            {

                /* Update the head pointer.  */
                *(internal_ptr -> tx_list_head) =  TX_NULL;
            }

            /* Clear the timer's list head pointer.  */
            internal_ptr -> tx_list_head =  TX_NULL;
        }
        else
        {

            /* At least one more timer is on the same expiration list.  */

            /* Update the links of the adjacent timers.  */
            (internal_ptr -> tx_active_next) -> tx_active_previous =  
                                                    internal_ptr -> tx_active_previous;
            (internal_ptr -> tx_active_previous) -> tx_active_next =
                                                    internal_ptr -> tx_active_next;

            /* Determine if the head pointer needs to be updated.  */
            if (*(internal_ptr -> tx_list_head) == internal_ptr)
            {

                /* Update the next timer in the list with the list head 
                   pointer.  */
                (internal_ptr -> tx_active_next) -> tx_list_head =  internal_ptr -> tx_list_head;

                /* Update the head pointer.  */
                *(internal_ptr -> tx_list_head) =  internal_ptr -> tx_active_next;
            }

            /* Clear the timer's list head pointer.  */
            internal_ptr -> tx_list_head =  TX_NULL;
        }
    }

    /* Restore interrupts to previous posture.  */
    TX_RESTORE

    /* Return TX_SUCCESS.  */
    return(TX_SUCCESS);
}

