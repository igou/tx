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
DISABLE_INTS    EQU     0xC0                ; IRQ & FIQ interrupts disabled
    ELSE
DISABLE_INTS    EQU     0x80                ; IRQ interrupts disabled
    ENDIF
;
;
    IMPORT      _tx_thread_current_ptr
    IMPORT      _tx_timer_time_slice
    IMPORT      _tx_thread_schedule
;
;
        AREA ||.text||, CODE, READONLY
;/**************************************************************************/ 
;/*                                                                        */ 
;/*  FUNCTION                                               RELEASE        */ 
;/*                                                                        */ 
;/*    _tx_thread_system_return                             ARM7/ARM       */ 
;/*                                                           4.0c         */ 
;/*  AUTHOR                                                                */ 
;/*                                                                        */ 
;/*    William E. Lamie, Express Logic, Inc.                               */ 
;/*                                                                        */ 
;/*  DESCRIPTION                                                           */ 
;/*                                                                        */ 
;/*    This function is target processor specific.  It is used to transfer */ 
;/*    control from a thread back to the ThreadX system.  Only a           */ 
;/*    minimal context is saved since the compiler assumes temp registers  */ 
;/*    are going to get slicked by a function call anyway.                 */ 
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
;/*    _tx_thread_schedule                   Thread scheduling loop        */ 
;/*                                                                        */ 
;/*  CALLED BY                                                             */ 
;/*                                                                        */ 
;/*    ThreadX components                                                  */ 
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
;/*                                            bits of CPSR, changed to    */ 
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
;VOID   _tx_thread_system_return(VOID)
;{
    EXPORT  _tx_thread_system_return
_tx_thread_system_return
;
;    /* Save minimal context on the stack.  */
;
    MOV     r0, #0                              ; Build a solicited stack type
    MRS     r1, CPSR                            ; Pickup the CPSR
    STMDB   sp!, {r0-r1, r4-r11, lr}            ; Save minimal context
;   
;   /* Lockout interrupts.  */
;
    ORR     r2, r1, #DISABLE_INTS               ; Build disable interrupt CPSR
    MSR     CPSR_cxsf, r2                       ; Disable interrupts
    LDR     r3, CURRENT_PTR                     ; Pickup address of current ptr
    LDR     r0, [r3, #0]                        ; Pickup current thread pointer
    LDR     r2, TIME_SLICE                      ; Pickup address of time slice
    LDR     r1, [r2, #0]                        ; Pickup current time slice
;
;    /* Save current stack and switch to system stack.  */
;    _tx_thread_current_ptr -> tx_stack_ptr =  sp;
;    sp = _tx_thread_system_stack_ptr;
;
    STR     sp, [r0, #8]                        ; Save thread stack pointer
;
;    /* Determine if the time-slice is active.  */
;    if (_tx_timer_time_slice)
;    {
;
    MOV     r4, #0                              ; Build clear value
    CMP     r1, #0                              ; Is a time-slice active?
    BEQ     __tx_thread_dont_reset_ts           ; No, don t reset the time-slice
;
;   /* Setup a new time-slice for the thread and clear the current time-slice.  */
;   _tx_thread_current_ptr -> tx_time_slice =  
;                   _tx_thread_current_ptr -> tx_new_time_slice;
;   _tx_timer_time_slice =  0;
;
    LDR     r1, [r0, #28]                       ; Pickup the fresh time-slice
    STR     r4, [r2, #0]                        ; Clear time-slice
    STR     r1, [r0, #24]                       ; Setup fresh time-slice
;
;    }
__tx_thread_dont_reset_ts
;
;    /* Clear the current thread pointer.  */
;    _tx_thread_current_ptr =  TX_NULL;
;
    STR     r4, [r3, #0]                        ; Clear current thread pointer
    B       _tx_thread_schedule                 ; Jump to scheduler!
;
;}
CURRENT_PTR
    DCD     _tx_thread_current_ptr
TIME_SLICE
    DCD     _tx_timer_time_slice
    END

