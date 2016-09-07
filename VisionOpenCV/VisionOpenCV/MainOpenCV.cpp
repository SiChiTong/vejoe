#include <iostream>
#include <cv.h>

#include "ImageHandler.h"
#include "MotionCalc.h"

using namespace std;

int main()
{
	int objXValue = -1;
	ImageHandler imageTool;	
	Mat sourceFrame,background,foreground;

	VideoCapture capture(0);
	if (!capture.isOpened()) return 0;
	if (!capture.read(sourceFrame))	return 0;
	MotionCalc motionCalc(sourceFrame.cols);

	//��˹��ϱ���/ǰ���ָ��
	BackgroundSubtractorMOG2 toolGaussBackground(100,16);
	bool stopFlag(false);
	while (!stopFlag)
	{
		if (!capture.read(sourceFrame))	break;
		imshow("ԭʼͼ��", sourceFrame);
		moveWindow("ԭʼͼ��",0,0);
		////��˹����ǰ��
		//toolGaussBackground(sourceFrame, foreground, -1);
		//toolGaussBackground.getBackgroundImage(background);
		////CamShiftĿ�����ʶ��
		//imageTool.TrackCamShift(sourceFrame,foreground);
		//��������ʶ��
		int xValue = imageTool.RecognitionHumanFace(sourceFrame);
		if(xValue >= 0 && xValue != objXValue)
		{
			objXValue = xValue;
			double nextAngle = motionCalc.CalcAngleByLocation(xValue);
			cout<<"�½Ƕ�"<<nextAngle<<endl;
		}
		//�����������
		if (waitKey(10) == 27)
		{//������ESC�˳�
			stopFlag = true;
		}
	}
	return 0;
}