#ifndef MODBUS_EVENT_H
#define MODBUS_EVENT_H


/* ----------------------- Type definitions ---------------------------------*/
typedef enum
{
	MB_EVENT_NORMAL,
	MB_EVENT_TIMER_EXPIRE,             
    MB_EVENT_READY,                   /*!< Startup finished. */
	MB_EVENT_FRAME_RECEIVED,          /*!< Frame received. */
    MB_EVENT_FRAME_RECEIVING,         /*!< Frame receive. */
    MB_EVENT_EXECUTE,                 /*!< Execute function. */
    MB_EVENT_FRAME_SENT               /*!< Frame sent. */
	
}mb_event_type_t;



typedef struct
{
	mb_port_t    port;
	mb_event_type_t type;
}mb_event_t;


typedef struct
{
	mb_event_t*  buf;
	uint16_t     buf_size;
	uint16_t     front;
	uint16_t     rear;
	uint16_t     num;
}mb_event_queue_t;


mb_bool Modbus_EventPost(mb_event_t event);
mb_bool Modbus_EventGet(mb_event_t* event);
void Modbus_EventInit(mb_event_t* buf, uint16_t size);

#endif

