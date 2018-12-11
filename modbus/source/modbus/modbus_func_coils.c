/****************************************Copyright (c)****************************************************
**                       		 	CJS <greyorbit@foxmail.com>
**                                    COPYRIGHT (C) 2014-2018
**                                SPDX-License-Identifier: LGPL-2.1+
**--------------File Info---------------------------------------------------------------------------------
** File name:               modbus_func_coils.c
** Last modified Date:      2014-01-08
** Last Version:            1.0.0
** Descriptions:            
**							
**--------------------------------------------------------------------------------------------------------
** Created by:              CJS
** Created date:            2014-01-08
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
#include "modbus_func_coils.h"
#include "modbus_hook.h"

#define READ_COILS_CMD_LEN            5
#define READ_COILS_ADDR_OFFSET        MB_PDU_DATA_OFFSET
#define READ_COILS_CNT_OFFSET        ( MB_PDU_DATA_OFFSET + 2 )
#define READ_COILS_CNT_MIN            1
#define READ_COILS_CNT_MAX            0x07D0

/*
 * <---------------------------------------   MODBUS FRAME  ---------------------------------------->
 *              <---------------------- WRITE SINGLE COIL PDU ----------------------->
 *  +-----------+---------------+-----------------------------------------------------+-------------+
 *  | Address   | Function Code |                     Data                            | CRC/LRC     |
 *  +-----------+---------------+-----------------------------------------------------+-------------+
 *  | Address   |     0x05      |Coil Addr Hi|Coil Addr Lo|Force Data Hi|Force Data Lo| CRC/LRC     |
 *  +-----------+---------------+-----------------------------------------------------+-------------+
 */
 
/*
 * <--------------------------------------------------------  MODBUS FRAME  ---------------------------------------------------------------->
 *        <------------------------------------------------ WRITE MULT COIL PDU ------------------------------------------------------------>
 *  +----+---------+----------------------------------------------------------------------------------------------------------------+-------+
 *  |Addr|Func Code|                                             Data                                                               |CRC/LRC|
 *  +----+---------+----------------------------------------------------------------------------------------------------------------+-------+
 *  |Addr|   0x0f  |Coil Addr Hi|Coil Addr Lo|Quantity of Coils Hi|Quantity of Coils Lo|Byte Count||Force Data Hi|Force Data Lo|....|CRC/LRC|
 *  +----+---------+----------------------------------------------------------------------------------------------------------------+-------+
 */
#define WRITE_SINGLE_COIL_CMD_LEN           5
#define WRITE_MULT_COILS_CMD_MIN_LEN        7
#define WRITE_MULT_COIL_CNT_MIN             1
#define WRITE_MULT_COIL_CNT_MAX             0x07D0

#define WRITE_SINGLE_COIL_VALUE_OFFSET      3

#define WRITE_COILS_ADDR_OFFSET             MB_PDU_DATA_OFFSET                       /*1*/
#define WRITE_MULT_COILS_CNT_OFFSET         ( MB_PDU_DATA_OFFSET + 2 )               /*3*/              
#define WRITE_MULT_COILS_BYTES_CNT_OFFSET   ( WRITE_MULT_COILS_CNT_OFFSET + 2 )      /*5*/
#define WRITE_MULT_COILS_VALUE_OFFSET       (WRITE_MULT_COILS_BYTES_CNT_OFFSET + 1)  /*6*/ 

