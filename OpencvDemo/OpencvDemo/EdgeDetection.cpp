#include "stdafx.h"
#include "EdgeDetection.h"


EdgeDetection::EdgeDetection(void)
{
}


EdgeDetection::~EdgeDetection(void)
{
}

Mat EdgeDetection::sobel(Mat source)
{
    Mat grayImage, resultImage;
    cvtColor(source, grayImage, CV_RGB2GRAY);     //转为灰度图
    Mat ax, ay;        
    Mat axx, ayy;
    Sobel(grayImage, ax, CV_16S, 1, 0,-1);       
    Sobel(grayImage, ay, CV_16S, 0, 1,-1);
    convertScaleAbs(ax, axx);      //将CV_16S转为CV_8U
    convertScaleAbs(ay, ayy);
    addWeighted(axx, 0.5, ayy, 0.5, 0,resultImage);     //将两图相加
    return resultImage;
}

Mat EdgeDetection::laplacian(Mat source)
{
    Mat grayImage, resultImage;
    cvtColor(source, grayImage, CV_RGB2GRAY);     //转为灰度图
    Laplacian(grayImage, resultImage, CV_16S,3);
    convertScaleAbs(resultImage, resultImage);     //将CV_16S转为CV_8U
    return resultImage;
}

Mat EdgeDetection::canny(Mat source)
{
    Mat grayImage, resultImage;
    cvtColor(source, grayImage, CV_RGB2GRAY);     //转为灰度图
    Canny(grayImage, resultImage, 100, 300, 3);
    return resultImage;
}
