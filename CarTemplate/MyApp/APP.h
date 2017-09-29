#include "Config.h"

//----------------------- OLED 显示---------------------------------------------
#ifdef APP_OLED_SHOW_INFORMATION
	void showSpeedValue(int speedLeft,int speedRight);
	void ShowVolCurValue(unsigned short int batteryVoltage,unsigned short int leftCurrent,unsigned short int rightCurrent);
#endif
//-----------------------End of OLED 显示---------------------------------------------

//----------------------- PID控制 ---------------------------------------------
#ifdef APP_PID_CONTROL

typedef enum
{
	//电机不转
	MotorStopping,
	
	//电机工作
	MotorWorking,
	
	//电机加速阶段
	MotorAccelerate,
	
	//电机匀速运动
	MotorKeepSpeed,
	
	//电机减速阶段
	MotorDecelerate
}enumMotorWorkingStatus;

	//速度环：左右轮子同时维持一个固定的速度不变
	//采样频率不得低于5ms（速度的计算频率为5ms，低于5ms会导致结果不准）
	void appVelocityStable(u8 sampleFrequence,int target);
	void keepVelocityStable(void);
	//电流环：左右轮子电机的电流保持不变
	void appCurrentStable(u8 sampleFrequence, u8 target);
	void keepCurrentStable(void);

	//速度跳变计时器
	void jumpVelocityTimer(void);	
	//速度跳变：在最大最小值之间，每几秒改变一个段位的速度
	void appJumpVelocity(u16 seconds, u16 stepLength);
#endif
//-----------------------End of PID控制 ---------------------------------------------
