#include "APP.h"

//----------------------- OLED ��ʾ---------------------------------------------
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
		//�����ٶ���ʾ
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
		//��ʾ��ˢ��
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
		
		//��ʾ��ˢ��
		OLED_Refresh_Gram();
	}
#endif
//-----------------------End of OLED ��ʾ---------------------------------------------


	
//----------------------- PID���� ---------------------------------------------
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
	u16 onceStepLength, onceSecondsTime, currentTargetVelocity, currentTimeCount;
	int currentChangeDirect, targetVelocity;
	u8 speedSampleFre, speedSampleCount;
	
	//�ٶȻ���ʼ��
	void velocityStableInitial(int target)
	{
		targetVelocity = target;
		float tempKP = 0.008, tempKI = 0.0005, tempKD = 0;
		float tempUpper = 3000, tempLower = -3000;
		
		Config_PID(&leftVelocityPidInfo, tempKP, tempKI,tempKD, tempUpper, tempLower);
		Config_PID(&rightVelocityPidInfo, tempKP, tempKI,tempKD, tempUpper, tempLower);
		
		speedSampleFre = getSpeedSampleFrequency();
		speedSampleCount = 0;
		
		Timer_Register(TIMER_3,keepVelocityStable);		
	}	
	
	//�ٶȻ�
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
		
		leftSpeed = getHallChangeSpeed(First);
		rightSpeed = getHallChangeSpeed(Second);
		
		leftPWM = Get_PID_Output(&leftVelocityPidInfo, targetVelocity - leftSpeed);
		rightPWM = Get_PID_Output(&rightVelocityPidInfo, -1*targetVelocity - rightSpeed);//���ֱ�������������Ŀ��ֵȡ��
		
		SetPwmValue((int)leftPWM,(int)rightPWM);
	}
	
	//�������Сֵ֮�䣬ÿ����ı�һ����λ���ٶ�
	void jumpVelocityTimer(void)
	{
		currentTimeCount ++ ; 
		if(currentTimeCount < onceSecondsTime) return;
		
		currentTimeCount = 0;
		if(currentTargetVelocity + onceStepLength >= PWM_EXTREME_VALUE)
			currentChangeDirect = -1;
		else if(currentTargetVelocity - onceStepLength < onceStepLength)
			currentChangeDirect = 1;
		currentTargetVelocity += (currentChangeDirect * onceStepLength);

		SetPwmValue(currentTargetVelocity,currentTargetVelocity);
	}

	void appJumpVelocity(u16 seconds, u16 stepLength)
	{
		onceStepLength = stepLength;
		onceSecondsTime = seconds * 1000 / 5;//ʱ��Ϊ5ms
		currentTargetVelocity = onceStepLength;
		currentTimeCount = 0;
		currentChangeDirect = 1;
		
		if(leftVelocityPidInfo._Kp  == 0 || rightVelocityPidInfo._Kp == 0)
		{
			velocityStableInitial(currentTargetVelocity);
		}
		
		Timer_Register(TIMER_3,jumpVelocityTimer);
	}
		
	//��������ʼ��
	void CurrentStableInitial(void)
	{
		float leftKP = 5, leftKI = 0.1, leftKD = 0.5;
		float rightKP = 5, rightKI = 0.1, rightKD = 0.5;
		float tempUpper = PWM_EXTREME_VALUE, tempLower = -PWM_EXTREME_VALUE;
		
		Config_PID(&leftCurrentPidInfo, leftKP, leftKI,leftKD, tempUpper, tempLower);
		Config_PID(&rightCurrentPidInfo, rightKP, rightKI,rightKD, tempUpper, tempLower);
	}
	
	//������
	void keepCurrentStable(int targetCurrent)
	{
		u16 batteryVol, leftCur, rightCur;
		float leftPWM, rightPWM;
		
		FilterADCValue();
		UpdateVolCurValue(1,2,3);
		GetVolCurValue(&batteryVol,&leftCur,&rightCur);
		
		leftPWM = Get_PID_Output(&leftCurrentPidInfo, targetCurrent - leftCur);
		rightPWM = Get_PID_Output(&rightCurrentPidInfo, targetCurrent - rightCur);//���ֱ�������������Ŀ��ֵȡ��
		
		SetPwmValue((int)leftPWM,(int)rightPWM);
	}
	
	void TEST_PidControl(void)
	{
		//�ٶ�����Ӧ��
		appJumpVelocity(15,1500);
		
		//�ٶȻ� PID ��ʼ�������������ƣ�
		velocityStableInitial(1000);		
//		//��������ʼ��
//		CurrentStableInitial();

	}
#endif
//-----------------------End of PID���� ---------------------------------------------
