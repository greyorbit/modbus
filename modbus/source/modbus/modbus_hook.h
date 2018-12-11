#ifndef MODBUS_HOOK_H
#define MODBUS_HOOK_H

typedef enum 
{
	MB_READ,
	MB_WRITE
}mb_operate_mode_t;

typedef struct
{
	uint16_t          mb_addr;       /*modbus 地址*/
	uint16_t          num_bytes;     /*modbus 操作的寄存器占的总字节数*/
	uint8_t           reg_type;      /*modbus 操作的寄存器类型8位，16位，32位*/
	mb_operate_mode_t mode;          /*modbus 操作类型，读或许写*/
}mb_reg_t;

mb_exception_t Modbus_HookCoils(uint8_t *reg_buf, uint16_t mb_addr, uint16_t cnt, mb_operate_mode_t mode);
mb_exception_t Modbus_HookRegister(uint8_t *reg_buf, mb_reg_t* reg);
mb_exception_t Modbus_HookAddrVerify(uint16_t addr, uint8_t func_code);
#endif
