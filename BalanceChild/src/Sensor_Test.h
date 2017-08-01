
/*

*/

#ifndef TEST_SENSOR_H
#define	TEST_SENSOR_H

	#include "pcb.h"
	
	void Infrared_SensorTest(uint16_t voltage);
	void Temp_Cal(uint16_t temp_adc);
	void Piezoelectric_IOInit(void);
	void Piezoelectric_WeighingSensor(void);
	void Get_Weight(void);
	void Display_SampleValue(void);
#endif

/*
end of file.
*/

