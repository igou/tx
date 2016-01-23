/*******************************************************************************
*  COPYRIGHT DaTang Mobile Communications Equipment CO.,LTD														   *
********************************************************************************
* Filename	:  os_cache.c													       
*
* Description	: some function about cache													   
*
* Notes		:                                        
*
*--------------------------------------------------------------------------------
* Change History: 
*--------------------------------------------------------------------------------
*       
*
*******************************************************************************/

#include "os_low_level_type.h" 
#define MCM_DATA_MEMORY_BASE   0xB0030000
#define MCM_TAG_MEMORY_BASE    0xB0020000

#define MCM_TAG_MASK    0x3ff0
#define MCM_MAX_LINES   1024

#define EBUS_CR3	(*(volatile u16 *)( 0x8059023c ))

/* local function*/
void cache_close(void);
void  cache_init(void);
void cache_enable(void);
void cache_close(void);
u32  cache_readlinetag (u32 segmentAddress);
void   cache_invalidatetag ( u32 segmentAddress );

/*******************************************************************************
* Function	: cache_init												   	   
* Description	: initizlize cache                                                  
* Relation	:
* Params	: 						     									
* 																			   
*   Name		Type		In/Out 		Description							   
* -------- 		---- 		------   	----------- 				       
*
*					 														   
* Return	:
*
* Notes		:                                 			   
*******************************************************************************/ 
void  cache_init( void )
{  
    u16 offset; 
    //clear tag
    for ( offset = 0; offset < MCM_MAX_LINES ; offset++ )
    {
        
        *(u32*)((offset * 16) + MCM_TAG_MEMORY_BASE ) = 0;
        
    }
    
    cache_enable();
}

/////////////////////////////////////////
/*******************************************************************************
* Function	: cache_enable												   	   
* Description	: enable cache                                                  
* Relation	:
* Params	: 						     									
* 																			   
*   Name		Type		In/Out 		Description							   
* -------- 		---- 		------   	----------- 				       
*
*					 														   
* Return	:
*
* Notes		:                                 			   
*******************************************************************************/ 
void cache_enable( void )
{
    
    EBUS_CR3 |= 0x0f0 ; //enable cache 
    
}

/*******************************************************************************
* Function	: cache_close												   	   
* Description	: disable cache                                                  
* Relation	:
* Params	: 						     									
* 																			   
*   Name		Type		In/Out 		Description							   
* -------- 		---- 		------   	----------- 				       
*
*					 														   
* Return	:
*
* Notes		:                                 			   
*******************************************************************************/ 

void cache_close( void )
{
    
    
    EBUS_CR3 &= 0xff0f ; //disable cache
    
}

/////////////////////////////////////////////////////// 
/*******************************************************************************
* Function	: cache_readlinetag												   	   
* Description	: read tag from cache in address segmentAddress                                                 
* Relation	: NA
* Params	: 				     									
* 																			   
*   Name		Type		In/Out 		Description							   
* -------- 		---- 		------   	----------- 				       
*  segmentAddress       u32             in               address from which to read tag
*					 														   
* Return	: 32bit value of tag at address of segmentAddress
*
* Notes		:                                 			   
*******************************************************************************/ 

u32  cache_readlinetag (u32 segmentAddress)
{
    u32  tagValue;
    
    tagValue = *(u32 *)(( segmentAddress & MCM_TAG_MASK ) + MCM_TAG_MEMORY_BASE );
    
    return (tagValue);
}

///////////////////////////////////////////////////
/*******************************************************************************
* Function	: cache_invalidatetag												   	   
* Description	: clear the tag in address segmentAddress                                                
* Relation	: NA
* Params	: 							     									
* 																			   
*   Name		Type		In/Out 		Description							   
* -------- 		---- 		------   	----------- 				       
*   segmentAddress      u32             in             address at which to invalid tag
*					 														   
* Return	: void
*
* Notes		:                                 			   
*******************************************************************************/ 


void   cache_invalidatetag ( u32 segmentAddress )
{
    
    *(u32 *)((segmentAddress & MCM_TAG_MASK) + MCM_TAG_MEMORY_BASE ) = 0;
    
}