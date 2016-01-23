/**************************************************************************/ 
/*                                                                        */ 
/*            Copyright (c) 1996-2004 by Express Logic Inc.               */ 
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
/**   Port Specific                                                       */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/ 
/*                                                                        */ 
/*  PORT SPECIFIC C INFORMATION                            RELEASE        */ 
/*                                                                        */ 
/*    tx_port.h                                            ARM7/ARM       */ 
/*                                                           4.0c         */ 
/*                                                                        */
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This file contains data type definitions that make the ThreadX      */ 
/*    real-time kernel function identically on a variety of different     */ 
/*    processor architectures.  For example, the size or number of bits   */ 
/*    in an "int" data type vary between microprocessor architectures and */ 
/*    even C compilers for the same microprocessor.  ThreadX does not     */ 
/*    directly use native C data types.  Instead, ThreadX creates its     */ 
/*    own special types that can be mapped to actual data types by this   */ 
/*    file to guarantee consistency in the interface and functionality.   */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */ 
/*                                                                        */ 
/*  06-15-1997     William E. Lamie         Initial ARM7/ARM Support      */ 
/*                                            Version 3.0                 */ 
/*  07-04-1997     William E. Lamie         Incorporate new generic code  */ 
/*                                            release, resulting in       */ 
/*                                            version 3.0a.               */  
/*  11-11-1997     William E. Lamie         Incorporated new generic code */ 
/*                                            and modified comment(s),    */ 
/*                                            resulting in version 3.0b.  */ 
/*  12-25-1997     William E. Lamie         Incorporated new generic code */ 
/*                                            and modified comment(s),    */ 
/*                                            resulting in version 3.0c.  */ 
/*  03-01-1998     William E. Lamie         Incorporated new generic code */ 
/*                                            and modified comment(s),    */ 
/*                                            resulting in version 3.0d.  */ 
/*  01-01-1999     William E. Lamie         Changed version string, added */ 
/*                                            in-line assembly support,   */  
/*                                            and modified comments,      */ 
/*                                            resulting in version 3.0e.  */ 
/*  03-01-1999     William E. Lamie         Changed version string and    */ 
/*                                            changed in-line assembly,   */  
/*                                            resulting in version 3.0f.  */ 
/*  03-01-2000     William E. Lamie         Incorporated new generic code */ 
/*                                            and modified comment(s),    */ 
/*                                            resulting in version 3.0g.  */ 
/*  01-28-2001     William E. Lamie         Incorporated new generic code */ 
/*                                            and modified comment(s),    */ 
/*                                            resulting in version 4.0.   */ 
/*  10-10-2002     William E. Lamie         Modified comment(s), added    */ 
/*                                            in-line initialization      */ 
/*                                            define, added timer stack   */ 
/*                                            and priority constants,     */ 
/*                                            modified interrupt and      */
/*                                            restore macros for mixed    */
/*                                            ARM and Thumb applications, */
/*                                            resulting in version 4.0a.  */
/*  01-01-2003     William E. Lamie         Incorporated new generic code */ 
/*                                            and modified comment(s),    */ 
/*                                            resulting in version 4.0b.  */ 
/*  07-15-2004     William E. Lamie         Modified comment(s) and       */ 
/*                                            changed version string,     */ 
/*                                            resulting in version 4.0c.  */ 
/*                                                                        */ 
/**************************************************************************/ 

#ifndef TX_PORT
#define TX_PORT


/* Define various constants for the port.  */ 

#define TX_MINIMUM_STACK         200         /* Minimum stack size       */
#ifdef  TX_ENABLE_FIQ_SUPPORT
#define TX_INT_DISABLE           0xC0        /* Disable IRQ & FIQ ints   */
#else   
#define TX_INT_DISABLE           0x80        /* Disable IRQ interrupts   */
#endif
#define TX_INT_ENABLE            0x00        /* Enable IRQ interrupts    */


/* Define the system timer thread's default stack size and priority.  */

#ifndef TX_TIMER_THREAD_STACK_SIZE
#define TX_TIMER_THREAD_STACK_SIZE  1024    /* Default timer thread stack size  */
#endif
#ifndef TX_TIMER_THREAD_PRIORITY    
#define TX_TIMER_THREAD_PRIORITY    0       /* Default timer thread priority    */ 
#endif


/* Define the in-line initialization constant so that modules with in-line
   initialization capabilities can prevent their initialization from being
   a function call.  */

#define TX_INLINE_INITIALIZATION


/* Define ThreadX specific types for the port.  */ 

/*modify by fangjiayuan to avoid conflict with DT OS API*/
typedef void                        VOID;
/*#define VOID                        void*/

typedef void *                      VOID_PTR;
typedef char                        CHAR;
typedef char *                      CHAR_PTR;
typedef unsigned char               UCHAR;
typedef unsigned char *             UCHAR_PTR;
typedef int                         INT;
typedef int *                       INT_PTR;
typedef unsigned int                UINT;
typedef unsigned int *              UINT_PTR;
typedef long                        LONG;
typedef long *                      LONG_PTR;
typedef unsigned long               ULONG;
typedef unsigned long *             ULONG_PTR;


/* Define register constants for the port.  These definitions are 
   prioritized in the order they are defined.  In other words, REG_1
   is assigned to the most used variable, while REG_4 is assigned to
   the least used variable.  */

#define REG_1                       register
#define REG_2                       register
#define REG_3                       register
#define REG_4                       register
#define REG_5                       register


/* Define the port extension field of the thread control block.  Nothing 
   additional is needed for this port so it is defined as white space.  */

#define TX_THREAD_PORT_EXTENSION    


/* Define ThreadX interrupt lockout and restore macros for protection on 
   access of critical kernel information.  The restore interrupt macro must 
   restore the interrupt posture of the running thread prior to the value 
   present prior to the disable macro.  In most cases, the save area macro
   is used to define a local function save area for the disable and restore
   macros.  */

#ifndef __thumb

#define TX_INTERRUPT_SAVE_AREA      register unsigned int interrupt_save, temp;

#ifdef  TX_ENABLE_FIQ_SUPPORT
#define TX_DISABLE                  __asm \
                                    { \
                                        MRS  interrupt_save, CPSR; \
                                        ORR  temp, interrupt_save, 0xC0; \
                                        MSR  CPSR_c, temp \
                                    }
#else
#define TX_DISABLE                  __asm \
                                    { \
                                        MRS  interrupt_save, CPSR; \
                                        ORR  temp, interrupt_save, 0x80; \
                                        MSR  CPSR_c, temp \
                                    }
#endif

#define TX_RESTORE                  __asm \
                                    { \
                                        MSR CPSR_c, interrupt_save \
                                    }


#else

unsigned int   _tx_thread_interrupt_disable(UINT new_posture);
unsigned int   _tx_thread_interrupt_restore(UINT new_posture);


#define TX_INTERRUPT_SAVE_AREA      unsigned int interrupt_save;

#define TX_DISABLE                  interrupt_save =  _tx_thread_interrupt_disable(TX_INT_DISABLE);
#define TX_RESTORE                  _tx_thread_interrupt_restore(interrupt_save);
#endif

/* Define the version ID of ThreadX.  This may be utilized by the application.  */

#ifdef  TX_THREAD_INIT
CHAR                            _tx_version_id[] = 
                                    "Copyright (c) 1996-2004 Express Logic Inc. * ThreadX ARM7/ARM Version G4.0b.4.0c *";
#else
extern  CHAR                    _tx_version_id[];
#endif


#endif

