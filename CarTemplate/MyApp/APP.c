#include "APP.h"

//----------------------- OLED 显示---------------------------------------------
#ifdef APP_OLED_SHOW_INFORMATION
	#include "Components.h"	
	void showSpeedValue(int speedLeft,int speedRight)
	{
		int showDirectNumber = 1;
		char showDirectChar = '+';
		OLED_ShowString(00,20,"LEFT");
		if( speedLeft<0)
		{
			showDirectNumber = -1;
			showDirectChar = '-';
		}
		OLED_ShowChar(60,20,showDirectChar,12,1);
		OLED_ShowNumber(75,20,showDirectNumber*speedLeft,6,12);
		//右轮速度显示
		OLED_ShowString(00,30,"RIGHT");
		showDirectNumber = 1;
		showDirectChar = '+';
		if(speedRight<0)		  
		{
			showDirectNumber = -1;
			showDirectChar = '-';
		}
		OLED_ShowChar(60,30,showDirectChar,12,1);
		OLED_ShowNumber(75,30,showDirectNumber * speedRight,6,12);
		//显示屏刷新
		OLED_Refresh_Gram();
	}
	
	void ShowVolCurValue(u16 batteryVoltage,u16 leftCurrent,u16 rightCurrent)
	{
		OLED_ShowString(00,40,"Volta");
		OLED_ShowString(58,40,".");
		OLED_ShowString(80,40,"V");
		OLED_ShowNumber(45,40,batteryVoltage/100,2,12);
		OLED_ShowNumber(68,40,batteryVoltage%100,2,12);
		if(batteryVoltage%100<10) 	
			OLED_ShowNumber(62,40,0,2,12);
		
		OLED_ShowString(00,50,"Cur");
		OLED_ShowNumber(50,50,leftCurrent,4,12);
		OLED_ShowNumber(80,50,rightCurrent,4,12);
		
		//显示屏刷新
		OLED_Refresh_Gram();
	}
#endif
//-----------------------End of OLED 显示---------------------------------------------


	
//----------------------- PID控制 ---------------------------------------------
#ifdef APP_PID_CONTROL
	
	#define USE_PID
	#include "Tools.h"
	#define COMPONENTS_HALL
	#define COMPONENTS_MOTOR
	#include "Components.h"
	#define USE_TIMER_TICK
	#include "Device.h"
	
	struct _PID_Info leftVelocityPidInfo, rightVelocityPidInfo;
	struct _PID_Info leftCurrentPidInfo, rightCurrentPidInfo;
	u16 onceStepLength, onceSeconds;
	
	//速度环初始化
	void velocityStableInitial()
	{
		float leftKP = 0.5, leftKI = 0.003, leftKD = 0.01;
		float rightKP = 1, rightKI = 0.003, rightKD = 0.01;
		float tempUpper = 3000, tempLower = -3000;
		
		Config_PID(&leftVelocityPidInfo, leftKP, leftKI,leftKD, tempUpper, tempLower);
		Config_PID(&rightVelocityPidInfo, rightKP, rightKI,rightKD, tempUpper, tempLower);
	}	
	
	//速度环
	void keepVelocityStable(int targetVelocity)
	{
		int leftSpeed, rightSpeed;
		float leftPWM, rightPWM;
		
		leftSpeed = getHallChangeSpeed(First);
		rightSpeed = getHallChangeSpeed(Second);
		
		leftPWM = Get_PID_Output(&leftVelocityPidInfo, targetVelocity - leftSpeed);
		rightPWM = Get_PID_Output(&rightVelocityPidInfo, -1*targetVelocity - rightSpeed);//右轮编码器反向所以目标值取反
		
		SetPwmValue(leftPWM,rightPWM);
	}
	
	//在最大最小值之间，每几秒改变一个段位的速度
	void JumpVelocityEachSeconds()
	{
		
	}

	void jumpVelocityInitial(u16 seconds, u16 stepLength)
	{
		onceStepLength = stepLength;
		onceSeconds = seconds;
		
		Timer_Register(TIMER_3,jumpVelocityTimer);
	}
	
	void jumpVelocityTimer()
	{
		
	}
		
	//电流环初始化
	void CurrentStableInitial()
	{
		float leftKP = 0.5, leftKI = 0.003, leftKD = 0.01;
		float rightKP = 1, rightKI = 0.003, rightKD = 0.01;
		float tempUpper = 3000, tempLower = -3000;
		
		Config_PID(&leftVelocityPidInfo, leftKP, leftKI,leftKD, tempUpper, tempLower);
		Config_PID(&rightVelocityPidInfo, rightKP, rightKI,rightKD, tempUpper, tempLower);
	}
	
	void TEST_PidControl(void)
	{
		//PID 控制器初始化
		velocityStableInitial();
		while(1)
		{
			//控制速度
			keepVelocityStable(1000);
		}
	}
#endif
//-----------------------End of PID控制 ---------------------------------------------
