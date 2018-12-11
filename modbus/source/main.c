#include "stm32f10x_usart.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "usart.h"
#include <stdio.h>
#include "modbus.h"
#include "modbus_typedef.h"
/* Private function prototypes -----------------------------------------------*/
#include <stddef.h>
#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

static mb_timer_t modbus_timer[2] =
{
	{
		.id = MB_PORT0,
		.expires = 3,
	},
	{
		.id = MB_PORT1,
		.expires = 4,	
	},		
	
};
mb_event_t event_buf[10];
static modbus_arg_t modbus_arg = 
{
	.slave_addr = 1,
	.timer = modbus_timer,
	.timer_quantity = 2,
	.event_buf = event_buf,
	.event_size = 10,
	
};
int main()
{
	Usart_Init(MB_PORT0);
	Usart_Init(MB_PORT1);
	SysTick_Config(SystemCoreClock / 1000);
	//printf ("Hello CJS Modbus\r\n"); 
	Modbus_Init(&modbus_arg);	

	while (1)
	{
		Modbus_Poll();
	}
}
/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  
  * @retval 
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(USART2, (uint8_t) ch);  

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET) 
  {
  
  }
  return ch;
}

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval 
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop*/
  while (1)
  {
  
  }
}
