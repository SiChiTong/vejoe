#include "Config.h"

//----------------------- OLED 显示---------------------------------------------
#ifdef APP_OLED_SHOW_INFORMATION
	void showSpeedValue(int speedLeft,int speedRight);
	void ShowVolCurValue(unsigned short int batteryVoltage,unsigned short int leftCurrent,unsigned short int rightCurrent);
#endif
//-----------------------End of OLED 显示---------------------------------------------

//----------------------- PID控制 ---------------------------------------------
#ifdef APP_PID_CONTROL
	//速度环初始化
	void velocityStableInitial(void);
	//速度环：左右轮子同时维持一个固定的速度不变
	void keepVelocityStable(int targetVelocity);

	//速度跳变计时器
	void jumpVelocityTimer(void);	
	//速度跳变初始化
	void jumpVelocityInitial(u16 seconds, u16 stepLength);
	//速度跳变：在最大最小值之间，每几秒改变一个段位的速度
	void JumpVelocityEachSeconds(void);
#endif
//-----------------------End of PID控制 ---------------------------------------------
