#pragma once
#include <iostream>
#include <core/core.hpp>
#include <highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv.hpp>
#include <cv.h>

using namespace cv;

class ImageHandler
{
public:
	ImageHandler(void);
	~ImageHandler(void);
	
	//颜色追踪
	bool TrackCamShift(Mat souceFrame,Mat foreground);
	//颜色追踪
	int TrackMotionTarget(Mat souceFrame,Mat foreground);
	//识别运动物体
	void RecognitionMotionTarget(Mat foreground);
	//人脸识别
	int RecognitionHumanFace(Mat sourceFrame);

	//图像测试
	void DemoImage(void);

	enum enumReconStatus
	{
		SET_TARGET = 0,
		TARGET_CAMSHIFT
	};
	Mat foreground;
	Rect selection, moveRange;
	int MAX_VISION;

private:
	//更新参数
	void UpdateParams(string);

	bool findTargetFlag;
	//首次识别脸部需要视频图像帧数，同一个脸部的距离误差范围，改变目标作为跳帧的阈值，重新定位前连续发生跳帧次数
	const int FIRST_FRAME_COUNT, MIN_SIZE_PIXEL, CHANGE_FACE_JUMP_FALG, CHANGE_FACE_MIN_COUNT;
	//识别区域的最小面积, 中值滤波数据量, 均值滤波数据量
	int MIN_RECT_AREA, MAX_RECT_AREA,FILTER_MIDDLE_COUNT, FILTER_MEAN_COUNT;
	//跳帧统计
	int jumpFrameCount;
	int hsize, moveFrameCount;
	//计数器代替定时器
	int stayCount, stayMaxCount ;
	//hsv转化数据范围滤波
	int vmin , vmax , smin ;
	float hranges[2] ;
	const float* phranges;
	int ch[2] ;
	vector<int> midFiltArray,meanFiltArray, sourceFiltArray;
	//当前追踪的目标，下一个移动目标点
	Rect nextTarget;
	RotatedRect nextTargetRotate, trackBox;
	//中间变量
	Mat hsv, hue, mask, hist, backproj,srcImage,tmpImage, shapeOperateKernal;
	//可变空间数组
	vector<vector<Point> > contourAll;
	vector<Vec4i>hierarchy;	
	//人脸识别
	CascadeClassifier faceCascade;
	vector<vector<Rect> > allFaceLatest;
};

