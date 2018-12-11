#ifndef MODBUS_H
#define MODBUS_H



/*!
 * Constants which defines the format of a modbus frame. The example is
 * shown for a Modbus RTU/ASCII frame. Note that the Modbus PDU is not
 * dependent on the underlying transport.
 *
 * <code>
 * <------------------------ MODBUS FRAME (1) ----------------------------->
 *              <----------- MODBUS PDU (1') ---------------->
 *  +-----------+---------------+----------------------------+-------------+
 *  | Address   | Function Code | Data                       | CRC/LRC     |
 *  +-----------+---------------+----------------------------+-------------+
 *  |           |               |                                   |
 * (2)        (3/2')           (3')                                (4)
 *
 * (1)  ... MB_SER_PDU_SIZE_MAX    = 256
 * (2)  ... MB_SER_PDU_ADDR_OFFSET = 0
 * (3)  ... MB_SER_PDU_PDU_OFFSET  = 1
 * (4)  ... MB_SER_PDU_SIZE_CRC    = 2
 *
 * (1') ... MB_PDU_SIZE_MAX        = 253
 * (2') ... MB_PDU_FUNC_OFFSET     = 0
 * (3') ... MB_PDU_DATA_OFFSET     = 1
 * </code>
 */

/* ----------------------- Defines ------------------------------------------*/
#define MB_FRAME_SIZE_MIN        4       /*!< Minimum size of a Modbus RTU frame. */
#define MB_FRAME_SIZE_MAX        256     /*!< Maximum size of a Modbus RTU frame. */
#define MB_FRAME_SIZE_CRC        2       /*!< Size of CRC field in PDU. */
#define MB_FRAME_SIZE_ADDR       1


#define MB_FRAME_ADDR_OFFSET     0
#define MB_FRAME_PDU_OFFSET      1
#define MB_PDU_SIZE_MAX          253     /*!< Maximum size of a PDU. */
#define MB_PDU_SIZE_MIN          1       /*!< Function Code */
#define MB_PDU_FUNC_OFFSET       0       /*!< Offset of function code in PDU. */
#define MB_PDU_DATA_OFFSET       1       /*!< Offset for response data in PDU. */



typedef enum
{
    MB_NORMAL,                     /*!< no error. */
 //   MB_ERR_NOREG,                  /*!< illegal register address. */
 //   MB_ERR_INVAL,                  /*!< illegal argument. */
 //   MB_ERR_PORTERR,                /*!< porting layer error. */
 //   MB_ERR_NORES,                  /*!< insufficient resources. */
    MB_IO_ERR,                     /*!< I/O error. */
	MB_CRC_ERR,                    /*!< I/O error. */
	MB_UNKNOWN_CMD,
//    MB_ERR_ILLSTATE,               /*!< protocol stack in illegal state. */
    MB_TIMEDOUT                /*!< timeout error occurred. */
}mb_state_t;


typedef enum 
{
	MB_FALSE = 0, 
	MB_TRUE = !MB_FALSE
}mb_bool;


#define MB_ADDRESS_BROADCAST    ( 0 )   /*! Modbus broadcast address. */
#define MB_ADDRESS_MIN          ( 1 )   /*! Smallest possible slave address. */
#define MB_ADDRESS_MAX          ( 247 ) /*! Biggest possible slave address. */
#define MB_FUNC_NONE                          (  0 )
#define MB_FUNC_READ_COILS                    (  1 )
#define MB_FUNC_READ_DISCRETE_INPUTS          (  2 )
#define MB_FUNC_WRITE_SINGLE_COIL             (  5 )
#define MB_FUNC_WRITE_MULT_COILS              ( 15 )
#define MB_FUNC_READ_HOLDING_REG              (  3 )
#define MB_FUNC_READ_INPUT_REG                (  4 )
#define MB_FUNC_WRITE_REG                     (  6 )
#define MB_FUNC_WRITE_MULT_REGS               ( 16 )
#define MB_FUNC_READWRITE_MULT_REGS           ( 23 )
#define MB_FUNC_DIAG_READ_EXCEPTION           (  7 )
#define MB_FUNC_DIAG_DIAGNOSTIC               (  8 )
#define MB_FUNC_DIAG_GET_COM_EVENT_CNT        ( 11 )
#define MB_FUNC_DIAG_GET_COM_EVENT_LOG        ( 12 )
#define MB_FUNC_OTHER_REPORT_SLAVEID          ( 17 )
#define MB_FUNC_ERROR                         ( 128 )


#define MB_FUNC_DOWNLOAD_CMD              (0x7A)
#define MB_FUNC_DOWNLOAD_APP              (0x7B)
#define MB_FUNC_DOWNLOAD_PROFILE          (0x7F)

typedef enum
{
    MB_EX_NONE = 0x00,
    MB_EX_ILLEGAL_FUNCTION = 0x01,
    MB_EX_ILLEGAL_DATA_ADDRESS = 0x02,
    MB_EX_ILLEGAL_DATA_VALUE = 0x03,
    MB_EX_SLAVE_DEVICE_FAILURE = 0x04,
    MB_EX_ACKNOWLEDGE = 0x05,
    MB_EX_SLAVE_BUSY = 0x06,
    MB_EX_MEMORY_PARITY_ERROR = 0x08,
    MB_EX_GATEWAY_PATH_FAILED = 0x0A,
    MB_EX_GATEWAY_TGT_FAILED = 0x0B
}mb_exception_t;

typedef mb_exception_t(*mb_func_t) ( uint8_t *frame, uint16_t *len );


typedef struct
{
    uint8_t        code;
    mb_func_t      handler;
}mb_func_handler_t;


#include "modbus_timer.h"
#include "modbus_event.h"

typedef struct
{
	uint8_t slave_addr;
	mb_timer_t* timer;
	uint8_t timer_quantity;
	mb_event_t* event_buf;
	uint16_t event_size;
}modbus_arg_t;

void Modbus_Init(modbus_arg_t* arg);
void Modbus_Poll(void);

#endif




