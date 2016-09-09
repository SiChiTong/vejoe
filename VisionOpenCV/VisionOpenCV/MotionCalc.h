#pragma once

class MotionCalc
{
public:
	MotionCalc(int imageWidth);
	~MotionCalc(void);
	//根据横坐标值计算对应角度
	double CalcAngleByLocation(int xValue);
	//向目标移动一个时间单位
	static double CalcAngleNextStep();
	//根据分区模式，向目标移动一个时间单位
	static double CalcAngleNextStepBySection();	
	//归零
	void MoveOrigin();
	//视野范围角度，速度改变的角度阈值
	const int MAX_VISION_ANGLE;
	//设置移动目标角度
	void setAngleTarget(double);
	//当前，目标角度（实时）
	static double currentAngle, targetAngle;
	//分区域相关 角度
	static double sectionAngle, sectionBoundAngle;

private:
	//移动角速度（弧度）
	//static  moveMaxSpeed, moveNormalSpeed,SPEED_THRESHOLD_ANGLE;
	//图像总宽度
	int videoImageWidth;
	//摄像头到图像的虚拟距离，用于计算目标角度
	double verticalDistance;
};

