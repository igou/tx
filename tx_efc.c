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
/**   Event Flags (EVE)                                                   */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define TX_SOURCE_CODE


/* Include necessary system files.  */

#include    "tx_api.h"
#include    "tx_eve.h"


/**************************************************************************/
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _tx_event_flags_create                              PORTABLE C      */ 
/*                                                           4.0b         */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function creates a group of 32 event flags.  All the flags are */ 
/*    initially in a cleared state.                                       */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    group_ptr                         Pointer to event flags group      */ 
/*                                        control block                   */ 
/*    name_ptr                          Pointer to event flags name       */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    TX_SUCCESS                        Successful completion status      */ 
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
UINT    _tx_event_flags_create(TX_EVENT_FLAGS_GROUP *group_ptr, CHAR *name_ptr)
{

TX_INTERRUPT_SAVE_AREA

TX_EVENT_FLAGS_GROUP   *tail_ptr;           /* Working event flags pointer  */


    /* Setup the basic event flags group fields.  */
    group_ptr -> tx_event_flags_name =             name_ptr;
    group_ptr -> tx_event_flags_current =          0;
    group_ptr -> tx_event_flags_suspension_list =  TX_NULL;
    group_ptr -> tx_event_flags_suspended_count =  0;
    group_ptr -> tx_event_flags_reset_search =     0;
    
    /* Disable interrupts to put the event flags group on the created list.  */
    TX_DISABLE

    /* Setup the event flags ID to make it valid.  */
    group_ptr -> tx_event_flags_id =  TX_EVENT_FLAGS_ID;

    /* Place the group on the list of created event flag groups.  First,
       check for an empty list.  */
    if (_tx_event_flags_created_ptr)
    {

        /* Pickup tail pointer.  */
        tail_ptr =  _tx_event_flags_created_ptr -> tx_event_flags_created_previous;

        /* Place the new event flag group in the list.  */
        _tx_event_flags_created_ptr -> tx_event_flags_created_previous =  group_ptr;
        tail_ptr -> tx_event_flags_created_next =  group_ptr;

        /* Setup this group's created links.  */
        group_ptr -> tx_event_flags_created_previous =  tail_ptr;
        group_ptr -> tx_event_flags_created_next =      _tx_event_flags_created_ptr;    
    }
    else
    {

        /* The created event flags list is empty.  Add event flag group to empty list.  */
        _tx_event_flags_created_ptr =                   group_ptr;
        group_ptr -> tx_event_flags_created_next =      group_ptr;
        group_ptr -> tx_event_flags_created_previous =  group_ptr;
    }

    /* Increment the number of event flag groups created.  */
    _tx_event_flags_created_count++;

    /* Restore interrupts.  */
    TX_RESTORE

    /* Return TX_SUCCESS.  */
    return(TX_SUCCESS);
}

