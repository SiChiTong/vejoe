
#ifndef SENSER_LED_H
#define SENSER_LED_H
	
#include "stm32f10x_gpio.h"


	//��ʱѭ��
	void delaySoft(unsigned int timeStay);
	
	//LED�Ƴ�ʼ��
	void InitialLED(void);
	
	//ѭ�����ص�
	void roundLight(void);
	
	//ͳһ���ص�
	void openCloseAllLED(void);
	
	//��ָ��LED
	void OpenLED(int);
	
	//�ر�ָ��LED
	void CloseLED(int);
	
	//��ȡָ��LED״̬
	u8 GetStatusLED(int);
	
	//����Demo
	void LEDTest(void);
#endif
