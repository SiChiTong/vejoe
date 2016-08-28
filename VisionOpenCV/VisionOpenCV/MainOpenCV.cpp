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
	ImageHandler imageTool ;
	Mat frame;

	enumImageStatus workState = SAMPLE_TARGET;
	VideoCapture capture(0);
	if (!capture.isOpened()) return 0;

	BackgroundSubtractorMOG mog;
	bool stopFlag(false);
	while (!stopFlag)
	{
		if (!capture.read(frame))
		{
			break;
		}
		mog(frame, imageTool.foreground, 0.03);
		//
		switch(workState)
		{
		case SAMPLE_TARGET:
			{
				imageTool.SelectMotionTarget();
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
			if(imageTool.RecognitionCamShift(frame))
			{
				workState = SAMPLE_TARGET;
			}
			break;
		}
		// show foreground
		imshow("动作提取图像", imageTool.foreground);
		imshow("原始图像", frame);
		Mat motion = frame.clone();
		rectangle(motion, Point((int)imageTool.x_min_value, (int)imageTool.y_min_value), Point((int)imageTool.x_max_value, (int)imageTool.y_max_value), 180,2,8,0);
		imshow("矩形标记动作", motion);
		if (waitKey(10) == 27)
		{//监听到ESC退出
			stopFlag = true;
		}
	}
	return 0;
}