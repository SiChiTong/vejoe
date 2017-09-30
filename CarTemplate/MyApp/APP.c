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
	
	#define ZERO_DRIFT_CALC_COUNT	 10000//��Ư�������(msΪ��λ)
	#define MAX_SPPED_FOR_LOCATION	3000//λ�û�������ٶ�
	#define ACCELERATE_EACH_INTERVAL   5//���ٶȣ�5ms�ļ��ٵ�λ
	#define ACCELERATE_TIME_INTERVAL   5//����ʱ������ms��
	#define END_LOCATION_LAST				 500//ʣ��ָ������ʱֹͣ�ٶȻ�����
	#define LOCATION_FINISH_BOUNDRAY 5//λ�û������Χ
	#define END_LOCATION_VELOCITY		 20//ʣ��ָ������ʱ�ٶ�
	
	struct _PID_Info leftVelocityPidInfo, rightVelocityPidInfo;
	struct _PID_Info leftCurrentPidInfo, rightCurrentPidInfo;
	struct _PID_Info leftLocationPidInfo, rightLocationPidInfo;
	u16 onceStepLength, onceSecondsTime, currentTimeCount;
	int currentChangeDirect, targetVelocity;
	u8 speedSampleFre, speedSampleCount;
  u8 currentSampleFre, currentSampleCount,	targetCurrent;
	u8 leftCurrentAdcIdx, rightCurrentAdcIdx;
	enumMotorWorkingStatus currentDeviceStatus,currentLeftStatus, currentRightStatus;
	u16 zeroDriftCount;
	u16 startLocationLeftValue, startLocationRightValue;  //λ�û�������ʱ���������ӵĳ�ʼλ��
	int targetLocationLeftValue, targetLocationRightValue;//λ�û����������ӵ�Ŀ��λ��
	int velocityLocationLeft, velocityLocationRight;			//λ�û����������ӵ��ٶ�
	int velocityLocLeftDirect, velocityLocRightDirect;		//λ�û�����������ת������
	u16 locIntevalCount;	//λ�û�ʱ�Ӽ�ʱ����
	u16 locationMaxLength;//λ�û��Ӽ��������
	
	//�ٶȻ���ʼ��
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
		
		leftSpeed = getHallChangeSpeed(HallEncoderLeftWheel);
		rightSpeed = getHallChangeSpeed(HallEncoderRightWheel);
		
		leftPWM = Get_PID_Output(&leftVelocityPidInfo, targetVelocity - leftSpeed);
		rightPWM = Get_PID_Output(&rightVelocityPidInfo, -1 * targetVelocity - rightSpeed);//���ֱ�������������Ŀ��ֵȡ��
		
		SetPwmValue((int)leftPWM,(int)rightPWM);
	}
	
	//�������Сֵ֮�䣬ÿ����ı�һ����λ���ٶ�
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
		onceSecondsTime = seconds * 1000;//ʱ��Ϊ1ms
		targetVelocity = onceStepLength;
		currentTimeCount = 0;
		currentChangeDirect = 1;
		
		if(leftVelocityPidInfo._Kp  == 0 || rightVelocityPidInfo._Kp == 0)
		{
			appVelocityStable(5,targetVelocity);
		}
		
		Timer_Register(TIMER_3,jumpVelocityTimer);
	}
		
	//��������ʼ��
	void appCurrentStable(u8 sampleFrequence, u8 target)
	{
		targetCurrent = target;
		currentSampleFre = sampleFrequence;
		currentSampleCount = 0;
		 
		float tempKP = 0.5, tempKI = 0.03, tempKD = 0;
		float tempUpper = 3000, tempLower = -3000;
//		float tempUpper = PWM_EXTREME_VALUE, tempLower = -PWM_EXTREME_VALUE;
		
		Config_PID(&leftCurrentPidInfo, tempKP, tempKI, tempKD, tempUpper, tempLower);
		Config_PID(&rightCurrentPidInfo, tempKP, tempKI, tempKD, tempUpper, tempLower);
		
		currentDeviceStatus = MotorStopping;
		
		Timer_Register(TIMER_3,keepCurrentStable);		
	}
	
	//������
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
			SetPwmValue(0,0);
			return;
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
//		if(targetCurrent - leftCur >= 3)
//			leftPWM ++;
//		else if(targetCurrent - leftCur <= -3)
//			leftPWM --;
//		leftPWM = 3000;
//		rightPWM = 0;
		
		SetPwmValue((int)leftPWM,(int)rightPWM);
	}
	
	//λ�û�
	void appLocationStable(int targetLeft,int targetRight)
	{//Ŀ����ٶȵķ���ֻ�������տ��ƣ���˽��ڳ�ʼ�������տ���ʱ�����÷���
	 //ֵ�ļ����״̬���ж϶�������������м��ж϶��þ���ֵ
		startLocationLeftValue = Read_ABS_Value(HallEncoderLeftWheel);
		startLocationRightValue = Read_ABS_Value(HallEncoderRightWheel);
		targetLocationLeftValue = startLocationLeftValue + targetLeft;
		targetLocationRightValue = startLocationRightValue + (-1) * targetRight;//���ֱ�������ֵΪǰ
		velocityLocLeftDirect = targetLeft>0?1:-1;
		velocityLocRightDirect = targetRight>0?-1:1;
		
		locIntevalCount = 0;
		locationMaxLength = (MAX_SPPED_FOR_LOCATION * (MAX_SPPED_FOR_LOCATION/1000/(ACCELERATE_EACH_INTERVAL/ACCELERATE_TIME_INTERVAL))) >> 1;
				
		float tempKP = 0.2, tempKI = 0.01, tempKD = 0;
		float tempUpper = 3000, tempLower = -3000;
		Config_PID(&leftLocationPidInfo, tempKP, tempKI,tempKD, tempUpper, tempLower);		
		Config_PID(&rightLocationPidInfo, tempKP, tempKI,tempKD, tempUpper, tempLower);	
		
		currentLeftStatus = MotorAccelerate;
		currentRightStatus = MotorAccelerate;
		
		Timer_Register(TIMER_3,keepLocationStable);		
	}
	
	int abs(int number)
	{
		if(number < 0)
			return -1*number;
		else return number;
	}
	
	void _keepLocationStableAccelerate(int startValue,int targetValue, int currentValue, int *nextVelocity, enumMotorWorkingStatus *nextStatus)
	{
		if(abs(targetValue - currentValue) <= abs(currentValue - startValue))
		{//ʣ�����ֻ��Ҫ���ټ��ɵ���
			*nextVelocity -= ACCELERATE_EACH_INTERVAL;
			*nextStatus = MotorDecelerate;
		}
		else if(abs(*nextVelocity + ACCELERATE_EACH_INTERVAL) < MAX_SPPED_FOR_LOCATION)
		{//���ٽ׶�
			*nextVelocity += ACCELERATE_EACH_INTERVAL;
		}
		else
		{//���ٵ��˼���
			*nextVelocity = MAX_SPPED_FOR_LOCATION;
			*nextStatus = MotorKeepSpeed;
		}
	}
	
	void _keepLocationStableKeepSpeed(int targetValue, int currentValue,int *nextVelocity, enumMotorWorkingStatus *nextStatus)
	{
		if(abs(targetValue - currentValue) <= locationMaxLength)
		{//�����˶�������ٵ�
			*nextVelocity -= ACCELERATE_EACH_INTERVAL;
			*nextStatus = MotorDecelerate;
		}
	}
	
	void _keepLocationStableDecelerate(int targetValue, int currentValue,int *nextVelocity, enumMotorWorkingStatus *nextStatus)
	{
		if(abs(targetValue - currentValue) <= END_LOCATION_LAST)
		{//��������С����
			*nextStatus = MotorFinishVelocityPID;
		}
		else
		{//���ٽ׶�
			*nextVelocity -= ACCELERATE_EACH_INTERVAL;
		}
	}
	
	void _keepLocationStableFinishVelocity(int targetValue, int currentValue,int *nextVelocity, enumMotorWorkingStatus *nextStatus)
	{
		if(abs(targetValue - currentValue) <= LOCATION_FINISH_BOUNDRAY)
		{//����߽�ֵֹͣ�˶�
			*nextVelocity = 0;
			*nextStatus = MotorStopping;
		}
		else
		{//���׶ε��������˶�
			*nextVelocity = END_LOCATION_VELOCITY;
		}
	}
	
	void _keepLocationStableWorking(int startValue,int targetValue, int currentValue, int *nextVelocity, enumMotorWorkingStatus *nextStatus)
	{
		switch(*nextStatus)
		{
			case MotorAccelerate:
				_keepLocationStableAccelerate(startValue,targetValue,currentValue,nextVelocity,nextStatus);
			break;
			case MotorKeepSpeed:
				_keepLocationStableKeepSpeed(targetValue,currentValue,nextVelocity,nextStatus);
			break;
			case MotorDecelerate:
				_keepLocationStableDecelerate(targetValue,currentValue,nextVelocity,nextStatus);
			break;
			case MotorFinishVelocityPID:
				_keepLocationStableFinishVelocity(targetValue,currentValue,nextVelocity,nextStatus);
			break;
			default:break;
		}
	}
	
	void _keepLocationStableVelocityPID(int currentVelocityLeft,int currentVelocityRight)
	{
		int leftSpeed, rightSpeed;
		float leftPWM, rightPWM;
		if(abs(currentVelocityLeft) > 0)
		{
			leftSpeed = getHallChangeSpeed(HallEncoderLeftWheel);
			leftPWM = Get_PID_Output(&leftLocationPidInfo, velocityLocLeftDirect * currentVelocityLeft - leftSpeed);
		}
		else
		{
			leftPWM = 0;
		}
		
		if(abs(currentVelocityRight) > 0)
		{
			rightSpeed = getHallChangeSpeed(HallEncoderRightWheel);		
			rightPWM = Get_PID_Output(&rightLocationPidInfo,velocityLocRightDirect * currentVelocityRight - rightSpeed);
		}
		else
		{
			rightPWM = 0;
		}
		SetPwmValue((int)leftPWM,(int)rightPWM);
	}
	
	void keepLocationStable(void)
	{
		if(locIntevalCount <= ACCELERATE_TIME_INTERVAL)
		{
			locIntevalCount+=1;
			return;
		}
		locIntevalCount=0;
		
		int tempLeftValue = Read_ABS_Value(HallEncoderLeftWheel),tempRightValue = Read_ABS_Value(HallEncoderRightWheel);
		_keepLocationStableWorking(startLocationLeftValue,targetLocationLeftValue,tempLeftValue,&velocityLocationLeft,&currentLeftStatus);
		_keepLocationStableWorking(startLocationRightValue,targetLocationRightValue,tempRightValue,&velocityLocationRight,&currentRightStatus);
		_keepLocationStableVelocityPID(velocityLocationLeft,velocityLocationRight);
	}
	
	void TEST_PidControl(void)
	{
		//�ٶ�����Ӧ��
		appJumpVelocity(10,1000);
		
//		//�ٶȻ� PID 
//		appVelocityStable(5,1000,2,3);		
//		//������PID
//		CurrentStableInitial();
//		//λ�û� PID
//		appLocationStable(-20000,15000);	

	}
#endif
//-----------------------End of PID���� ---------------------------------------------
