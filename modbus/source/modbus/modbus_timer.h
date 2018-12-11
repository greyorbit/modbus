#ifndef MODBUS_TIMER_H
#define MODBUS_TIMER_H

typedef struct 
{
	uint32_t ms;    
	uint32_t us;    
}mb_time_t;


typedef struct 
{
	uint8_t           id;
	uint32_t          prev_cnt;
	mb_time_t        prev_time;
	uint32_t          expires;
	mb_bool           flag;
}mb_timer_t;

void Modbus_TimerPoll(void);
void Modbus_TimerEnable(void);
void Modbus_TimerInit(mb_timer_t* timer, uint8_t quantity);
void Modbus_TimerSetExpire(uint32_t n_ms);

#endif

