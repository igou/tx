;/**************************************************************************/ 
;/*                                                                        */ 
;/*            Copyright (c) 1996-2004 by Express Logic Inc.               */ 
;/*                                                                        */ 
;/*  This software is copyrighted by and is the sole property of Express   */ 
;/*  Logic, Inc.  All rights, title, ownership, or other interests         */ 
;/*  in the software remain the property of Express Logic, Inc.  This      */ 
;/*  software may only be used in accordance with the corresponding        */ 
;/*  license agreement.  Any unauthorized use, duplication, transmission,  */ 
;/*  distribution, or disclosure of this software is expressly forbidden.  */ 
;/*                                                                        */ 
;/*  This Copyright notice may not be removed or modified without prior    */ 
;/*  written consent of Express Logic, Inc.                                */ 
;/*                                                                        */ 
;/*  Express Logic, Inc. reserves the right to modify this software        */ 
;/*  without notice.                                                       */ 
;/*                                                                        */ 
;/*  Express Logic, Inc.                     info@expresslogic.com         */
;/*  11423 West Bernardo Court               http://www.expresslogic.com   */
;/*  San Diego, CA  92127                                                  */
;/*                                                                        */
;/**************************************************************************/
;
;
;/**************************************************************************/
;/**************************************************************************/
;/**                                                                       */ 
;/** ThreadX Component                                                     */ 
;/**                                                                       */
;/**   Timer (TIM)                                                         */
;/**                                                                       */
;/**************************************************************************/
;/**************************************************************************/
;
;#define TX_SOURCE_CODE
;
;
;/* Include necessary system files.  */
;
;#include   "tx_api.h"
;#include   "tx_tim.h"
;#include   "tx_thr.h"
;
;
;Define Assembly language external references...
;
    IMPORT      _tx_timer_time_slice
    IMPORT      _tx_timer_system_clock
    IMPORT      _tx_timer_current_ptr
    IMPORT      _tx_timer_list_start
    IMPORT      _tx_timer_list_end
    IMPORT      _tx_timer_expired_time_slice
    IMPORT      _tx_timer_expired
    IMPORT      _tx_timer_thread
    IMPORT      _tx_thread_current_ptr
    IMPORT      _tx_thread_time_slice
    IMPORT      _tx_thread_resume
    IMPORT      _tx_thread_preempt_disable
;
;
        AREA ||.text||, CODE, READONLY
        PRESERVE8
