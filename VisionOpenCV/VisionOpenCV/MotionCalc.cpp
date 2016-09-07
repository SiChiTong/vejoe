#include "MotionCalc.h"
#include <math.h>

using namespace std;
#define M_PI       3.14159265358979323846

MotionCalc::MotionCalc(int imageWidth):MAX_VISION_ANGLE(120)
{
	videoImageWidth = imageWidth;
	verticalDistance = (imageWidth/2.0) / tan(MAX_VISION_ANGLE/2*M_PI/180);
	moveSpeed = 5;
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
	return angle / M_PI * 180;
}

void MotionCalc::MoveOrigin()
{

}