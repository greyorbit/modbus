/****************************************Copyright (c)****************************************************
**                       		 	CJS <greyorbit@foxmail.com>
**                                    COPYRIGHT (C) 2014-2018
**                                SPDX-License-Identifier: LGPL-2.1+
**--------------File Info---------------------------------------------------------------------------------
** File name:               usart.c
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
#include "modbus_typedef.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "sys_tick.h"
#include "usart.h"

#define MAX_COM_NUM   2

static volatile uint8_t  uart_rec_buf[MAX_COM_NUM][1024] = {0};           
static volatile uint32_t uart_rec_cnt[MAX_COM_NUM] = {0};	              

USART_TypeDef* com_usart[MAX_COM_NUM] = {USART1, USART2}; 

GPIO_TypeDef* com_port[MAX_COM_NUM] = {USART1_GPIO, USART2_GPIO};
 
const uint32_t com_usart_clk[MAX_COM_NUM] = {USART1_CLK, USART2_CLK};

const uint32_t com_port_clk[MAX_COM_NUM] = {USART1_GPIO_CLK, USART2_GPIO_CLK};

const uint16_t com_tx_pin[MAX_COM_NUM] = {USART1_TxPin, USART2_TxPin};

const uint16_t com_rx_pin[MAX_COM_NUM] = {USART1_RxPin, USART2_RxPin};

const uint16_t com_irq[MAX_COM_NUM] = {USART1_IRQn, USART2_IRQn};
/*********************************************************************************************************
** Function name:           Usart_Nvic_Init
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
void Usart_Nvic_Init(mb_port_t n)
{
  	NVIC_InitTypeDef NVIC_InitStructure;

  	/* Configure the NVIC Preemption Priority Bits */  
 	 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  
 	 /* Enable the USART1 Interrupt */
  	NVIC_InitStructure.NVIC_IRQChannel = com_irq[n];
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
}
/*********************************************************************************************************
** Function name:           Usart_Gpio_Init
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
void Usart_Gpio_Init(mb_port_t n)
{
	GPIO_InitTypeDef GPIO_InitStructure;

 	 
 	RCC_APB2PeriphClockCmd(com_port_clk[n], ENABLE);

	 
	if(n == MB_PORT0)
	{
		RCC_APB2PeriphClockCmd(USART1_CLK, ENABLE); 
	}
	else
	{
		RCC_APB1PeriphClockCmd(USART2_CLK, ENABLE); 
	}
	
	
	 
	GPIO_InitStructure.GPIO_Pin = com_rx_pin[n];
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(com_port[n], &GPIO_InitStructure);
	
	 
	GPIO_InitStructure.GPIO_Pin = com_tx_pin[n];;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(com_port[n], &GPIO_InitStructure);
}

/*********************************************************************************************************
** Function name:           Usart_Init
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
void Usart_Init(mb_port_t n)
{
	USART_InitTypeDef USART_InitStruct;

	Usart_Nvic_Init(n);
	Usart_Gpio_Init(n);
	                                                                                
	USART_InitStruct.USART_BaudRate = 9600;                  					    
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;  						 
	USART_InitStruct.USART_StopBits = USART_StopBits_1;       						 
	USART_InitStruct.USART_Parity = USART_Parity_No;          						 
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;    
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                    
	 
	USART_Init(com_usart[n], &USART_InitStruct);
	 
	USART_ITConfig(com_usart[n], USART_IT_RXNE, ENABLE);	 
 
	USART_Cmd(com_usart[n], ENABLE);
}
/*********************************************************************************************************
** Function name:           Usart_SendByte
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
__inline void Usart_SendByte(mb_port_t n, uint8_t data)
{
	com_usart[n]->DR = (data & (uint16_t)0x01FF);
	while((com_usart[n]->SR & USART_FLAG_TXE) == (uint16_t)RESET);
}

/*********************************************************************************************************
** Function name:           Usart_SendBytes
** Descriptions:             
** input parameters:         
** output parameters:       
** Returned value:          
** Created by:              CJS
** Created Date:             2014-02-26    
**--------------------------------------------------------------------------------------------------------
** Modified by:               
** Modified date:            
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
void Usart_SendBytes(mb_port_t n, uint8_t *buf, uint32_t num)
{
    for (; num != 0; num--) 
	{
        Usart_SendByte(n, *buf);                                   
        buf++;
    }
}
/*********************************************************************************************************
** Function name:           Usart_GetByte
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
uint8_t Usart_GetByte(void)
{
  	while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
	return (uint8_t)USART1->DR;
}


#if 1
/*********************************************************************************************************
** Function name:           Usart_GetRecBuf
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
uint8_t* Usart_GetRecBuf(mb_port_t com)	
{
	return (uint8_t*)uart_rec_buf[com];
}

/*********************************************************************************************************
** Function name:           Usart_Get_Rec_Count
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
uint32_t Usart_Get_Rec_Count(mb_port_t com)	
{
	return uart_rec_cnt[com];
}

/*********************************************************************************************************
** Function name:           Usart_SetRecCount
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
void Usart_SetRecCount(mb_port_t com, uint32_t n)	
{
	 uart_rec_cnt[com] = n;
}

/*********************************************************************************************************
** Function name:           Usart_ClearRecCount
** Descriptions:             
** input parameters:        
** output parameters:       
** Returned value:          
** Created by:              CJS
** Created Date:            2014/01/07
**--------------------------------------------------------------------------------------------------------
** Modified by:                
** Modified date:              
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
void Usart_ClearRecCount(mb_port_t com)	
{
	 uart_rec_cnt[com] = 0;;
}

#endif
/*********************************************************************************************************
** Function name:           USART1_IRQHandler
** Descriptions:             
** input parameters:        
** output parameters:       
** Returned value:          
** Created by:              CJS
** Created Date:            2013/05/29
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{				
		uart_rec_buf[MB_PORT0][uart_rec_cnt[MB_PORT0]] = (uint16_t)(USART1->DR & (uint16_t)0x01FF);
		uart_rec_cnt[MB_PORT0]++;
		if(uart_rec_cnt[MB_PORT0] >= 1024)
			uart_rec_cnt[MB_PORT0] = 0;
	}
	
}
/*********************************************************************************************************
** Function name:           USART2_IRQHandler
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
void USART2_IRQHandler(void)
{
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{					
		uart_rec_buf[MB_PORT1][uart_rec_cnt[MB_PORT1]] = (uint16_t)(USART2->DR & (uint16_t)0x01FF);
		uart_rec_cnt[MB_PORT1]++;

		if(uart_rec_cnt[MB_PORT1] >= 1024)
			uart_rec_cnt[MB_PORT1] = 0;
	}
	
}
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
