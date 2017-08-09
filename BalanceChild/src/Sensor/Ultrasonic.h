#ifndef SENSER_ULTRASONIC_H
#define SENSER_ULTRASONIC_H
	
#include "stm32f10x.h"


//初始化传感器
void InitialUltrasonic(void);
	
//测试Demo
u32 readDistance(void);

#endif
