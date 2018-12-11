/****************************************Copyright (c)****************************************************
**                       		 	CJS <greyorbit@foxmail.com>
**                                    COPYRIGHT (C) 2014-2018
**                                SPDX-License-Identifier: LGPL-2.1+ 
**--------------File Info---------------------------------------------------------------------------------
** File name:               modbus.c
** Last modified Date:      2014-01-03
** Last Version:            1.0.0
** Descriptions:            
**							
**--------------------------------------------------------------------------------------------------------
** Created by:              CJS
** Created date:            2014-01-03
** Version:                 1.0.0
** Descriptions:            modbus  
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
#include "modbus_func_coils.h"
#include "modbus_func_register.h"
#include "modbus_func_custom.h"
#include "modbus_rtu.h"
#include "modbus_porting.h"
#include "modbus_timer.h"


static uint8_t modbus_slave_addr = 0;

static const mb_func_handler_t  mb_func_handler[] =
{
	{MB_FUNC_READ_COILS,                 Modbus_ReadCoils},
	{MB_FUNC_READ_DISCRETE_INPUTS,       Modbus_ReadDiscreteInput},
	{MB_FUNC_WRITE_SINGLE_COIL,          Modbus_WriteSingleCoil},
	{MB_FUNC_WRITE_MULT_COILS,           Modbus_WriteMultipleCoils},
	{MB_FUNC_READ_HOLDING_REG,           Modbus_ReadHoldingRegister},
	{MB_FUNC_WRITE_REG,                  Modbus_WriteSingleRegister},
	{MB_FUNC_WRITE_MULT_REGS,            Modbus_WriteMultipleRegister},
	{MB_FUNC_READ_INPUT_REG,             Modbus_ReadInputRegister},
};

/*********************************************************************************************************
** Function name:           Modbus_Init
** Descriptions:            Initializes  modbus slave address, timer, and event.       
** input parameters:        id---modbus slave address
**                          expire---modbus interframe gap 
** output parameters:          
** Returned value:           
** Created by:              CJS
** Created Date:            2014-01-03
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
void Modbus_Init(modbus_arg_t* arg)
{
	modbus_slave_addr = arg->slave_addr;
	Modbus_TimerInit(arg->timer,arg->timer_quantity);
	Modbus_EventInit(arg->event_buf, arg->event_size);
}
/*********************************************************************************************************
** Function name:           Modbus_Received
** Descriptions:             
** input parameters:        port---communicaion port number                           
** output parameters:       frame---pointer to the frame base address
                            p_pud---pointer to the pdu pointer
                            pdu_len---pointer to the pdu lenght  
** output parameters:       
** Returned value:           
** Created by:              CJS
** Created Date:            2014-01-03
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
void Modbus_EventReceived(mb_port_t port,uint8_t* frame, uint8_t **p_pdu, uint16_t *pdu_len)
{
	mb_state_t         state;
	mb_event_t         event;
	
	event.port = port;
	
	state = Modbus_RtuReceive(port, frame, p_pdu, pdu_len);
	/*
	 *Clear the receive buffer count, so that the modbus port able to receive data 
	 *during responding
	 */
	Modbus_ClearRecCount(port);                                            
	if(state == MB_NORMAL)
	{
		if( (modbus_slave_addr == *frame) || (MB_ADDRESS_BROADCAST == *frame) )
		{	
		    event.type = MB_EVENT_EXECUTE;
			Modbus_EventPost(event);
		}
	}
}
/*********************************************************************************************************
** Function name:           Modbus_EventExecute
** Descriptions:            
** input parameters:        port---communicaion port number                           
** output parameters:       frame---pointer to the frame base address
                            pud---pointer to the pdu pointer
                            pdu_len---pointer to the pdu lenght  
** output parameters:           
** Returned value:           
** Created by:              CJS
** Created Date:            2014-01-03
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
void Modbus_EventExecute(mb_port_t port, uint8_t slave_addr, uint8_t *pdu, uint16_t *pdu_len)
{
    uint8_t            func_code;	
	uint16_t           i;
	mb_exception_t     exception;
	
	func_code = pdu[MB_PDU_FUNC_OFFSET];
	exception = MB_EX_ILLEGAL_FUNCTION;

	for( i = 0; i < ( sizeof(mb_func_handler) / sizeof(mb_func_handler_t) ); i++ )
	{
		if( mb_func_handler[i].code == func_code )
		{
			exception = mb_func_handler[i].handler(pdu, pdu_len);
			break;
		}
	}

	if(slave_addr != MB_ADDRESS_BROADCAST)
	{
		if( exception != MB_EX_NONE )
		{
			/* An exception occured. Build an error frame. */
			*pdu_len = 0;
			pdu[(*pdu_len)++] = (uint8_t)( func_code | MB_FUNC_ERROR );
			pdu[(*pdu_len)++] = exception;
		}
		Modbus_RtuSent(port, slave_addr, pdu, *pdu_len);
	}	
}

/*********************************************************************************************************
** Function name:           Modbus_Poll
** Descriptions:             
** input parameters:        
** output parameters:       
** Returned value:             
** Created by:              CJS
** Created Date:            2014-01-03
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
void Modbus_Poll(void)
{
	static uint8_t     *pdu[3]; 
	static uint8_t     frame[3];
	static uint16_t    pdu_len[3]; 

	mb_event_t         event;

	Modbus_TimerPoll();
	if( Modbus_EventGet(&event) == MB_TRUE )
	{
		switch(event.type)
		{
			case MB_EVENT_READY:

				break;
			
			case MB_EVENT_FRAME_RECEIVED:			
				Modbus_EventReceived(event.port, &frame[event.port], &pdu[event.port], &pdu_len[event.port]);
				break;
			
			case MB_EVENT_FRAME_RECEIVING:
				
				break;			
			
			case MB_EVENT_EXECUTE:				
				Modbus_EventExecute(event.port, frame[event.port], pdu[event.port], &pdu_len[event.port]);				
				break;
			
			case MB_EVENT_FRAME_SENT:
				//Indicate the slave is senting a frame for DMA or multithreading.
				break;
			
			default:
				break;
		}
	}
}

