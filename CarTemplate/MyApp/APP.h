#include "Config.h"

//----------------------- OLED ��ʾ---------------------------------------------
#ifdef APP_OLED_SHOW_INFORMATION
	void showSpeedValue(int speedLeft,int speedRight);
	void ShowVolCurValue(unsigned short int batteryVoltage,unsigned short int leftCurrent,unsigned short int rightCurrent);
#endif
//-----------------------End of OLED ��ʾ---------------------------------------------

//----------------------- PID���� ---------------------------------------------
#ifdef APP_CONTROL_PID
	void keepVelocityStable(u16 velocity);

#endif
//-----------------------End of PID���� ---------------------------------------------
