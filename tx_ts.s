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
;/**   Thread Control (THR)                                                */
;/**                                                                       */
;/**************************************************************************/
;/**************************************************************************/
;
;
;#define    TX_SOURCE_CODE
;
;
;/* Include necessary system files.  */
;
;#include   "tx_api.h"
;#include   "tx_thr.h"
;#include   "tx_tim.h"
;
    IF :DEF:TX_ENABLE_FIQ_SUPPORT
ENABLE_INTS     EQU     0xC0                    ; IRQ & FIQ Interrupts enabled mask
    ELSE
ENABLE_INTS     EQU     0x80                    ; IRQ Interrupts enabled mask
    ENDIF
;
;
    IMPORT     _tx_thread_execute_ptr
    IMPORT     _tx_thread_current_ptr
    IMPORT     _tx_timer_time_slice
;
;
        AREA ||.text||, CODE, READONLY
;/**************************************************************************/ 
;/*                                                                        */ 
;/*  FUNCTION                                               RELEASE        */ 
;/*                                                                        */ 
;/*    _tx_thread_schedule                                  ARM7/ARM       */ 
;/*                                                           4.0c         */ 
;/*  AUTHOR                                                                */ 
;/*                                                                        */ 
;/*    William E. Lamie, Express Logic, Inc.                               */ 
;/*                                                                        */ 
;/*  DESCRIPTION                                                           */ 
;/*                                                                        */ 
;/*    This function waits for a thread control block pointer to appear in */ 
;/*    the _tx_thread_execute_ptr variable.  Once a thread pointer appears */ 
;/*    in the variable, the corresponding thread is resumed.               */ 
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
;/*    None                                                                */
;/*                                                                        */ 
;/*  CALLED BY                                                             */ 
;/*                                                                        */ 
;/*    _tx_initialize_kernel_enter          ThreadX entry function         */ 
;/*    _tx_thread_system_return             Return to system from thread   */ 
;/*    _tx_thread_context_restore           Restore thread's context       */ 
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
;/*  03-01-2000     William E. Lamie         Modified comment(s) and       */ 
;/*                                            added flags modifier to MSR */ 
;/*                                            instructions, resulting     */ 
;/*                                            in version 3.0g.            */ 
;/*  01-28-2001     William E. Lamie         Modified comment(s),          */ 
;/*                                            resulting in version 4.0.   */ 
;/*  10-10-2002     William E. Lamie         Modified comment(s), added    */ 
;/*                                            support for mixed Thumb     */ 
;/*                                            mode calls, added logic to  */ 
;/*                                            preserve other bits of      */ 
;/*                                            CPSR, changed to physical   */ 
;/*                                            register names, and changed */ 
;/*                                            PC-relative addressing,     */ 
;/*                                            resulting in version 4.0a.  */ 
;/*  01-01-2003     William E. Lamie         Modified comment(s),          */ 
;/*                                            resulting in version 4.0b.  */ 
;/*  07-15-2004     William E. Lamie         Modified comment(s),          */ 
;/*                                            resulting in version 4.0c.  */ 
;/*                                                                        */ 
;/**************************************************************************/ 
;VOID   _tx_thread_schedule(VOID)
;{
    EXPORT  _tx_thread_schedule
_tx_thread_schedule
;
;    /* Enable interrupts.  */
;
    MRS     r2, CPSR                            ; Pickup CPSR
    BIC     r0, r2, #ENABLE_INTS                ; Clear the disable bit(s)
    MSR     CPSR_cxsf, r0                       ; Enable interrupts
;
;    /* Wait for a thread to execute.  */
;    do
;    {
    LDR     r1, EXECUTE_PTR                     ; Address of thread execute ptr
;
__tx_thread_schedule_loop
;
    LDR     r0, [r1, #0]                        ; Pickup next thread to execute
    CMP     r0, #0                              ; Is it NULL?
    BEQ     __tx_thread_schedule_loop           ; If so, keep looking for a thread
;
;    }
;    while(_tx_thread_execute_ptr == TX_NULL);
;    
;    /* Yes! We have a thread to execute.  Lockout interrupts and
;       transfer control to it.  */
;
    MSR     CPSR_cxsf, r2                       ; Disable interrupts
;
;    /* Setup the current thread pointer.  */
;    _tx_thread_current_ptr =  _tx_thread_execute_ptr;
;
    LDR     r1, CURRENT_PTR                     ; Pickup address of current thread 
    STR     r0, [r1, #0]                        ; Setup current thread pointer
;
;    /* Increment the run count for this thread.  */
;    _tx_thread_current_ptr -> tx_run_count++;
;
    LDR     r2, [r0, #4]                        ; Pickup run counter
    LDR     r3, [r0, #24]                       ; Pickup time-slice for this thread
    ADD     r2, r2, #1                          ; Increment thread run-counter
    STR     r2, [r0, #4]                        ; Store the new run counter
;
;    /* Setup time-slice, if present.  */
;    _tx_timer_time_slice =  _tx_thread_current_ptr -> tx_time_slice;
;
    LDR     r2, TIME_SLICE                      ; Pickup address of time slice 
                                                ;   variable
    LDR     sp, [r0, #8]                        ; Switch stack pointers
    STR     r3, [r2, #0]                        ; Setup time-slice
;
;    /* Switch to the thread's stack.  */
;    sp =  _tx_thread_execute_ptr -> tx_stack_ptr;
;
;    /* Determine if an interrupt frame or a synchronous task suspension frame
;   is present.  */
;
    LDMIA   sp!, {r0, r1}                       ; Pickup the stack type and saved CPSR
    CMP     r0, #0                              ; Check for synchronous context switch
    MSRNE   SPSR_cxsf, r1                       ;   Setup SPSR for return
    LDMNEIA sp!, {r0-r12, lr, pc}^              ; Return to point of thread interrupt
    LDMIA   sp!, {r4-r11, lr}                   ; Return to thread synchronously
    MSR     CPSR_cxsf, r1                       ;   Recover CPSR
    IF  {INTER} = {TRUE}
    BX      lr                                  ; Return to caller
    ELSE
    MOV     pc, lr                              ; Return to caller
    ENDIF
;
;}
;
EXECUTE_PTR
    DCD     _tx_thread_execute_ptr
CURRENT_PTR
    DCD     _tx_thread_current_ptr
TIME_SLICE
    DCD     _tx_timer_time_slice
    END

