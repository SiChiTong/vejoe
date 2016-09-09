#include "MotionCalc.h"
#include <math.h>
#include <iostream>

using namespace std;
#define M_PI       3.14159265358979323846
#define MOVE_MAX_SPEED 0.05 //2度
#define MOVE_NORMAL_SPEED 0.02 //1度
#define SPEED_THRESHOLD_ANGLE 0.7 //40度

//当前，目标角度（实时）
double MotionCalc::currentAngle, MotionCalc::targetAngle;

MotionCalc::MotionCalc(int imageWidth):MAX_VISION_ANGLE(60)
{
	videoImageWidth = imageWidth;
	verticalDistance = (imageWidth/2.0) / tan(MAX_VISION_ANGLE/2*M_PI/180);
}


MotionCalc::~MotionCalc(void)
{
}


double MotionCalc::CalcAngleByLocation(int xValue)
{
	//计算相对中线距离
	double horiLen = xValue - videoImageWidth / 2.0;
	//通过反正切计算弧度
	double angle = atan2(fabs(horiLen),verticalDistance);
	//角度的方向：摄像头对称成像，所以正负反过来了
	angle *= horiLen>0?-1:1;
	//转换为角度
	return angle;
}

double  MotionCalc::CalcAngleNextStep()
{
	double tmpDiff = fabs(targetAngle - currentAngle);
	int tmpFlag = targetAngle > currentAngle ? 1 : -1;
	if(tmpDiff > MOVE_NORMAL_SPEED)
	{
		if(tmpDiff < SPEED_THRESHOLD_ANGLE)
			currentAngle += MOVE_NORMAL_SPEED * tmpFlag;
		else
			currentAngle += MOVE_MAX_SPEED * tmpFlag;
	}
	return currentAngle;
}

void MotionCalc::MoveOrigin()
{
	currentAngle = 0;
}

void MotionCalc::setAngleTarget(double degree)
{
	targetAngle = degree;	
}

	