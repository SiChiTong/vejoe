#include "labyrinth.h"

labyrinthStatus labCarStatus;

labyrinthStatus GetNextPlan(void)
{
	labyrinthStatus tempLabStatus = labCarStatus;
	
	switch(labCarStatus)
	{
		case labStatusMoving:
			if(Distance <= COLLIDE_MIN_DISTANCE)
			{//�˶�������С����
				tempLabStatus = labStatusStop;
			}
			break;
			
		case labStatusStop://ֹͣ�˶�����ʼ��ȡ���Ҽ��
			tempLabStatus = labStatusGetOneDistantce;
		break;
		
		case labStatusGetOneDistantce://��ȡ��һ�����򣬻�ȡ��һ������
			tempLabStatus = labStatusGetTwoDistance;
		break;
		
		default:break;
	}	
}
