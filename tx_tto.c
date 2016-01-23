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
/*    _tx_thread_timeout                                  PORTABLE C      */ 
/*                                                           4.0b         */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function handles thread timeout processing.  Timeouts occur in */ 
/*    two flavors, namely the thread sleep timeout and all other service  */ 
/*    call timeouts.  Thread sleep timeouts are processed locally, while  */ 
/*    the others are processed by the appropriate suspension clean-up     */ 
/*    service.                                                            */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    timeout_input                         Contains the thread pointer   */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    None                                                                */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    Suspension Cleanup Functions                                        */ 
/*    _tx_thread_resume                                                   */ 
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    _tx_timer_thread_entry                Timer thread function         */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */ 
/*                                                                        */ 
/*  12-31-1996     William E. Lamie         Initial Version 3.0           */ 
/*  11-11-1997     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0b.  */ 
/*  01-01-1999     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0e.  */ 
/*  11-01-1999     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0f.  */ 
/*  01-28-2001     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 4.0.   */ 
/*  07-15-2002     William E. Lamie         Modified comment(s) and       */ 
/*                                            optimized out extra disable */ 
/*                                            interrupts call, resulting  */ 
/*                                            in version 4.0a.            */ 
/*  01-01-2003     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 4.0b.  */ 
/*                                                                        */ 
/**************************************************************************/ 
VOID    _tx_thread_timeout(ULONG timeout_input)
{

TX_INTERRUPT_SAVE_AREA

TX_THREAD       *thread_ptr;                /* Pointer to thread          */
VOID            (*suspend_cleanup)(struct TX_THREAD_STRUCT *);

    /* Pickup the thread pointer.  */
    thread_ptr =  (TX_THREAD *) timeout_input;

    /* Disable interrupts.  */
    TX_DISABLE

    /* Determine how the thread is currently suspended.  */
    if (thread_ptr -> tx_state == TX_SLEEP)
    {

        /* Increment the disable preemption flag.  */
        _tx_thread_preempt_disable++;

        /* Restore interrupts.  */
        TX_RESTORE

        /* Lift the suspension on the sleeping thread.  */
        _tx_thread_resume(thread_ptr);
    }
    else
    {

        /* Process all other suspension timeouts.  */
    
        /* Pickup the cleanup routine address.  */
        suspend_cleanup =  thread_ptr -> tx_suspend_cleanup;

        /* Restore interrupts.  */
        TX_RESTORE

        /* Call any cleanup routines.  */
        if (suspend_cleanup)

            /* Yes, there is a function to call.  */
            (suspend_cleanup)(thread_ptr);
    }
}
