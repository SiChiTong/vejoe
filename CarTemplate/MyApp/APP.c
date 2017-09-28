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
	
	#define ZERO_DRIFT_CALC_COUNT	1000//零漂计算次数
	
	struct _PID_Info leftVelocityPidInfo, rightVelocityPidInfo;
	struct _PID_Info leftCurrentPidInfo, rightCurrentPidInfo;
	u16 onceStepLength, onceSecondsTime, currentTimeCount;
	int currentChangeDirect, targetVelocity;
	u8 speedSampleFre, speedSampleCount;
  u8 currentSampleFre, currentSampleCount,	targetCurrent;
	u8 leftCurrentAdcIdx, rightCurrentAdcIdx;
	enumMotorWorkingStatus currentDeviceStatus;
	u16 zeroDriftCount;
	
	//速度环初始化
	void appVelocityStable(u8 sampleFrequence,int target)
	{
		targetVelocity = target;
		speedSampleFre = sampleFrequence;
		speedSampleCount = 0;
		
		float tempKP = 0.2, tempKI = 0.01, tempKD = 0;
		float tempUpper = 3000, tempLower = -3000;
//		float tempUpper = PWM_EXTREME_VALUE, tempLower = -PWM_EXTREME_VALUE;
		
		Config_PID(&leftVelocityPidInfo, tempKP, tempKI,tempKD, tempUpper, tempLower);		
		Config_PID(&rightVelocityPidInfo, tempKP, tempKI,tempKD, tempUpper, tempLower);		
		
		Timer_Register(TIMER_3,keepVelocityStable);		
	}	
	
	//速度环
	void keepVelocityStable(void)
	{		
		if(speedSampleCount < speedSampleFre)		
 		{		
 			speedSampleCount ++;		
 			return;		
 		}		
 		speedSampleCount = 0;	
		
		int leftSpeed, rightSpeed;
		float leftPWM, rightPWM;
		
		leftSpeed = getHallChangeSpeed(HallEncoderLeftWheel);
		rightSpeed = getHallChangeSpeed(HallEncoderRightWheel);
		
		leftPWM = Get_PID_Output(&leftVelocityPidInfo, targetVelocity - leftSpeed);
		rightPWM = Get_PID_Output(&rightVelocityPidInfo, -1 * targetVelocity - rightSpeed);//右轮编码器反向所以目标值取反
		
		SetPwmValue((int)leftPWM,(int)rightPWM);
	}
	
	//在最大最小值之间，每几秒改变一个段位的速度
	void jumpVelocityTimer(void)
	{
		currentTimeCount ++ ; 
		if(currentTimeCount < onceSecondsTime) return;		
		currentTimeCount = 0;
		
		if(targetVelocity + onceStepLength >= PWM_EXTREME_VALUE)
			currentChangeDirect = -1;
		else if(targetVelocity - onceStepLength < onceStepLength)
			currentChangeDirect = 1;
		targetVelocity += (currentChangeDirect * onceStepLength);
	}

	void appJumpVelocity(u16 seconds, u16 stepLength)
	{
		onceStepLength = stepLength;
		onceSecondsTime = seconds * 1000;//时钟为1ms
		targetVelocity = onceStepLength;
		currentTimeCount = 0;
		currentChangeDirect = 1;
		
		if(leftVelocityPidInfo._Kp  == 0 || rightVelocityPidInfo._Kp == 0)
		{
			appVelocityStable(5,targetVelocity);
		}
		
		Timer_Register(TIMER_3,jumpVelocityTimer);
	}
		
	//电流环初始化
	void appCurrentStable(u8 sampleFrequence, u8 target,u8 leftCurIdx, u8 rightCurIdx)
	{
		targetCurrent = target;
		currentSampleFre = sampleFrequence;
		leftCurrentAdcIdx = leftCurIdx;
		rightCurrentAdcIdx = rightCurIdx;
		currentSampleCount = 0;
		 
		float tempKP = 0.5, tempKI = 0.03, tempKD = 0;
		float tempUpper = 3000, tempLower = -3000;
//		float tempUpper = PWM_EXTREME_VALUE, tempLower = -PWM_EXTREME_VALUE;
		
		Config_PID(&leftCurrentPidInfo, tempKP, tempKI, tempKD, tempUpper, tempLower);
		Config_PID(&rightCurrentPidInfo, tempKP, tempKI, tempKD, tempUpper, tempLower);
		
		currentDeviceStatus = MotorStopping;
		
		Timer_Register(TIMER_3,keepCurrentStable);		
	}
	
	//电流环
	void keepCurrentStable(void)
	{
		if(currentDeviceStatus == MotorStopping)
		{
			if(zeroDriftCount < ZERO_DRIFT_CALC_COUNT)
			{
				RefreshCurrentZeroDriftValue();
				zeroDriftCount += 1;
			}
			else
			{
				zeroDriftCount = 0;
				currentDeviceStatus = MotorWorking;
			}
		}
		if(currentSampleCount < currentSampleFre)		
 		{		
 			currentSampleCount ++;		
 			return;		
 		}		
 		currentSampleCount = 0;	
		
		u16 batteryVol, leftCur, rightCur;
		float leftPWM, rightPWM;
		
		FilterADCValue();
		RefreshVolCurValue();
		GetVolCurValue(&batteryVol,&leftCur,&rightCur);
		
		leftPWM = Get_PID_Output(&leftCurrentPidInfo, targetCurrent - leftCur);
		rightPWM = Get_PID_Output(&rightCurrentPidInfo, targetCurrent - rightCur);
		
		SetPwmValue((int)leftPWM,(int)rightPWM);
	}
	
	void TEST_PidControl(void)
	{
		//速度跳变应用
		appJumpVelocity(10,1000);
		
//		//速度环 PID （电流环类似）
//		appVelocityStable(5,1000,2,3);		
//		//电流环PID
//		CurrentStableInitial();

	}
#endif
//-----------------------End of PID控制 ---------------------------------------------
