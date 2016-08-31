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

	//��ǰ�Ƕȣ�ʵʱ��
	double currentAngle;

private:
	//��Ұ��Χ�Ƕ�
	const int MAX_VISION_ANGLE;
	//�ƶ����ٶ�
	int moveSpeed;
	//ͼ���ܿ��
	int videoImageWidth;
	//����ͷ��ͼ���������룬���ڼ���Ŀ��Ƕ�
	double verticalDistance;
};

