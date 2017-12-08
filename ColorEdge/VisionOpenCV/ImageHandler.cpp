#include "ImageHandler.h"
#include <numeric>
#include <fstream>

using namespace std;

#define MIN_RECT_AREA 500
#define MAX_RECT_AREA 300000

ImageHandler::ImageHandler(void)
{
	element = getStructuringElement(MORPH_RECT, Size(5, 5));

	HsvRed.HLow=156;
	HsvRed.SLow=43;
	HsvRed.VLow=46;
	HsvRed.HUp=180;
	HsvRed.SUp=255;
	HsvRed.VUp=255;
	
	HsvBlue.HLow=100;
	HsvBlue.SLow=43;
	HsvBlue.VLow=46;
	HsvBlue.HUp=124;
	HsvBlue.SUp=255;
	HsvBlue.VUp=255;

	HsvGreen.HLow=35;
	HsvGreen.SLow=43;
	HsvGreen.VLow=46;
	HsvGreen.HUp=77;
	HsvGreen.SUp=255;
	HsvGreen.VUp=255;
	
	HsvYellow.HLow=26;
	HsvYellow.SLow=43;
	HsvYellow.VLow=46;
	HsvYellow.HUp=34;
	HsvYellow.SUp=255;
	HsvYellow.VUp=255;
	
	HsvPurple.HLow=125;
	HsvPurple.SLow=43;
	HsvPurple.VLow=46;
	HsvPurple.HUp=155;
	HsvPurple.SUp=255;
	HsvPurple.VUp=255;
	
	HsvBlack.HLow=0;
	HsvBlack.SLow=0;
	HsvBlack.VLow=0;
	HsvBlack.HUp=180;
	HsvBlack.SUp=255;
	HsvBlack.VUp=46;
	
	HsvWhite.HLow=0;
	HsvWhite.SLow=0;
	HsvWhite.VLow=221;
	HsvWhite.HUp=180;
	HsvWhite.SUp=30;
	HsvWhite.VUp=255;
	
	HsvGray.HLow=0;
	HsvGray.SLow=0;
	HsvGray.VLow=46;
	HsvGray.HUp=180;
	HsvGray.SUp=43;
	HsvGray.VUp=220;
}


ImageHandler::~ImageHandler(void)
{
}

HSVBoundray ImageHandler::getColorBoundary(int color){
	HSVBoundray hsvResult;
	switch (color)
	{
	case 1://Red:
		hsvResult = HsvRed;
		break;
	case 2://Blue:
		hsvResult = HsvBlue;
		break;
	case 3://Green:
		hsvResult = HsvGreen;
		break;
	case 4://Yello:
		hsvResult = HsvYellow;
		break;
	case 5://Purple:
		hsvResult = HsvPurple;
		break;
	case 6://Black:
		hsvResult = HsvBlack;
		break;
	case 7://White:
		hsvResult = HsvWhite;
		break;
	case 8://Gray:
		hsvResult = HsvGray;
		break;
	default:
		break;
	}
	return hsvResult;
}


void edgeSobel(Mat grayImage){
	Mat  resultImage;
    Mat ax, ay;        
    Mat axx, ayy;
    Sobel(grayImage, ax, CV_16S, 1, 0,-1);       
    Sobel(grayImage, ay, CV_16S, 0, 1,-1);
    convertScaleAbs(ax, axx);      //将CV_16S转为CV_8U
    convertScaleAbs(ay, ayy);
    addWeighted(axx, 0.5, ayy, 0.5, 0,resultImage);     //将两图相加
    imshow("全部边缘", resultImage);	
	moveWindow("全部边缘",800,500);
}

vector<vector<Point> > contourAll;
vector<Vec4i>hierarchy;	

void edgeTest(Mat srcImage)
{
	Mat resultImage(srcImage.size(),srcImage.type(),Scalar(0));
	//找到所有轮廓
	findContours(srcImage, contourAll, hierarchy, RETR_EXTERNAL , CHAIN_APPROX_SIMPLE);
	int shapeCount = contourAll.size(), maxAreaValue=0, maxAreaIdx = -1;
	vector<vector<Point> >contoursAppr(shapeCount);
	vector<Rect> boundRect(shapeCount);
	//找到最大连通域
	for(int i = 0; i < shapeCount; i ++)
	{//找到所有物体
		approxPolyDP(Mat(contourAll[i]), contoursAppr[i], 5, true);
		boundRect[i] = boundingRect(Mat(contoursAppr[i]));
		if(boundRect[i].area() < MIN_RECT_AREA || boundRect[i].area() > MAX_RECT_AREA) continue;
		//绘制边缘
		drawContours(resultImage,contourAll,i,Scalar(255), 1);
	}
	
    imshow("根据面积滤波后边缘", resultImage);	
	moveWindow("根据面积滤波后边缘",0,500);
}

void ImageHandler::recoginceColor(Mat sourceFrame,int color){
	Mat imgHSV,resultImage;
	vector<Mat> hsvSplit;
	cvtColor(sourceFrame, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV

	HSVBoundray hsv = getColorBoundary(color);

	inRange(imgHSV, Scalar(hsv.HLow, hsv.SLow, hsv.VLow), Scalar(hsv.HUp,hsv.SUp,hsv.VUp), resultImage); //Threshold the image

	//开操作 (去除一些噪点)
	morphologyEx(resultImage, resultImage, MORPH_OPEN, element);
	//闭操作 (连接一些连通域)
	morphologyEx(resultImage, resultImage, MORPH_CLOSE, element);

    imshow("颜色模块", resultImage);	
	moveWindow("颜色模块",800,0);
	
	edgeSobel(resultImage);
	edgeTest(resultImage);
}


