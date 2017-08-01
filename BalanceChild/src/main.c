#include "Sensor/LED.h"
#include "Sensor/Clock.h"
#include "Sensor/Switch.h"

int main(void)
{
	//²âÊÔ¶¨Ê±Æ÷£ºLEDÁÁÃğ
//	ClockTest();
	
	//²âÊÔLED£ºÑ­»·ÁÁÃğ
//	LEDTest();
	
//	ClockGradualTest();
//	while(TRUE);
	
	InitialSwitch();
	while(TRUE)
	{
		SwitchTest();
	}
}
