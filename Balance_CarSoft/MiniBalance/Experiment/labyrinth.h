#ifndef __LABYRINTH_H
#define __LABYRINTH_H
#include "sys.h"

#define COLLIDE_MIN_DISTANCE 100 //��ײ��С����

//�Թ���С��״̬
typedef enum
{
	//δ��ʼ��
	labStatusNone = 0x00,
	
	//�����ϰ���ֹͣ
	labStatusStop,
	
	//�ƶ���
	labStatusMoving,
	
	//�鿴��һ������ľ���
	labStatusGetOneDistantce,
	
	//�鿴����һ������ľ���
	labStatusGetTwoDistance
	
}labyrinthStatus;


//��ȡ��һ����������
labyrinthStatus GetNextPlan(void);

#endif
