
#ifndef SENSER_CLOCK_H
#define SENSER_CLOCK_H
	
#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#include "LED.h"


	//��ʼ����ʱ��
	void InitialTimer(void);
	
	//���Զ�ʱ��Demo
	void ClockTest(void);
	
	//���Զ�ʱ������Demo
	void ClockGradualTest(void);
#endif
