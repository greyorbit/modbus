/****************************************Copyright (c)****************************************************
**                       		 	CJS <greyorbit@foxmail.com>
**                                    COPYRIGHT (C) 2014-2018
**                                SPDX-License-Identifier: LGPL-2.1+
**--------------File Info---------------------------------------------------------------------------------
** File name:               modbus.c
** Last modified Date:      2014-01-09
** Last Version:            1.0.0
** Descriptions:            
**							
**--------------------------------------------------------------------------------------------------------
** Created by:              CJS
** Created date:            2014-01-09
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
#include "modbus_porting.h"
#include "modbus_hook.h"

static modbus_phy_addr_t modbus_phy_addr;
/*********************************************************************************************************
** Function name:           Modbus_HookAddrVerify
** Descriptions:             
** input parameters:        addr---modbus register or coil address
**                          func_code---modbus function code
** output parameters:       
** Returned value:          status    
** Created by:              CJS
** Created Date:            2014-01-16
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
mb_exception_t Modbus_HookAddrVerify(uint16_t addr, uint8_t func_code)
{
	mb_exception_t status = MB_EX_NONE;

	switch(func_code)
	{
		case MB_FUNC_READ_COILS:
		case MB_FUNC_WRITE_SINGLE_COIL:			
		case MB_FUNC_WRITE_MULT_COILS:
			if( !(addr >= MB_COILS_START_ADDR && addr <= MB_COILS_END_ADDR) )
				status = MB_EX_ILLEGAL_DATA_ADDRESS;
			break;
		
		case MB_FUNC_READ_DISCRETE_INPUTS:
			if( !(addr >= MB_DISCRETE_INPUT_START_ADDR && addr <= MB_DISCRETE_INPUT_END_ADDR) )
				status = MB_EX_ILLEGAL_DATA_ADDRESS;
			break;
			
		case MB_FUNC_READ_INPUT_REG:
			if( !(addr >= MB_INPUT_REGS_START_ADDR && addr <= MB_INPUT_REGS_END_ADDR) )
				status = MB_EX_ILLEGAL_DATA_ADDRESS;
			break;
		
		case MB_FUNC_READ_HOLDING_REG:	
		case MB_FUNC_WRITE_REG:
		case MB_FUNC_WRITE_MULT_REGS:
			if( !(addr >= MB_HOLDING_REGS_START_ADDR && addr <= MB_HOLDING_REGS_END_ADDR) )
				status = MB_EX_ILLEGAL_DATA_ADDRESS;
			break;
			
		default:
			status = MB_EX_ILLEGAL_FUNCTION;
			break;
	}
	
	return status;
}
/*********************************************************************************************************
** Function name:           Modbus_HookAddrRemap
** Descriptions:            Remap the modbus address to physical address of slave
** input parameters:        mb_addr---modbus address
                            phy_addr---modus slave physical address 
** output parameters:       
** Returned value:          status
** Created by:              CJS
** Created Date:            2014-01-10
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
static mb_exception_t Modbus_HookAddrRemap(uint16_t mb_addr, void** phy_addr)
{
	mb_exception_t status = MB_EX_NONE;
	 
	if((mb_addr >= MB_COILS_START_ADDR) &&(mb_addr <= MB_COILS_END_ADDR))
	{
		*phy_addr = &modbus_phy_addr.coils[mb_addr - MB_COILS_START_ADDR];
	}
	else if((mb_addr >= MB_DISCRETE_INPUT_START_ADDR) &&(mb_addr <= MB_DISCRETE_INPUT_END_ADDR))
	{
		*phy_addr = &modbus_phy_addr.discrete_input[mb_addr - MB_DISCRETE_INPUT_START_ADDR];
	}
	else if((mb_addr >= MB_HOLDING_REGS_START_ADDR) &&(mb_addr <= MB_HOLDING_REGS_END_ADDR))
	{
		*phy_addr = &modbus_phy_addr.holding_regs[mb_addr - MB_HOLDING_REGS_START_ADDR];
	}
	else if((mb_addr >= MB_INPUT_REGS_START_ADDR) &&(mb_addr <= MB_INPUT_REGS_END_ADDR))
	{
		*phy_addr = &modbus_phy_addr.input_regs[mb_addr - MB_INPUT_REGS_START_ADDR];
	}
	else 
	{
		status = MB_EX_ILLEGAL_DATA_ADDRESS;
	}

	
	return status;	
}

/*********************************************************************************************************
** Function name:           Modbus_SetBits    
** Descriptions:             
** input parameters:                                 
** output parameters:       
** Returned value:          
** Created by:              CJS
** Created Date:            2014-01-14
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
static void Modbus_SetBits(bit_t *dst, uint8_t* value, uint8_t num_bits)
{
	uint8_t i,j;
	
	for(j=0; j<(num_bits / 8); j++)
	{
		for(i=0; i<8; i++)
		{
			*dst++ = ( ( (*value) >>  i )	& 0x01 );	
		}	
		value++;
	}
	
	for(i=0; i<(num_bits % 8); i++)
	{
		*dst++ = ( ( (*value) >>  i )	& 0x01 );	
	}

}

/*********************************************************************************************************
** Function name:           Modbus_ReadBits    
** Descriptions:            
** input parameters:                              
** output parameters:       
** Returned value:          
** Created by:              CJS
** Created Date:            2014-01-14
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
static void Modbus_ReadBits(bit_t *src, uint8_t* dst, uint8_t num_bits)
{
	uint8_t i,j;
	
	for(j=0; j<(num_bits / 8); j++)
	{
		*dst = 0;
		for(i=0; i<8; i++)
		{
			*dst |= ( ( (*src++) & 0x01 ) <<  i );	
		}	
		dst++;
	}
	
	*dst = 0;
	for(i=0; i<(num_bits % 8); i++)
	{
		*dst |= ( ( (*src++) & 0x01 ) <<  i );	
	}
}

/*********************************************************************************************************
** Function name:           Modbus_ReadBytes    
** Descriptions:             
** input parameters:                               
** output parameters:       
** Returned value:          
** Created by:              CJS
** Created Date:            2014-01-14
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
static void Modbus_ReadBytes(uint8_t *src, uint8_t* dst, uint8_t num_bytes, uint8_t reg_size)
{
	uint16_t n;
	
	n = num_bytes / reg_size;
	
	switch (reg_size)
	{
		case 1:
			while(n--)
			{
				*dst++ = *src++;
			}
			break;
		case 2:		
			while(n--)
			{
				*dst = *(src + 1);
				*(dst + 1) = *src;
				dst += 2;
				src += 2;
			}
			break;
		case 4:
			while(n--)
			{
				*dst = *(src + 1);
				*(dst + 1) = *src;
				dst += 2;
				src += 2;
			}
			break;
		default:
			break;
	}

// 	while(num_bytes--)
// 	{
// 		*dst++ = *src++;
// 	}

}

/*********************************************************************************************************
** Function name:           Modbus_SetBytes    
** Descriptions:            
** input parameters:                                
** output parameters:       
** Returned value:          
** Created by:              CJS
** Created Date:            2014-01-14
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
static void Modbus_SetBytes(uint8_t *dst, uint8_t* value, uint8_t num_bytes, uint8_t reg_size)
{
	uint16_t n;
	
	n = num_bytes / reg_size;
	
	switch (reg_size)
	{
		case 1:
			while(n--)
			{
				*dst++ = *value++;
			}
			break;
		case 2:
		
			while(n--)
			{
				*dst = *(value + 1);
				*(dst + 1) = *value;
				dst += 2;
				value += 2;
			}
			break;
		case 4:
			while(n--)
			{
				*dst = *(value + 1);
				*(dst + 1) = *value;
				dst += 2;
				value += 2;
			}
			break;
		default:
			break;
	}
}

/*********************************************************************************************************
** Function name:           Modbus_HookCoils
** Descriptions:             
** input parameters:                 
** output parameters:       
** Returned value:          
** Created by:              CJS
** Created Date:            2014-01-09
**--------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:           
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
mb_exception_t Modbus_HookCoils(uint8_t *reg_buf, uint16_t mb_addr, uint16_t cnt, mb_operate_mode_t mode)
{
	void* phy_addr;	
	bit_t* p;	
	mb_exception_t status = MB_EX_NONE;
	 
	status = Modbus_HookAddrRemap(mb_addr, &phy_addr);
	
	p = (bit_t*)phy_addr;
	
	if( status != MB_EX_NONE)
		return status;
		
	switch(mode)
	{
		case MB_READ:
			Modbus_ReadBits(p, reg_buf, cnt);
			break;
		
		case MB_WRITE:
			Modbus_SetBits(p, reg_buf, cnt);		
			break;
		
		default:
			break;
	}
	
	return status;
}


/*********************************************************************************************************
** Function name:           Modbus_HookRegister
** Descriptions:            
** input parameters:                                 
** output parameters:       
** Returned value:          
** Created by:              CJS
** Created Date:            2014-01-09
**--------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:           
** Descriptions:            
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
mb_exception_t Modbus_HookRegister(uint8_t *reg_buf, mb_reg_t* reg)
{
	void* phy_addr; 
	uint8_t* p;	
	mb_exception_t status = MB_EX_NONE;
	 
	status = Modbus_HookAddrRemap(reg->mb_addr, &phy_addr);
	
	p = (uint8_t*)phy_addr;
	
	if( status != MB_EX_NONE)
		return status;
		
	switch(reg->mode)
	{
		case MB_READ:
			Modbus_ReadBytes(p, reg_buf, reg->num_bytes, reg->reg_type);
			break;
		
		case MB_WRITE:
			Modbus_SetBytes(p, reg_buf, reg->num_bytes, reg->reg_type);		
			break;
		
		default:
			break;
	}
	
	return status;
}








