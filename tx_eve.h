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


/**************************************************************************/ 
/*                                                                        */ 
/*  COMPONENT DEFINITION                                   RELEASE        */ 
/*                                                                        */ 
/*    tx_eve.h                                            PORTABLE C      */ 
/*                                                           4.0b         */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This file defines the ThreadX event flags management component,     */ 
/*    including all data types and external references.  It is assumed    */ 
/*    that tx_api.h and tx_port.h have already been included.             */ 
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
/*  01-28-2001     William E. Lamie         Modified comment(s), added    */ 
/*                                            in-line event flag init     */ 
/*                                            capability, and new event   */ 
/*                                            flag services, resulting in */ 
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

#ifndef  TX_EVE_H
#define  TX_EVE_H


/* Define event flags control specific data definitions.  */

#define TX_EVENT_FLAGS_ID           0x4456444EUL
#define TX_EVENT_FLAGS_AND_MASK     0x2
#define TX_EVENT_FLAGS_CLEAR_MASK   0x1


/* Determine if in-line component initialization is supported by the 
   caller.  */
#ifdef      TX_INVOKE_INLINE_INITIALIZATION
            /* Yes, in-line initialization is supported, remap the 
               event flag initialization function.  */
#define     _tx_event_flags_initialize() \
                    _tx_event_flags_created_ptr =      TX_NULL; \
                    _tx_event_flags_created_count =    0;
#define     TX_EVENT_FLAGS_INIT
#else
            /* No in-line initialization is supported, use standard 
               function call.  */
VOID        _tx_event_flags_initialize(VOID);
#endif


/* Define event flags management function prototypes.  */

UINT        _tx_event_flags_create(TX_EVENT_FLAGS_GROUP *group_ptr, CHAR *name_ptr);
UINT        _tx_event_flags_delete(TX_EVENT_FLAGS_GROUP *group_ptr);
UINT        _tx_event_flags_get(TX_EVENT_FLAGS_GROUP *group_ptr, ULONG requested_flags,
                    UINT get_option, ULONG *actual_flags_ptr, ULONG wait_option);
UINT        _tx_event_flags_info_get(TX_EVENT_FLAGS_GROUP *group_ptr, CHAR **name, ULONG *current_flags, 
                    TX_THREAD **first_suspended, ULONG *suspended_count, 
                    TX_EVENT_FLAGS_GROUP **next_group);
UINT        _tx_event_flags_set(TX_EVENT_FLAGS_GROUP *group_ptr, ULONG flags_to_set, 
                    UINT set_option);
VOID        _tx_event_flags_cleanup(TX_THREAD *thread_ptr);


/* Define error checking shells for API services.  These are only referenced by the 
   application.  */

UINT        _txe_event_flags_create(TX_EVENT_FLAGS_GROUP *group_ptr, CHAR *name_ptr);
UINT        _txe_event_flags_delete(TX_EVENT_FLAGS_GROUP *group_ptr);
UINT        _txe_event_flags_get(TX_EVENT_FLAGS_GROUP *group_ptr, ULONG requested_flags,
                    UINT get_option, ULONG *actual_flags_ptr, ULONG wait_option);
UINT        _txe_event_flags_info_get(TX_EVENT_FLAGS_GROUP *group_ptr, CHAR **name, ULONG *current_flags, 
                    TX_THREAD **first_suspended, ULONG *suspended_count, 
                    TX_EVENT_FLAGS_GROUP **next_group);
UINT        _txe_event_flags_set(TX_EVENT_FLAGS_GROUP *group_ptr, ULONG flags_to_set, 
                    UINT set_option);


/* Event flags management component data declarations follow.  */

/* Determine if the initialization function of this component is including
   this file.  If so, make the data definitions really happen.  Otherwise,
   make them extern so other functions in the component can access them.  */

#ifdef  TX_EVENT_FLAGS_INIT
#define EVENT_FLAGS_DECLARE 
#else
#define EVENT_FLAGS_DECLARE extern
#endif


/* Define the head pointer of the created event flags list.  */

EVENT_FLAGS_DECLARE  TX_EVENT_FLAGS_GROUP *   _tx_event_flags_created_ptr;


/* Define the variable that holds the number of created event flag groups. */

EVENT_FLAGS_DECLARE  ULONG                    _tx_event_flags_created_count;


#endif
