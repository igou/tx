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
/**   Thread Control                                                      */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define TX_SOURCE_CODE


/* Include necessary system files.  */

#include    "tx_api.h"
#include    "tx_thr.h"
#include    "tx_tim.h"


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _txe_thread_time_slice_change                       PORTABLE C      */ 
/*                                                           4.0b         */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function checks for errors in the time slice change function   */ 
/*    call.                                                               */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    thread_ptr                            Pointer to thread             */ 
/*    new_time_slice                        New time slice                */ 
/*    old_time_slice                        Old time slice                */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    TX_THREAD_ERROR                       Invalid thread pointer        */ 
/*    TX_CALLER_ERROR                       Invalid caller of function    */ 
/*    status                                Actual completion status      */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    _tx_thread_time_slice_change          Actual time-slice change      */ 
/*                                            function                    */ 
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
UINT    _txe_thread_time_slice_change(TX_THREAD *thread_ptr, ULONG new_time_slice,
                    ULONG *old_time_slice)
{

REG_1   UINT        status;                 /* Return status           */


    /* First, check for an invalid thread pointer.  */
    if ((!thread_ptr) || (thread_ptr -> tx_thread_id != TX_THREAD_ID))

        /* Thread pointer is invalid, return appropriate error code.  */
        return(TX_THREAD_ERROR);

    /* Check for a valid old time-slice pointer.  */
    if (!old_time_slice)

        /* Invalid destination pointer, return appropriate error code.  */
        return(TX_PTR_ERROR);

    /* Check for invalid caller of this function.  */
    if ((!_tx_thread_current_ptr) || (_tx_thread_system_state))

        /* Invalid caller of this function, return appropriate error code.  */
        return(TX_CALLER_ERROR);

    /* Call actual change time slice function.  */
    status =  _tx_thread_time_slice_change(thread_ptr, new_time_slice, old_time_slice);

    /* Return actual completion status.  */
    return(status);
}
