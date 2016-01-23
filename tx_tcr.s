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
SVC_MODE        EQU     0xD3                    ; SVC mode
IRQ_MODE        EQU     0xD2                    ; IRQ mode
FIQ_MODE        EQU     0xD1                    ; FIQ mode
    IF  :DEF:TX_ENABLE_FIQ_SUPPORT
DISABLE_INTS    EQU     0xC0                    ; Disable IRQ interrupts
    ELSE
DISABLE_INTS    EQU     0x80                    ; Disable IRQ interrupts
    ENDIF
MODE_MASK       EQU     0x1F                    ; Mode mask 
THUMB_MASK      EQU     0x20                    ; Thumb bit mask
FIQ_MODE_BITS   EQU     0x11                    ; FIQ mode bits
IRQ_MODE_BITS   EQU     0x12                    ; IRQ mode bits
SVC_MODE_BITS   EQU     0x13                    ; SVC mode value

;
    IMPORT      _tx_thread_system_state
    IMPORT      _tx_thread_current_ptr
    IMPORT      _tx_thread_system_stack_ptr
    IMPORT      _tx_thread_execute_ptr
    IMPORT      _tx_timer_time_slice
    IMPORT      _tx_thread_schedule
    IMPORT      _tx_thread_preempt_disable
;
;
        AREA ||.text||, CODE, READONLY
