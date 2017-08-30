#include "ImageHandler.h"
#include <numeric>
#include <fstream>

using namespace std;


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

HSVBoundray ImageHandler::getColorBoundary(ColorType color){
	HSVBoundray hsvResult;
	switch (color)
	{
	case Red:
		hsvResult = HsvRed;
		break;
	case Blue:
		hsvResult = HsvBlue;
		break;
	case Green:
		hsvResult = HsvGreen;
		break;
	case Yello:
		hsvResult = HsvYellow;
		break;
	case Purple:
		hsvResult = HsvPurple;
		break;
	case Black:
		hsvResult = HsvBlack;
		break;
	case White:
		hsvResult = HsvWhite;
		break;
	case Gray:
		hsvResult = HsvGray;
		break;
	default:
		break;
	}
	return hsvResult;
}

void ImageHandler::recoginceColor(Mat sourceFrame,ColorType color){
	Mat imgHSV,resultImage;
	vector<Mat> hsvSplit;
	cvtColor(sourceFrame, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV

	HSVBoundray hsv = getColorBoundary(color);

	inRange(imgHSV, Scalar(hsv.HLow, hsv.SLow, hsv.VLow), Scalar(hsv.HUp,hsv.SUp,hsv.VUp), resultImage); //Threshold the image

	//开操作 (去除一些噪点)
	morphologyEx(resultImage, resultImage, MORPH_OPEN, element);
	//闭操作 (连接一些连通域)
	morphologyEx(resultImage, resultImage, MORPH_CLOSE, element);

    imshow("Thresholded", resultImage);	
	moveWindow("Thresholded",700,0);
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