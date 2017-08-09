#include "labyrinth.h"
#include "sys.h"

#define PI 3.14159265

static float ratioPulse2Distance = 1.0f, ratioDistance2Angle = 1.0f;
u16 tempLeft,tempRight,lastLeft,lastRight,tempLeftDiff, tempRightDiff;
u16 oneDirectDistance,otherDirectDistance;
int targetPulseForTurn;
int hallLeftCount = 0,hallRightCount = 0;

labyrinthStatus labCarStatus;

void initialLabyrinthConfig(void)
{
	//启动迷宫模式
	labCarStatus = labStatusStart;//labStatusNone:停止
	
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

int CheckMovingForwardIsEnd(void)
{
	Read_Distane();
	
	return Distance>0 && Distance <= COLLIDE_MIN_DISTANCE;
}

void clearRememberParams(void)
{
	lastLeft = 0;
	lastRight = 0;
	hallLeftCount = 0;
	hallRightCount = 0;
}

void carMoveTurnLeft(void)
{	
	Flag_Qian=0;
	Flag_Hou=0;
	Flag_Left=1;
	Flag_Right=0;
}

void carMoveForward(void)
{	
	Flag_Qian=0;
	Flag_Hou=1;
	Flag_Left=0;
	Flag_Right=0;
}


int CheckTurningIsEnd(void)
{
	tempLeft=Read_Encoder(2);
	tempRight=Read_Encoder(4);
	
	tempLeftDiff = abs(tempLeft - lastLeft);
	tempRightDiff = abs(tempRight - lastRight);	
	if(tempLeftDiff > ENCODER_TIM_PERIOD)
	{//发生溢出，值的变化方向发生了反转
		hallLeftCount += (tempLeft - (ENCODER_TIM_PERIOD - lastLeft));
	}
	else
	{//常规变化
		hallLeftCount += abs(tempLeft - lastLeft);
	}
	
	if(tempRightDiff > ENCODER_TIM_PERIOD)
	{
		hallRightCount += (tempRight - (ENCODER_TIM_PERIOD - lastRight));
	}
	else
	{
		hallRightCount += tempRightDiff;
	}
	
	lastLeft = tempLeft;
	lastRight = tempRight;
	return hallLeftCount + hallRightCount >= targetPulseForTurn * 2;
}

void labyrinthAlgorithmMain(void)
{
	switch(labCarStatus)
	{
		case labStatusMovingForward:
		{
			carMoveForward();
			
			if(CheckMovingForwardIsEnd())
			{
				carMoveTurnLeft();
				labCarStatus = labStatusCheckingOneDirection;
				calcPulseForTurnSemiCircle();
			}
		}
		break;
		case labStatusCheckingOneDirection:
		{
			if(CheckTurningIsEnd())
			{
				clearRememberParams();
				Read_Distane();
				oneDirectDistance = Distance;
				labCarStatus = labStatusGetOtherDistantce;
				calcPulseForTurnRound();
			}
		}
		break;
		case labStatusGetOtherDistantce:
		{
			if(CheckTurningIsEnd())
			{
				clearRememberParams();
				Read_Distane();
				otherDirectDistance = Distance;
				if(oneDirectDistance > otherDirectDistance + 0xFF)
				{//距离相差0xff毫米,作为误差过滤
					calcPulseForTurnRound();
					labCarStatus = labStatusTurningRound;
				}
				else
				{//当前方向即为目标方向
					carMoveForward();
					labCarStatus = labStatusMovingForward;
				}
			}
		}
		break;
		case labStatusTurningRound:
		{			
			if(CheckTurningIsEnd())
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
