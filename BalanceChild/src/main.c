#include "Sensor/LED.h"
#include "Sensor/Clock.h"
#include "Sensor/Switch.h"
#include "Sensor/Hall.h"
#include "Sensor/exti.h"
#include "delay.h"
#include "motor.h"

	int Encoder_Left=0,Encoder_Right=0;
	int speedLeft,speedRight;
	int getVelocity(int,int);
	void TurnAngle(int,BOOL);
	void setVelocity(int);
	
	int targetPulse = 0;
	
int main(void)
{
	//测试定时器：LED亮灭
//	ClockTest();
	
	//测试LED：循环亮灭
//	LEDTest();
	
//	ClockGradualTest();
//	while(TRUE);
	
//	InitialSwitch();
//	while(TRUE)
//	{
//		SwitchTest();
//	}
	delay_init();
	MiniBalance_PWM_Init(7199,0);   //=====初始化PWM 10KHZ，用于驱动电机 如需初始化电调接口 
	
	Encoder_Init_TIM2();
	Encoder_Init_TIM4();
	u16 tempLeft,tempRight,lastLeft,lastRight;
	int timeLen=5;
	
//	AIN2=1;
//	AIN1=0;
//	PWMA=5000;
//	BIN1=1;
//	BIN2=0;
//	PWMB=2500;
//	while(TRUE)
//	{
//		tempLeft=Read_Encoder(2);
//		speedLeft = getVelocity(tempLeft - Encoder_Left,timeLen);
//		Encoder_Left = tempLeft;		

//		tempRight=Read_Encoder(4);
//		speedRight = getVelocity(tempRight - Encoder_Right,timeLen);
//		Encoder_Right = tempRight;
//	}
	
	TurnAngle(180,TRUE);
	while(TRUE)
	{
		tempLeft=Read_Encoder(2);
		tempRight=Read_Encoder(4);
		if(tempLeft < lastLeft)
			lastLeft -= ENCODER_TIM_PERIOD;
		if(tempRight < lastRight)
			lastRight -= ENCODER_TIM_PERIOD;
		
		Encoder_Left += tempLeft - lastLeft;
		Encoder_Right += tempRight - lastRight;
		
		if(Encoder_Left + Encoder_Right >= targetPulse * 2)
			setVelocity(0);
		else
			setVelocity(1000);
		
		lastLeft = tempLeft;
		lastRight = tempRight;
	}
}

int countOneRound = 11 * 4;	//一圈脉冲数
int ratioWheelSpeed = 30; 	//减速比
int diameterWheel = 66;			//轮子直径mm
int unitSpeedTime = 100;		//速度的时间单位ms
int axisDistance =180;			//小车，两个轮子间距mm

int getVelocity(int countPulse,int timeMs)
{
	if(countPulse < 0)
		countPulse += ENCODER_TIM_PERIOD;
	int speedSecond = (countPulse * PI * diameterWheel * unitSpeedTime)/ (timeMs * countOneRound * ratioWheelSpeed) ;
	return speedSecond;
}

void setVelocity(int speed)
{	
	PWMA=speed;
	PWMB=speed;
}

int pulseCount2Distance(int npulse)
{
	return (npulse * PI * diameterWheel) / (countOneRound * ratioWheelSpeed);
}

int distance2PulseCount(int distance)
{
	return (distance * countOneRound * ratioWheelSpeed) / (PI * diameterWheel);
}

void moveLength(int distanceMM)
{
	int distance = distanceMM * 0.8;
	
	
}

void TurnAngle(int angle,BOOL isLeft)
{	
	AIN1=0;
	AIN2=1;
	BIN1=0;
	BIN2=1;
	
	if(isLeft)
	{
		AIN1=1;
		AIN2=0;
		BIN1=1;
		BIN2=0;		
	}
	
	int distance = axisDistance * PI * (angle / 360.0);
	targetPulse = distance2PulseCount(distance);
}
