#include <iostream>
#include <cv.h>

#include "ImageHandler.h"

using namespace std;

int main()
{
	ImageHandler imageTool ;
	Mat sourceFrame,background,foreground;

	VideoCapture capture(0);
	if (!capture.isOpened()) return 0;

	//��˹��ϱ���/ǰ���ָ��
	BackgroundSubtractorMOG2 toolGaussBackground(100,16);
	bool stopFlag(false);
	while (!stopFlag)
	{
		if (!capture.read(sourceFrame))	break;		
		imshow("ԭʼͼ��", sourceFrame);
		moveWindow("ԭʼͼ��",0,0);
		//��˹����ǰ��
		toolGaussBackground(sourceFrame, foreground, -1);
		toolGaussBackground.getBackgroundImage(background);
		//CamShiftĿ�����ʶ��
		imageTool.TrackCamShift(sourceFrame,foreground);
		//��������ʶ��
		imageTool.RecognitionHumanFace(sourceFrame);
		//�����������
		if (waitKey(10) == 27)
		{//������ESC�˳�
			stopFlag = true;
		}
	}
	return 0;
}