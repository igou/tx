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
;
    IF  :DEF:TX_ENABLE_FIQ_SUPPORT
DISABLE_INTS    EQU     0xC0                    ; IRQ & FIQ interrupts disabled
    ELSE
DISABLE_INTS    EQU     0x80                    ; IRQ interrupts disabled
    ENDIF
FIQ_DISABLE     EQU     0x40                    ; FIQ disable bit
IRQ_DISABLE     EQU     0x80                    ; IRQ disable bit
MODE_MASK       EQU     0x1F                    ; Mode mask
SYS_MODE_BITS   EQU     0x1F                    ; System mode bits

    IMPORT      _tx_thread_system_state
    IMPORT      _tx_thread_current_ptr
    IMPORT      __tx_irq_processing_return
    IF  :DEF:TX_ENABLE_FIQ_SUPPORT
    IMPORT      __tx_fiq_processing_return
    ENDIF
;
;
        AREA ||.text||, CODE, READONLY
;/**************************************************************************/ 
;/*                                                                        */ 
;/*  FUNCTION                                               RELEASE        */ 
;/*                                                                        */ 
;/*    _tx_thread_context_save                              ARM7/ARM       */ 
;/*                                                           4.0c         */ 
;/*  AUTHOR                                                                */ 
;/*                                                                        */ 
;/*    William E. Lamie, Express Logic, Inc.                               */ 
;/*                                                                        */ 
;/*  DESCRIPTION                                                           */ 
;/*                                                                        */ 
;/*    This function saves the context of an executing thread in the       */ 
;/*    beginning of interrupt processing.  The function also ensures that  */ 
;/*    the system stack is used upon return to the calling ISR.            */ 
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
;VOID   _tx_thread_context_save(VOID)
;{
    EXPORT  _tx_thread_context_save
_tx_thread_context_save
;
;    /* Upon entry to this routine, it is assumed that IRQ interrupts are locked
;       out, we are in IRQ mode, and all registers are intact.  */
;
;    /* Check for a nested interrupt condition.  */
;    if (_tx_thread_system_state++)
;    {
;
    STMDB   sp!, {r0-r3}                        ; Save some working registers 
    IF :DEF:TX_ENABLE_FIQ_SUPPORT
    MRS     r0, CPSR                            ; Pickup the CPSR
    ORR     r0, r0, #DISABLE_INTS               ; Build disable interrupt CPSR
    MSR     CPSR_cxsf, r0                       ; Disable interrupts
    ENDIF
    LDR     r3, SYSTEM_STATE                    ; Pickup address of system state var
    LDR     r2, [r3, #0]                        ; Pickup system state
    CMP     r2, #0                              ; Is this the first interrupt?
    BEQ     __tx_thread_not_nested_save         ; Yes, not a nested context save
;
;    /* Nested interrupt condition.  */
;
    ADD     r2, r2, #1                          ; Increment the interrupt counter
    STR     r2, [r3, #0]                        ; Store it back in the variable
;
;   /* Save the rest of the scratch registers on the stack and return to the
;      calling ISR.  */
;
    MRS     r0, SPSR                            ; Pickup saved SPSR
    SUB     lr, lr, #4                          ; Adjust point of interrupt 
    STMDB   sp!, {r0, r10, r12, lr}             ; Store other registers
;
;    /* Return to the ISR.  */
;
    MOV     r10, #0                             ; Clear stack limit
    B       __tx_irq_processing_return          ; Continue IRQ processing 
;
__tx_thread_not_nested_save
;    }
;
;    /* Otherwise, not nested, check to see if a thread was running.  */
;    else if (_tx_thread_current_ptr)
;    {
;
    ADD     r2, r2, #1                          ; Increment the interrupt counter
    STR     r2, [r3, #0]                        ; Store it back in the variable
    LDR     r1, CURRENT_PTR                     ; Pickup address of current thread ptr
    LDR     r0, [r1, #0]                        ; Pickup current thread pointer
    CMP     r0, #0                              ; Is it NULL?
    BEQ     __tx_thread_idle_system_save        ; If so, interrupt occurred in 
                                                ;   scheduling loop - nothing needs saving!
;
;    /* Save minimal context of interrupted thread.  */
;
    MRS     r2, SPSR                            ; Pickup saved SPSR
    SUB     lr, lr, #4                          ; Adjust point of interrupt 
    STMDB   sp!, {r2, r10, r12, lr}             ; Store other registers
;
;    /* Save the current stack pointer in the thread's control block.  */
;    _tx_thread_current_ptr -> tx_stack_ptr =  sp;
;
;    /* Switch to the system stack.  */
;    sp =  _tx_thread_system_stack_ptr;
;
    MOV     r10, #0                             ; Clear stack limit
    B       __tx_irq_processing_return          ; Continue IRQ processing 
;
;    }
;    else
;    {
;
__tx_thread_idle_system_save
;
;    /* Interrupt occurred in the scheduling loop.  */
;
;    /* Not much to do here, just adjust the stack pointer, and return to IRQ 
;       processing.  */
;
    MOV     r10, #0                             ; Clear stack limit
    ADD     sp, sp, #16                         ; Recover saved registers
    B       __tx_irq_processing_return          ; Continue IRQ processing  
;
;    }
;}
;
;
    IF  :DEF:TX_ENABLE_IRQ_NESTING
;/**************************************************************************/ 
;/*                                                                        */ 
;/*  FUNCTION                                               RELEASE        */ 
;/*                                                                        */ 
;/*    _tx_thread_irq_nesting_start                         ARM7/ARM       */ 
;/*                                                           4.0c         */ 
;/*  AUTHOR                                                                */ 
;/*                                                                        */ 
;/*    William E. Lamie, Express Logic, Inc.                               */ 
;/*                                                                        */ 
;/*  DESCRIPTION                                                           */ 
;/*                                                                        */ 
;/*    This function is called by the application from IRQ mode after      */ 
;/*    _tx_thread_context_save has been called and switches the IRQ        */ 
;/*    processing to the system mode so nested IRQ interrupt processing    */ 
;/*    is possible (system mode has its own "lr" register).  Note that     */ 
;/*    this function assumes that the system mode stack pointer was setup  */ 
;/*    during low-level initialization (tx_ill).                           */ 
;/*                                                                        */ 
;/*    This function returns with IRQ interrupts enabled.                  */ 
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
;VOID   _tx_thread_irq_nesting_start(VOID)
;{
    EXPORT  _tx_thread_irq_nesting_start
_tx_thread_irq_nesting_start
    MOV     r3,lr                               ; Save ISR return address
    MRS     r0, CPSR                            ; Pickup the CPSR
    BIC     r0, r0, #MODE_MASK                  ; Clear the mode bits
    ORR     r0, r0, #SYS_MODE_BITS              ; Build system mode CPSR
    MSR     CPSR_cxsf, r0                       ; Enter system mode
    STMDB   sp!, {lr, r1}                       ; Push the system mode lr on the system mode stack
                                                ;   and push r1 just to keep 8-byte alignment
    BIC     r0, r0, #IRQ_DISABLE                ; Build enable IRQ CPSR
    MSR     CPSR_cxsf, r0                       ; Enter system mode
    MOV     pc, r3                              ; Return to ISR
    ENDIF
;
;
    IF  :DEF:TX_ENABLE_FIQ_SUPPORT
;/**************************************************************************/ 
;/*                                                                        */ 
;/*  FUNCTION                                               RELEASE        */ 
;/*                                                                        */ 
;/*    _tx_thread_fiq_context_save                          ARM7/ARM       */ 
;/*                                                           4.0c         */ 
;/*  AUTHOR                                                                */ 
;/*                                                                        */ 
;/*    William E. Lamie, Express Logic, Inc.                               */ 
;/*                                                                        */ 
;/*  DESCRIPTION                                                           */ 
;/*                                                                        */ 
;/*    This function saves the context of an executing thread in the       */ 
;/*    beginning of interrupt processing.  The function also ensures that  */ 
;/*    the system stack is used upon return to the calling ISR.            */ 
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
;/*  07-15-2004     William E. Lamie         Modified comment(s),          */ 
;/*                                            resulting in version 4.0c.  */ 
;/*                                                                        */ 
;/**************************************************************************/ 
; VOID   _tx_thread_fiq_context_save(VOID)
;{
    EXPORT  _tx_thread_fiq_context_save
_tx_thread_fiq_context_save
;
;    /* Upon entry to this routine, it is assumed that IRQ interrupts are locked
;       out, we are in IRQ mode, and all registers are intact.  */
;
;    /* Check for a nested interrupt condition.  */
;    if (_tx_thread_system_state++)
;    {
;
    STMDB   sp!, {r0-r3}                        ; Save some working registers 
    LDR     r3, SYSTEM_STATE                    ; Pickup address of system state var
    LDR     r2, [r3]                            ; Pickup system state
    CMP     r2, #0                              ; Is this the first interrupt?
    BEQ     __tx_thread_fiq_not_nested_save     ; Yes, not a nested context save
;
;    /* Nested interrupt condition.  */
;
    ADD     r2, r2, #1                          ; Increment the interrupt counter
    STR     r2, [r3]                            ; Store it back in the variable
;
;    /* Save the rest of the scratch registers on the stack and return to the
;       calling ISR.  */
;
    MRS     r0, SPSR                            ; Pickup saved SPSR
    SUB     lr, lr, #4                          ; Adjust point of interrupt 
;   STMDB   sp!, {r0, r10, r12, lr}             ;comment by wjr
						; Store other registers
;/*------------------------add start by wjr-------------------------*/		
	STMDB   sp!, {r0, lr}             	;no save r10,r12_fiq,because i design according to:
						;1. in FIQ mode never ennable interrupt
						;2. never insert C code between this function and 
						;  _tx_thread_fiq_nesting_start
;/*------------------------add end--------------------------------*/	

;
;    /* Return to the ISR.  */
;
    MOV     r10, #0                             ; Clear stack limit
    B       __tx_fiq_processing_return          ; Continue FIQ processing 
;
__tx_thread_fiq_not_nested_save
;    }  
;
;    /* Otherwise, not nested, check to see if a thread was running.  */
;    else if (_tx_thread_current_ptr)
;    {   
;
    ADD     r2, r2, #1                          ; Increment the interrupt counter
    STR     r2, [r3]                            ; Store it back in the variable
    LDR     r1, CURRENT_PTR                     ; Pickup address of current thread ptr
    LDR     r0, [r1]                            ; Pickup current thread pointer
    CMP     r0, #0                              ; Is it NULL?
    BEQ     __tx_thread_fiq_idle_system_save    ; If so, interrupt occurred in 
;                                               ;   scheduling loop - nothing needs saving! 
;
;    /* Save minimal context of interrupted thread.  */
;
    MRS     r2, SPSR                            ; Pickup saved SPSR
    SUB     lr, lr, #4                          ; Adjust point of interrupt 
    STMDB   sp!, {r2, lr}                       ; Store other registers, Note that we don't
;                                               ;   need to save sl and ip since FIQ has 
;                                               ;   copies of these registers.  Nested 
;                                               ;   interrupt processing does need to save
;                                               ;   these registers.
;
;    /* Save the current stack pointer in the thread's control block.  */
;    _tx_thread_current_ptr -> tx_stack_ptr =  sp; 
;
;    /* Switch to the system stack.  */
;    sp =  _tx_thread_system_stack_ptr; 
;loop_test1
;	b loop_test1
;
    MOV     r10, #0                             ; Clear stack limit
    B       __tx_fiq_processing_return          ; Continue FIQ processing 
;
;   }
;   else
;   {
;
__tx_thread_fiq_idle_system_save
;
;    /* Interrupt occurred in the scheduling loop.  */
;
;    /* Not much to do here, save the current SPSR and LR for possible
;       use in IRQ interrupted in idle system conditions, and return to 
;       FIQ interrupt processing.  */
;
    MRS     r0, SPSR                            ; Pickup saved SPSR
    SUB     lr, lr, #4                          ; Adjust point of interrupt 
    STMDB   sp!, {r0, lr}                       ; Store other registers that will get used
;                                               ;   or stripped off the stack in context 
;                                               ;   restore 
;/*----------------------------------------add start by wjr -------------*/

    MOV     r10, #0                             ; Clear stack limit
    
 ;/*----------------------------------------add end --------------------*/
   B       __tx_fiq_processing_return          ; Continue FIQ processing 

;
;    }
;}  
    ENDIF


    IF  :DEF:TX_ENABLE_FIQ_NESTING
;/**************************************************************************/ 
;/*                                                                        */ 
;/*  FUNCTION                                               RELEASE        */ 
;/*                                                                        */ 
;/*    _tx_thread_fiq_nesting_start                         ARM7/ARM       */ 
;/*                                                           4.0c         */ 
;/*  AUTHOR                                                                */ 
;/*                                                                        */ 
;/*    William E. Lamie, Express Logic, Inc.                               */ 
;/*                                                                        */ 
;/*  DESCRIPTION                                                           */ 
;/*                                                                        */ 
;/*    This function is called by the application from FIQ mode after      */ 
;/*    _tx_thread_fiq_context_save has been called and switches the FIQ    */ 
;/*    processing to the system mode so nested FIQ interrupt processing    */ 
;/*    is possible (system mode has its own "lr" register).  Note that     */ 
;/*    this function assumes that the system mode stack pointer was setup  */ 
;/*    during low-level initialization (tx_ill).                           */ 
;/*                                                                        */ 
;/*    This function returns with FIQ interrupts enabled.                  */ 
;/*                                                                        */ 
;/*  INPUT                                                                 */ 
;/*                                                                        */ 
;/*    ;None                                                                */ 
;/*	   r2 => mode1 read of CIREG => current highest priority interrupt group Number,  */
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
;/*	26-11-2004		wangjinrong	Add code to save r12_sys, r10_sys. */
;/*						add code to zero r10_sys.	*/
;/*						add code to Read CIReg Code on last base .*/
;/*                                      add comment .                      */ 
;/**************************************************************************/ 
;VOID   _tx_thread_fiq_nesting_start(VOID)
;{
    EXPORT  _tx_thread_fiq_nesting_start
_tx_thread_fiq_nesting_start
;	b _tx_thread_fiq_nesting_start
    MOV     r3,lr                               ; Save ISR return address
    MRS     r0, CPSR                            ; Pickup the CPSR
    BIC     r0, r0, #MODE_MASK                  ; Clear the mode bits
    ORR     r0, r0, #SYS_MODE_BITS              ; Build system mode CPSR
    MSR     CPSR_cxsf, r0                       ; Enter system mode
;   STMDB   sp!, {lr, r1}                       ; comment by wjr
						;Push the system mode lr on the system mode stack
                                                ;   and push r1 just to keep 8-byte alignment
;/*----------------------------------------add start by wjr -------------*/
    STMDB   sp!, {lr, r12, r10, r1}             ; Push the system mode lr and sl and ip on the system mode stack
    mov		r10, #0				;stack limit check in SYS mode.

	ldr		r1, MCU_CIREG		;read CIREG mode 0 addr.
;	add		r1, r1, #0x20		;convert to mode 1
	ldrh	r2, [r1, #0x20]			;mode 1 read CIREG, get current highest priority 
						;interrupt group Number, at the same time set AIREG,
						;and clear FIQ LINE and clr CIREG to Zero
	and		r2, r2, #0xF		;need only lowest 4 bit.	
	mov		r1, #0x80000000		;IRQXBARCONFIG0 addr.
	add		r1, r1, r2, LSL #1	;xbarop addr.
	ldrh	r2, [r1]			;mode0 read xbarconfig, get group Number
						;  corresponding to CIREG mode1 read.
;loop_test	
;	b loop_test	
;/*----------------------------------------add end------------------------*/
    BIC     r0, r0, #FIQ_DISABLE                ; Build enable FIQ CPSR
    MSR     CPSR_cxsf, r0                       ; Enter system mode
    MOV     pc, r3                              ; Return to ISR
    ENDIF
;
;
SYSTEM_STATE
    DCD     _tx_thread_system_state
CURRENT_PTR
    DCD     _tx_thread_current_ptr
;/*-------------------------------add start by wjr -------------------------*/
MCU_CIREG
	DCD		0x80000180		;ARMCIReg mode0 addr
;/*----------------------------------------add end-------------	*/
    END

