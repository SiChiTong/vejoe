
#ifndef SENSER_CLOCK_H
#define SENSER_CLOCK_H
	
#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#include "LED.h"


	//初始化定时器
	void InitialTimer(void);
	
	//测试定时器Demo
	void ClockTest(void);
	
	//测试定时器渐变Demo
	void ClockGradualTest(void);
#endif
