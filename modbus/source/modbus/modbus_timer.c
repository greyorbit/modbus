/****************************************Copyright (c)****************************************************
**                       		 	CJS <greyorbit@foxmail.com>
**                                    COPYRIGHT (C) 2014-2018
**                                SPDX-License-Identifier: LGPL-2.1+
**--------------File Info---------------------------------------------------------------------------------
** File name:               modbus_timer.c
** Last modified Date:      2014-01-06
** Last Version:            1.0.0
** Descriptions:            
**							
**--------------------------------------------------------------------------------------------------------
** Created by:              CJS
** Created date:            2014-01-06
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
#include "modbus_typedef.h"
#include "modbus.h"
#include "modbus_event.h"
#include "modbus_porting.h"
#include "modbus_timer.h"

//static mb_timer_t modbus_timer0;
//static mb_timer_t modbus_timer1;


static mb_timer_t* modbus_timer;
static uint8_t modbus_timer_quantity = 0;
/*********************************************************************************************************
** Function name:           Modbus_TimerDisable 
** Descriptions:             
** input parameters:        
** output parameters:       
** Returned value:          
** Created by:              CJS
** Created Date:            2014-01-06
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
__inline void Modbus_TimerDisable(void)
{
 
}

/*********************************************************************************************************
** Function name:           Modbus_TimerEnable 
** Descriptions:             
** input parameters:        
** output parameters:       
** Returned value:          
** Created by:              CJS
** Created Date:            2014-01-06
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
void Modbus_TimerEnable(void)
{ 
 
}

/*********************************************************************************************************
** Function name:           Modbus_TimerPoll
** Descriptions:           	 
** input parameters:         
** output parameters:       
** Returned value:          
** Created by:              CJS
** Created Date:            2014-01-02
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
void Modbus_TimerInit(mb_timer_t* timer, uint8_t quantity)
{
	modbus_timer_quantity = quantity;
	modbus_timer = timer;
}

/*********************************************************************************************************
** Function name:           Modbus_CreatTimer
** Descriptions:           	 
** input parameters:        timer---the pointer to modubus timer 
** output parameters:       
** Returned value:          
** Created by:              CJS
** Created Date:            2014-02-25
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
void Modbus_CreatTimer(mb_timer_t *timer)
{
	uint32_t          cur_cnt = 0;
	mb_time_t         cur_time;
	mb_event_t        event;
	
	event.port = (mb_port_t)timer->id;
	cur_cnt = Modbus_GetRecCount((mb_port_t)timer->id); 
	
	if( (timer->prev_cnt != cur_cnt) && (cur_cnt != 0) ) 
	{
		timer->prev_cnt = cur_cnt;	
		timer->prev_time = Modbus_GetTime();
	    timer->flag = MB_TRUE;
	}
	
	if(timer->flag == MB_TRUE)
	{
		cur_time = Modbus_GetTime();

		if( cur_time.ms >= (timer->expires + timer->prev_time.ms) ) 
		{
			timer->flag = MB_FALSE;
			event.type = MB_EVENT_FRAME_RECEIVED;
			Modbus_EventPost(event);
			timer->prev_cnt = 0;  
		}
		else
		{
			event.type = MB_EVENT_FRAME_RECEIVING;
			Modbus_EventPost(event);
		}
	}
	else
	{
 
	}
}
/*********************************************************************************************************
** Function name:           Modbus_TimerPoll
** Descriptions:           	 
** input parameters:         
** output parameters:       
** Returned value:          
** Created by:              CJS
** Created Date:            2014-01-02
**--------------------------------------------------------------------------------------------------------
** Modified by:              
** Modified date:            
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/

void Modbus_TimerPoll(void)
{
	uint8_t i;
	
	for(i=0; i<modbus_timer_quantity; i++)
	{
		Modbus_CreatTimer(&modbus_timer[i]);
	}
//	Modbus_CreatTimer(&modbus_timer0);
//	Modbus_CreatTimer(&modbus_timer1);
}


