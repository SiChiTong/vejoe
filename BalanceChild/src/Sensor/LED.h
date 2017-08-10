
#ifndef SENSER_LED_H
#define SENSER_LED_H
	
#include "stm32f10x_gpio.h"


	//延时循环
	void delaySoft(unsigned int timeStay);
	
	//LED灯初始化
	void InitialLED(void);
	
	//循环开关灯
	void roundLight(void);
	
	//统一开关灯
	void openCloseAllLED(void);
	
	//打开指定LED
	void OpenLED(int);
	
	//关闭指定LED
	void CloseLED(int);
	
	//获取指定LED状态
	u8 GetStatusLED(int);
	
	//测试Demo
	void LEDTest(void);
#endif
