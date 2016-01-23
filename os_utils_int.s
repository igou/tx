;/*******************************************************************************
;* Function/Macro: int tp_os_register_vector( GROUP_t group, PRIO_t prio, u32 fun ).                                                       
;* Description  : register C interrupt handler into  C_IntHandlerTbl,
;*        if fun EQUAL 0 , log out interrupt handler from C_IntHandlerTbl.
;*                                                     
;* Relation : none
;* Params   :                                                           
;*                                                                             
;*   Name       Type        In/Out      Description                            
;* --------     ----        ------      -----------                        
;*  group       GROUP_t     In          interrupt group number
;*  prio        PRIO_t      In          interrupt priority in the group
;*  fun         u32         In          interrupt handler pointer
;*
;* return value :
;* 
;* -------                  --------
;* value                    meaning
;*   0                      SUCCESS
;*   1                      FAILURE
;*                                                     
;* Computation Time £ºnone
;* Total execution time £ºnone
;*
;* Notes        :     this function is called by user before using interrupt
;*                                                           
;*******************************************************************************/
;0.01              wangjinrong                2005-10-10      base
;0.02              wangjinrong                2006-01-12      Enh00000475:   add boundary code and synchronize
;                                                                                                     code  for single interrupt open/disable

    IMPORT   C_IntHandlerTbl
    IMPORT   timer_handler_func_tbl
    AREA utils , CODE,READONLY
    
C_INTHANDLERTBL
    DCD     C_IntHandlerTbl
TIMER_HANDLERTBL
    DCD     timer_handler_func_tbl  
LAB
    DCD     LAB
    

;   EXPORT  tp_os_register_vector
    
os_register_vector_base                 ;ret=register_inthandler(group,prio,fun) 
    adr     r12, LAB            ;0--success  1---parameter error   2--memery error
    ldr     r3,  LAB                    
    teq     r12, r3
    movne   r0,  #2
    bxne    r14
    cmp     r0, #16
    bpl     error_ret
    cmp     r1, #16
    bpl     error_ret
;   cmp     r2, #0
;   beq     error_ret
    ldr     r12, C_INTHANDLERTBL        ;*( C_IntHandlerTbl + group * 64 + prio * 4) = fun
    mov     r3, r0, LSL #6
    add     r3, r3, r1, LSL #2
    add     r3, r12, r3
    str     r2, [r3]
    mov     r0, #0
    bx      r14
error_ret
    mov     r0, #1
    bx      r14 
    
    
;/*******************************************************************************
;* Function/Macro: int tp_os_unmask_irq( GROUP_t group, PRIO_t prio).                                                      
;* Description  :  unmask a interrupt source from interrupt controller , after this ,
;*         interrupt can come into interupt controllor.
;*        
;*                                                     
;* Relation : none
;* Params   :                                                           
;*                                                                             
;*   Name       Type        In/Out      Description                            
;* --------     ----        ------      -----------                        
;*  group       GROUP_t     In          interrupt group number
;*  prio        PRIO_t      In          interrupt priority in the group
;*
;*
;* return value :
;* 
;* value                    meaning
;* -------                  --------
;*   0                      SUCCESS
;*   1                      FAILURE
;*                                                                                                     
;* Computation Time £ºnone
;* Total execution time £ºnone
;*
;* Notes        :     this function is called by user before using interrupt
;* history recode:
;* ----------------------
;* 2004-12-10                    wangjinrong           base function
;* 2005-02-05                      wangjinrong           bug change for swi unmask 
;*                                                             
;*******************************************************************************/               
;   EXPORT  tp_os_unmask_irq        ;ret = tp_os_unmask_irq(GROUP,PRIO) 0--success   1--parameter error

os_unmask_irq_base
    stmfd   sp!, {r4,lr}
    mov     r4, #0
