#include "Config.h"

//----------------------- OLED ��ʾ---------------------------------------------
#ifdef APP_OLED_SHOW_INFORMATION
	void showSpeedValue(int speedLeft,int speedRight);
	void ShowVolCurValue(unsigned short int batteryVoltage,unsigned short int leftCurrent,unsigned short int rightCurrent);
#endif
//-----------------------End of OLED ��ʾ---------------------------------------------

//----------------------- PID���� ---------------------------------------------
#ifdef APP_PID_CONTROL

typedef enum
{
	//�����ת
	MotorStopping,
	
	//�������
	MotorWorking,
	
	//������ٽ׶�
	MotorAccelerate,
	
	//��������˶�
	MotorKeepSpeed,
	
	//������ٽ׶�
	MotorDecelerate
}enumMotorWorkingStatus;

	//�ٶȻ�����������ͬʱά��һ���̶����ٶȲ���
	//����Ƶ�ʲ��õ���5ms���ٶȵļ���Ƶ��Ϊ5ms������5ms�ᵼ�½����׼��
	void appVelocityStable(u8 sampleFrequence,int target);
	void keepVelocityStable(void);
	//���������������ӵ���ĵ������ֲ���
	void appCurrentStable(u8 sampleFrequence, u8 target);
	void keepCurrentStable(void);

	//�ٶ������ʱ��
	void jumpVelocityTimer(void);	
	//�ٶ����䣺�������Сֵ֮�䣬ÿ����ı�һ����λ���ٶ�
	void appJumpVelocity(u16 seconds, u16 stepLength);
#endif
//-----------------------End of PID���� ---------------------------------------------
