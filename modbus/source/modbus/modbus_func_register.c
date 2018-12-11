/****************************************Copyright (c)****************************************************
**                       		 	CJS <greyorbit@foxmail.com>
**                                    COPYRIGHT (C) 2014-2018
**                                SPDX-License-Identifier: LGPL-2.1+
**--------------File Info---------------------------------------------------------------------------------
** File name:               modbus.c
** Last modified Date:      2014-01-14
** Last Version:            1.0.0
** Descriptions:             
**							
**--------------------------------------------------------------------------------------------------------
** Created by:              CJS
** Created date:            2014-01-14
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
#include "modbus_func_register.h"
#include "modbus_hook.h"

#define READ_REG_CMD_LEN            5
#define READ_REG_ADDR_OFFSET        MB_PDU_DATA_OFFSET
#define READ_REG_CNT_OFFSET        ( MB_PDU_DATA_OFFSET + 2 )
#define READ_REG_CNT_MIN            1
#define READ_REG_CNT_MAX            0x07D0


#define WRITE_SINGLE_REG_CMD_LEN           5
#define WRITE_MULT_REG_CMD_MIN_LEN         8
#define WRITE_MULT_REG_CNT_MIN             1
#define WRITE_MULT_REG_CNT_MAX             0x07D0

#define WRITE_SINGLE_REG_VALUE_OFFSET      3

#define WRITE_REG_ADDR_OFFSET              MB_PDU_DATA_OFFSET                       /*1*/
#define WRITE_MULT_REG_CNT_OFFSET          ( MB_PDU_DATA_OFFSET + 2 )               /*3*/              
#define WRITE_MULT_REG_BYTES_CNT_OFFSET    ( WRITE_MULT_REG_CNT_OFFSET + 2 )        /*5*/
#define WRITE_MULT_REG_VALUE_OFFSET        (WRITE_MULT_REG_BYTES_CNT_OFFSET + 1)    /*6*/ 

