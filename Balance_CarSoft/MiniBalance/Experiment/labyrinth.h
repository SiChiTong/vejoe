#ifndef __LABYRINTH_H
#define __LABYRINTH_H
#include "sys.h"

#define COLLIDE_MIN_DISTANCE 100 //避撞最小距离

//迷宫中小车状态
typedef enum
{
	//遇到障碍物停止
	Stop = 0x00,
	
	//查看了一个方向的距离
	GetOneDistantce,
	
	//查看了另一个方向的距离
	GetTwoDistance,
	
	//移动中
	Moving
	
}labyrinthStatus;

#endif
