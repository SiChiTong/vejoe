#pragma once
#include <iostream>
#include <core/core.hpp>
#include <highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include <opencv.hpp>
#include <cv.h>

using namespace cv;

class ImageHandler
{
public:
	ImageHandler(void);
	~ImageHandler(void);
	
	bool RecognitionCamShift(Mat frame);
	void SelectMotionTarget(void);

	void DemoImage(void);

	enum enumReconStatus
	{
		SET_TARGET = 0,
		TARGET_CAMSHIFT
	};
	Mat foreground;
	Rect selection, moveRange;
	int rect_x, rect_y, rect_h, rect_w;
	double x_max_value, x_min_value , y_max_value, y_min_value ;

private:	
	int hsize ;
	float hranges[2] ;
	const float* phranges;
	int ch[2] ;
	//计数器代替定时器
	int stayCount, stayMaxCount ;
	//hsv转化数据范围滤波
	int vmin , vmax , smin ;	
	//中间变量
	Mat hsv, hue, mask, hist, backproj,srcImage;
	//可变空间数组
	vector<vector<Point>> contours;
	vector<Vec4i>hierarchy;
};

