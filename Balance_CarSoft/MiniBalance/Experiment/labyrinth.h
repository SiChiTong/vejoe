#ifndef __LABYRINTH_H
#define __LABYRINTH_H

#define COLLIDE_MIN_DISTANCE 200 			//��ײ��С����mm
#define PULSE_COUNT_ONE_ROUND  (11 * 4)	//һȦ������
#define RATIO_WHEEL_SPEED  30 					//���ٱ�
#define DIAMETER_WHEEL 66								//����ֱ��mm
#define AXIS_DISTANCE_WHEEL 180					//С�����������Ӽ��mm

//�Թ���С��״̬
typedef enum
{
	//δ��ʼ����δ��ʼ��
	labStatusNone = 0x00,
	
	//�����Թ�ģʽ
	labStatusStart,
	
	//�ƶ���
	labStatusMovingForward,
	
	//�����ϰ���鿴һ������ľ���
	labStatusCheckingOneDirection,	
	
	//�鿴��һ������ľ���
	labStatusGetOtherDistantce,
	
	//�鿴����һ������ľ��룬׼����ͷ����һ�εľ����Զ��
	labStatusTurningRound
	
}labyrinthStatus;

//С���ƶ�����
typedef enum
{
	moveForward,
	
	moveBackWard,
	
	moveTurnLeft,
	
	moveTurnRight
}MovementDirection;


//��ʼ������
void initialLabyrinthConfig(void);
//��ת����ת��Ҫ������
void calcPulseForTurnSemiCircle(void);
//��ͷ��Ҫ������
void calcPulseForTurnRound(void);
//�ﵽ��С����
int CheckMovingForwardIsEnd(void);
//�ﵽת���Ƕ�
int CheckTurningIsEnd(void);
//�㷨������
void labyrinthAlgorithmMain(void);

#endif
