#include "MotionCalc.h"
#include <math.h>
#include <iostream>

using namespace std;
#define M_PI       3.14159265358979323846
#define MOVE_MAX_SPEED 0.05 //3度
#define MOVE_NORMAL_SPEED 0.02 //1度
#define	SPEED_THRESHOLD_ANGLE 0.7 //40度
#define VISION_SECTION_COUNT	5	//视角 分区域 总数
#define ONE_SIDE_SECTION_BOUND	0.1	//边界缓冲区域 占比

//当前，目标角度（实时）
double MotionCalc::currentAngle, MotionCalc::targetAngle;
//分区域相关 角度
double MotionCalc::sectionAngle, MotionCalc::sectionBoundAngle;

MotionCalc::MotionCalc(int imageWidth):MAX_VISION_ANGLE(60)
{
	videoImageWidth = imageWidth;
	verticalDistance = (imageWidth / 2.0) / tan(MAX_VISION_ANGLE / 2 * M_PI / 180);
	sectionAngle = MAX_VISION_ANGLE / VISION_SECTION_COUNT * M_PI / 180;
	sectionBoundAngle = sectionAngle * ONE_SIDE_SECTION_BOUND;
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

double  MotionCalc::CalcAngleNextStepBySection()
{
	int currentSection = currentAngle / sectionAngle, targetSection = targetAngle / sectionAngle;
	int sectionDiff = targetSection - currentSection;
	double overLen = currentAngle - sectionAngle * currentSection;
	double tmpValue = targetAngle - max(currentSection, targetSection) * sectionAngle;

	if(abs(sectionDiff) > 1 || abs(sectionDiff) == 1 && abs(tmpValue) > sectionBoundAngle)
	{//相隔至少一个区域，或者在相邻区域，并且目标不在边界缓冲区
		currentAngle += MOVE_NORMAL_SPEED * (sectionDiff>0?1:-1);
	}
	else if(abs(sectionDiff) == 0)
	{//当前位置在目标区域了
		tmpValue = overLen - sectionAngle / 2.0;
		if(abs(tmpValue) > MOVE_NORMAL_SPEED)
		{//未达到中心位置
			currentAngle += MOVE_NORMAL_SPEED * (tmpValue>0?-1:1);
		}
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

	