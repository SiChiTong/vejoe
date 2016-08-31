#pragma once
class MotionCalc
{
public:
	MotionCalc(int imageWidth);
	~MotionCalc(void);
	//根据横坐标值计算对应角度
	double CalcAngleByLocation(int xValue);
	//归零
	void MoveOrigin();

	//当前角度（实时）
	double currentAngle;

private:
	//视野范围角度
	const int MAX_VISION_ANGLE;
	//移动角速度
	int moveSpeed;
	//图像总宽度
	int videoImageWidth;
	//摄像头到图像的虚拟距离，用于计算目标角度
	double verticalDistance;
};

