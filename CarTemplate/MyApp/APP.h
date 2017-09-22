#include "Config.h"

//----------------------- OLED ��ʾ---------------------------------------------
#ifdef APP_OLED_SHOW_INFORMATION
	void showSpeedValue(int speedLeft,int speedRight);
	void ShowVolCurValue(unsigned short int batteryVoltage,unsigned short int leftCurrent,unsigned short int rightCurrent);
#endif
//-----------------------End of OLED ��ʾ---------------------------------------------

//----------------------- PID���� ---------------------------------------------
#ifdef APP_PID_CONTROL
	//�ٶȻ�����������ͬʱά��һ���̶����ٶȲ���
	void velocityStableInitial(int target);
	void keepVelocityStable(void);
	//���������������ӵ���ĵ������ֲ���
	void CurrentStableInitial(void);
	void keepCurrentStable(int targetCurrent);

	//�ٶ������ʱ��
	void jumpVelocityTimer(void);	
	//�ٶ����䣺�������Сֵ֮�䣬ÿ����ı�һ����λ���ٶ�
	void appJumpVelocity(u16 seconds, u16 stepLength);
#endif
//-----------------------End of PID���� ---------------------------------------------
