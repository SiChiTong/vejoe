#include "MotionCalc.h"
#include <iostream>

using namespace std;
#define M_PI       3.14159265358979323846
#define MOVE_MAX_SPEED 0.05	//3��
#define MOVE_NORMAL_SPEED 0.02	//1��
#define MOVE_EACH_TIME	10		//ÿ���ƶ�ʱ�䣨��������
#define	SPEED_THRESHOLD_ANGLE 0.7	//40��
#define VISION_SECTION_COUNT	5	//�ӽ� ������ ����
#define ONE_SIDE_SECTION_BOUND	0.1	//�߽绺������ ռ��

//��ǰ��Ŀ��Ƕȣ�ʵʱ��
double MotionCalc::currentAngle, MotionCalc::targetAngle;
//��������� �Ƕ�
double MotionCalc::sectionAngle, MotionCalc::sectionBoundAngle, MotionCalc::moveSpeed;

bool MotionCalc::moveFlag;

MotionCalc::MotionCalc(int imageWidth):MAX_VISION_ANGLE(60)
{
	videoImageWidth = imageWidth;
	verticalDistance = (imageWidth / 2.0) / tan(MAX_VISION_ANGLE / 2 * M_PI / 180);
	sectionAngle = MAX_VISION_ANGLE / VISION_SECTION_COUNT * M_PI / 180;
	sectionBoundAngle = sectionAngle * ONE_SIDE_SECTION_BOUND;
	moveFlag = false;
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
	angle *= horiLen>0?1:-1;
	//ת��Ϊ�Ƕ�
	return angle;
}

double  MotionCalc::CalcAngleNextStep()
{//ֱ������Ŀ��λ��
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
	if(!moveFlag)
	{//�ƶ���
		int currentSection = currentAngle / sectionAngle, targetSection = targetAngle / sectionAngle;
		int sectionDiff = targetSection - currentSection;
		int sectionCount = abs(sectionDiff);
		double overLen = currentAngle - sectionAngle * currentSection;
		double tmpValue = targetAngle - max(currentSection, targetSection) * sectionAngle;
		moveFlag = true;
		if(sectionCount > 1 || sectionCount == 1 && abs(tmpValue) > sectionBoundAngle)
			moveSpeed = sectionCount * sectionAngle / MOVE_EACH_TIME * (sectionDiff>0?1:-1);
		else if(currentSection == 0 && targetSection ==0 && currentAngle * targetAngle < 0 && abs(targetAngle) > sectionBoundAngle)
			moveSpeed = 1 * sectionAngle / MOVE_EACH_TIME * (targetAngle>0?1:-1);
		else{
			moveSpeed = 0;
			moveFlag = false;
		}
	}
	currentAngle += moveSpeed;

	if(((currentAngle - targetAngle) * moveSpeed) >= 0)
	{//����Ŀ�ĵ�
		moveFlag = false;
	}
	return currentAngle;
}

void MotionCalc::MoveOrigin()
{
	currentAngle = 0;
}

void MotionCalc::setAngleTarget(double degree)
{
	if(!moveFlag)
		targetAngle = degree;	
}

	