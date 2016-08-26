#include <iostream>
#include <core/core.hpp>
#include <highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include <opencv.hpp>
#include <cv.h>

using namespace cv;
using namespace std;

void DemoImage(void);

//------------------------------------//
//存放H通道数据
vector <Mat> channels, back_channels;
//------------------------------------//
Mat grad_x, grad_y, grad_xy, grad_abx, grad_aby;
Mat frame,srcImage, element, dilates, rect;
Mat foreground;
//可变空间数组
vector<vector<Point>> contours;
vector<Vec4i>hierarchy;
//
double x_max_value, x_min_value = 0, y_max_value, y_min_value = 0;
int rect_x, rect_y, rect_h, rect_w;
//hsv转化数据范围滤波
int vmin = 10, vmax = 256, smin = 30;
Rect selection;
//-------------------------------------//
#define    MIN_TARGET_AREAR     500
#define    MAX_TARGET_AREAR     10000
//-------------------------------------//
enum
{
    TARTGET_FOLLOW = 0,
    SAMPLE_TARGET = 1
};

//函数声明区
int Camshift_cal(void);
void Select_MotionTarget(void);
//-------------------------------------//
//-------------------------------------//

int main()
{
	DemoImage();

	    // areas;
    QApplication a(argc, argv);
    //state;
    static unsigned int state = SAMPLE_TARGET;
    VideoCapture capture(0);
    if (!capture.isOpened())
    {
        return 0;
    }

    namedWindow("Extracted Foreground");
    namedWindow("Source Video");

    BackgroundSubtractorMOG mog;
    bool stop(false);
    while (!stop)
    {
        if (!capture.read(frame))
        {
            break;
        }
        mog(frame, foreground, 0.03);
        //
        switch(state)
        {
            case SAMPLE_TARGET:
                {
                    Select_MotionTarget();
                    //------------------------------------//
                    Rect areas(rect_x, rect_y, rect_h, rect_w);
                    //面积在一定的范围内才进入目标赛选程序
                    if((areas.area() >= MIN_TARGET_AREAR) && (areas.area() <= MAX_TARGET_AREAR))
                    {
                        selection.x = areas.x;
                        selection.y = areas.y;
                        selection.width = areas.width;
                        selection.height = areas.height;
                        state = TARTGET_FOLLOW;
                    }
                    break;
                }

            case TARTGET_FOLLOW:
                {
                    if(Camshift_cal())
                    {
                        state = SAMPLE_TARGET;
                    }
                    break;
                }

        }
        // show foreground
        imshow("Extracted Foreground", foreground);
        imshow("Source Video", frame);
        //然后再画矩形框
        Mat motion = frame.clone();
        rectangle(motion, Point(x_min_value, y_min_value), Point(x_max_value, y_max_value), 180,2,8,0);
        imshow("drawing", motion);
        //------------------------------//
        if (waitKey(10) == 27)
        {
            stop = true;
        }
    }
	return 0;
}


//---------------------------------------//
//
//
//---------------------------------------//
#define  AREAS_MOTION  100  //
enum
{
    SET_TARGET = 0,
    TARGET_CAMSHIFT
};
//跟踪目标消失标志
unsigned int target_flag = 0;
int hsize = 16;
float hranges[] = {0,180};
const float* phranges = hranges;
Rect trackWindow;
//中间变量
Mat hsv, hue, mask, hist, backproj;
int _vmin = vmin, _vmax = vmax;
int ch[] = {0, 0};
//记录跟踪目标的实时位置
vector<Point>Position_Buf;
Point g_cur_position, g_last_position, g_offset_postion;  //记录中心位置坐标
//qt中的定时器
Timer_Motion timer;
//---------------------------------------------//
//
//---------------------------------------------//
int Camshift_cal(void)
{
    static unsigned int state_fsm = SET_TARGET;
    Mat dstImage;      
    //-----------------------------------------//
    //根据选定的区域选择跟踪目标, 将RGB转化为HSV颜色空间
    medianBlur(frame, dstImage, 3);
    GaussianBlur(dstImage, dstImage, Size(3,3), 0,0);
    cvtColor(dstImage, hsv, COLOR_BGR2HSV);//
    //将数据设定在规定的范围内
    inRange(hsv, Scalar(0, smin, MIN(_vmin,_vmax)),
    Scalar(180, 256, MAX(_vmin, _vmax)), mask);
    //
    hue.create(hsv.size(), hsv.depth());
    //取出H通道放入hue中
    mixChannels(&hsv, 1, &hue, 1, ch, 1);

    switch(state_fsm)
    {
        case SET_TARGET:
            {
                target_flag = 0;   
                //----------------------------------//计算直方图
                Mat roi(hue, selection), maskroi(mask, selection);
                //计算直方图，放入hist.
                calcHist(&roi, 1, 0, maskroi, hist, 1, &hsize, &phranges);
                //归一化处理
                normalize(hist, hist, 0, 255, CV_MINMAX);
                //初始化窗口
                trackWindow = selection;

                state_fsm = TARGET_CAMSHIFT;
                break;
            }

        case TARGET_CAMSHIFT :
            {
                //反向投影图,放入backproj, H通道的范围在0~180的范围内
                calcBackProject(&hue, 1, 0, hist, backproj, &phranges);
                backproj &= mask;
                //camshift算法
                RotatedRect trackBox = CamShift(backproj, trackWindow,
                                    TermCriteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 15, 2.0));

                //判断camshift目标追寻不到, 通过返回的矩形框的面积来判断
                ellipse(dstImage, trackBox, Scalar(0,0,255), 3, CV_AA);
                imshow("motion", dstImage);

                //判断跟踪的目标位置是否发生改变，若没有则开始计时
                Select_MotionTarget();
                Rect roud(rect_x, rect_y, rect_h, rect_w);
                //----------------------------------------//
                //该计算只是每帧之间的移动面积差，若不行估计要移动多帧后在计算。
                if(roud.area() <= AREAS_MOTION) //说明物体基本没有移动
                {
                    timer.start_timer(); //定时7S中
                }
                else
                timer.stop_timer();

                if((trackBox.size.area() <= 300) || (timer.return_flag() == 1))
                {
                    target_flag = 1;
                    state_fsm = SET_TARGET;
                    destroyWindow("motion");
                    timer.clr_flag();
                }
                //
                break;
            }
    }
    return target_flag;
}

/*

*/
void Select_MotionTarget(void)
{
    medianBlur(foreground, srcImage, 9);
    GaussianBlur(srcImage, srcImage, Size(5,5), 0,0);
    //二值化处理
    threshold(srcImage, srcImage, 180, 255, THRESH_BINARY);
    //------------------------------//
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
    rect_x = x_min_value; rect_y = y_min_value;
    rect_h = x_max_value - x_min_value;
    rect_w = y_max_value - y_min_value;
    //------------------------------------//
}


//一个Demo图片
void DemoImage(void){
		//读入图片，注意图片路径
	Mat image=imread("girl.jpg");


	//图片读入成功与否判定
	if(!image.data)
	{
		cout<<"you idiot！where did you hide lena！"<<endl;


		//等待按键
		system("pause");
		return ;
	}


	//创建一个名字为“Lena”的图像显示窗口，（不提前声明也可以）
	namedWindow("Lena",1);


	//显示图像
	imshow("Lena",image);


	//等待按键
	waitKey();
}