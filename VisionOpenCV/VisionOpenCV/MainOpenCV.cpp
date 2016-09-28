#include <iostream>
#include <cv.h>

#include "ImageHandler.h"
#include "MotionCalc.h"

#include	 <fstream>
#include     <unistd.h>     /*Unix 标准函数定义*/

using namespace std;

int main()
{
	//sleep(40);
	ofstream outTest("/usr/software/dataVisionmain.txt", ios::out);
	//摄像头宽、高（分辨率），显示图片宽度（高度等比例缩放）
	const int CAMERA_WIDTH = 640, CAMERA_HIGHT = 480, COMPRESS_WIDTH = 400;
	int objXValue = -1, compressHight, angleMin,angleMax, xValue;
	ImageHandler imageTool;	
	Mat sourceFrame,foreground, compressFrame;
	compressHight = 1.0 * CAMERA_HIGHT * COMPRESS_WIDTH / CAMERA_WIDTH;
	outTest << "ready" <<endl;
	VideoCapture capture(0);
	outTest << "open" <<endl;
	if (!capture.isOpened()) return 0;
	outTest << "opened" <<endl;
	MotionCalc motionCalc(COMPRESS_WIDTH,imageTool.MAX_VISION);
	angleMax = motionCalc.MAX_VISION_ANGLE / 2;
	angleMin = -1 * angleMax;	
	//高斯混合背景/前景分割方法
	BackgroundSubtractorMOG2 toolGaussBackground(100,16);
//	BackgroundSubtractorMOG toolGaussBackground(100,16,0.6);
	bool stopFlag(false);
	outTest << "start loop" <<endl;
	int idx=0;
	while (!stopFlag)
	{
		idx ++;
		outTest << idx <<endl;
		if (!capture.read(sourceFrame))
		{
			outTest << "camera fail" <<endl;
			capture.open(0);
			cout<<endl<<capture.isOpened()<<"Camera Read Fail;"<<endl;
			if (!capture.isOpened() || !capture.read(sourceFrame)) break;
		}
		outTest << "handler" <<endl;
		resize(sourceFrame,compressFrame,Size(COMPRESS_WIDTH, compressHight));
		//显示原始图像
		//imshow("Source Image", compressFrame);
		//moveWindow("Source Image",0,0);
		//高斯分离前景
		toolGaussBackground(compressFrame, foreground, -1);
//		toolGaussBackground(compressFrame, foreground, 0.4);
		//运动目标识别
		xValue = imageTool.TrackMotionTarget(compressFrame,foreground);		
		outTest <<xValue<< "target find" <<objXValue<<endl;
		//人脸跟踪识别
		//xValue = imageTool.RecognitionHumanFace(compressFrame);
		//显示结果
		if(xValue >= 0 && (xValue >= objXValue + 2 || xValue <= objXValue - 2))
		{//转动不低于2度才显示
			outTest << "calc start" <<endl;
			objXValue = xValue;
			motionCalc.CalcAngleNextStepBySection(xValue);
			outTest << "calc finished" <<endl;
		}
		outTest << "item finish" <<endl;
		//程序结束开关
		if (waitKey(10) == 27)
		{//监听到ESC退出
			stopFlag = true;
		}
	}
	return 0;
}

