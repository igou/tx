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
/*    _tx_thread_delete                                   PORTABLE C      */ 
/*                                                           4.0b         */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function handles application delete thread requests.  The      */ 
/*    thread to delete must be in a terminated or completed state,        */ 
/*    otherwise this function just returns an error code.                 */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    thread_ptr                            Pointer to thread to suspend  */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    status                                Return completion status      */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    None                                                                */ 
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    Application code                                                    */ 
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
/*  07-15-2002     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 4.0a.  */ 
/*  01-01-2003     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 4.0b.  */ 
/*                                                                        */ 
/**************************************************************************/ 
UINT    _tx_thread_delete(TX_THREAD *thread_ptr)
{

TX_INTERRUPT_SAVE_AREA

UINT    status =  TX_SUCCESS;


    /* Check the specified thread's current status.  */
    if ((thread_ptr -> tx_state == TX_COMPLETED) || 
                                (thread_ptr -> tx_state == TX_TERMINATED)) 
    {

        /* Lockout interrupts while the thread is being deleted.  */
        TX_DISABLE

        /* Clear the thread ID to make it invalid.  */
        thread_ptr -> tx_thread_id =  0;

        /* See if the thread is the only one on the list.  */
        if (thread_ptr == thread_ptr -> tx_created_next)
        {

            /* Only created thread, just set the created list to NULL.  */
            _tx_thread_created_ptr =  TX_NULL;
        }
        else
        {

            /* Otherwise, not the only created thread, link-up the neighbors.  */
            (thread_ptr -> tx_created_next) -> tx_created_previous =
                                            thread_ptr -> tx_created_previous;
            (thread_ptr -> tx_created_previous) -> tx_created_next =
                                            thread_ptr -> tx_created_next;

            /* See if we have to update the created list head pointer.  */
            if (_tx_thread_created_ptr == thread_ptr)
            
                /* Yes, move the head pointer to the next link. */
                _tx_thread_created_ptr =  thread_ptr -> tx_created_next; 
        }

        /* Decrement the thread created counter.  */
        _tx_thread_created_count--;

        /* Restore interrupts.  */
        TX_RESTORE
    }
    else

        /* Invalid thread deletion.  */
        status =  TX_DELETE_ERROR;

    /* Return completion status.  */
    return(status);
}