/*********************************************************************************************************
** Function name:           Modbus_GetRegisterSize
** Descriptions:             
** input parameters:         
** output parameters:       
** Returned value:          
** Created by:              CJS
** Created Date:            2014-01-15
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
__inline static uint8_t Modbus_GetRegisterSize(uint32_t addr)
{
	if(1)
	{
		return 2;
	}
	else
	{
		return 4;
	}
}

/*********************************************************************************************************
** Function name:           Read_Register
** Descriptions:            modbus read register function, receiving and transmitting share the pdu buffer							
** input parameters:        *pdu---pointer to the pdu base address receive from the modbus master
**							*len---pointer to the pdu length receive from the modbus master
                            func_code---function code
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
static mb_exception_t Read_Register(uint8_t *pdu, uint16_t *len, uint8_t func_code)
{
	uint16_t        cnt;
	uint8_t         *cur_pdu;
	mb_reg_t reg;
	mb_exception_t  status = MB_EX_NONE;

	if( *len == READ_REG_CMD_LEN )
	{	
		/*get the starting address of registers*/	
		reg.mb_addr = (uint16_t)( pdu[READ_REG_ADDR_OFFSET] << 8 ) | pdu[READ_REG_ADDR_OFFSET + 1]; 
		/*get the quantity of registers*/
		cnt  = (uint16_t)(pdu[READ_REG_CNT_OFFSET] << 8) | pdu[READ_REG_CNT_OFFSET + 1];            
		/*-1:because the address count from zero*/
		status = Modbus_HookAddrVerify(reg.mb_addr + cnt - 1, func_code);                         
		if(status != MB_EX_NONE)
		{
			return status;	
		}
		
		if( (cnt >= READ_REG_CNT_MIN) && (cnt < READ_REG_CNT_MAX) )
		{
			cur_pdu = &pdu[MB_PDU_FUNC_OFFSET];
			*len    = 0;  
			
			/*
			 *fill function code to pdu
			 */
            *cur_pdu++ = func_code; 
            *len += 1;
			
			/*
			 * 
			 *  +-----------+---------------+----------------------------+-------------+
             *  | Address   | Function Code | Data                       | CRC/LRC     |
             *  +-----------+---------------+----------------------------+-------------+
			 *  +-----------+---------------+----------------------------+-------------+
             *  | Address   | Function Code | num_bytes|    Reg Status   | CRC/LRC     |
             *  +-----------+---------------+----------------------------+-------------+
			 *                                         ^cur_pdu++ 
			 */
			reg.reg_type = Modbus_GetRegisterSize(reg.mb_addr); 
			reg.num_bytes = cnt*reg.reg_type;                                                     

			*cur_pdu++ = reg.num_bytes;
			*len += 1;
			
		    reg.mode = MB_READ;
			Modbus_HookRegister(cur_pdu, &reg);                         
			
			*len += reg.num_bytes;

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
** Function name:           Modbus_ReadHoldingRegister
** Descriptions:            receiving and transmitting share the pdu buffer							
** input parameters:        *pdu---pointer to the pdu base address receive from the modbus master
**							*len---pointer to the pdu length receive from the modbus master
** output parameters:       *pdu---pointer to the pdu base address transmit to the modbus master   
**							*len---pointer to the pdu length transmit to the modbus master
** Returned value:          status
** Created by:              CJS
** Created Date:            2014-01-15
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
mb_exception_t Modbus_ReadHoldingRegister(uint8_t *pdu, uint16_t *len)
{
	return Read_Register(pdu, len, MB_FUNC_READ_HOLDING_REG);
}

/*********************************************************************************************************
** Function name:           Modbus_ReadInputRegister
** Descriptions:            receiving and transmitting share the pdu buffer							
** input parameters:        *pdu---pointer to the pdu base address receive from the modbus master
**							*len---pointer to the pdu length receive from the modbus master
** output parameters:       *pdu---pointer to the pdu base address transmit to the modbus master   
**							*len---pointer to the pdu length transmit to the modbus master
** Returned value:          status
** Created by:              CJS
** Created Date:            2014-01-15
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
mb_exception_t Modbus_ReadInputRegister(uint8_t *pdu, uint16_t *len)
{
	return Read_Register(pdu, len, MB_FUNC_READ_INPUT_REG);
}

/*********************************************************************************************************
** Function name:           Modbus_WriteSingleRegister
** Descriptions:            receiving and transmitting share the pdu buffer							
** input parameters:        *pdu---pointer to the pdu base address receive from the modbus master
**							*len---pointer to the pdu length receive from the modbus master
** output parameters:       
** Returned value:          status
** Created by:              CJS
** Created Date:            2014-01-15
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
mb_exception_t Modbus_WriteSingleRegister(uint8_t *pdu, uint16_t *len)
{
	mb_exception_t  status = MB_EX_NONE;
	mb_reg_t        reg;
	
    if( *len == WRITE_SINGLE_REG_CMD_LEN )
    {
		reg.mb_addr = (uint16_t)( pdu[WRITE_REG_ADDR_OFFSET] << 8 ) | pdu[WRITE_REG_ADDR_OFFSET + 1];              
		
		status = Modbus_HookAddrVerify(reg.mb_addr, MB_FUNC_WRITE_REG);
		if(status != MB_EX_NONE)
		{
			return status;	
		}
		reg.reg_type  = Modbus_GetRegisterSize(reg.mb_addr);   
		reg.num_bytes = reg.reg_type;
		reg.mode      = MB_WRITE;
	    Modbus_HookRegister(&pdu[WRITE_SINGLE_REG_VALUE_OFFSET], &reg);      
    }
    else
    {
        status = MB_EX_ILLEGAL_DATA_VALUE;
    }
    return status;
}	

/*********************************************************************************************************
** Function name:           Modbus_WriteMultipleRegister
** Descriptions:            receiving and transmitting share the pdu buffer							
** input parameters:        *pdu---pointer to the pdu base address receive from the modbus master
**							*len---pointer to the pdu length receive from the modbus master
** output parameters:       
** Returned value:          status
** Created by:              CJS
** Created Date:            2014-01-15
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
mb_exception_t Modbus_WriteMultipleRegister(uint8_t *pdu, uint16_t *len)
{
	uint8_t         reg_cnt;
	mb_reg_t        reg;
	mb_exception_t  status = MB_EX_NONE;

    if( *len >= WRITE_MULT_REG_CMD_MIN_LEN )
    {
		reg.mb_addr = (uint16_t)(pdu[WRITE_REG_ADDR_OFFSET] << 8 ) | pdu[WRITE_REG_ADDR_OFFSET + 1];             
		reg_cnt = (uint16_t)(pdu[WRITE_MULT_REG_CNT_OFFSET] << 8) | pdu[WRITE_MULT_REG_CNT_OFFSET + 1];          
				
		status = Modbus_HookAddrVerify(reg.mb_addr + reg_cnt, MB_FUNC_WRITE_MULT_REGS);
		if(status != MB_EX_NONE)
		{
			return status;	
		}
		
		reg.reg_type = Modbus_GetRegisterSize(reg.mb_addr); 
		
        if( (reg_cnt >= WRITE_MULT_REG_CNT_MIN) && (reg_cnt <= WRITE_MULT_REG_CNT_MAX) )
        {
			reg.num_bytes = reg.reg_type*reg_cnt;
			reg.mode      = MB_WRITE;
			Modbus_HookRegister(&pdu[WRITE_MULT_REG_VALUE_OFFSET], &reg);  

			*len = WRITE_MULT_REG_BYTES_CNT_OFFSET;                                                           
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




