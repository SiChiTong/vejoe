#include "ImageHandler.h"
#include <numeric>
#include <fstream>

using namespace std;

ImageHandler::ImageHandler(void)
{
}


ImageHandler::~ImageHandler(void)
{
}

void ImageHandler::edgeSobel(Mat sourceFrame){
	Mat grayImage, resultImage;
	cvtColor(sourceFrame, grayImage, CV_RGB2GRAY);     //转为灰度图
    Mat ax, ay;        
    Mat axx, ayy;
    Sobel(grayImage, ax, CV_16S, 1, 0,-1);       
    Sobel(grayImage, ay, CV_16S, 0, 1,-1);
    convertScaleAbs(ax, axx);      //将CV_16S转为CV_8U
    convertScaleAbs(ay, ayy);
    addWeighted(axx, 0.5, ayy, 0.5, 0,resultImage);     //将两图相加
    imshow("Sobel", resultImage);	
	moveWindow("Sobel",0,500);
}

void ImageHandler::edgeLaplacian(Mat sourceFrame){
	Mat grayImage, resultImage;
	cvtColor(sourceFrame, grayImage, CV_RGB2GRAY);     //转为灰度图
    Laplacian(grayImage, resultImage, CV_16S,3);
    convertScaleAbs(resultImage, resultImage);     //将CV_16S转为CV_8U
    imshow("Laplacian", resultImage);	
	moveWindow("Laplacian",600,500);
}

void ImageHandler::edgeCanny(Mat sourceFrame){
	Mat grayImage, resultImage;
	cvtColor(sourceFrame, grayImage, CV_RGB2GRAY);     //转为灰度图
    Canny(grayImage, resultImage, 100, 300, 3);
    imshow("Canny", resultImage);
	moveWindow("Canny",1200,500);
}


//一个Demo图片
void ImageHandler::DemoImage(void){
	//读入图片，注意图片路径
	Mat image=imread("img.jpg");

	//图片读入成功与否判定
	if(!image.data)		return ;

	//显示图像
	imshow("image1",image);	
	moveWindow("人脸识别",800,500);
	
	//等待按键
	waitKey();
}