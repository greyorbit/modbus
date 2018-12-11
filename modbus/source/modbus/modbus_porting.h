#ifndef MODBUS_UTILS_H
#define MODBUS_UTILS_H

#define MB_COILS_SIZE            16
#define MB_DISCRETE_INPUT_SIZE   10
#define MB_HOLDING_REGS_SIZE     32
#define MB_INPUT_REGS_SIZE        8

#define MB_COILS_START_ADDR            100
#define MB_COILS_END_ADDR              (MB_COILS_START_ADDR + MB_COILS_SIZE)

#define MB_DISCRETE_INPUT_START_ADDR   500
#define MB_DISCRETE_INPUT_END_ADDR     (MB_DISCRETE_INPUT_START_ADDR + MB_DISCRETE_INPUT_SIZE)

#define MB_HOLDING_REGS_START_ADDR     1000
#define MB_HOLDING_REGS_END_ADDR       (MB_HOLDING_REGS_START_ADDR + MB_HOLDING_REGS_SIZE)

#define MB_INPUT_REGS_START_ADDR       2000
#define MB_INPUT_REGS_END_ADDR         (MB_INPUT_REGS_START_ADDR + MB_INPUT_REGS_SIZE)


typedef struct
{
	uint8_t  discrete_input[MB_DISCRETE_INPUT_SIZE];
	uint8_t  coils[MB_COILS_SIZE];
	uint16_t holding_regs[MB_HOLDING_REGS_SIZE];
	uint16_t input_regs[MB_INPUT_REGS_SIZE];
}modbus_phy_addr_t;

uint32_t Modbus_GetRecCount(mb_port_t n);
void Modbus_ClearRecCount(mb_port_t n);
uint8_t* Modbus_GetRecBuf(mb_port_t n);
void Modbus_SendBytes(mb_port_t n, uint8_t *buf, uint32_t num);
mb_time_t Modbus_GetTime(void);
#endif


