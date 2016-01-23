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
#include    "tx_ini.h"
#include    "tx_thr.h"
#include    "tx_tim.h"


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _txe_timer_create                                   PORTABLE C      */ 
/*                                                           4.0b         */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function checks for errors in the create application timer     */ 
/*    function call.                                                      */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    timer_ptr                         Pointer to timer control block    */ 
/*    name_ptr                          Pointer to timer name             */ 
/*    expiration_function               Application expiration function   */ 
/*    initial_ticks                     Initial expiration ticks          */ 
/*    reschedule_ticks                  Reschedule ticks                  */ 
/*    auto_activate                     Automatic activation flag         */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    TX_TIMER_ERROR                    Invalid timer control block       */ 
/*    TX_TICK_ERROR                     Invalid initial expiration count  */ 
/*    TX_ACTIVATE_ERROR                 Invalid timer activation option   */ 
/*    TX_CALLER_ERROR                   Invalid caller of this function   */ 
/*    status                            Actual completion status          */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    _tx_timer_create                  Actual timer create function      */ 
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
UINT    _txe_timer_create(TX_TIMER *timer_ptr, CHAR *name_ptr, 
            VOID (*expiration_function)(ULONG), ULONG expiration_input,
            ULONG initial_ticks, ULONG reschedule_ticks, UINT auto_activate)
{

REG_1   UINT        status;                 /* Return status           */


    /* First, check for an invalid timer pointer.  */
    if ((!timer_ptr) || (timer_ptr -> tx_timer_id == TX_TIMER_ID))

        /* Timer pointer is invalid, return appropriate error code.  */
        return(TX_TIMER_ERROR);

    /* Check for an illegal initial tick value.  */
    if (!initial_ticks)

        /* Invalid initial tick value, return appropriate error code.  */
        return(TX_TICK_ERROR);

    /* Check for an illegal activation.  */
    if ((auto_activate != TX_AUTO_ACTIVATE) && (auto_activate != TX_NO_ACTIVATE))

        /* Invalid activation selected, return appropriate error code.  */
        return(TX_ACTIVATE_ERROR);

    /* Check for invalid caller of this function.  */
    if (((!_tx_thread_current_ptr) &&
        ((_tx_thread_system_state & 0xFFFFFFF0UL) != TX_INITIALIZE_IN_PROGRESS)) ||
        ((_tx_thread_current_ptr) && (_tx_thread_system_state)) ||
        (_tx_thread_current_ptr == &_tx_timer_thread))

        /* Invalid caller of this function, return appropriate error code.  */
        return(TX_CALLER_ERROR);

    /* Call actual application timer create function.  */
    status =  _tx_timer_create(timer_ptr, name_ptr, expiration_function, expiration_input,
                                                initial_ticks, reschedule_ticks, auto_activate);

    /* Return actual completion status.  */
    return(status);
}

