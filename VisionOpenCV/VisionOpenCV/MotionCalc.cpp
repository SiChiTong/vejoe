#include "MotionCalc.h"
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <highgui/highgui.hpp>



using namespace std;
#define M_PI       3.14159265358979323846
#define MOVE_MAX_SPEED 0.05	//3度
#define MOVE_NORMAL_SPEED 0.02	//1度
#define MOVE_EACH_TIME	10		//每次移动时间（脉冲数）
#define	SPEED_THRESHOLD_ANGLE 0.7	//40度
#define VISION_SECTION_COUNT	5	//视角 分区域 总数
#define ONE_SIDE_SECTION_BOUND	0.1	//边界缓冲区域 占比
#define	DEMO_RESULT_RADIUS		500 // 指针 半径

MotionCalc::MotionCalc(int imageWidth,int maxVision):MAX_VISION_ANGLE(maxVision)
{
	videoImageWidth = imageWidth;
	verticalDistance = (imageWidth / 2.0) / tan(MAX_VISION_ANGLE / 2 * M_PI / 180);
	sectionAngle = MAX_VISION_ANGLE / VISION_SECTION_COUNT * M_PI / 180;
	sectionBoundAngle = sectionAngle * ONE_SIDE_SECTION_BOUND;
	visionMaxRadian = MAX_VISION_ANGLE / 2.0 / 180 * M_PI;

	currentAngle = 0;
	angleChar[1] = '\0';//手动添加结束符，否则会自动补0x02作为结束符
	camPosDemoResult = Point(DEMO_RESULT_RADIUS/2,0);
	objPosDemoResult = Point(DEMO_RESULT_RADIUS/2,DEMO_RESULT_RADIUS/2);
	colorDemoResult = Scalar(255,255,255);
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
{//直接移向目标位置
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

//计算并显示最终结果
void  MotionCalc::CalcAngleNextStepBySection(int xValue)
{
	//弧度转换
	targetAngle = CalcAngleByLocation(xValue);
	//计算速度
	int currentSection = currentAngle / sectionAngle, targetSection = targetAngle / sectionAngle;
	int sectionDiff = targetSection - currentSection;
	int sectionCount = abs(sectionDiff);
	double overLen = currentAngle - sectionAngle * currentSection;
	double tmpValue = targetAngle - max(currentSection, targetSection) * sectionAngle;
	if(sectionCount > 1 || sectionCount == 1 && abs(tmpValue) > sectionBoundAngle)
		moveSpeed = sectionCount * sectionAngle / MOVE_EACH_TIME * (sectionDiff>0?1:-1);
	else if(currentSection == 0 && targetSection ==0 && currentAngle * targetAngle < 0 && abs(targetAngle) > sectionBoundAngle)
		moveSpeed = 1 * sectionAngle / MOVE_EACH_TIME * (targetAngle>0?1:-1);
	else
		moveSpeed = 0;

	while(((currentAngle - targetAngle) * moveSpeed) < 0){//到达目的地
		if(abs(currentAngle + moveSpeed) >= visionMaxRadian) break;//安全域检查
		currentAngle += moveSpeed;
		//发给设备
		angleChar[0] = int(currentAngle * 180 * 2 / M_PI)+90;//+90转为正数
		//serial.sendMsg(angleChar);
		////绘制指针模拟头动
		//Mat demoResultInfo = Mat::zeros(DEMO_RESULT_RADIUS/2,DEMO_RESULT_RADIUS,CV_8UC1);
		////标注摄像头位置
		//circle(demoResultInfo,camPosDemoResult,6,colorDemoResult,5);
		////计算当前角度
		//objPosDemoResult.x = DEMO_RESULT_RADIUS / 2.0 * (1.0 - sin(currentAngle));
		//objPosDemoResult.y = DEMO_RESULT_RADIUS / 2.0 * cos(currentAngle);
		//line(demoResultInfo, camPosDemoResult, objPosDemoResult,colorDemoResult,3);
		//if(xValue > 0)
		//{//-1为没捕捉到运动物体
		//	circle(demoResultInfo,Point(DEMO_RESULT_RADIUS - xValue * 1.2,0), 7,colorDemoResult,2);
		//}
		//imshow("Result", demoResultInfo);
		//moveWindow("Result",500,0);
		waitKey(1000/MOVE_EACH_TIME); //发送脉冲间隔		
	}
}

void MotionCalc::MoveOrigin()
{
	currentAngle = 0;
}
	