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


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _tx_thread_resume_api                               PORTABLE C      */ 
/*                                                           4.0b         */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function processes application resume thread services.  Actual */ 
/*    thread resumption is performed in the core service.                 */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    thread_ptr                            Pointer to thread to resume   */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    status                                Service return status         */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    _tx_thread_resume                     Resume thread                 */ 
/*    _tx_thread_system_return              Return to system              */ 
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
/*  11-11-1997     William E. Lamie         Added clearing of the delayed */ 
/*                                            suspension flag if thread   */ 
/*                                            requested is suspended and  */ 
/*                                            has delayed suspension set  */ 
/*                                            and modified comments,      */ 
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
UINT    _tx_thread_resume_api(TX_THREAD *thread_ptr)
{

TX_INTERRUPT_SAVE_AREA

UINT    status;                         /* Return status  */


    /* Lockout interrupts while the thread is being resumed.  */
    TX_DISABLE

    /* Determine if the thread is suspended or in the process of suspending.
       If so, call the thread resume processing.  */
    if (thread_ptr -> tx_state == TX_SUSPENDED) 
    {

        /* Temporarily disable preemption.  */
        _tx_thread_preempt_disable++;

        /* Restore interrupts.  */
        TX_RESTORE

        /* Call the actual resume service to process the request.  */
        if (_tx_thread_resume(thread_ptr))

            /* If resume service returns a TX_TRUE, preemption needs to 
               take place.  */
            _tx_thread_system_return();

        /* Setup successful return status.  */
        status =  TX_SUCCESS;
    }
    else if (thread_ptr -> tx_delayed_suspend)
    {

        /* Clear the delayed suspension.  */
        thread_ptr -> tx_delayed_suspend =  TX_FALSE;

        /* Restore interrupts.  */
        TX_RESTORE

        /* Setup delayed suspend lifted return status.  */
        status =  TX_SUSPEND_LIFTED;
    }
    else
    {

        /* Restore interrupts.  */
        TX_RESTORE

        /* Setup invalid resume return status.  */
        status =  TX_RESUME_ERROR;
    }

    /* Return completion status. */
    return(status);
}

