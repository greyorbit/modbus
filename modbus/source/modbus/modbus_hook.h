#ifndef MODBUS_HOOK_H
#define MODBUS_HOOK_H

typedef enum 
{
	MB_READ,
	MB_WRITE
}mb_operate_mode_t;

typedef struct
{
	uint16_t          mb_addr;       /*modbus ��ַ*/
	uint16_t          num_bytes;     /*modbus �����ļĴ���ռ�����ֽ���*/
	uint8_t           reg_type;      /*modbus �����ļĴ�������8λ��16λ��32λ*/
	mb_operate_mode_t mode;          /*modbus �������ͣ�������д*/
}mb_reg_t;

mb_exception_t Modbus_HookCoils(uint8_t *reg_buf, uint16_t mb_addr, uint16_t cnt, mb_operate_mode_t mode);
mb_exception_t Modbus_HookRegister(uint8_t *reg_buf, mb_reg_t* reg);
mb_exception_t Modbus_HookAddrVerify(uint16_t addr, uint8_t func_code);
#endif
