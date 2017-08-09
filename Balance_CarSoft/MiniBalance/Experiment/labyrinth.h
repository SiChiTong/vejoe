#ifndef __LABYRINTH_H
#define __LABYRINTH_H

#define COLLIDE_MIN_DISTANCE 200 			//避撞最小距离mm
#define PULSE_COUNT_ONE_ROUND  (11 * 4)	//一圈脉冲数
#define RATIO_WHEEL_SPEED  30 					//减速比
#define DIAMETER_WHEEL 66								//轮子直径mm
#define AXIS_DISTANCE_WHEEL 180					//小车，两个轮子间距mm

//迷宫中小车状态
typedef enum
{
	//未初始化（未开始）
	labStatusNone = 0x00,
	
	//启动迷宫模式
	labStatusStart,
	
	//移动中
	labStatusMovingForward,
	
	//遇到障碍物，查看一个方向的距离
	labStatusCheckingOneDirection,	
	
	//查看另一个方向的距离
	labStatusGetOtherDistantce,
	
	//查看了另一个方向的距离，准备调头（第一次的距离更远）
	labStatusTurningRound
	
}labyrinthStatus;

//小车移动方向
typedef enum
{
	moveForward,
	
	moveBackWard,
	
	moveTurnLeft,
	
	moveTurnRight
}MovementDirection;


//初始化变量
void initialLabyrinthConfig(void);
//左转、右转需要脉冲数
void calcPulseForTurnSemiCircle(void);
//调头需要脉冲数
void calcPulseForTurnRound(void);
//达到最小距离
int CheckMovingForwardIsEnd(void);
//达到转动角度
int CheckTurningIsEnd(void);
//算法主函数
void labyrinthAlgorithmMain(void);

#endif
