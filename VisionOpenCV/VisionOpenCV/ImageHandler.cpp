#include "ImageHandler.h"


using namespace std;


#define  AREAS_MOTION  100  //
#define    MIN_TARGET_AREAR     500
#define    MAX_TARGET_AREAR     10000


ImageHandler::ImageHandler(void)
{
	vmin = 10;
	vmax = 256;
	smin = 30;
	x_min_value = 0;
	y_min_value = 0;

	hsize = 16;
	hranges[0]=0;
	hranges[1]=180;
	phranges = hranges;
	stayCount = 0;
	stayMaxCount = 100000;
	ch[0]=0;
	ch[1] =0;
}


ImageHandler::~ImageHandler(void)
{
}

//使用CamShift算法捕获运动并标记
bool ImageHandler::RecognitionCamShift(Mat frame)
{
	//跟踪目标消失标志
	bool targetExistsFlag = false;
	static enumReconStatus state_fsm = SET_TARGET;
	Mat dstImage;
	//使用中值滤波器进行模糊操作（平滑处理）：中值滤波将图像的每个像素用邻域 (以当前像素为中心的正方形区域)像素的中值代替
	medianBlur(frame, dstImage, 3);
	//高斯滤波：这个像素滤波后的值是根据其相邻像素（包括自己那个点）与一个滤波模板进行相乘
	GaussianBlur(dstImage, dstImage, Size(3,3), 0,0);
	//将彩色图像转换为HSV格式，保存到hsv中
	cvtColor(dstImage, hsv, COLOR_BGR2HSV);
	//将数据设定在规定的范围内
	inRange(hsv, Scalar(0, smin, MIN(vmin,vmax)),
		Scalar(180, 256, MAX(vmin, vmax)), mask);
	//
	hue.create(hsv.size(), hsv.depth());
	//取出H通道放入hue中
	mixChannels(&hsv, 1, &hue, 1, ch, 1);

	switch(state_fsm)
	{
	case SET_TARGET:
		{
			targetExistsFlag = false;   
			//计算直方图
			Mat roi(hue, selection), maskroi(mask, selection);
			//计算直方图，放入hist.
			calcHist(&roi, 1, 0, maskroi, hist, 1, &hsize, &phranges);
			//归一化处理
			normalize(hist, hist, 0, 255, CV_MINMAX);

			state_fsm = TARGET_CAMSHIFT;
			break;
		}

	case TARGET_CAMSHIFT :
		{
			//反向投影图,放入backproj, H通道的范围在0~180的范围内
			calcBackProject(&hue, 1, 0, hist, backproj, &phranges);
			backproj &= mask;
			//camshift算法
			RotatedRect trackBox = CamShift(backproj, selection,TermCriteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 15, 2.0));

			//判断camshift目标追寻不到, 通过返回的矩形框的面积来判断
			ellipse(dstImage, trackBox, Scalar(0,0,255), 3, CV_AA);
			imshow("运动对象补货", dstImage);

			//判断跟踪的目标位置是否发生改变，若没有则开始计时
			SelectMotionTarget();
			//该计算只是每帧之间的移动面积差，若不行估计要移动多帧后在计算。
			if(moveRange.height * moveRange.width <= AREAS_MOTION) //说明物体基本没有移动
				stayCount++; 
			else
				stayCount=0;

			if(trackBox.size.area() <= 300 || stayCount >= stayMaxCount)
			{
				targetExistsFlag = true;
				state_fsm = SET_TARGET;
				destroyWindow("运动对象补货");
				stayCount=0;
			}
			break;
		}
	}
	return targetExistsFlag;
}

//确定运动区域
void ImageHandler::SelectMotionTarget(void)
{
	medianBlur(foreground, srcImage, 9);
	GaussianBlur(srcImage, srcImage, Size(5,5), 0,0);
	//二值化处理
	threshold(srcImage, srcImage, 180, 255, THRESH_BINARY);
	Canny(srcImage, srcImage, 3, 9, 3);
	//找到所有轮廓
	findContours(srcImage, contours, hierarchy, RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0,0));
	vector<vector<Point>>contours_poly(contours.size());
	vector<Rect>boundRect(contours.size());
	vector<int>array_x(contours.size()), array_y(contours.size());

	for(unsigned int i = 0; i < contours.size(); i ++)
	{
		approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
		boundRect[i] = boundingRect(Mat(contours_poly[i]));
		array_x[i] = boundRect[i].x;
		array_y[i] = boundRect[i].y;
	}
	//找到最大值,最小值
	minMaxLoc(array_x, &x_min_value, &x_max_value, 0, 0);
	minMaxLoc(array_y, &y_min_value, &y_max_value, 0, 0);
	//计算面积
	moveRange.x = (int)x_min_value; 
	moveRange.y = (int)y_min_value;
	moveRange.height = (int)x_max_value - (int)x_min_value;
	moveRange.width = (int)y_max_value - (int)y_min_value;
}

//一个Demo图片
void ImageHandler::DemoImage(void){
	//读入图片，注意图片路径
	Mat image=imread("girl.jpg");
	
	//图片读入成功与否判定
	if(!image.data)		return ;
	
	//创建一个名字为“Lena”的图像显示窗口，（不提前声明也可以）
	namedWindow("Lena",1);
	
	//显示图像
	imshow("Lena",image);
	
	//等待按键
	waitKey();
}