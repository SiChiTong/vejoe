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
	//计算相对中线距离
	double horiLen = xValue - videoImageWidth / 2.0;
	//通过反正切计算弧度
	double angle = atan2(fabs(horiLen),verticalDistance);
	//角度的方向：摄像头对称成像，所以正负反过来了
	angle *= horiLen>0?-1:1;
	//转换为角度
	return angle / M_PI * 180;
}

void MotionCalc::MoveOrigin()
{

}