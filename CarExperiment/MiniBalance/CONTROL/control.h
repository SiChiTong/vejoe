#ifndef __CONTROL_H
#define __CONTROL_H

	#include "sys.h"

	#define PI 3.14159265
	#define DIFFERENCE 100
	
	struct Control_Info
	{
		u8 _FSM;								//0:idle(I)状态，1:balance(B)状态， 2:motion(M)状态
		u8 _FSM_Trans_Flag;			//0：no trans, 1:I->B, 2:B->I, 3:B->M, 4:M->B, 5:M->I,	
		u8 _iFSM;								//0:刚进入时，禁止电机；1：等待放下
		u8 _bFSM;								//0:普通平衡状态；1：有前后偏移或者角度偏移状态；2：被拿起状态
		
		int _leftPWM;
		int _rightPWM;
		int _leftBalancePWM;
		int _rightBalancePWM;
		int _leftMovePWM;
		int _rightMovePWM;
		int _leftTurnPWM;
		int _rightTurnPWM;
		
		u8 _motion_Flag;
	};
	
	extern	int Balance_Pwm,Velocity_Pwm,Turn_Pwm;
	
	void _get_Angle(void);
	void _get_Encoder(void);
	void _get_Voltage(void);
	void _get_Temperature(void);
	
	int EXTI2_IRQHandler(void);
	int Balance(float angle,float gyro);
	int velocity(int encoder_left,int encoder_right);
	int turn(int encoder_left,int encoder_right,float gyro);
	void Set_Pwm(int moto1,int moto2);
	void Key(void);
	void Xianfu_Pwm(void);
	u8 Turn_Off(float angle, int voltage);
	void Get_Angle(u8 way);
	void updateEcoderDelta(void);
	int myabs(int a);
	int Pick_Up(float Acceleration, float Angle, int encoder_left, int encoder_right);
	int Put_Down(float Angle, int encoder_left, int encoder_right);
	void Get_MC6(void);
	
#endif
