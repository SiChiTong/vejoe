
/*

*/

#ifndef TEST_SENSOR_H
#define	TEST_SENSOR_H

	#include "sys.h"
	
	void Infrared_SensorTest(uint16_t voltage);
	void Temp_Cal(uint16_t temp_adc);
	void Piezoelectric_IOInit(void);
	void Piezoelectric_WeighingSensor(void);
	void Get_Weight(void);
	void Display_SampleValue(void);
	//----------------------------//
	//ADC
	//Ó²¼þADC¸öÊý 
	typedef void (*HANDLER08)(uint16_t);
	
	enum
	{
	HardWare_ADC1 = 0,
	HardWare_ADC2,
	ADC_NUM
	};
	//channel
	enum
	{
		ADC_CHANNEL_IN0 = 0x0001,
		ADC_CHANNEL_IN1 = 0x0002,
		ADC_CHANNEL_IN2 = 0x0004,
		ADC_CHANNEL_IN3 = 0x0008,	
		ADC_CHANNEL_IN4 = 0x0010,
		ADC_CHANNEL_IN5 = 0x0020,
		ADC_CHANNEL_IN6 = 0x0040,
		ADC_CHANNEL_IN7 = 0x0080,
		ADC_CHANNEL_IN8 = 0x0100,
		ADC_CHANNEL_IN9 = 0x0200,
		ADC_CHANNEL_IN10 = 0x0400,
		ADC_CHANNEL_IN11 = 0x0800,	
		ADC_CHANNEL_IN12 = 0x1000,
		ADC_CHANNEL_IN13 = 0x2000,
		ADC_CHANNEL_IN14 = 0x4000,
		ADC_CHANNEL_IN15 = 0x8000,
		ADC_CHANNEL_NUM = 16
	};
	//struct 
	struct adc_info
	{
		uint8_t index;
		uint16_t  adc_value[6];
		HANDLER08 callback;
	};
	void IO_Config(void);
	void Config_ADC(uint8_t adc, uint16_t channel, HANDLER08 _callback);
	void ADC1_2_IRQHandler(void); 
	void Enable_ADC(uint8_t adc, uint8_t state);
	void Set_SampleChannel(uint8_t adc, uint8_t ADC_Channel);
	void ADC_DMAConfig(void);
	void Get_ADCSampleValue(uint8_t channel, uint16_t *value);		

	//---------------------------------///
	uint16_t Weight_Filter(uint8_t filter_bit, u16 *order_value);
	uint16_t Average_Filter(uint8_t filter_bit, u16 *order_value);
	
	void Move_Array(uint8_t array_bit, u16 *array);	
#endif

/*
end of file.
*/

