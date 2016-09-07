#pragma once

class MotionCalc
{
public:
	MotionCalc(int imageWidth);
	~MotionCalc(void);
	//���ݺ�����ֵ�����Ӧ�Ƕ�
	double CalcAngleByLocation(int xValue);
	//����
	void MoveOrigin();
	//��Ұ��Χ�Ƕ�
	const int MAX_VISION_ANGLE;
	//��ǰ�Ƕȣ�ʵʱ��
	double currentAngle;

private:
	//�ƶ����ٶ�
	int moveSpeed;
	//ͼ���ܿ��
	int videoImageWidth;
	//����ͷ��ͼ���������룬���ڼ���Ŀ��Ƕ�
	double verticalDistance;
};

