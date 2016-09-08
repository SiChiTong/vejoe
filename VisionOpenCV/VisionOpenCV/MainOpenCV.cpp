#include <iostream>
#include <cv.h>

#include "ImageHandler.h"
#include "MotionCalc.h"

using namespace std;

int main()
{
	//����ͷ���ߣ��ֱ��ʣ�����ʾͼƬ��ȣ��߶ȵȱ������ţ�
	const int CAMERA_WIDTH = 640, CAMERA_HIGHT = 480, COMPRESS_WIDTH = 400;
	int objXValue = -1, compressHight, angleMin,angleMax, xValue;
	double nextAngle;
	ImageHandler imageTool;	
	Mat sourceFrame,foreground, compressFrame;
	compressHight = 1.0 * CAMERA_HIGHT * COMPRESS_WIDTH / CAMERA_WIDTH;
	
	VideoCapture capture(0);
	if (!capture.isOpened()) return 0;
	MotionCalc motionCalc(COMPRESS_WIDTH);
	angleMax = motionCalc.MAX_VISION_ANGLE / 2;
	angleMin = -1 * angleMax;

	//��˹��ϱ���/ǰ���ָ��
	BackgroundSubtractorMOG2 toolGaussBackground(100,16);
	bool stopFlag(false);
	while (!stopFlag)
	{
		if (!capture.read(sourceFrame))	break;
		resize(sourceFrame,compressFrame,Size(COMPRESS_WIDTH, compressHight));
		//��ʾԭʼͼ��
		imshow("Source Image", compressFrame);
		moveWindow("Source Image",0,0);
		//��˹����ǰ��
		toolGaussBackground(compressFrame, foreground, -1);
		//�˶�Ŀ��ʶ��
		xValue = imageTool.TrackMotionTarget(compressFrame,foreground);
		////��������ʶ��
		//xValue = imageTool.RecognitionHumanFace(compressFrame);
		if(xValue >= 0 && (xValue >= objXValue + 1 || xValue <= objXValue - 1))
		{//ת��������һ�Ȳ���ʾ
			objXValue = xValue;
			nextAngle = motionCalc.CalcAngleByLocation(xValue);
			imageTool.ShowDemoInfo(nextAngle);
		}

		//�����������
		if (waitKey(10) == 27)
		{//������ESC�˳�
			stopFlag = true;
		}
	}
	return 0;
}