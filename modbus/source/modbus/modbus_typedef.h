#ifndef PLC_TYPEDEF_H
#define PLC_TYPEDEF_H

//#define BIT_BANK

/*********************************************************************************************************

*********************************************************************************************************/
#include "stdint.h"

typedef enum 
{
	MB_PORT0 = 0,
	MB_PORT1 = 1,
	MB_PORT2 = 2,
	MB_PORT3 = 3,
} mb_port_t; 

/*********************************************************************************************************

*********************************************************************************************************/
#ifdef  BIT_BANK

typedef uint32_t   bit_t;

#else

typedef uint8_t  bit_t;

#endif




#endif