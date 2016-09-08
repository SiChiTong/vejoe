#include <iostream>
#include <cv.h>
#include <pthread.h>

#include "ImageHandler.h"
#include "MotionCalc.h"
#pragma comment(lib,"pthreadVC2.lib")

using namespace std;

void *showObjectResult(void *);

double (* calcAngle)();
void (*showAngle)(double);
bool stopFlag(false);

double nextAngle;
int main()
{
	//����ͷ���ߣ��ֱ��ʣ�����ʾͼƬ��ȣ��߶ȵȱ������ţ�
	const int CAMERA_WIDTH = 640, CAMERA_HIGHT = 480, COMPRESS_WIDTH = 400;
	int objXValue = -1, compressHight, angleMin,angleMax, xValue;
	//double nextAngle;
	ImageHandler imageTool;	
	Mat sourceFrame,foreground, compressFrame;
	compressHight = 1.0 * CAMERA_HIGHT * COMPRESS_WIDTH / CAMERA_WIDTH;
	
	VideoCapture capture(0);
	if (!capture.isOpened()) return 0;
	MotionCalc motionCalc(COMPRESS_WIDTH);
	angleMax = motionCalc.MAX_VISION_ANGLE / 2;
	angleMin = -1 * angleMax;
	//���߳���ʾλ��
	calcAngle = motionCalc.CalcAngleNextStep;
	showAngle = imageTool.ShowDemoInfo;
	pthread_t showThread;
	int thErr = pthread_create(&showThread, NULL,showObjectResult, NULL);
	if(thErr != 0){cout << "Mult thread create fail:" << thErr <<endl; return -1;}
	

	//��˹��ϱ���/ǰ���ָ��
	BackgroundSubtractorMOG2 toolGaussBackground(100,16);
	
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
		if(xValue >= 0 && (xValue >= objXValue + 2 || xValue <= objXValue - 2))
		{//ת��������һ�Ȳ���ʾ
			objXValue = xValue;
			nextAngle = motionCalc.CalcAngleByLocation(xValue);
			motionCalc.setAngleTarget(nextAngle);
		}

		//�����������
		if (waitKey(10) == 27)
		{//������ESC�˳�
			stopFlag = true;
		}
	}
	return 0;
}


void* showObjectResult(void * arg)
{
	while(!stopFlag){
		cout << nextAngle << '+';
		showAngle(calcAngle());
		waitKey(100);
	}
	return 0;
}