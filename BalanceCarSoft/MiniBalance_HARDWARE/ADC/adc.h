#ifndef __ADC_H
#define __ADC_H	
#include "sys.h"

	#define Battery_Ch 4
	void Adc_Init(void);
	u16 Get_Adc(u8 ch);
	void Get_SampleValue(uint16_t value);

	int Get_battery_volt(void);   
	
#endif 













