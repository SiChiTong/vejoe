#include "labyrinth.h"

labyrinthStatus labCarStatus;

labyrinthStatus GetNextPlan(void)
{
	labyrinthStatus tempLabStatus = labCarStatus;
	
	switch(labCarStatus)
	{
		case labStatusMoving:
			if(Distance <= COLLIDE_MIN_DISTANCE)
			{//运动到了最小距离
				tempLabStatus = labStatusStop;
			}
			break;
			
		case labStatusStop://停止运动，开始获取左右间距
			tempLabStatus = labStatusGetOneDistantce;
		break;
		
		case labStatusGetOneDistantce://获取了一个方向，获取另一个方向
			tempLabStatus = labStatusGetTwoDistance;
		break;
		
		default:break;
	}	
}
