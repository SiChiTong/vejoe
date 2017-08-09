#ifndef __TIMER_H
#define __TIMER_H

#include "Device.h"

void TIM3_Cap_Init(u16 arr,u16 psc);
void delay_init(void);

void Read_Distane(void);
void TIM3_IRQHandler(void);
void TIM8_Cap_Init(u16 arr, u16 psc);
void TIM8_CC_IRQHandler(void);

#endif
