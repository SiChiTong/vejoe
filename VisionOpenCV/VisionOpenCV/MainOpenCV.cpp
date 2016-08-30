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

	//高斯混合背景/前景分割方法
	BackgroundSubtractorMOG2 toolGaussBackground(100,16);
	bool stopFlag(false);
	while (!stopFlag)
	{
		if (!capture.read(sourceFrame))	break;		
		imshow("原始图像", sourceFrame);
		moveWindow("原始图像",0,0);
		//高斯分离前景
		toolGaussBackground(sourceFrame, foreground, -1);
		toolGaussBackground.getBackgroundImage(background);
		//CamShift目标跟踪识别
		imageTool.TrackCamShift(sourceFrame,foreground);
		//人脸跟踪识别
		imageTool.RecognitionHumanFace(sourceFrame);
		//程序结束开关
		if (waitKey(10) == 27)
		{//监听到ESC退出
			stopFlag = true;
		}
	}
	return 0;
}