/*********************************************************************************************************
** Function name:           Read_Coils
** Descriptions:            modbus read coils function, receiving and transmitting share the pdu buffer							
** input parameters:        *pdu---pointer to the pdu base address receive from the modbus master
**							*len---pointer to the pdu length receive from the modbus master
                            func_code---function code
** output parameters:       *pdu---pointer to the pdu base address transmit to the modbus master   
**							*len---pointer to the pdu length transmit to the modbus master
** Returned value:          status
** Created by:              CJS
** Created Date:            2014-01-08
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
static mb_exception_t Read_Coils(uint8_t *pdu, uint16_t *len, uint8_t func_code)
{
	uint32_t        mb_addr;
	uint16_t        cnt;
	uint8_t         *cur_pdu;
	uint8_t         ret_cnt;
	mb_exception_t  status = MB_EX_NONE;

	if( *len == READ_COILS_CMD_LEN )
	{	
		/*get the starting address of coils*/	
		mb_addr = (uint16_t)( pdu[READ_COILS_ADDR_OFFSET] << 8 ) | pdu[READ_COILS_ADDR_OFFSET + 1]; 
		/*get the quantity of coils*/
		cnt  = (uint16_t)(pdu[READ_COILS_CNT_OFFSET] << 8) | pdu[READ_COILS_CNT_OFFSET + 1];        
		/*-1:because the address count from zero*/
		status = Modbus_HookAddrVerify(mb_addr + cnt - 1, func_code);                            
		if(status != MB_EX_NONE)
		{
			return status;	
		}		
		
		if( (cnt >= READ_COILS_CNT_MIN) && (cnt < READ_COILS_CNT_MAX))
		{
			cur_pdu = &pdu[MB_PDU_FUNC_OFFSET];
			*len    = 0;
			
			/*
			 *fill function code to pdu
			 */
            *cur_pdu++ = func_code; 
            *len += 1;
			/*
			The coils in the response message are packed as one coil per bit of the data field. Status is
			indicated as 1= ON and 0= OFF. The LSB of the first data byte contains the output addressed
			in the query. The other coils follow toward the high order end of this byte, and from low order
			to high order in subsequent bytes.
			If the returned output quantity is not a multiple of eight, the remaining bits in the final data
			byte will be padded with zeros (toward the high order end of the byte). The Byte Count field
			specifies the quantity of complete bytes of data.
			*/
			if( (cnt & 0x0007) != 0 )//output quantity is not a multiple of eight
			{
				ret_cnt = (uint8_t)(cnt / 8 + 1);
			}
			else
			{
				ret_cnt = (uint8_t)(cnt / 8);
			}
			
			/*
			 *fill the retured ouput quantity
			 *  +-----------+---------------+----------------------------+-------------+
             *  | Address   | Function Code | Data                       | CRC/LRC     |
             *  +-----------+---------------+----------------------------+-------------+
			 *  +-----------+---------------+----------------------------+-------------+
             *  | Address   | Function Code | ret_cnt|    Coils Status   | CRC/LRC     |
             *  +-----------+---------------+----------------------------+-------------+
			 *                                       ^cur_pdu++ 
			 */
            *cur_pdu++ = ret_cnt;              
            *len += 1;
			/*call modbus coils hook function to fill the status of coils.*/
			Modbus_HookCoils(cur_pdu, mb_addr, cnt, MB_READ);           
			
			*len += ret_cnt;

		}
		else
		{
			status = MB_EX_ILLEGAL_DATA_VALUE;
		}
	}
	else
	{
		status = MB_EX_ILLEGAL_DATA_VALUE;
	}
	return status;
}

