#ifndef MODBUS_RTU_H
#define MODBUS_RTU_H

mb_state_t Modbus_RtuReceive(mb_port_t port, uint8_t * frame, uint8_t ** p_pdu, uint16_t * pdu_len);
mb_state_t Modbus_RtuSent(mb_port_t port, uint8_t slave_addr, uint8_t *pdu, uint16_t len);

#endif

