#include "labyrinth.h"
#include "sys.h"

#define PI 3.14159265

static float ratioPulse2Distance = 1.0f, ratioDistance2Angle = 1.0f;
u8  carIsMoving=1;  //开始时要默认为运动状态
//u16 tempLeft,lastLeft,tempLeftDiff,tempRight,lastRight, tempRightDiff;
u16 oneDirectDistance,otherDirectDistance;
u32 distanceLatestQueue[3] = {COLLIDE_MIN_DISTANCE,COLLIDE_MIN_DISTANCE,COLLIDE_MIN_DISTANCE};


labyrinthStatus labCarStatus;

void initialLabyrinthConfig(void)
{
	//启动迷宫模式
	labCarStatus = labStatusStart;//labStatusNone：关闭
	
	ratioPulse2Distance = (1.0f *PULSE_COUNT_ONE_ROUND * RATIO_WHEEL_SPEED) / (PI * DIAMETER_WHEEL);	
	ratioDistance2Angle = (AXIS_DISTANCE_WHEEL * PI) / 360.0f;
	
	if(labCarStatus == labStatusStart)
	{
		labCarStatus  = labStatusMovingForward;
	}
}

int pulseCount2Distance(int npulse)
{
//	return (npulse * PI * DIAMETER_WHEEL) / (PULSE_COUNT_ONE_ROUND * RATIO_WHEEL_SPEED);
	return npulse / ratioPulse2Distance;
}

int distance2PulseCount(int distance)
{
//	return (distance * PULSE_COUNT_ONE_ROUND * RATIO_WHEEL_SPEED) / (PI * DIAMETER_WHEEL);
	return distance * ratioPulse2Distance;
}

int GetPulseAngle(int angle)
{	
	//int distance = AXIS_DISTANCE_WHEEL * PI * (angle / 360.0);
	int distance = ratioDistance2Angle * angle;
	return distance2PulseCount(distance);
}

void calcPulseForTurnSemiCircle(void)
{
	targetPulseForTurn = GetPulseAngle(90);
}

void calcPulseForTurnRound(void)
{
	targetPulseForTurn = GetPulseAngle(180);
}

void clearRememberParams(void)
{
	lastLeft = 0;
	lastRight = 0;
	hallDiffCount = 0;
}

void carMoveTurnLeft(void)
{	
	Flag_Qian=0;
	Flag_Hou=0;
	Flag_Left=1;
	Flag_Right=0;
	Flag_sudu=10;
	carIsMoving = 1;
}

void carMoveForward(void)
{	
	Flag_Qian=0;
	Flag_Hou=1;
	Flag_Left=0;
	Flag_Right=0;
	Flag_sudu=10;
	carIsMoving = 1;
}

void carStopping(void)
{	
	Flag_Qian=0;
	Flag_Hou=0;
	Flag_Left=0;
	Flag_Right=0;
	carIsMoving = 0;
}

void CheckMovingForwardIsEnd(void)
{
	Read_Distane();
	distanceLatestQueue[0] = distanceLatestQueue[1];
	distanceLatestQueue[1] = distanceLatestQueue[2];
	distanceLatestQueue[2] = Distance;

  carIsMoving = !(Distance < COLLIDE_MIN_DISTANCE &&
					distanceLatestQueue[1] < COLLIDE_MIN_DISTANCE &&
					distanceLatestQueue[0] < COLLIDE_MIN_DISTANCE);	
	
	if(!carIsMoving)
	{
		delayMs_NOWait(CAR_STOP_WAITTING_TIME_MS);
		carStopping();
	}
}

void CheckTurningIsEnd(void)
{
	tempLeft=Read_Encoder(2);
	tempRight=Read_Encoder(4);
	
	if(lastLeft ==0 && lastRight == 0)
	{		
		lastLeft = tempLeft;
		lastRight = tempRight;
		return;
	}
	
	tempLeftDiff = abs(tempLeft - lastLeft);
	tempRightDiff = abs(tempRight - lastRight);	
	if(tempLeftDiff > ENCODER_TIM_PERIOD_HALF)
	{//发生溢出，值的变化方向发生了反转
		hallLeftDiff = ((tempLeft>lastLeft?lastLeft:tempLeft) + (ENCODER_TIM_PERIOD - (tempLeft>lastLeft?tempLeft:lastLeft)));
	}
	else
	{//常规变化
		hallLeftDiff = tempLeftDiff;
	}
	
	if(tempRightDiff > ENCODER_TIM_PERIOD_HALF)
	{
		hallRightDiff = ((tempRight>lastRight?lastRight:tempRight) + (ENCODER_TIM_PERIOD - (tempRight>lastRight?tempRight:lastRight)));
	}
	else
	{
		hallRightDiff = tempRightDiff;
	}
	
	hallDiffCount += (hallLeftDiff + hallRightDiff);
	lastLeft = tempLeft;
	lastRight = tempRight;
	
	carIsMoving = !(hallDiffCount >= targetPulseForTurn * 2);
	if(!carIsMoving)
	{
		delayMs_NOWait(CAR_STOP_WAITTING_TIME_MS);
		carStopping();
	}
}

void labyrinthAlgorithmMain(void)
{
	if(!labyrinthCanRunning) return;
	
	switch(labCarStatus)
	{
		case labStatusMovingForward:
		{			
			if(carIsMoving)				
			{
				carMoveForward();
				CheckMovingForwardIsEnd();
			}
			else
			{				
				clearRememberParams();
				carMoveTurnLeft();
				labCarStatus = labStatusCheckingOneDirection;
				calcPulseForTurnSemiCircle();
			}
		}
		break;
		case labStatusCheckingOneDirection:
		{
			if(carIsMoving)
			{
				CheckTurningIsEnd();
			}
			else
			{
				clearRememberParams();
				Read_Distane();
				oneDirectDistance = Distance;				
				carMoveTurnLeft();
				labCarStatus = labStatusGetOtherDistantce;
				calcPulseForTurnRound();
			}
		}
		break;
		case labStatusGetOtherDistantce:
		{
			if(carIsMoving)
			{
				CheckTurningIsEnd();
			}
			else
			{
				clearRememberParams();
				Read_Distane();
				otherDirectDistance = Distance;
				if(oneDirectDistance > otherDirectDistance + 0xFF)//两边距离相差0xff毫米,作为误差过滤
				{//首次的转向距离较长，则调头
					carMoveTurnLeft();
					labCarStatus = labStatusTurningRound;
					calcPulseForTurnRound();
				}
				else
				{//当前方向即为目标方向，则继续前进
					carMoveForward();
					labCarStatus = labStatusMovingForward;
				}
			}
		}
		break;
		case labStatusTurningRound:
		{
			if(carIsMoving)
			{
				CheckTurningIsEnd();
			}
			else
			{
				clearRememberParams();
				carMoveForward();
				labCarStatus = labStatusMovingForward;
			}
		}
		break;
		default:break;
	}
}
