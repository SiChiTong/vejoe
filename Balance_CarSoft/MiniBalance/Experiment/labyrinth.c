#include "labyrinth.h"
#include "sys.h"

#define PI 3.14159265

static float ratioPulse2Distance = 1.0f, ratioDistance2Angle = 1.0f;
u16 tempLeft,tempRight,lastLeft,lastRight;
int targetPulseForTurn;

labyrinthStatus labCarStatus;

void initialLabyrinthConfig(void)
{
	//启动迷宫模式
	labCarStatus = labStatusStart;//labStatusNone：关闭
	
	ratioPulse2Distance = (1.0f *PULSE_COUNT_ONE_ROUND * RATIO_WHEEL_SPEED) / (PI * DIAMETER_WHEEL);	
	ratioDistance2Angle = (AXIS_DISTANCE_WHEEL * PI) / 360.0f;
	labCarStatus = labStatusNone;
	
	if(labCarStatus == labStatusStart)
	{
		carMoveDirect = moveForward;
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
	
	return Distance <= COLLIDE_MIN_DISTANCE;
}


int CheckTurningIsEnd(void)
{
	tempLeft=Read_Encoder(2);
		tempRight=Read_Encoder(4);
		if(tempLeft < lastLeft)
			lastLeft -= ENCODER_TIM_PERIOD;
		if(tempRight < lastRight)
			lastRight -= ENCODER_TIM_PERIOD;
		
		Encoder_Left += tempLeft - lastLeft;
		Encoder_Right += tempRight - lastRight;
		
		return Encoder_Left + Encoder_Right >= targetPulseForTurn * 2;
}
