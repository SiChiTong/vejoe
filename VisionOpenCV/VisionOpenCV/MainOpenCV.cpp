#include <iostream>
#include <core/core.hpp>
#include <highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include <opencv.hpp>
#include <cv.h>

#include "ImageHandler.h"

using namespace std;

#define    MIN_TARGET_AREAR     500
#define    MAX_TARGET_AREAR     10000

enum enumImageStatus
{
	TARTGET_FOLLOW = 0,
	SAMPLE_TARGET = 1
};

int main()
{
	
	int nIdx=0;
	ImageHandler imageTool ;
	Mat sourceFrame,background,foreground;

	enumImageStatus workState = SAMPLE_TARGET;
	VideoCapture capture(0);
	if (!capture.isOpened()) return 0;
	
	Mat shapeOperateKernal = getStructuringElement(MORPH_RECT, Size(5, 5));
	Mat tmpImage;

	//高斯混合背景/前景分割方法
	BackgroundSubtractorMOG2 toolGaussBackground(100,16);
	bool stopFlag(false);
	while (!stopFlag)
	{
		if (!capture.read(sourceFrame))	break;

		toolGaussBackground(sourceFrame, foreground, -1);
		toolGaussBackground.getBackgroundImage(background);
		imshow("背景图像", background);
		moveWindow("背景图像",0,0);
		//
		switch(workState)
		{
		case SAMPLE_TARGET:
			{
				imageTool.RecognitionMotionTarget(foreground);
				//面积在一定的范围内才进入目标赛选程序
				int areaObject = imageTool.moveRange.height * imageTool.moveRange.width;
				if(areaObject >= MIN_TARGET_AREAR && areaObject <= MAX_TARGET_AREAR)
				{
					imageTool.selection = imageTool.moveRange;
					workState = TARTGET_FOLLOW;
				}
				break;
			}
		case TARTGET_FOLLOW:
			if(imageTool.TrackCamShift(sourceFrame,foreground))
			{
				workState = SAMPLE_TARGET;
			}
			break;
		}
		
		imageTool.RecognitionHumanFace(sourceFrame);

		if (waitKey(10) == 27)
		{//监听到ESC退出
			stopFlag = true;
		}
	}
	return 0;
}