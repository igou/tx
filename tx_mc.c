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
/**   Mutex (MUT)                                                         */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define TX_SOURCE_CODE


/* Include necessary system files.  */

#include    "tx_api.h"
#include    "tx_mut.h"


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _tx_mutex_create                                    PORTABLE C      */ 
/*                                                           4.0b         */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function creates a mutex with optional priority inheritance as */ 
/*    specified in this call.                                             */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    mutex_ptr                             Pointer to mutex control block*/ 
/*    name_ptr                              Pointer to mutex name         */ 
/*    inherit                               Priority inheritance option   */ 
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
/*  01-28-2001     William E. Lamie         Initial Version 4.0           */ 
/*  07-15-2002     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 4.0a.  */ 
/*  01-01-2003     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 4.0b.  */ 
/*                                                                        */ 
/**************************************************************************/ 
UINT    _tx_mutex_create(TX_MUTEX *mutex_ptr, CHAR *name_ptr, UINT inherit)
{

TX_INTERRUPT_SAVE_AREA

TX_MUTEX   *tail_ptr;                   /* Working mutex pointer  */


    /* Setup the basic mutex fields.  */
    mutex_ptr -> tx_mutex_name =             name_ptr;
    mutex_ptr -> tx_mutex_ownership_count =  0;
    mutex_ptr -> tx_mutex_inherit =          inherit;
    mutex_ptr -> tx_mutex_suspension_list =  TX_NULL;
    mutex_ptr -> tx_mutex_suspended_count =  0;
    
    /* Disable interrupts to place the mutex on the created list.  */
    TX_DISABLE

    /* Setup the mutex ID to make it valid.  */
    mutex_ptr -> tx_mutex_id =  TX_MUTEX_ID;

    /* Place the mutex on the list of created mutexes.  First,
       check for an empty list.  */
    if (_tx_mutex_created_ptr)
    {

        /* Pickup tail pointer.  */
        tail_ptr =  _tx_mutex_created_ptr -> tx_mutex_created_previous;

        /* Place the new mutex in the list.  */
        _tx_mutex_created_ptr -> tx_mutex_created_previous =  mutex_ptr;
        tail_ptr -> tx_mutex_created_next =                   mutex_ptr;

        /* Setup this mutex's next and previous created links.  */
        mutex_ptr -> tx_mutex_created_previous =  tail_ptr;
        mutex_ptr -> tx_mutex_created_next =      _tx_mutex_created_ptr;    
    }
    else
    {

        /* The created mutex list is empty.  Add mutex to empty list.  */
        _tx_mutex_created_ptr =                   mutex_ptr;
        mutex_ptr -> tx_mutex_created_next =      mutex_ptr;
        mutex_ptr -> tx_mutex_created_previous =  mutex_ptr;
    }

    /* Increment the number of mutexes created counter.  */
    _tx_mutex_created_count++;

    /* Restore interrupts.  */
    TX_RESTORE

    /* Return TX_SUCCESS.  */
    return(TX_SUCCESS);
}