/*********************************************************************************************************
** Function name:           Modbus_ReadCoils
** Descriptions:            receiving and transmitting share the pdu buffer							
** input parameters:        *pdu---pointer to the pdu base address receive from the modbus master
**							*len---pointer to the pdu length receive from the modbus master
** output parameters:       *pdu---pointer to the pdu base address transmit to the modbus master   
**							*len---pointer to the pdu length transmit to the modbus master
** Returned value:          status
** Created by:              CJS
** Created Date:            2014-01-08
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
mb_exception_t Modbus_ReadCoils(uint8_t *pdu, uint16_t *len)
{
	return Read_Coils(pdu, len, MB_FUNC_READ_COILS);
}

/*********************************************************************************************************
** Function name:           Modbus_ReadDiscreteInput
** Descriptions:            receiving and transmitting share the pdu buffer							
** input parameters:        *pdu---pointer to the pdu base address receive from the modbus master
**							*len---pointer to the pdu length receive from the modbus master
** output parameters:       *pdu---pointer to the pdu base address transmit to the modbus master   
**							*len---pointer to the pdu length transmit to the modbus master
** Returned value:          status
** Created by:              CJS
** Created Date:            2014-01-16
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
mb_exception_t Modbus_ReadDiscreteInput(uint8_t *pdu, uint16_t *len)
{
	return Read_Coils(pdu, len, MB_FUNC_READ_DISCRETE_INPUTS);
}

/*********************************************************************************************************
** Function name:           Modbus_WriteSingleCoil
** Descriptions:            receiving and transmitting share the pdu buffer							
** input parameters:        *pdu---pointer to the pdu base address receive from the modbus master
**							*len---pointer to the pdu length receive from the modbus master
** output parameters:       
** Returned value:          status
** Created by:              CJS
** Created Date:            2014-01-08
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
mb_exception_t Modbus_WriteSingleCoil(uint8_t *pdu, uint16_t *len)
{
	uint32_t        mb_addr;
	uint8_t         value[2];
	mb_exception_t  status = MB_EX_NONE;

    if( *len == WRITE_SINGLE_COIL_CMD_LEN )
    {
		mb_addr = (uint16_t)( pdu[WRITE_COILS_ADDR_OFFSET] << 8 ) | pdu[WRITE_COILS_ADDR_OFFSET + 1];
		
		status = Modbus_HookAddrVerify(mb_addr, MB_FUNC_WRITE_SINGLE_COIL);
		if(status != MB_EX_NONE)
		{
			return status;	
		}	
		
        if( ( pdu[WRITE_SINGLE_COIL_VALUE_OFFSET + 1] == 0x00 ) &&
            ( ( pdu[WRITE_SINGLE_COIL_VALUE_OFFSET] == 0xFF ) ||
              ( pdu[WRITE_SINGLE_COIL_VALUE_OFFSET] == 0x00 ) ) )
        {
            value[1] = 0;
            if( pdu[WRITE_SINGLE_COIL_VALUE_OFFSET] == 0xFF )
            {
                value[0] = 1;
            }
            else
            {
                value[0] = 0;
            }
            Modbus_HookCoils(value, mb_addr, 1, MB_WRITE);      
        }
        else
        {
            status = MB_EX_ILLEGAL_DATA_VALUE;
        }
    }
    else
    {
        status = MB_EX_ILLEGAL_DATA_VALUE;
    }
    return status;
}	


/*********************************************************************************************************
** Function name:           Modbus_WriteMultipleCoils
** Descriptions:            receiving and transmitting share the pdu buffer							
** input parameters:        *pdu---pointer to the pdu base address receive from the modbus master
**							*len---pointer to the pdu length receive from the modbus master
** output parameters:       
** Returned value:          status
** Created by:              CJS
** Created Date:            2014-01-08
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
mb_exception_t Modbus_WriteMultipleCoils(uint8_t *pdu, uint16_t *len)
{

	uint32_t        mb_addr;
	uint16_t        coils_cnt;
	uint8_t         bytes_cnt;
	uint8_t         bytes_cnt_verify;
	mb_exception_t  status = MB_EX_NONE;

    if( *len >= WRITE_MULT_COILS_CMD_MIN_LEN )
    {
		mb_addr   = (uint16_t)( pdu[WRITE_COILS_ADDR_OFFSET] << 8 ) | pdu[WRITE_COILS_ADDR_OFFSET + 1];          
		coils_cnt = (uint16_t)( pdu[WRITE_MULT_COILS_CNT_OFFSET] << 8) | pdu[WRITE_MULT_COILS_CNT_OFFSET + 1];     
		bytes_cnt = pdu[WRITE_MULT_COILS_BYTES_CNT_OFFSET];                                                       

		status = Modbus_HookAddrVerify(mb_addr+coils_cnt, MB_FUNC_WRITE_MULT_COILS);
		if(status != MB_EX_NONE)
		{
			return status;	
		}	
		
		if( (coils_cnt & 0x0007) != 0 )
        {
            bytes_cnt_verify = (uint8_t)(coils_cnt / 8 + 1);
        }
        else
        {
            bytes_cnt_verify = (uint8_t)(coils_cnt / 8);
        }
		
		
        if( (bytes_cnt_verify == bytes_cnt) && 
			(coils_cnt >= WRITE_MULT_COIL_CNT_MIN) && 
			(coils_cnt <= WRITE_MULT_COIL_CNT_MAX) )
        {
            Modbus_HookCoils(&pdu[WRITE_MULT_COILS_VALUE_OFFSET], mb_addr, coils_cnt, MB_WRITE);      

			*len = WRITE_MULT_COILS_BYTES_CNT_OFFSET;                                                           
        }
        else
        {
            status = MB_EX_ILLEGAL_DATA_VALUE;
        }
    }
    else
    {
        status = MB_EX_ILLEGAL_DATA_VALUE;
    }
    return status;
}


