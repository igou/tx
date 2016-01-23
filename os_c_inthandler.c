/*******************************************************************************
*  COPYRIGHT DaTang Mobile Communications Equipment CO.,LTD   *
********************************************************************************
* Filename        : os_c_inthandler.c                                                                       
*
* Description     : This file alloc C interrupt handler table, and function op XBar 
*                                                                                                
*
* Notes           : NA                                       
*
*--------------------------------------------------------------------------------
* Change History: 
*--------------------------------------------------------------------------------
*          
*  0.01              2004-12-15       wangjinrong
*******************************************************************************/

#define NO_DEFINE (unsigned int)0
#define NO_USED (unsigned int)0
#define ARMXBARCONFIG0_addr 0x80000000

/*******************************************************************************
* Function	: 												   	   
* Description	:                                            
* Relation	: C_IntHandlerTbl[] is the C interrupt handler entry, 
*		  user can register his interrupt handler into this  table.
*		  register function is tp_os_register_vector() in os_utils_int.s
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

unsigned int C_IntHandlerTbl[16*16]={
    /* GROUP 0 */
    /*	0		  1		 2		  3		  4		  5	 	   6		7		8		 9		10		11		12		13		 14		 15		*/	
    NO_DEFINE, NO_USED,NO_USED,NO_USED,NO_USED,NO_USED, NO_USED, NO_USED,NO_USED, NO_USED,NO_USED,NO_USED,NO_USED,NO_USED, NO_USED, NO_USED,
        /* GROUP 1 */
        NO_DEFINE, NO_USED,NO_USED,NO_USED,NO_USED,NO_USED, NO_USED, NO_USED,NO_USED, NO_USED,NO_USED,NO_USED,NO_USED,NO_USED, NO_USED, NO_USED,
        /* GROUP 2 */
        NO_DEFINE, NO_USED,NO_USED,NO_USED,NO_USED,NO_USED, NO_USED, NO_USED,NO_USED, NO_USED,NO_USED,NO_USED,NO_USED,NO_USED, NO_USED, NO_USED,
        /* GROUP 3 */
        NO_DEFINE, NO_USED,NO_USED,NO_USED,NO_USED,NO_USED, NO_USED, NO_USED,NO_USED, NO_USED,NO_USED,NO_USED,NO_USED,NO_USED, NO_USED, NO_USED,
        
        /* GROUP 4 */
        NO_DEFINE, NO_USED,NO_USED,NO_USED,NO_USED,NO_USED, NO_USED, NO_USED,NO_USED, NO_USED,NO_USED,NO_USED,NO_USED,NO_USED, NO_USED, NO_USED,
        /* GROUP 5*/
        NO_DEFINE, NO_USED,NO_USED,NO_USED,NO_USED,NO_USED, NO_USED, NO_USED,NO_USED, NO_USED,NO_USED,NO_USED,NO_USED,NO_USED, NO_USED, NO_USED,
        /* GROUP 6 */
        NO_DEFINE, NO_USED,NO_USED,NO_USED,NO_USED,NO_USED, NO_USED, NO_USED,NO_USED, NO_USED,NO_USED,NO_USED,NO_USED,NO_USED, NO_USED, NO_USED,
        /* GROUP 7 */
        NO_DEFINE, NO_USED,NO_USED,NO_USED,NO_USED,NO_USED, NO_USED, NO_USED,NO_USED, NO_USED,NO_USED,NO_USED,NO_USED,NO_USED, NO_USED, NO_USED,
        
        /* GROUP 8 */
        NO_DEFINE, NO_USED,NO_USED,NO_DEFINE,NO_DEFINE,NO_DEFINE, NO_DEFINE, NO_DEFINE,NO_DEFINE, NO_DEFINE,NO_DEFINE,NO_DEFINE,NO_DEFINE,NO_DEFINE, NO_DEFINE, NO_DEFINE,
        /* GROUP 9 */
        NO_DEFINE, NO_USED,NO_USED,NO_DEFINE,NO_DEFINE,NO_DEFINE, NO_DEFINE, NO_DEFINE,NO_DEFINE, NO_DEFINE,NO_DEFINE,NO_DEFINE,NO_DEFINE,NO_DEFINE, NO_DEFINE, NO_DEFINE,
        /* GROUP 10 */
        NO_DEFINE, NO_USED,NO_USED,NO_DEFINE,NO_DEFINE,NO_DEFINE, NO_DEFINE, NO_DEFINE,NO_DEFINE, NO_DEFINE,NO_DEFINE,NO_DEFINE,NO_DEFINE,NO_DEFINE, NO_DEFINE, NO_DEFINE,
        /* GROUP 11*/
        NO_DEFINE, NO_USED,NO_USED,NO_DEFINE,NO_DEFINE,NO_DEFINE, NO_DEFINE, NO_DEFINE,NO_DEFINE, NO_DEFINE,NO_DEFINE,NO_DEFINE,NO_DEFINE,NO_DEFINE, NO_DEFINE, NO_DEFINE,
        
        /* GROUP 12 */
        NO_DEFINE, NO_USED,NO_USED,NO_DEFINE,NO_DEFINE,NO_DEFINE, NO_DEFINE, NO_DEFINE,NO_DEFINE, NO_DEFINE,NO_DEFINE,NO_DEFINE,NO_DEFINE,NO_DEFINE, NO_DEFINE, NO_DEFINE,
        /* GROUP 13*/
        NO_DEFINE, NO_USED,NO_USED,NO_DEFINE,NO_DEFINE,NO_DEFINE, NO_DEFINE, NO_DEFINE,NO_DEFINE, NO_DEFINE,NO_DEFINE,NO_DEFINE,NO_DEFINE,NO_DEFINE, NO_DEFINE, NO_DEFINE,
        /* GROUP 14*/
        NO_DEFINE, NO_USED,NO_USED,NO_DEFINE,NO_DEFINE,NO_DEFINE, NO_DEFINE, NO_DEFINE,NO_DEFINE, NO_DEFINE,NO_DEFINE,NO_DEFINE,NO_DEFINE,NO_DEFINE, NO_DEFINE, NO_DEFINE,
        /* GROUP 15*/
        NO_DEFINE,NO_USED ,NO_USED,NO_DEFINE,NO_DEFINE,NO_DEFINE, NO_DEFINE, NO_DEFINE,NO_DEFINE, NO_DEFINE,NO_DEFINE,NO_DEFINE,NO_DEFINE,NO_DEFINE, NO_DEFINE, NO_DEFINE
};

/*******************************************************************************
* Function	: xbar_config												   	   
* Description	: config value of register ARMXBARCONFIG0- ARMXBARCONFIG15 ,set the 
*		  priority of all interrupt group ,this function should be called by 
*		  BSP or KERNEL before interrupt enabled .                                              
* Relation	: C_IntHandlerTbl[] is the C interrupt handler entry, 
*		  user can register his interrupt handler into this  table.
*		  register function is tp_os_register_vector() in os_utils_int.s
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
void xbar_config(void)
{
    int i;
    for(i=0;i<=15;i++)
    {
        (*(volatile unsigned short *)(ARMXBARCONFIG0_addr + 2*i )) = i;
    }
}
