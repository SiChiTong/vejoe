#pragma once

class MotionCalc
{
public:
	MotionCalc(int imageWidth);
	~MotionCalc(void);
	//���ݺ�����ֵ�����Ӧ�Ƕ�
	double CalcAngleByLocation(int xValue);
	//��Ŀ���ƶ�һ��ʱ�䵥λ
	static double CalcAngleNextStep();
	//���ݷ���ģʽ����Ŀ���ƶ�һ��ʱ�䵥λ
	static double CalcAngleNextStepBySection();	
	//����
	void MoveOrigin();
	//��Ұ��Χ�Ƕȣ��ٶȸı�ĽǶ���ֵ
	const int MAX_VISION_ANGLE;
	//�����ƶ�Ŀ��Ƕ�
	void setAngleTarget(double);
	//��ǰ��Ŀ��Ƕȣ�ʵʱ��
	static double currentAngle, targetAngle;
	//��������� �Ƕ�
	static double sectionAngle, sectionBoundAngle;

private:
	//�ƶ����ٶȣ����ȣ�
	//static  moveMaxSpeed, moveNormalSpeed,SPEED_THRESHOLD_ANGLE;
	//ͼ���ܿ��
	int videoImageWidth;
	//����ͷ��ͼ���������룬���ڼ���Ŀ��Ƕ�
	double verticalDistance;
};

