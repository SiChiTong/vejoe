#pragma once
#include <iostream>
#include <core/core.hpp>
#include <highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include <opencv.hpp>
#include <cv.h>

using namespace cv;

struct HSVBoundray
{
	int HLow;
	int SLow;
	int VLow;
	int HUp;
	int SUp;
	int VUp;
};

enum ColorType
{
	Red,
	Blue,
	Green,
	Yello,
	Purple,
	Black,
	White,
	Gray
};

class ImageHandler
{
public:

	ImageHandler(void);
	~ImageHandler(void);
	
	void recoginceColor(Mat,int);

private:
	HSVBoundray getColorBoundary(int);

	Mat element;
	HSVBoundray HsvRed,HsvBlue,HsvGreen,HsvYellow,HsvPurple,HsvBlack,HsvWhite,HsvGray,HsvWater;
};

