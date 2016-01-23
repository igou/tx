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


/**************************************************************************/ 
/*                                                                        */ 
/*  COMPONENT DEFINITION                                   RELEASE        */ 
/*                                                                        */ 
/*    tx_tim.h                                            PORTABLE C      */ 
/*                                                           4.0b         */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This file defines the ThreadX timer management component, including */ 
/*    data types and external references.  It is assumed that tx_api.h    */
/*    and tx_port.h have already been included.                           */
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
/*  01-28-2001     William E. Lamie         Modified comment(s), and new  */ 
/*                                            timer services, resulting   */ 
/*                                            in version 4.0.             */ 
/*  07-15-2002     William E. Lamie         Modified comment(s), added    */ 
/*                                            logic for the system timer  */ 
/*                                            thread's stack area, and    */ 
/*                                            changed file included name, */ 
/*                                            resulting in version 4.0a.  */ 
/*  01-01-2003     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 4.0b.  */ 
/*                                                                        */ 
/**************************************************************************/ 

#ifndef  TX_TIM_H
#define  TX_TIM_H


/* Define timer management specific data definitions.  */

#define TX_TIMER_ID                 0x4154494DUL
#define TX_TIMER_ENTRIES            32
#define TX_TIMER_APPLICATION_TYPE   1
#define TX_TIMER_TASK_TYPE          0


/* Define timer management function prototypes.  */

VOID        _tx_timer_initialize(VOID);
UINT        _tx_timer_activate(TX_INTERNAL_TIMER *timer_ptr);
UINT        _tx_timer_activate_api(TX_TIMER *timer_ptr);
UINT        _tx_timer_change(TX_TIMER *timer_ptr, ULONG initial_ticks, 
                                                    ULONG reschedule_ticks);
UINT        _tx_timer_create(TX_TIMER *timer_ptr, CHAR *name_ptr, 
                VOID (*expiration_function)(ULONG), ULONG expiration_input,
                ULONG initial_ticks, ULONG reschedule_ticks, UINT auto_activate);
UINT        _tx_timer_deactivate(TX_INTERNAL_TIMER *timer_ptr);
UINT        _tx_timer_deactivate_api(TX_TIMER *timer_ptr);
UINT        _tx_timer_delete(TX_TIMER *timer_ptr);
UINT        _tx_timer_info_get(TX_TIMER *timer_ptr, CHAR **name, UINT *active, ULONG *remaining_ticks, 
                ULONG *reschedule_ticks, TX_TIMER **next_timer);
VOID        _tx_timer_thread_entry(ULONG timer_thread_input);
ULONG       _tx_time_get(VOID);
VOID        _tx_time_set(ULONG new_time);


/* Define error checking shells for API services.  These are only referenced by the 
   application.  */

UINT        _txe_timer_activate_api(TX_TIMER *timer_ptr);
UINT        _txe_timer_change(TX_TIMER *timer_ptr, ULONG initial_ticks, 
                                                    ULONG reschedule_ticks);
UINT        _txe_timer_create(TX_TIMER *timer_ptr, CHAR *name_ptr, 
                VOID (*expiration_function)(ULONG), ULONG expiration_input,
                ULONG initial_ticks, ULONG reschedule_ticks, UINT auto_activate);
UINT        _txe_timer_deactivate_api(TX_TIMER *timer_ptr);
UINT        _txe_timer_delete(TX_TIMER *timer_ptr);
UINT        _txe_timer_info_get(TX_TIMER *timer_ptr, CHAR **name, UINT *active, ULONG *remaining_ticks, 
                ULONG *reschedule_ticks, TX_TIMER **next_timer);



/* Timer management component data declarations follow.  */

/* Determine if the initialization function of this component is including
   this file.  If so, make the data definitions really happen.  Otherwise,
   make them extern so other functions in the component can access them.  */

#ifdef  TX_TIMER_INIT
#define TIMER_DECLARE 
#else
#define TIMER_DECLARE extern
#endif


/* Define the system clock value that is continually incremented by the 
   periodic timer interrupt processing.  */

TIMER_DECLARE ULONG     _tx_timer_system_clock;


/* Define the current time slice value.  If non-zero, a time-slice is active.
   Otherwise, the time_slice is not active.  */

TIMER_DECLARE ULONG     _tx_timer_time_slice;


/* Define the time-slice expiration flag.  This is used to indicate that a time-slice
   has happened.  In some ports, this variable is not used.  */

TIMER_DECLARE UINT      _tx_timer_expired_time_slice;


/* Define the thread and application timer entry list.  This list provides a direct access
   method for insertion of times less than TX_TIMER_ENTRIES.  */

TIMER_DECLARE TX_INTERNAL_TIMER *_tx_timer_list[TX_TIMER_ENTRIES];


/* Define the boundary pointers to the list.  These are setup to easily manage
   wrapping the list.  */

TIMER_DECLARE TX_INTERNAL_TIMER **_tx_timer_list_start;
TIMER_DECLARE TX_INTERNAL_TIMER **_tx_timer_list_end;


/* Define the current timer pointer in the list.  This pointer is moved sequentially
   through the timer list by the timer interrupt handler.  */

TIMER_DECLARE TX_INTERNAL_TIMER **_tx_timer_current_ptr;


/* Define the timer expiration flag.  This is used to indicate that a timer 
   has expired.  */

TIMER_DECLARE UINT      _tx_timer_expired;


/* Define the timer thread's control block.  */

TIMER_DECLARE TX_THREAD _tx_timer_thread;


/* Define the variable that holds the timer thread's starting stack address.  */

TIMER_DECLARE VOID      *_tx_timer_stack_start;


/* Define the variable that holds the timer thread's stack size.  */

TIMER_DECLARE ULONG     _tx_timer_stack_size;


/* Define the variable that holds the timer thread's priority.  */

TIMER_DECLARE UINT      _tx_timer_priority;


/* Define the created timer list head pointer.  */

TIMER_DECLARE TX_TIMER *_tx_timer_created_ptr;


/* Define the created timer count.  */

TIMER_DECLARE ULONG     _tx_timer_created_count;


/* Define the system timer thread's stack.   The default size is defined
   in tx_port.h.  */

TIMER_DECLARE UCHAR     _tx_timer_thread_stack_area[TX_TIMER_THREAD_STACK_SIZE];


#endif