do_unmask_remask
    cmp     r0, #16
    bpl     error_ret1
    cmp     r1, #16
    bpl     error_ret1
    cmp     r0, #8
    bpl     swi_unmask
    mov     r3, #0x80000000
    mov     r2, #0x200              ;ARMHWMASK
    add     r3, r3, r2
    add     r3, r3, r0, LSL #1          ;ARMHWMASK(group)
    mov     r2, #1
    mov     r2, r2, LSL r1              ;r2= 1<<prio
    teq     r4, #0
    
    streqh  r2, [r3, #0x20]                 ;mode1 write ARMHWMASK
    strneh  r2, [r3, #0x40]                 ;clr ARMHWMASK
    b       error_ret0
swi_unmask
    cmp     r1, #3
    bpl     error_ret1
    mov     r3, #0x80000000
    mov     r2, #0x300
    add     r3, r3, r2              ;r3 ARMSWMASK
    sub     r2, r0, #8
    add     r12, r1, r2 , LSL #1            ;r12 = (group-8)*2 + prio -1
    sub     r12, r12, #1
    mov     r2, #1
    mov     r12, r2, LSL r12            ;r12= 1<<((group-8)*2 +prio-1)
    teq     r4, #0
    streqh   r12, [r3, #0x20]                   ;mode1 write ARMSWMASK
    strneh  r12,  [r3, #0x40]                   ;mode2 write ARMSWMASK
error_ret0
    mov     r0, #0
    ldmfd   sp!, {r4, lr}
    bx      r14
error_ret1
    mov     r0, #1
    ldmfd   sp!, {r4, lr}
    bx      r14
    
;/*******************************************************************************
;* Function/Macro: int tp_os_remask_irq( GROUP_t group, PRIO_t prio).                                                      
;* Description  :  remask a interrupt source from interrupt controller , after this ,
;*         interrupt CANNOT come into interupt controllor.
;*        
;*                                                     
;* Relation : none
;* Params   :                                                           
;*                                                                             
;*   Name       Type        In/Out      Description                            
;* --------     ----        ------      -----------                        
;*  group       GROUP_t     In          interrupt group number
;*  prio        PRIO_t      In          interrupt priority in the group
;*
;*
;* return value :
;* 
;* -------                  --------
;* value                    meaning
;*   0                      SUCCESS
;*   1                      FAILURE
;*                                                                                                     
;* Computation Time £ºnone
;* Total execution time £ºnone
;*
;* Notes        :     this function is called by user before using interrupt
;*                                                             
;*******************************************************************************/   
                
;;  EXPORT  tp_os_remask_irq        ;ret = tp_os_remask_irq(GROUP,PRIO) 0--success   1--parameter error
    
os_remask_irq_base
    stmfd   sp!, {r4, lr}
    mov     r4, #1  
    b       do_unmask_remask
    
;;/*-----------------------------FOLLOWING add for support CAST--------------------------------*/   
    MACRO
    CAST_ADJUST
    IF  :DEF:NOT_SUPPORT_CAST
    ELSE
    cmp r0, #8
    addpl   r1, r1, #1
    ENDIF
    MEND
    
    EXPORT  tp_os_register_vector
    
tp_os_register_vector           ;ret=register_inthandler(group,prio,fun) 
    stmfd   sp!, {r4, lr}
    CAST_ADJUST
    bl  os_register_vector_base
    ldmfd   sp!, {r4, lr}
    mov     pc, lr
    
    
    EXPORT  tp_os_unmask_irq        ;ret = tp_os_unmask_irq(GROUP,PRIO) 0--success   1--parameter error

tp_os_unmask_irq
;0.02  Enh00000475 : begin
FIRQ_DIS_MASK    equ  0xc0
;0.02 Enh00000475:  end
    stmfd   sp!, {r4,r5, lr}
    
    mrs     r5,CPSR
    orr      r4,r5,#FIRQ_DIS_MASK
    MSR  cpsr_cxsf, r4            
        
    CAST_ADJUST
    bl  os_unmask_irq_base
        
    msr   cpsr_cxsf,r5              

    ldmfd   sp!, {r4,r5,lr}
    
    mov     pc, lr
    
        
    EXPORT  tp_os_remask_irq        ;ret = tp_os_remask_irq(GROUP,PRIO) 0--success   1--parameter error
    
tp_os_remask_irq
    stmfd   sp!, {r4, r5,lr}
 ;0.02  Enh00000475: begin   
    mrs      r5,CPSR
    ORR    r4, r5,#FIRQ_DIS_MASK
    MSR   cpsr_cxsf,r4
 ;0.02  Enh00000475: end   
    CAST_ADJUST
    bl  os_remask_irq_base
MODE_MASK   equ      0x1f    
FIQ_MODE          EQU     0x11
FIQS_DIST       equ      0x0e
     msr    cpsr_cxsf,r5   
    AND       r5,r5,#MODE_MASK    
    CMP       r5,#(FIQ_MODE+FIQS_DIST)    
    MOV           R2,#0
    MOVEQ      R2,#1
    CMPNE       R5,#FIQ_MODE
    MOVEQ      R2,#1    
          
    ldmfd   sp!, {r4, r5,lr}
    
    TEQ     R2,#1
    
    moveq    r3,#0x1
    movne    r3,#0x10
wait_loop    
    mov    r2,#0x010000000
    ldr       r1,[r2,r3,LSL #2]
    subs    r3,r3,#1    
    bpl       wait_loop   
    
    mov     pc, lr  

;/*******************************************************************************
;* Function/Macro: int tp_os_timer_register( int B,unsigned int func).                                                     
;* Description  :  register  a timer interrupt to  timer_handler_func_tbl[], after this ,
;*         TIMER B,C can use
;*        
;*                                                     
;* Relation : none
;* Params   :                                                           
;*                                                                             
;*   Name       Type            In/Out      Description                            
;* --------     ----            ------      -----------                        
;*  timer_B     int             In          TIMER number
;*  func        unsigned int    In           interrupt handler
;*
;*
;* return value :
;* 
;* -------                  --------
;* value                    meaning
;*   0                      SUCCESS
;*   1                      FAILURE
;*                                                                                                     
;* Computation Time £ºnone
;* Total execution time £ºnone
;*
;* Notes        :     this function is called by user before using interrupt
;*                                                             
;*******************************************************************************/   
tp_os_timer_register            ;RET=tp_os_timer_register(int B,unsigned int func) 0--success   1--parameter error
                                ; B=1------>timerB     B=2------->timerC 
    EXPORT  tp_os_timer_register
    
    cmp     r0, #3
    bpl err_ret_timer
    cmp r0, #1
    bmi err_ret_timer
    sub r0, r0, #1
    mov r0, r0, LSL #2
    LDR r3, TIMER_HANDLERTBL
    str r1, [r3, r0]
    mov r0, #0
    mov pc, lr
err_ret_timer
    mov r0, #1
    mov pc, lr      
            
    END
        
    
    
    