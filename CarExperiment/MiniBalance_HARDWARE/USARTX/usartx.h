#ifndef __USRAT3_H
#define __USRAT3_H 
#include "sys.h"	  	

extern u8 Usart2_Receive;

extern uint8_t g_bluetooth_flag;

void uart1_init(u32 bound);
void USART1_IRQHandler(void);
#endif

