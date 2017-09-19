
#include "Config.h"

#ifdef DEVICE_SAFETY_CHECK
	#define 	ADC_VALUE_COUNT								6
	typedef struct 
	{
		u8 index;
		u8 channelIdxArray[ADC_VALUE_COUNT];
		u8 weightFilterIdxArray[ADC_VALUE_COUNT];
		u8 averageFilterIdxArray[ADC_VALUE_COUNT];
		u16  adcSourceValuesArray[ADC_VALUE_COUNT];
		u16  adcWeightFilterValuesArray[ADC_VALUE_COUNT];
		u16  adcFilterResultValuesArray[ADC_VALUE_COUNT];
		
		HANDLER08 callback;
	}StructAdcInfo;
	
	enum
	{
		HardWare_ADC1 = 0,
		HardWare_ADC2,
		ADC_NUM
	};
	
	StructAdcInfo adcInfoArray[ADC_NUM];
#endif