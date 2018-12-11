/****************************************Copyright (c)****************************************************
**                       		 	CJS <greyorbit@foxmail.com>
**                                    COPYRIGHT (C) 2014-2018
**                                SPDX-License-Identifier: LGPL-2.1+
**--------------File Info---------------------------------------------------------------------------------
** File name:               sys_tick.c
** Last modified Date:      2014-02-26 
** Last Version:            1.0.0
** Descriptions:             
**
**--------------------------------------------------------------------------------------------------------
** Created by:              CJS
** Created date:            2014-02-26 
** Version:                 1.0.0
** Descriptions:             
**
**--------------------------------------------------------------------------------------------------------
** Modified by:				
** Modified date:  			          
** Version:                 
** Descriptions:                        
**
*********************************************************************************************************/
#include "stm32f10x.h"
#include "sys_tick.h"


static volatile sys_tick_t system_tick;

/*********************************************************************************************************
** Function name:           SysTick_Handler
** Descriptions:             					
** input parameters:        
** output parameters:       
** Returned value:          
** Created by:              CJS
** Created Date:            2014-02-26 
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
void SysTick_Handler(void)
{
	system_tick.ms ++;
}

/*********************************************************************************************************
** Function name:           System_GetTick
** Descriptions:             					
** input parameters:        
** output parameters:       
** Returned value:          
** Created by:              CJS
** Created Date:            2014-02-26 
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
sys_tick_t* System_GetTick(void)
{
	uint32_t val = 0;	
	//SysTick->CTRL &= ~(SysTick_CTRL_ENABLE_Msk);
	val = SysTick->VAL;
	//SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	system_tick.us = (72000 - val)/72;
	
	return (sys_tick_t*)&system_tick;
}


uint32_t System_GetMs(void)
{
	return system_tick.ms;
}


/*********************************************************************************************************
** Function name:           System_DelayMs
** Descriptions:             			
** input parameters:        
** output parameters:       
** Returned value:          
** Created by:              CJS
** Created Date:            2014-02-26 
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:        
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
void System_DelayMs(uint32_t n)
{
	sys_tick_t current_tick;

	current_tick.ms = system_tick.ms;
	current_tick.us = SysTick->VAL;
	while((system_tick.ms < (current_tick.ms  + n)) || (SysTick->VAL > current_tick.us))
	{	
	};
}



/*********************************************************************************************************
** Function name:           System_DelayUs
** Descriptions:             			
** input parameters:        
** output parameters:       
** Returned value:          
** Created by:              CJS
** Created Date:            2014-02-26 
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
void System_DelayUs(uint32_t n)
{

}
