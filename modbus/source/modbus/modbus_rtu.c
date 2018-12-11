/****************************************Copyright (c)****************************************************
**                       		 	CJS <greyorbit@foxmail.com>
**                                    COPYRIGHT (C) 2014-2018
**                                SPDX-License-Identifier: LGPL-2.1+
**--------------File Info---------------------------------------------------------------------------------
** File name:               modbus_rtu.c
** Last modified Date:      2014-01-10
** Last Version:            1.0.0
** Descriptions:            modbus rtu
**							
**--------------------------------------------------------------------------------------------------------
** Created by:              CJS
** Created date:            2014-01-10
** Version:                 1.0.0
** Descriptions:            modbus rtu
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
#include "modbus_rtu.h"
#include "modbus_porting.h"
#include "modbus_crc16.h"

#define RTU_SEND_BUF_MAX_SIZE         500

static uint8_t  rtu_send_buf[RTU_SEND_BUF_MAX_SIZE];
/*********************************************************************************************************
** Function name:           Modbus_RtuInit     
** Descriptions:             
** input parameters:        
** output parameters:       
** Returned value:          
** Created by:              CJS
** Created Date:            2014-01-10
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
void Modbus_RtuInit(void)
{
	
}

/*********************************************************************************************************
** Function name:           Modbus_RtuReceive     
** Descriptions:             
** input parameters:        port---communicaion port number                           
** output parameters:       frame---pointer to the frame base address
                            p_pud---pointer to the pdu pointer
                            pdu_len---pointer to the pdu lenght   
** Returned value:          state---MB_NORMAL  successfully   
                                 ---other      error
** Created by:              CJS
** Created Date:            2014-01-10
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
mb_state_t Modbus_RtuReceive(mb_port_t port, uint8_t * frame, uint8_t ** p_pdu, uint16_t * pdu_len)
{
	mb_state_t state = MB_NORMAL;
	uint8_t  *rec_buf;
	uint8_t  *send_buf;
	uint32_t rec_len = 0;
	uint32_t i;
	
	rec_len = Modbus_GetRecCount(port); 
	rec_buf = Modbus_GetRecBuf(port); 
	
	send_buf = rtu_send_buf;
	i = rec_len;
	
	while(i--)
	{
		*send_buf++ = *rec_buf++;
	}
	
    if( rec_len >= MB_FRAME_SIZE_MIN )
    {
		if( Modbus_Crc16( (void *)rtu_send_buf, rec_len ) == 0 )
		{
			*frame    = rtu_send_buf[MB_FRAME_ADDR_OFFSET];
			*p_pdu   = &(rtu_send_buf[MB_FRAME_PDU_OFFSET]);
			*pdu_len = (uint16_t)( rec_len - MB_FRAME_SIZE_ADDR - MB_FRAME_SIZE_CRC );
		}
		else
		{
			state = MB_CRC_ERR;
		}
    }
    else
    {

    }
	
    return state;	
}

/*********************************************************************************************************
** Function name:           Modbus_RtuSent     
** Descriptions:             
** input parameters:        port---communicaion port number                           
** output parameters:       slave_addr---the slave address
                            p_pud---pointer to the pdu
                            pdu_len---the pdu lenght   
** Returned value:          state          
** Created by:              CJS
** Created Date:            2014-01-10
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
mb_state_t Modbus_RtuSent(mb_port_t port, uint8_t slave_addr, uint8_t *pdu, uint16_t len)
{
	mb_state_t state = MB_NORMAL;
	uint16_t crc = 0;
    uint8_t* send_frame = 0;
    uint16_t send_cnt = 0;
	
	send_frame = pdu - 1;      
	send_cnt = 1;
	
	send_frame[MB_FRAME_ADDR_OFFSET] = slave_addr;
	send_cnt += len;
	
	crc = Modbus_Crc16( (void *)send_frame, send_cnt);
	
	send_frame[send_cnt++] = (uint8_t)(crc & 0xff);
	send_frame[send_cnt++] = (uint8_t)(crc >> 8);
	
	Modbus_SendBytes(port, send_frame, send_cnt);

    return state;		
}