;/**************************************************************************/ 
;/*                                                                        */ 
;/*  FUNCTION                                               RELEASE        */ 
;/*                                                                        */ 
;/*    _tx_thread_context_restore                           ARM7/ARM       */ 
;/*                                                           4.0c         */ 
;/*  AUTHOR                                                                */ 
;/*                                                                        */ 
;/*    William E. Lamie, Express Logic, Inc.                               */ 
;/*                                                                        */ 
;/*  DESCRIPTION                                                           */ 
;/*                                                                        */ 
;/*    This function restores the interrupt context if it is processing a  */ 
;/*    nested interrupt.  If not, it returns to the interrupt thread if no */ 
;/*    preemption is necessary.  Otherwise, if preemption is necessary or  */ 
;/*    if no thread was running, the function returns to the scheduler.    */ 
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
;/*    _tx_thread_schedule                   Thread scheduling routine     */ 
;/*                                                                        */ 
;/*  CALLED BY                                                             */ 
;/*                                                                        */ 
;/*    ISRs                                  Interrupt Service Routines    */ 
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
;/*                                            logic to preserve other     */ 
;/*                                            other bits of CPSR, added   */ 
;/*                                            optional FIQ support, added */ 
;/*                                            nested IRQ and FIQ interrupt*/ 
;/*                                            support, changed to         */ 
;/*                                            physical register names,    */ 
;/*                                            and changed PC-relative     */ 
;/*                                            addressing, resulting in    */ 
;/*                                            version 4.0a.               */ 
;/*  01-01-2003     William E. Lamie         Modified comment(s),          */ 
;/*                                            resulting in version 4.0b.  */ 
;/*  07-15-2004     William E. Lamie         Modified comment(s),          */ 
;/*                                            resulting in version 4.0c.  */ 
;/*                                                                        */ 
;/**************************************************************************/ 
;VOID   _tx_thread_context_restore(VOID)
;{
    EXPORT  _tx_thread_context_restore
_tx_thread_context_restore
;
;    /* Lockout interrupts.  */
;
    MRS     r3, CPSR                            ; Pickup current CPSR
    ORR     r0, r3, #DISABLE_INTS               ; Build interrupt disable value
    MSR     CPSR_cxsf, r0                       ; Lockout interrupts
;
;    /* Determine if interrupts are nested.  */
;    if (--_tx_thread_system_state)
;    {
;
    LDR     r3, SYSTEM_STATE                    ; Pickup address of system state var
    LDR     r2, [r3, #0]                        ; Pickup system state
    SUB     r2, r2, #1                          ; Decrement the counter
    STR     r2, [r3, #0]                        ; Store the counter 
    CMP     r2, #0                              ; Was this the first interrupt?
    BEQ     __tx_thread_not_nested_restore      ; If so, not a nested restore
;
;    /* Interrupts are nested.  */
;
;    /* Just recover the saved registers and return to the point of 
;       interrupt.  */
;
    LDMIA   sp!, {r0, r10, r12, lr}             ; comment by wjr
						; Recover SPSR, POI, and scratch regs
											
    MSR     SPSR_cxsf, r0                       ; Put SPSR back
    LDMIA   sp!, {r0-r3}                        ; Recover r0-r3
    MOVS    pc, lr                              ; Return to point of interrupt
;
;    }
__tx_thread_not_nested_restore
;
;    /* Determine if a thread was interrupted and no preemption is required.  */
;    else if (((_tx_thread_current_ptr) && (_tx_thread_current_ptr == _tx_thread_execute_ptr) 
;               || (_tx_thread_preempt_disable))
;    {
;
    LDR     r1, CURRENT_PTR                     ; Pickup address of current thread ptr
    LDR     r0, [r1, #0]                        ; Pickup actual current thread pointer
    CMP     r0, #0                              ; Is it NULL?
    BEQ     __tx_thread_idle_system_restore     ; Yes, idle system was interrupted
;
    LDR     r3, PREEMPT_DISABLE                 ; Pickup preempt disable address
    LDR     r2, [r3, #0]                        ; Pickup actual preempt disable flag
    CMP     r2, #0                              ; Is it set?
    BNE     __tx_thread_no_preempt_restore      ; Yes, don't preempt this thread
    LDR     r3, EXECUTE_PTR                     ; Pickup address of execute thread ptr
    LDR     r2, [r3, #0]                        ; Pickup actual execute thread pointer
    CMP     r0, r2                              ; Is the same thread highest priority?
    BNE     __tx_thread_preempt_restore         ; No, preemption needs to happen
;
;
__tx_thread_no_preempt_restore
;
;    /* Restore interrupted thread or ISR.  */
;
;    /* Pickup the saved stack pointer.  */
;    tmp_ptr =  _tx_thread_current_ptr -> tx_stack_ptr;
;
;   /* Recover the saved context and return to the point of interrupt.  */
;
    LDMIA   sp!, {r0, r10, r12, lr}             ; Recover SPSR, POI, and scratch regs
    MSR     SPSR_cxsf, r0                       ; Put SPSR back
    LDMIA   sp!, {r0-r3}                        ; Recover r0-r3
    MOVS    pc, lr                              ; Return to point of interrupt
;
;    }
;    else
;    {
__tx_thread_preempt_restore
;
    LDMIA   sp!, {r3, r10, r12, lr}             ; Recover temporarily saved registers
    MOV     r1, lr                              ; Save lr (point of interrupt)
    MOV     r2, #SVC_MODE                       ; Build SVC mode CPSR
    MSR     CPSR_c, r2                          ; Enter SVC mode
    STR     r1, [sp, #-4]!                      ; Save point of interrupt
    STMDB   sp!, {r4-r12, lr}                   ; Save upper half of registers
    MOV     r4, r3                              ; Save SPSR in r4
    MOV     r2, #IRQ_MODE                       ; Build IRQ mode CPSR
    MSR     CPSR_c, r2                          ; Enter IRQ mode
    LDMIA   sp!, {r0-r3}                        ; Recover r0-r3
    MOV     r5, #SVC_MODE                       ; Build SVC mode CPSR
    MSR     CPSR_c, r5                          ; Enter SVC mode
    STMDB   sp!, {r0-r3}                        ; Save r0-r3 on thread's stack
    MOV     r3, #1                              ; Build interrupt stack type
    STMDB   sp!, {r3, r4}                       ; Save interrupt stack type and SPSR
    LDR     r1, CURRENT_PTR                     ; Pickup address of current thread ptr
    LDR     r0, [r1, #0]                        ; Pickup current thread pointer
    STR     sp, [r0, #8]                        ; Save stack pointer in thread control
                                                ;   block
    BIC     r4, r4, #THUMB_MASK                 ; Clear the Thumb bit of CPSR
    ORR     r3, r4, #DISABLE_INTS               ; Or-in interrupt lockout bit(s)
    MSR     CPSR_cxsf, r3                       ; Lockout interrupts
;
;    /* Save the remaining time-slice and disable it.  */
;    if (_tx_timer_time_slice)
;    {
;
    LDR     r3, TIME_SLICE                      ; Pickup time-slice variable address
    LDR     r2, [r3, #0]                        ; Pickup time-slice
    CMP     r2, #0                              ; Is it active?
    BEQ     __tx_thread_dont_save_ts            ; No, don't save it
;
;        _tx_thread_current_ptr -> tx_time_slice =  _tx_timer_time_slice;
;        _tx_timer_time_slice =  0;
;
    STR     r2, [r0, #24]                       ; Save thread's time-slice
    MOV     r2, #0                              ; Clear value
    STR     r2, [r3, #0]                        ; Disable global time-slice flag
;
;    }
__tx_thread_dont_save_ts
;
;
;    /* Clear the current task pointer.  */
;    _tx_thread_current_ptr =  TX_NULL;
;
    MOV     r0, #0                              ; NULL value
    STR     r0, [r1, #0]                        ; Clear current thread pointer
;
;    /* Return to the scheduler.  */
;    _tx_thread_schedule();
;
    B       _tx_thread_schedule                 ; Return to scheduler
;    }
;
__tx_thread_idle_system_restore
;
;    /* Just return back to the scheduler!  */
;
    MRS     r3, CPSR                            ; Pickup current CPSR
    BIC     r3, r3, #MODE_MASK                  ; Clear the mode portion of the CPSR
    ORR     r3, r3, #SVC_MODE_BITS              ; Or-in new interrupt lockout bit
    MSR     CPSR_cxsf, r3                       ; Lockout interrupts
    B       _tx_thread_schedule                 ; Return to scheduler
;}
;
;
    IF  :DEF:TX_ENABLE_IRQ_NESTING
;/**************************************************************************/ 
;/*                                                                        */ 
;/*  FUNCTION                                               RELEASE        */ 
;/*                                                                        */ 
;/*    _tx_thread_irq_nesting_end                           ARM7/ARM       */ 
;/*                                                           4.0c         */ 
;/*  AUTHOR                                                                */ 
;/*                                                                        */ 
;/*    William E. Lamie, Express Logic, Inc.                               */ 
;/*                                                                        */ 
;/*  DESCRIPTION                                                           */ 
;/*                                                                        */ 
;/*    This function is called by the application from IRQ mode after      */ 
;/*    _tx_thread_irq_nesting_start has been called and switches the IRQ   */ 
;/*    processing from system mode back to IRQ mode prior to the ISR       */ 
;/*    calling _tx_thread_context_restore.  Note that this function        */ 
;/*    assumes the system stack pointer is in the same position after      */ 
;/*    nesting start function was called.                                  */ 
;/*                                                                        */ 
;/*    This function assumes that the system mode stack pointer was setup  */ 
;/*    during low-level initialization (tx_ill).                           */ 
;/*                                                                        */ 
;/*    This function returns with IRQ interrupts disabled.                 */ 
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
;/*    ISRs                                                                */ 
;/*                                                                        */ 
;/*  RELEASE HISTORY                                                       */ 
;/*                                                                        */ 
;/*    DATE              NAME                      DESCRIPTION             */ 
;/*                                                                        */ 
;/*  10-10-2002     William E. Lamie         Initial Version 4.0a          */ 
;/*  01-01-2003     William E. Lamie         Modified comment(s),          */ 
;/*                                            resulting in version 4.0b.  */ 
;/*  07-15-2004     William E. Lamie         Modified comment(s), and      */ 
;/*                                            added logic to keep 8-byte  */ 
;/*                                            alignment, resulting        */ 
;/*                                            in version 4.0c.            */ 
;/*                                                                        */ 
;/**************************************************************************/ 
;VOID   _tx_thread_irq_nesting_end(VOID)
;{
    EXPORT  _tx_thread_irq_nesting_end
_tx_thread_irq_nesting_end
    MOV     r3,lr                               ; Save ISR return address
    MRS     r0, CPSR                            ; Pickup the CPSR
    ORR     r0, r0, #DISABLE_INTS               ; Build disable interrupt value
    MSR     CPSR_cxsf, r0                       ; Disable interrupts
    LDMIA   sp!, {lr, r1}                       ; Pickup saved lr (and r1 throw-away for 
                                                ;   8-byte alignment logic)
    BIC     r0, r0, #MODE_MASK                  ; Clear mode bits
    ORR     r0, r0, #IRQ_MODE_BITS              ; Build IRQ mode CPSR
    MSR     CPSR_cxsf, r0                       ; Re-enter IRQ mode
    MOV     pc, r3                              ; Return to ISR
    ENDIF
;
;
    IF  :DEF:TX_ENABLE_FIQ_SUPPORT
;/**************************************************************************/ 
;/*                                                                        */ 
;/*  FUNCTION                                               RELEASE        */ 
;/*                                                                        */ 
;/*    _tx_thread_fiq_context_restore                       ARM7/ARM       */ 
;/*                                                           4.0c         */ 
;/*  AUTHOR                                                                */ 
;/*                                                                        */ 
;/*    William E. Lamie, Express Logic, Inc.                               */ 
;/*                                                                        */ 
;/*  DESCRIPTION                                                           */ 
;/*                                                                        */ 
;/*    This function restores the fiq interrupt context when processing a  */ 
;/*    nested interrupt.  If not, it returns to the interrupt thread if no */ 
;/*    preemption is necessary.  Otherwise, if preemption is necessary or  */ 
;/*    if no thread was running, the function returns to the scheduler.    */ 
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
;/*    _tx_thread_schedule                   Thread scheduling routine     */ 
;/*                                                                        */ 
;/*  CALLED BY                                                             */ 
;/*                                                                        */ 
;/*    FIQ ISR                               Interrupt Service Routines    */ 
;/*                                                                        */ 
;/*  RELEASE HISTORY                                                       */ 
;/*                                                                        */ 
;/*    DATE              NAME                      DESCRIPTION             */ 
;/*                                                                        */ 
;/*  10-10-2002     William E. Lamie         Initial Version 4.0a          */ 
;/*  01-01-2003     William E. Lamie         Modified comment(s),          */ 
;/*                                            resulting in version 4.0b.  */ 
;/*  07-15-2004     William E. Lamie         Modified comment(s),          */ 
;/*                                            resulting in version 4.0c.  */ 
;/*                                                                        */ 
;/**************************************************************************/ 
;VOID   _tx_thread_fiq_context_restore(VOID)
;{
    EXPORT  _tx_thread_fiq_context_restore
_tx_thread_fiq_context_restore
;
;    /* Lockout interrupts.  */
;
    MRS     r3, CPSR                            ; Pickup current CPSR
    ORR     r0, r3, #DISABLE_INTS               ; Build interrupt disable value
    MSR     CPSR_cxsf, r0                       ; Lockout interrupts
;
;    /* Determine if interrupts are nested.  */
;    if (--_tx_thread_system_state)
;    {
;
    LDR     r3, SYSTEM_STATE                    ; Pickup address of system state var
    LDR     r2, [r3]                            ; Pickup system state
    SUB     r2, r2, #1                          ; Decrement the counter
    STR     r2, [r3]                            ; Store the counter 
    CMP     r2, #0                              ; Was this the first interrupt?
    BEQ     __tx_thread_fiq_not_nested_restore  ; If so, not a nested restore
;
;    /* Interrupts are nested.  */
;
;    /* Just recover the saved registers and return to the point of 
;       interrupt.  */
;
;    LDMIA   sp!, {r0, r10, r12, lr}            ; comment by wjr , 
						; Recover SPSR, POI, and scratch regs
;/*------------------------add start-------------------------*/		
	LDMIA   sp!, {r0,  lr}              	;no restore r10,r12_fiq,because i design according to:
						;1. in FIQ mode never ennable interrupt
						;2. never insert C code between this function and 
						;  _tx_thread_fiq_nesting_start
;/*------------------------add end--------------------------------*/	
    MSR     SPSR_cxsf, r0                       ; Put SPSR back
    LDMIA   sp!, {r0-r3}                        ; Recover r0-r3
    MOVS    pc, lr                              ; Return to point of interrupt
;
;    }
__tx_thread_fiq_not_nested_restore
;
;    /* Determine if a thread was interrupted and no preemption is required.  */
;    else if (((_tx_thread_current_ptr) && (_tx_thread_current_ptr == _tx_thread_execute_ptr) 
;               || (_tx_thread_preempt_disable))
;    {
;
;/*------------------------add comment wjr,remove some code--------------------------------*/
;	/*	monaco all peripherals cause all  MCU interrupt via only FIQ  line */
;    LDR     r1, [sp]                            ; comment by wjr,Pickup the saved SPSR
;  	 MOV     r2, #MODE_MASK                  ; comment by wjr,Build mask to isolate the interrupted mode
; 	 AND     r1, r1, r2                      ;comment by wjr, Isolate mode bits
;	 CMP     r1, #IRQ_MODE_BITS              ; comment by wjr,Was an interrupt taken in IRQ mode before we
                                                 ;   got to context save? */
;    BEQ     __tx_thread_fiq_no_preempt_restore  ; comment by wjr,Yes, just go back to point of interrupt
;/*----------------------------add end-------------------------------*/

    LDR     r1, CURRENT_PTR                     ; Pickup address of current thread ptr
    LDR     r0, [r1]                            ; Pickup actual current thread pointer
    CMP     r0, #0                              ; Is it NULL?
    BEQ     __tx_thread_fiq_idle_system_restore ; Yes, idle system was interrupted

    LDR     r3, PREEMPT_DISABLE                 ; Pickup preempt disable address
    LDR     r2, [r3]                            ; Pickup actual preempt disable flag
    CMP     r2, #0                              ; Is it set?
    BNE     __tx_thread_fiq_no_preempt_restore  ; Yes, don't preempt this thread
    LDR     r3, EXECUTE_PTR                     ; Pickup address of execute thread ptr
    LDR     r2, [r3]                            ; Pickup actual execute thread pointer
    CMP     r0, r2                              ; Is the same thread highest priority?
    BNE     __tx_thread_fiq_preempt_restore     ; No, preemption needs to happen


__tx_thread_fiq_no_preempt_restore
;
;    /* Restore interrupted thread or ISR.  */
;
;    /* Pickup the saved stack pointer.  */
;    tmp_ptr =  _tx_thread_current_ptr -> tx_stack_ptr; 
;
;    /* Recover the saved context and return to the point of interrupt.  */
;
    LDMIA   sp!, {r0, lr}                       ; Recover SPSR, POI, and scratch regs
    MSR     SPSR_cxsf, r0                       ; Put SPSR back
    LDMIA   sp!, {r0-r3}                        ; Recover r0-r3
    MOVS    pc, lr                              ; Return to point of interrupt
;
;    }
;    else
;    {
__tx_thread_fiq_preempt_restore
;
    LDMIA   sp!, {r3, lr}                       ; Recover temporarily saved registers
    MOV     r1, lr                              ; Save lr (point of interrupt)
    MOV     r2, #SVC_MODE                       ; Build SVC mode CPSR
    MSR     CPSR_cxsf, r2                       ; Enter SVC mode
    STR     r1, [sp, #-4]!                      ; Save point of interrupt
    STMDB   sp!, {r4-r12, lr}                   ; Save upper half of registers
    MOV     r4, r3                              ; Save SPSR in r4
    MOV     r2, #FIQ_MODE                       ; Build FIQ mode CPSR
    MSR     CPSR_cxsf, r2                       ; Re-enter FIQ mode
    LDMIA   sp!, {r0-r3}                        ; Recover r0-r3
    MOV     r5, #SVC_MODE                       ; Build SVC mode CPSR
    MSR     CPSR_cxsf, r5                       ; Enter SVC mode
    STMDB   sp!, {r0-r3}                        ; Save r0-r3 on thread's stack
    MOV     r3, #1                              ; Build interrupt stack type
    STMDB   sp!, {r3, r4}                       ; Save interrupt stack type and SPSR
    LDR     r1, CURRENT_PTR                     ; Pickup address of current thread ptr
    LDR     r0, [r1]                            ; Pickup current thread pointer
    STR     sp, [r0, #8]                        ; Save stack pointer in thread control
                                                ;   block  */
    BIC     r4, r4, #THUMB_MASK                 ; Clear the Thumb bit of CPSR
    ORR     r3, r4, #DISABLE_INTS               ; Or-in interrupt lockout bit(s)
    MSR     CPSR_cxsf, r3                       ; Lockout interrupts
;
;    /* Save the remaining time-slice and disable it.  */
;    if (_tx_timer_time_slice)
;    {
;
    LDR     r3, TIME_SLICE                      ; Pickup time-slice variable address
    LDR     r2, [r3]                            ; Pickup time-slice
    CMP     r2, #0                              ; Is it active?
    BEQ     __tx_thread_fiq_dont_save_ts        ; No, don't save it
;
;        _tx_thread_current_ptr -> tx_time_slice =  _tx_timer_time_slice;
;        _tx_timer_time_slice =  0; 
;
    STR     r2, [r0, #24]                       ; Save thread's time-slice
    MOV     r2, #0                              ; Clear value
    STR     r2, [r3]                            ; Disable global time-slice flag
;
;    }
__tx_thread_fiq_dont_save_ts
;
;
;    /* Clear the current task pointer.  */
;    _tx_thread_current_ptr =  TX_NULL;
;
    MOV     r0, #0                              ; NULL value
    STR     r0, [r1]                            ; Clear current thread pointer
;
;    /* Return to the scheduler.  */
;    _tx_thread_schedule();
;
    B       _tx_thread_schedule                 ; Return to scheduler
;    }
;
__tx_thread_fiq_idle_system_restore
;
;    /* Just return back to the scheduler!  */
;
    ADD     sp, sp, #24                         ; Recover FIQ stack space
    MRS     r3, CPSR                            ; Pickup current CPSR
    BIC     r3, r3, #MODE_MASK                  ; Clear the mode portion of the CPSR
    ORR     r3, r3, #SVC_MODE_BITS              ; Or-in new interrupt lockout bit
    MSR     CPSR_cxsf, r3                       ; Lockout interrupts
    B       _tx_thread_schedule                 ; Return to scheduler
;
;}
    ENDIF
;
;
    IF  :DEF:TX_ENABLE_FIQ_NESTING
;/**************************************************************************/ 
;/*                                                                        */ 
;/*  FUNCTION                                               RELEASE        */ 
;/*                                                                        */ 
;/*    _tx_thread_fiq_nesting_end                           ARM7/ARM       */ 
;/*                                                           4.0c         */ 
;/*  AUTHOR                                                                */ 
;/*                                                                        */ 
;/*    William E. Lamie, Express Logic, Inc.                               */ 
;/*                                                                        */ 
;/*  DESCRIPTION                                                           */ 
;/*                                                                        */ 
;/*    This function is called by the application from FIQ mode after      */ 
;/*    _tx_thread_fiq_nesting_start has been called and switches the FIQ   */ 
;/*    processing from system mode back to FIQ mode prior to the ISR       */ 
;/*    calling _tx_thread_fiq_context_restore.  Note that this function    */ 
;/*    assumes the system stack pointer is in the same position after      */ 
;/*    nesting start function was called.                                  */ 
;/*                                                                        */ 
;/*    This function assumes that the system mode stack pointer was setup  */ 
;/*    during low-level initialization (tx_ill).                           */ 
;/*                                                                        */ 
;/*    This function returns with FIQ interrupts disabled.                 */ 
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
;/*    ISRs                                                                */ 
;/*                                                                        */ 
;/*  RELEASE HISTORY                                                       */ 
;/*                                                                        */ 
;/*    DATE              NAME                      DESCRIPTION             */ 
;/*                                                                        */ 
;/*  10-10-2002     William E. Lamie         Initial Version 4.0a          */ 
;/*  01-01-2003     William E. Lamie         Modified comment(s),          */ 
;/*                                            resulting in version 4.0b.  */ 
;/*  07-15-2004     William E. Lamie         Modified comment(s), and      */ 
;/*                                            added logic to keep 8-byte  */ 
;/*                                            alignment, resulting        */ 
;/*                                            in version 4.0c.            */ 
;/*                                                                        */ 
;/**************************************************************************/ 
;VOID   _tx_thread_fiq_nesting_end(VOID)
;{
    EXPORT  _tx_thread_fiq_nesting_end
_tx_thread_fiq_nesting_end
    MOV     r3,lr                               ; Save ISR return address
    MRS     r0, CPSR                            ; Pickup the CPSR
    ORR     r0, r0, #DISABLE_INTS               ; Build disable interrupt value
    MSR     CPSR_cxsf, r0                       ; Disable interrupts
;/*----------------------add start --------------------*/
;   LDMIA   sp!, {lr, r1}                       ; comment by wjr 
						;Pickup saved lr (and r1 throw-away for 
                                                ;   8-byte alignment logic)
    LDMIA   sp!, {lr, r12, r10, r1}             ; Pickup saved lr, ip, sl (and r1 throw-away for 
                                                ;   8-byte alignment logic) from SYS stack
;/*--------------------add end-------------------------*/                                                
    BIC     r0, r0, #MODE_MASK                  ; Clear mode bits
    ORR     r0, r0, #FIQ_MODE_BITS              ; Build FIQ mode CPSR
    MSR     CPSR_cxsf, r0                       ; Re-enter FIQ mode
    MOV     pc, r3                              ; Return to ISR
    ENDIF
;
;
SYSTEM_STATE
    DCD     _tx_thread_system_state
CURRENT_PTR
    DCD     _tx_thread_current_ptr
SYSTEM_STACK_PTR
    DCD     _tx_thread_system_stack_ptr
EXECUTE_PTR
    DCD     _tx_thread_execute_ptr
TIME_SLICE
    DCD     _tx_timer_time_slice
PREEMPT_DISABLE
    DCD     _tx_thread_preempt_disable
    END

