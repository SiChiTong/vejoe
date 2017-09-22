#include "Config.h"

//----------------------- OLED ��ʾ---------------------------------------------
#ifdef APP_OLED_SHOW_INFORMATION
	void showSpeedValue(int speedLeft,int speedRight);
	void ShowVolCurValue(unsigned short int batteryVoltage,unsigned short int leftCurrent,unsigned short int rightCurrent);
#endif
//-----------------------End of OLED ��ʾ---------------------------------------------

//----------------------- PID���� ---------------------------------------------
#ifdef APP_PID_CONTROL
	//�ٶȻ���ʼ��
	void velocityStableInitial(void);
	//�ٶȻ�����������ͬʱά��һ���̶����ٶȲ���
	void keepVelocityStable(int targetVelocity);

	//�ٶ������ʱ��
	void jumpVelocityTimer(void);	
	//�ٶ������ʼ��
	void jumpVelocityInitial(u16 seconds, u16 stepLength);
	//�ٶ����䣺�������Сֵ֮�䣬ÿ����ı�һ����λ���ٶ�
	void JumpVelocityEachSeconds(void);
#endif
//-----------------------End of PID���� ---------------------------------------------
