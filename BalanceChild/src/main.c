#include "Sensor/LED.h"
#include "Sensor/Clock.h"
#include "Sensor/Switch.h"

int main(void)
{
	//���Զ�ʱ����LED����
//	ClockTest();
	
	//����LED��ѭ������
//	LEDTest();
	
//	ClockGradualTest();
//	while(TRUE);
	
	InitialSwitch();
	while(TRUE)
	{
		SwitchTest();
	}
}
