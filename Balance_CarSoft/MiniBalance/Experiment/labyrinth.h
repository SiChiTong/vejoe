#ifndef __LABYRINTH_H
#define __LABYRINTH_H
#include "sys.h"

#define COLLIDE_MIN_DISTANCE 100 //��ײ��С����

//�Թ���С��״̬
typedef enum
{
	//�����ϰ���ֹͣ
	Stop = 0x00,
	
	//�鿴��һ������ľ���
	GetOneDistantce,
	
	//�鿴����һ������ľ���
	GetTwoDistance,
	
	//�ƶ���
	Moving
	
}labyrinthStatus;

#endif
