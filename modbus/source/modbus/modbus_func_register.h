#ifndef MODBUS_FUNC_REGISTER_H
#define MODBUS_FUNC_REGISTER_H

mb_exception_t Modbus_ReadHoldingRegister(uint8_t *pdu, uint16_t *len);
mb_exception_t Modbus_WriteSingleRegister(uint8_t *pdu, uint16_t *len);
mb_exception_t Modbus_WriteMultipleRegister(uint8_t *pdu, uint16_t *len);
mb_exception_t Modbus_ReadInputRegister(uint8_t *pdu, uint16_t *len);
#endif


