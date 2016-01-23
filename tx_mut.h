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


/**************************************************************************/ 
/*                                                                        */ 
/*  COMPONENT DEFINITION                                   RELEASE        */ 
/*                                                                        */ 
/*    tx_mut.h                                            PORTABLE C      */ 
/*                                                           4.0b         */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This file defines the ThreadX mutex management component,           */ 
/*    including all data types and external references.  It is assumed    */ 
/*    that tx_api.h and tx_port.h have already been included.             */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */ 
/*                                                                        */ 
/*  01-28-2001     William E. Lamie         Initial Version 4.0           */ 
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

#ifndef  TX_MUT_H
#define  TX_MUT_H


/* Define mutex control specific data definitions.  */

#define TX_MUTEX_ID                     0x4D555445UL


/* Determine if in-line component initialization is supported by the 
   caller.  */
#ifdef      TX_INVOKE_INLINE_INITIALIZATION
            /* Yes, in-line initialization is supported, remap the 
               mutex initialization function.  */
#define     _tx_mutex_initialize() \
                    _tx_mutex_created_ptr =        TX_NULL; \
                    _tx_mutex_created_count =      0; 
#define     TX_MUTEX_INIT
#else
            /* No in-line initialization is supported, use standard 
               function call.  */
VOID        _tx_mutex_initialize(VOID);
#endif


/* Define mutex management function prototypes.  */

UINT        _tx_mutex_create(TX_MUTEX *mutex_ptr, CHAR *name_ptr, UINT inherit);
UINT        _tx_mutex_delete(TX_MUTEX *mutex_ptr);
UINT        _tx_mutex_get(TX_MUTEX *mutex_ptr, ULONG wait_option);
UINT        _tx_mutex_info_get(TX_MUTEX *mutex_ptr, CHAR **name, ULONG *count, TX_THREAD **owner, 
                    TX_THREAD **first_suspended, ULONG *suspended_count, 
                    TX_MUTEX **next_mutex);
UINT        _tx_mutex_prioritize(TX_MUTEX *mutex_ptr);
UINT        _tx_mutex_put(TX_MUTEX *mutex_ptr);
VOID        _tx_mutex_cleanup(TX_THREAD *thread_ptr);
VOID        _tx_mutex_priority_change(TX_THREAD *thread_ptr, UINT priority, UINT threshold);


/* Define error checking shells for API services.  These are only referenced by the 
   application.  */

UINT        _txe_mutex_create(TX_MUTEX *mutex_ptr, CHAR *name_ptr, UINT inherit);
UINT        _txe_mutex_delete(TX_MUTEX *mutex_ptr);
UINT        _txe_mutex_get(TX_MUTEX *mutex_ptr, ULONG wait_option);
UINT        _txe_mutex_info_get(TX_MUTEX *mutex_ptr, CHAR **name, ULONG *count, TX_THREAD **owner, 
                    TX_THREAD **first_suspended, ULONG *suspended_count, 
                    TX_MUTEX **next_mutex);
UINT        _txe_mutex_prioritize(TX_MUTEX *mutex_ptr);
UINT        _txe_mutex_put(TX_MUTEX *mutex_ptr);


/* Mutex management component data declarations follow.  */

/* Determine if the initialization function of this component is including
   this file.  If so, make the data definitions really happen.  Otherwise,
   make them extern so other functions in the component can access them.  */

#ifdef  TX_MUTEX_INIT
#define MUTEX_DECLARE 
#else
#define MUTEX_DECLARE extern
#endif


/* Define the head pointer of the created mutex list.  */

MUTEX_DECLARE  TX_MUTEX *   _tx_mutex_created_ptr;


/* Define the variable that holds the number of created mutexes. */

MUTEX_DECLARE  ULONG        _tx_mutex_created_count;


#endif
