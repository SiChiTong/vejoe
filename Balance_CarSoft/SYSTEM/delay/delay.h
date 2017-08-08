#ifndef __DELAY_H
#define __DELAY_H 			   
#include "sys.h"  

	 
void delay_init(void);
void delay_us(u32 nus);
void delay_ms(u32 nus);

void SysTick_Handler(void);

#endif





























