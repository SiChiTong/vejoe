#ifndef SENSER_ULTRASONIC_H
#define SENSER_ULTRASONIC_H
	
#include "stm32f10x.h"


//��ʼ��������
void InitialUltrasonic(void);
	
//����Demo
u32 readDistance(void);

#endif
