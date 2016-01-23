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
;
;
SVC_MODE        EQU     0x13                    ; SVC mode
    IF :DEF:TX_ENABLE_FIQ_SUPPORT
CPSR_MASK       EQU     0xDF                    ; Mask initial CPSR, IRQ & FIQ ints enabled
    ELSE
CPSR_MASK       EQU     0x9F                    ; Mask initial CPSR, IRQ ints enabled
    ENDIF
;
;
        AREA ||.text||, CODE, READONLY
;/**************************************************************************/ 
;/*                                                                        */ 
;/*  FUNCTION                                               RELEASE        */ 
;/*                                                                        */ 
;/*    _tx_thread_stack_build                               ARM7/ARM       */ 
;/*                                                           4.0c         */ 
;/*  AUTHOR                                                                */ 
;/*                                                                        */ 
;/*    William E. Lamie, Express Logic, Inc.                               */ 
;/*                                                                        */ 
;/*  DESCRIPTION                                                           */ 
;/*                                                                        */ 
;/*    This function builds a stack frame on the supplied thread's stack.  */
;/*    The stack frame results in a fake interrupt return to the supplied  */
;/*    function pointer.                                                   */ 
;/*                                                                        */ 
;/*  INPUT                                                                 */ 
;/*                                                                        */ 
;/*    thread_ptr                            Pointer to thread control blk */
;/*    function_ptr                          Pointer to return function    */
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
;/*    _tx_thread_create                     Create thread service         */
;/*                                                                        */ 
;/*  RELEASE HISTORY                                                       */ 
;/*                                                                        */ 
;/*    DATE              NAME                      DESCRIPTION             */ 
;/*                                                                        */ 
;/*  06-15-1997     William E. Lamie         Initial Version 3.0           */ 
;/*  11-11-1997     William E. Lamie         Modified comment(s),          */ 
;/*                                            resulting in version 3.0b.  */ 
;/*  03-01-1998     William E. Lamie         Modified comment(s),          */ 
;/*                                            resulting in version 3.0d.  */ 
;/*  01-01-1999     William E. Lamie         Modified comment(s),          */ 
;/*                                            resulting in version 3.0e.  */ 
;/*  03-01-2000     William E. Lamie         Modified comment(s),          */ 
;/*                                            resulting in version 3.0g.  */ 
;/*  01-28-2001     William E. Lamie         Modified comment(s),          */ 
;/*                                            resulting in version 4.0.   */ 
;/*  10-10-2002     William E. Lamie         Modified comment(s), added    */ 
;/*                                            support for mixed Thumb     */ 
;/*                                            mode calls, added logic to  */ 
;/*                                            preserve other bits in the  */ 
;/*                                            initial CPSR, and changed   */ 
;/*                                            to physical register names, */ 
;/*                                            resulting in version 4.0a.  */ 
;/*  01-01-2003     William E. Lamie         Modified comment(s),          */ 
;/*                                            resulting in version 4.0b.  */ 
;/*  07-15-2004     William E. Lamie         Modified comment(s), and      */ 
;/*                                            added logic for 8-byte      */ 
;/*                                            stack alignment, resulting  */ 
;/*                                            in version 4.0c.            */ 
;/*                                                                        */ 
;/**************************************************************************/ 
;VOID   _tx_thread_stack_build(TX_THREAD *thread_ptr, VOID (*function_ptr)(VOID))
;{
    EXPORT  _tx_thread_stack_build
_tx_thread_stack_build
;
;       
;    /* Build a fake interrupt frame.  The form of the fake interrupt stack
;       on the ARM7 should look like the following after it is built:
;       
;       Stack Top:      1           Interrupt stack frame type
;                       CPSR        Initial value for CPSR
;                       a1 (r0)     Initial value for a1
;                       a2 (r1)     Initial value for a2
;                       a3 (r2)     Initial value for a3
;                       a4 (r3)     Initial value for a4
;                       v1 (r4)     Initial value for v1
;                       v2 (r5)     Initial value for v2
;                       v3 (r6)     Initial value for v3
;                       v4 (r7)     Initial value for v4
;                       v5 (r8)     Initial value for v5
;                       sb (r9)     Initial value for sb
;                       sl (r10)    Initial value for sl
;                       fp (r11)    Initial value for fp
;                       ip (r12)    Initial value for ip
;                       lr (r14)    Initial value for lr
;                       pc (r15)    Initial value for pc
;                       0           For stack backtracing
;
;    Stack Bottom: (higher memory address)  */
;
    LDR     r2, [r0, #16]                       ; Pickup end of stack area
    BIC     r2, r2, #7                          ; Ensure 8-byte alignment
    SUB     r2, r2, #76                         ; Allocate space for the stack frame
;
;    /* Actually build the stack frame.  */
;
    MOV     r3, #1                              ; Build interrupt stack type
    STR     r3, [r2, #0]                        ; Store stack type
    MOV     r3, #0                              ; Build initial register value
    STR     r3, [r2, #8]                        ; Store initial r0
    STR     r3, [r2, #12]                       ; Store initial r1
    STR     r3, [r2, #16]                       ; Store initial r2
    STR     r3, [r2, #20]                       ; Store initial r3
    STR     r3, [r2, #24]                       ; Store initial r4
    STR     r3, [r2, #28]                       ; Store initial r5
    STR     r3, [r2, #32]                       ; Store initial r6
    STR     r3, [r2, #36]                       ; Store initial r7
    STR     r3, [r2, #40]                       ; Store initial r8
    STR     r3, [r2, #44]                       ; Store initial r9
    LDR     r3, [r0, #12]                       ; Pickup stack starting address
    STR     r3, [r2, #48]                       ; Store initial r10 (sl)
    MOV     r3, #0                              ; Build initial register value
    STR     r3, [r2, #52]                       ; Store initial r11
    STR     r3, [r2, #56]                       ; Store initial r12
    STR     r3, [r2, #60]                       ; Store initial lr
    STR     r1, [r2, #64]                       ; Store initial pc
    STR     r3, [r2, #68]                       ; 0 for back-trace
    MRS     r1, CPSR                            ; Pickup CPSR
    BIC     r1, r1, #CPSR_MASK                  ; Mask mode bits of CPSR
    ORR     r3, r1, #SVC_MODE                   ; Build CPSR, SVC mode, interrupts enabled
    STR     r3, [r2, #4]                        ; Store initial CPSR
;
;    /* Setup stack pointer.  */
;    thread_ptr -> tx_stack_ptr =  r2;
;
    STR     r2, [r0, #8]                        ; Save stack pointer in thread's
                                                ;   control block
    IF  {INTER} = {TRUE}
    BX      lr                                  ; Return to caller
    ELSE
    MOV     pc, lr                              ; Return to caller
    ENDIF
;}
    END

