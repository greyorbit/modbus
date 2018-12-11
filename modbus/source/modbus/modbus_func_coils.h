#ifndef MODBUS_FUNC_COILS_H
#define MODBUS_FUNC_COILS_H


mb_exception_t Modbus_ReadCoils(uint8_t *frame, uint16_t *len);
mb_exception_t Modbus_ReadDiscreteInput(uint8_t *pdu, uint16_t *len);
mb_exception_t Modbus_WriteSingleCoil(uint8_t *pdu, uint16_t *len);
mb_exception_t Modbus_WriteMultipleCoils(uint8_t *pdu, uint16_t *len);

#endif
