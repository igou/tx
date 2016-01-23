/***************************************************************************************************
*  COPYRIGHT DaTang Mobile Communications Equipment CO.,LTD   *
*****************************************************************************************************
* Filename        : os_irq_switch.c                                                                       
*
* Description     : This file is to enable or disable interrupt 
*                                                                                                
*
* Notes           : NA                                       
*
*--------------------------------------------------------------------------------------------------------
* Change History: 
*--------------------------------------------------------------------------------------------------------
*          
*  0.01              2005-01-13       wangjinrong
*		     
*                    
*                                                       
***********************************************************************************************************/



/*******************************************************************************
* Function	: tp_os_thread_interrupt_disable												   	   
* Description	: disable interrupt
*		                                               
* Relation	: none
*		  
* Params	: 						     									
* 																			   
*   Name		Type		In/Out 		Description							   
* -------- 		---- 		------   	----------- 				       
*  none			none		none		none
*					 														   
* Return	: none
*
* Notes		: none
*  		                                    			   
*******************************************************************************/
void  tp_os_thread_interrupt_disable()
{
    __asm{
        mrs   r0, CPSR;
        orr	r0,r0,#0xc0
        msr   CPSR_c,r0
    }
}

/*******************************************************************************
* Function	: tp_os_thread_interrupt_restore												   	   
* Description	: enable interrupt
*		                                               
* Relation	: none
*		  
* Params	: 						     									
* 																			   
*   Name		Type		In/Out 		Description							   
* -------- 		---- 		------   	----------- 				       
*  none			none		none		none
*					 														   
* Return	: none
*
* Notes		: none
*  		                                    			   
*******************************************************************************/

void      tp_os_thread_interrupt_restore()
{
    __asm{
        mrs   r0, CPSR;
        bic   r0,r0,#0xc0
        msr   CPSR_c,r0
    }
}