;/**************************************************************************/ 
;/*                                                                        */ 
;/*  FUNCTION                                               RELEASE        */ 
;/*                                                                        */ 
;/*    _tx_timer_interrupt                                  ARM7/ARM       */ 
;/*                                                           4.0c         */ 
;/*  AUTHOR                                                                */ 
;/*                                                                        */ 
;/*    William E. Lamie, Express Logic, Inc.                               */ 
;/*                                                                        */ 
;/*  DESCRIPTION                                                           */ 
;/*                                                                        */ 
;/*    This function processes the hardware timer interrupt.  This         */ 
;/*    processing includes incrementing the system clock and checking for  */ 
;/*    time slice and/or timer expiration.  If either is found, the        */ 
;/*    interrupt context save/restore functions are called along with the  */ 
;/*    expiration functions.                                               */ 
;/*                                                                        */ 
;/*  INPUT                                                                 */ 
;/*                                                                        */ 
;/*    None                                                                */ 
;/*                                                                        */ 
;/*  OUTPUT                                                                */ 
;/*                                                                        */ 
;/*    None                                                                */ 
;/*                                                                        */ 
;/*  CALLS                                                                 */ 
;/*                                                                        */ 
;/*    _tx_thread_resume                     Resume timer processing thread*/ 
;/*    _tx_thread_time_slice                 Time slice interrupted thread */ 
;/*                                                                        */ 
;/*  CALLED BY                                                             */ 
;/*                                                                        */ 
;/*    interrupt vector                                                    */ 
;/*                                                                        */ 
;/*  RELEASE HISTORY                                                       */ 
;/*                                                                        */ 
;/*    DATE              NAME                      DESCRIPTION             */ 
;/*                                                                        */ 
;/*  06-15-1997     William E. Lamie         Initial Version 3.0           */ 
;/*  11-11-1997     William E. Lamie         Modified comment(s),          */ 
;/*                                            resulting in version 3.0b.  */ 
;/*  01-01-1999     William E. Lamie         Modified comment(s),          */ 
;/*                                            resulting in version 3.0e.  */ 
;/*  03-01-2000     William E. Lamie         Modified comment(s),          */ 
;/*                                            resulting in version 3.0g.  */ 
;/*  01-28-2001     William E. Lamie         Modified comment(s),          */ 
;/*                                            resulting in version 4.0.   */ 
;/*  10-10-2002     William E. Lamie         Modified comment(s), added    */ 
;/*                                            support for mixed Thumb     */ 
;/*                                            mode calls, CPSR, changed   */ 
;/*                                            to physical register names, */ 
;/*                                            and changed PC-relative     */ 
;/*                                            addressing, resulting       */ 
;/*                                            in version 4.0a.            */ 
;/*  01-01-2003     William E. Lamie         Modified comment(s),          */ 
;/*                                            resulting in version 4.0b.  */ 
;/*  07-15-2004     William E. Lamie         Modified comment(s), and      */ 
;/*                                            added logic for 8-byte      */ 
;/*                                            stack alignment, resulting  */ 
;/*                                            in version 4.0c.            */ 
;/*                                                                        */ 
;/**************************************************************************/ 
;VOID   _tx_timer_interrupt(VOID)
;{
    EXPORT  _tx_timer_interrupt
_tx_timer_interrupt
;
;    /* Upon entry to this routine, it is assumed that context save has already
;       been called, and therefore the compiler scratch registers are available
;       for use.  */
;
;    /* Increment the system clock.  */
;    _tx_timer_system_clock++;
;
    LDR     r1, SYSTEM_CLOCK                    ; Pickup address of system clock
    LDR     r0, [r1, #0]                        ; Pickup system clock
    ADD     r0, r0, #1                          ; Increment system clock
    STR     r0, [r1, #0]                        ; Store new system clock
;
;    /* Test for time-slice expiration.  */
;    if (_tx_timer_time_slice)
;    {
;
    LDR     r3, TIME_SLICE                      ; Pickup address of time-slice 
    LDR     r2, [r3, #0]                        ; Pickup time-slice
    CMP     r2, #0                              ; Is it non-active?
    BEQ     __tx_timer_no_time_slice            ; Yes, skip time-slice processing
;
;       /* Decrement the time_slice.  */
;       _tx_timer_time_slice--;
;
    SUB     r2, r2, #1                          ; Decrement the time-slice
    STR     r2, [r3, #0]                        ; Store new time-slice value
;
;       /* Check for expiration.  */
;       if (__tx_timer_time_slice == 0)
;
    CMP     r2, #0                              ; Has it expired?
    BNE     __tx_timer_no_time_slice            ; No, skip expiration processing
;
;       /* Set the time-slice expired flag.  */
;       _tx_timer_expired_time_slice =  TX_TRUE;
;
    LDR     r3, EXPIRED_TIME_SLICE              ; Pickup address of expired flag
    MOV     r0, #1                              ; Build expired value
    STR     r0, [r3, #0]                        ; Set time-slice expiration flag
;
;    }
;
__tx_timer_no_time_slice
;
;    /* Test for timer expiration.  */
;    if (*_tx_timer_current_ptr)
;    {
;
    LDR     r1, TIMER_CURRENT_PTR               ; Pickup current timer pointer addr
    LDR     r0, [r1, #0]                        ; Pickup current timer
    LDR     r2, [r0, #0]                        ; Pickup timer list entry
    CMP     r2, #0                              ; Is there anything in the list?
    BEQ     __tx_timer_no_timer                 ; No, just increment the timer
;
;        /* Set expiration flag.  */
;        _tx_timer_expired =  TX_TRUE;
;
    LDR     r3, EXPIRED                         ; Pickup expiration flag address
    MOV     r2, #1                              ; Build expired value
    STR     r2, [r3, #0]                        ; Set expired flag
    B       __tx_timer_done                     ; Finished timer processing
;
;    }
;    else
;    {
__tx_timer_no_timer
;
;        /* No timer expired, increment the timer pointer.  */
;        _tx_timer_current_ptr++;
;
    ADD     r0, r0, #4                          ; Move to next timer
;
;        /* Check for wrap-around.  */
;        if (_tx_timer_current_ptr == _tx_timer_list_end)
;
    LDR     r3, LIST_END                        ; Pickup addr of timer list end
    LDR     r2, [r3, #0]                        ; Pickup list end
    CMP     r0, r2                              ; Are we at list end?
    BNE     __tx_timer_skip_wrap                ; No, skip wrap-around logic
;
;            /* Wrap to beginning of list.  */
;            _tx_timer_current_ptr =  _tx_timer_list_start;
;
    LDR     r3, LIST_START                      ; Pickup addr of timer list start
    LDR     r0, [r3, #0]                        ; Set current pointer to list start
;
__tx_timer_skip_wrap
;
    STR     r0, [r1, #0]                        ; Store new current timer pointer
;    }
;
__tx_timer_done
;
;
;    /* See if anything has expired.  */
;    if ((_tx_timer_expired_time_slice) || (_tx_timer_expired))
;    {
;
    LDR     r3, EXPIRED_TIME_SLICE              ; Pickup addr of expired flag
    LDR     r2, [r3, #0]                        ; Pickup time-slice expired flag
    CMP     r2, #0                              ; Did a time-slice expire?
    BNE     __tx_something_expired              ; If non-zero, time-slice expired
    LDR     r1, EXPIRED                         ; Pickup addr of other expired flag
    LDR     r0, [r1, #0]                        ; Pickup timer expired flag
    CMP     r0, #0                              ; Did a timer expire?
    BEQ     __tx_timer_nothing_expired          ; No, nothing expired
;
__tx_something_expired
;
;
    STMDB   sp!, {lr, r0}                       ; Save the lr register on the stack
                                                ;   and save r0 just to keep 8-byte alignment
;
;    /* Did a timer expire?  */
;    if (_tx_timer_expired)
;    {
;
    LDR     r1, EXPIRED                         ; Pickup addr of expired flag
    LDR     r0, [r1, #0]                        ; Pickup timer expired flag
    CMP     r0, #0                              ; Check for timer expiration
    BEQ     __tx_timer_dont_activate            ; If not set, skip timer activation
;
;        /* Increment the preempt disable counter in preparation for 
;           thread resumption.  */
;        _tx_thread_preempt_disable++;
;
    LDR     r3, PREEMPT_DISABLE                 ; Pickup addr of preempt disable 
    LDR     r2, [r3, #0]                        ; Pickup actual flag
    ADD     r2, r2, #1                          ; Increment the preempt disable count
    STR     r2, [r3, #0]                        ; Store it back
;
;        /* Activate the system timer thread.  */
;        _tx_thread_resume(&_tx_timer_thread);
;
    LDR     r0, TIMER_THREAD                    ; Get timer thread control block addr
    BL      _tx_thread_resume                   ; Call thread resume to wake up the
                                                ;   timer thread
;
;    }
__tx_timer_dont_activate
;
;    /* Did time slice expire?  */
;    if (_tx_timer_expired_time_slice)
;    {
;
    LDR     r3, EXPIRED_TIME_SLICE              ; Pickup addr of time-slice expired 
    LDR     r2, [r3, #0]                        ; Pickup the actual flag
    CMP     r2, #0                              ; See if the flag is set
    BEQ     __tx_timer_not_ts_expiration        ; No, skip time-slice processing
;
;        /* Time slice interrupted thread.  */
;        if (!_tx_thread_time_slice())
;            _tx_timer_time_slice =  _tx_thread_current_ptr -> tx_time_slice;
;
    BL      _tx_thread_time_slice               ; Call time-slice processing
    CMP     r0, #0                              ; Check return status
    BNE     __tx_timer_not_ts_expiration        ; If time-sliced, skip reset processing
    LDR     r1, CURRENT_PTR                     ; Pickup addr of current thread pointer
    LDR     r0, [r1, #0]                        ; Pickup thread pointer
    LDR     r2, [r0, #24]                       ; Pickup fresh time-slice for thread
                                                ;  (a fresh time slice was setup in 
                                                ;   the _tx_thread_time_slice function)
    LDR     r3, TIME_SLICE                      ; Pickup addr of time-slice variable
    STR     r2, [r3, #0]                        ; Setup new time-slice
;
;    }
;
__tx_timer_not_ts_expiration
;
    LDR     r3, EXPIRED_TIME_SLICE              ; Pickup address of expired time-slice flag
    MOV     r0, #0                              ; Clear value
    STR     r0, [r3, #0]                        ; Clear time-slice expired flag
;
    LDMIA   sp!, {lr, r0}                       ; Recover lr register (r0 is just there for
                                                ;   the 8-byte stack alignment
;
;    }
;
__tx_timer_nothing_expired
;
    IF  {INTER} = {TRUE}
    BX      lr                                  ; Return to caller
    ELSE
    MOV     pc, lr                              ; Return to caller
    ENDIF
;
;}
TIME_SLICE
    DCD     _tx_timer_time_slice
SYSTEM_CLOCK
    DCD     _tx_timer_system_clock
TIMER_CURRENT_PTR
    DCD     _tx_timer_current_ptr
LIST_START
    DCD     _tx_timer_list_start
LIST_END
    DCD     _tx_timer_list_end
EXPIRED_TIME_SLICE
    DCD     _tx_timer_expired_time_slice
EXPIRED
    DCD     _tx_timer_expired
TIMER_THREAD
    DCD     _tx_timer_thread
CURRENT_PTR
    DCD     _tx_thread_current_ptr
THREAD_TIME_SLICE
    DCD     _tx_thread_time_slice
RESUME
    DCD     _tx_thread_resume
PREEMPT_DISABLE
    DCD     _tx_thread_preempt_disable
    END

