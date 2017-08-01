#ifndef __LABYRINTH_H
#define __LABYRINTH_H
#include "sys.h"

#define COLLIDE_MIN_DISTANCE 100 //避撞最小距离

//迷宫中小车状态
typedef enum
{
	//未初始化
	labStatusNone = 0x00,
	
	//遇到障碍物停止
	labStatusStop,
	
	//移动中
	labStatusMoving,
	
	//查看了一个方向的距离
	labStatusGetOneDistantce,
	
	//查看了另一个方向的距离
	labStatusGetTwoDistance
	
}labyrinthStatus;


//获取下一步操作命令
labyrinthStatus GetNextPlan(void);

#endif
