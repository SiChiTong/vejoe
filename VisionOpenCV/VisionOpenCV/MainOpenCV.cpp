#include <iostream>
#include <cv.h>

#include "ImageHandler.h"
#include "MotionCalc.h"

using namespace std;

int main()
{
	//����ͷ���ߣ��ֱ��ʣ�����ʾͼƬ��ȣ��߶ȵȱ������ţ�
	const int CAMERA_WIDTH = 640, CAMERA_HIGHT = 480, COMPRESS_WIDTH = 400;
	int objXValue = -1, compressHight, angleMin,angleMax;
	ImageHandler imageTool;	
	Mat sourceFrame,background,foreground, compressFrame;
	compressHight = 1.0 * CAMERA_HIGHT * COMPRESS_WIDTH / CAMERA_WIDTH;

	VideoCapture capture(0);
	if (!capture.isOpened()) return 0;
	if (!capture.read(sourceFrame))	return 0;
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
		imshow("Source Image", sourceFrame);
		moveWindow("Source Image",0,0);
		////��˹����ǰ��
		//toolGaussBackground(sourceFrame, foreground, -1);
		//toolGaussBackground.getBackgroundImage(background);
		////CamShiftĿ�����ʶ��
		//imageTool.TrackCamShift(sourceFrame,foreground);

		//��������ʶ��
		int xValue = imageTool.RecognitionHumanFace(compressFrame);
		if(xValue >= 0 && (xValue >= objXValue + 1 || xValue <= objXValue - 1))
		{//ת��������һ�Ȳ���ʾ
			objXValue = xValue;
			double nextAngle = motionCalc.CalcAngleByLocation(xValue);
			for(int i=angleMin;i<angleMax;i += 2)
			{
				cout << ((nextAngle + 2 >= i && nextAngle - 2 <= i) ? "|":"_");
			}
			cout<<endl;
		}

		//�����������
		if (waitKey(10) == 27)
		{//������ESC�˳�
			stopFlag = true;
		}
	}
	return 0;
}