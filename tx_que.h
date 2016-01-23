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


/**************************************************************************/ 
/*                                                                        */ 
/*  COMPONENT DEFINITION                                   RELEASE        */ 
/*                                                                        */ 
/*    tx_que.h                                            PORTABLE C      */ 
/*                                                           4.0b         */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This file defines the ThreadX queue management component,           */ 
/*    including all data types and external references.  It is assumed    */ 
/*    that tx_api.h and tx_port.h have already been included.             */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */ 
/*                                                                        */ 
/*  12-31-1996     William E. Lamie         Initial Version 3.0           */ 
/*  07-04-1997     William E. Lamie         Removed message copy typedefs */ 
/*                                            since code caused several   */ 
/*                                            compiler warnings,          */ 
/*                                            resulting in version 3.0a.  */ 
/*  11-11-1997     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0b.  */ 
/*  01-01-1999     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0e.  */ 
/*  11-01-1999     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0f.  */ 
/*  01-28-2001     William E. Lamie         Modified comment(s), added    */ 
/*                                            in-line queue initialize    */ 
/*                                            capability, and new queue   */ 
/*                                            services, resulting in      */ 
/*                                            version 4.0.                */ 
/*  07-15-2002     William E. Lamie         Modified comment(s), changed  */ 
/*                                            file included name, and     */ 
/*                                            added conditional around    */ 
/*                                            the in-line initialization, */ 
/*                                            resulting in version 4.0a.  */ 
/*  01-01-2003     William E. Lamie         Modified comment(s), and      */ 
/*                                            modified the in-line        */ 
/*                                            initialization code,        */ 
/*                                            resulting in version 4.0b.  */ 
/*                                                                        */ 
/**************************************************************************/ 

#ifndef  TX_QUE_H
#define  TX_QUE_H


/* Define queue control specific data definitions.  */

#define TX_QUEUE_ID                     0x51554555UL


/* Determine if in-line component initialization is supported by the 
   caller.  */
#ifdef      TX_INVOKE_INLINE_INITIALIZATION
            /* Yes, in-line initialization is supported, remap the 
               queue initialization function.  */
#define     _tx_queue_initialize() \
                    _tx_queue_created_ptr =            TX_NULL; \
                    _tx_queue_created_count =          0; 
#define     TX_QUEUE_INIT
#else
            /* No in-line initialization is supported, use standard 
               function call.  */
VOID        _tx_queue_initialize(VOID);
#endif


/* Define queue management function prototypes.  */

UINT        _tx_queue_create(TX_QUEUE *queue_ptr, CHAR *name_ptr, UINT message_size, 
                        VOID *queue_start, ULONG queue_size);
UINT        _tx_queue_delete(TX_QUEUE *queue_ptr);
UINT        _tx_queue_flush(TX_QUEUE *queue_ptr);
UINT        _tx_queue_info_get(TX_QUEUE *queue_ptr, CHAR **name, ULONG *enqueued, ULONG *available_storage,
                    TX_THREAD **first_suspended, ULONG *suspended_count, TX_QUEUE **next_queue);
UINT        _tx_queue_prioritize(TX_QUEUE *queue_ptr);
UINT        _tx_queue_receive(TX_QUEUE *queue_ptr, VOID *destination_ptr, ULONG wait_option);
UINT        _tx_queue_send(TX_QUEUE *queue_ptr, VOID *source_ptr, ULONG wait_option);
UINT        _tx_queue_front_send(TX_QUEUE *queue_ptr, VOID *source_ptr, ULONG wait_option);
VOID        _tx_queue_cleanup(TX_THREAD *thread_ptr);

/* Define error checking shells for API services.  These are only referenced by the 
   application.  */

UINT        _txe_queue_create(TX_QUEUE *queue_ptr, CHAR *name_ptr, UINT message_size, 
                        VOID *queue_start, ULONG queue_size);
UINT        _txe_queue_delete(TX_QUEUE *queue_ptr);
UINT        _txe_queue_flush(TX_QUEUE *queue_ptr);
UINT        _txe_queue_info_get(TX_QUEUE *queue_ptr, CHAR **name, ULONG *enqueued, ULONG *available_storage,
                    TX_THREAD **first_suspended, ULONG *suspended_count, TX_QUEUE **next_queue);
UINT        _txe_queue_prioritize(TX_QUEUE *queue_ptr);
UINT        _txe_queue_receive(TX_QUEUE *queue_ptr, VOID *destination_ptr, ULONG wait_option);
UINT        _txe_queue_send(TX_QUEUE *queue_ptr, VOID *source_ptr, ULONG wait_option);
UINT        _txe_queue_front_send(TX_QUEUE *queue_ptr, VOID *source_ptr, ULONG wait_option);


/* Queue management component data declarations follow.  */

/* Determine if the initialization function of this component is including
   this file.  If so, make the data definitions really happen.  Otherwise,
   make them extern so other functions in the component can access them.  */

#ifdef  TX_QUEUE_INIT
#define QUEUE_DECLARE 
#else
#define QUEUE_DECLARE extern
#endif


/* Define the head pointer of the created queue list.  */

QUEUE_DECLARE  TX_QUEUE *   _tx_queue_created_ptr;


/* Define the variable that holds the number of created queues. */

QUEUE_DECLARE  ULONG        _tx_queue_created_count;


#endif
