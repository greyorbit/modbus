/****************************************Copyright (c)****************************************************
**                       		 	SAMKOON TECHNOLOGY CORP.
**                                    COPYRIGHT (C) 2014-2018
**--------------File Info---------------------------------------------------------------------------------
** File name:               920cache.c
** Last modified Date:      2013-05-29
** Last Version:            1.0.0
** Descriptions:            STM32 USART1 Çý¶¯
**
**--------------------------------------------------------------------------------------------------------
** Created by:              CJS
** Created date:            2013-05-29
** Version:                 1.0.0
** Descriptions:            STM32 USART1 Çý¶¯
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:           
** Version:                 
** Descriptions:            
**
*********************************************************************************************************/
#ifndef _USART_H_
#define _USART_H_

#include "modbus_typedef.h"
#define USART1_GPIO              GPIOA
#define USART1_CLK               RCC_APB2Periph_USART1
#define USART1_GPIO_CLK          RCC_APB2Periph_GPIOA
#define USART1_RxPin             GPIO_Pin_10
#define USART1_TxPin             GPIO_Pin_9

#define USART2_GPIO              GPIOA
#define USART2_CLK               RCC_APB1Periph_USART2
#define USART2_GPIO_CLK          RCC_APB2Periph_GPIOA
#define USART2_RxPin             GPIO_Pin_3
#define USART2_TxPin             GPIO_Pin_2

// #define EVAL_COM2                        USART2
// #define EVAL_COM2_CLK                    RCC_APB1Periph_USART2
// #define EVAL_COM2_TX_PIN                 GPIO_Pin_2
// #define EVAL_COM2_TX_GPIO_PORT           GPIOA
// #define EVAL_COM2_TX_GPIO_CLK            RCC_APB2Periph_GPIOA
// #define EVAL_COM2_RX_PIN                 GPIO_Pin_3
// #define EVAL_COM2_RX_GPIO_PORT           GPIOA
// #define EVAL_COM2_RX_GPIO_CLK            RCC_APB2Periph_GPIOA
// #define EVAL_COM2_IRQn                   USART2_IRQn

void Usart_Init(mb_port_t n);
void Usart_SendByte(mb_port_t n, uint8_t data);
void Usart_SendBytes(mb_port_t n, uint8_t *buf, uint32_t num);
uint8_t* Usart_GetRecBuf(mb_port_t com);
uint32_t Usart_Get_Rec_Count(mb_port_t com);

void Usart_SetRecCount(mb_port_t com, uint32_t n);
void Usart_ClearRecCount(mb_port_t com);


#endif

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
