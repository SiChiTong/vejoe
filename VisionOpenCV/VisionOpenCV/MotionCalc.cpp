#include "MotionCalc.h"
#include <math.h>
#include <iostream>

using namespace std;
#define M_PI       3.14159265358979323846
#define MOVE_MAX_SPEED 0.05 //2��
#define MOVE_NORMAL_SPEED 0.02 //1��
#define SPEED_THRESHOLD_ANGLE 0.7 //40��

//��ǰ��Ŀ��Ƕȣ�ʵʱ��
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
	//����������߾���
	double horiLen = xValue - videoImageWidth / 2.0;
	//ͨ�������м��㻡��
	double angle = atan2(fabs(horiLen),verticalDistance);
	//�Ƕȵķ�������ͷ�ԳƳ�������������������
	angle *= horiLen>0?-1:1;
	//ת��Ϊ�Ƕ�
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

	