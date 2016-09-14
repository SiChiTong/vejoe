#pragma once
#include <cv.h>

using namespace cv;

class MotionCalc
{
public:
	MotionCalc(int,int);
	~MotionCalc(void);
	//根据横坐标值计算对应角度
	double CalcAngleByLocation(int xValue);
	//向目标移动一个时间单位
	double CalcAngleNextStep();
	//根据分区模式，向目标移动一个时间单位
	void CalcAngleNextStepBySection(int);	
	//归零
	void MoveOrigin();
	//视野范围角度，速度改变的角度阈值
	const int MAX_VISION_ANGLE;

private:
	//更新参数
	void UpdateParams(string);

	//图像总宽度
	int videoImageWidth;
	//摄像头到图像的虚拟距离，用于计算目标角度
	double verticalDistance;
	//移动速度
	double moveSpeed, visionMaxRadian;
	//当前，目标角度（实时）
	double currentAngle, targetAngle;
	//分区域相关 角度
	double sectionAngle, sectionBoundAngle;
	
	Scalar colorDemoResult;
	Point camPosDemoResult, objPosDemoResult;

};

