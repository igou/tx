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
/*    _tx_time_get                                        PORTABLE C      */ 
/*                                                           4.0b         */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function retrieves the internal, free-running, system clock    */ 
/*    and returns it to the caller.                                       */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    None                                                                */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    _tx_timer_system_clock            Returns the system clock value    */ 
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
ULONG   _tx_time_get(VOID)
{

TX_INTERRUPT_SAVE_AREA

ULONG   temp_time;

    /* Disable interrupts.  */
    TX_DISABLE

    /* Pickup the system clock time.  */
    temp_time =  _tx_timer_system_clock;

    /* Restore interrupts.  */
    TX_RESTORE

    /* Return the time.  */
    return(temp_time);
}

