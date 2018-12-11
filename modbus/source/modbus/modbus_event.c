/****************************************Copyright (c)****************************************************
**                       		 	CJS <greyorbit@foxmail.com>
**                                    COPYRIGHT (C) 2014-2018
**                                SPDX-License-Identifier: LGPL-2.1+
**--------------File Info---------------------------------------------------------------------------------
** File name:               modbus_event.c
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






static mb_event_queue_t queue;
/*********************************************************************************************************
** Function name:           Modbus_EventQueueInit  
** Descriptions:             
** input parameters:        
** output parameters:       
** Returned value:           
** Created by:              CJS
** Created Date:            2014-01-07
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
static mb_event_queue_t* Modbus_EventQueueInit(mb_event_t* buf, uint16_t size)
{
	queue.buf = buf;
	queue.buf_size = size;
	queue.front = size - 1;
	queue.rear = queue.front;
	queue.num = 0;
	return &queue;
}
/*********************************************************************************************************
** Function name:           Modbus_EventQueueIn 
** Descriptions:             
** input parameters:         
** output parameters:       
** Returned value:           
** Created by:              CJS
** Created Date:            2014-01-07
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
static mb_bool Modbus_EventQueueIn(mb_event_queue_t* q, mb_event_t event)
{
	if(q->num == queue.buf_size) 
	{ 
		return MB_FALSE;                                  
	} 
	else 
	{ 
		q->rear = (q->rear+1) % queue.buf_size; 
		q->buf[q->rear] = event; 
		q->num++; 
		return MB_TRUE;                                  
	} 	
}
/*********************************************************************************************************
** Function name:           Modbus_EventQueueOut          
** Descriptions:             
** input parameters:         
** output parameters:       
** Returned value:           
** Created by:              CJS
** Created Date:            2014-01-07
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
static mb_bool Modbus_EventQueueOut(mb_event_queue_t* q, mb_event_t* event)
{
	if(q->num == 0) 
	{ 
		return MB_FALSE;                                  
	} 
	else 
	{ 
		q->front = (q->front+1) % queue.buf_size; 
		*event = q->buf[q->front];                      
		q->num--; 
		return MB_TRUE;                                 
	} 	
}
/*********************************************************************************************************
** Function name:           Modbus_EventInit 
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
void Modbus_EventInit(mb_event_t* buf, uint16_t size)
{
	Modbus_EventQueueInit(buf, size);
}

/*********************************************************************************************************
** Function name:           Modbus_EventPost 
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
mb_bool Modbus_EventPost(mb_event_t event)
{
    
    return Modbus_EventQueueIn(&queue, event);	
}

/*********************************************************************************************************
** Function name:           Modbus_EventGet 
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
mb_bool Modbus_EventGet(mb_event_t* event)
{
	mb_bool event_happend;	
    event_happend = Modbus_EventQueueOut(&queue, event);
    return event_happend;
}


