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
/**   Queue (QUE)                                                         */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define TX_SOURCE_CODE


/* Include necessary system files.  */

#include    "tx_api.h"
#include    "tx_thr.h"
#include    "tx_que.h"


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _txe_queue_info_get                                 PORTABLE C      */ 
/*                                                           4.0b         */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function checks for errors in the queue information get        */ 
/*    service.                                                            */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    queue_ptr                         Pointer to queue control block    */ 
/*    name                              Destination for the queue name    */ 
/*    enqueued                          Destination for enqueued count    */ 
/*    available_storage                 Destination for available storage */ 
/*    first_suspended                   Destination for pointer of first  */ 
/*                                        thread suspended on this queue  */ 
/*    suspended_count                   Destination for suspended count   */ 
/*    next_queue                        Destination for pointer to next   */ 
/*                                        queue on the created list       */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    TX_QUEUE_ERROR                    Invalid queue pointer             */ 
/*    TX_PTR_ERROR                      Invalid destination pointer (NULL)*/ 
/*    status                            Completion status                 */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    _tx_queue_info_get                Actual information get service    */ 
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
UINT    _txe_queue_info_get(TX_QUEUE *queue_ptr, CHAR **name, ULONG *enqueued, ULONG *available_storage,
                    TX_THREAD **first_suspended, ULONG *suspended_count, TX_QUEUE **next_queue)
{

UINT    status;


    /* First, check for an invalid queue pointer.  */
    if ((!queue_ptr) || (queue_ptr -> tx_queue_id != TX_QUEUE_ID))

        /* Queue pointer is invalid, return appropriate error code.  */
        return(TX_QUEUE_ERROR);

    /* Next, check for NULL destination pointers in the arguments.  */
    if ((name == TX_NULL) || (enqueued == TX_NULL) || (available_storage == TX_NULL) || 
        (first_suspended == TX_NULL) || (suspended_count == TX_NULL) ||
        (next_queue == TX_NULL))

        /* One or more of the supplied destination pointers are NULL (invalid).  */
        return(TX_PTR_ERROR);

    /* Otherwise, call the actual queue information get service.  */
    status =  _tx_queue_info_get(queue_ptr, name, enqueued, available_storage, first_suspended, 
                                                            suspended_count, next_queue);

    /* Return completion status.  */
    return(status);
}

