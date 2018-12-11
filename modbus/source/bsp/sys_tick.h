#ifndef _SYS_TICK_H_
#define _SYS_TICK_H_

typedef struct 
{
	uint32_t ms;   //记录SysTick的毫秒计数
	uint32_t us;   //记录SysTick Current Value Register 的值
}sys_tick_t;


void Clear_Modbus_Tick(void);
void Start_Fram_End_Check(void);
uint8_t Get_Fram_End_Flag(void);
sys_tick_t* System_GetTick(void);

#endif

