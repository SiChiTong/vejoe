#include "mainwindow.h"
#include <QApplication>
#include <QObject>
#include "hummanfollow.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include <opencv.hpp>
#include <cv.h>

using namespace cv;
using namespace std;
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
int main(int argc, char* argv[])
{
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
    static unsigned int state_fsm = 0;
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

/*
计算中心点的坐标
*/
void CalCentreCoordinate(void)
{

}
//timer interrupt function.

/*
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

Mat g_srcImage, g_dstImage;

int g_nElementShape = MORPH_RECT;

//
int g_nMaxIterationNum = 10, g_nOpenCloseNum = 0, g_nErodeDilateNum = 0;
int g_nTopBlackHatNum = 0;

static void on_OpenClose(int, void *);
//static void on_ErodeDilate(int, void*);
//static void on_TopBlackHat(int, void *);
//static void ShowHelpText();

int main(int argc, char * argv[])
{
    g_srcImage = imread("home.jpg");

    if(!g_srcImage.data)
        printf("read picture error!");

    namedWindow("src picture");
    imshow("src picture", g_srcImage);

    namedWindow("open calu");

    g_nOpenCloseNum = 9;
    g_nErodeDilateNum = 9;
    g_nTopBlackHatNum = 2;

    createTrackbar("set value", "open calu", &g_nOpenCloseNum, g_nMaxIterationNum *2 +1, on_OpenClose);

    while(1)
    {
        int c;
        on_OpenClose(g_nOpenCloseNum, 0);
        c =waitKey(0);

    }
}

static void on_OpenClose(int, void*)
{
    int offset = g_nOpenCloseNum - g_nMaxIterationNum;
    int Absolute_offset = offset > 0 ? offset : -offset;

    Mat element = getStructuringElement(g_nElementShape, Size(Absolute_offset *2 + 1, Absolute_offset *2 + 1), Point(Absolute_offset, Absolute_offset));

    if(offset < 0)
    {
        morphologyEx(g_srcImage, g_dstImage, CV_MOP_OPEN, element);
    }
    else
        morphologyEx(g_srcImage, g_dstImage, CV_MOP_CLOSE, element);

    imshow("open calu", g_dstImage);
}
*/
/*

*/
/*
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

int main(int argc, char * argv[])
{
    Mat src = imread("home.jpg");

    imshow("yuns", src);

    Rect ccomp;

    floodFill(src, Point(50, 300), Scalar(115,255,55), &ccomp, Scalar(20,20,20), Scalar(20,20,20));
    imshow("xiaoguo", src);
    waitKey(0);
    return 0;
}*/

/*
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

int main(int argc, char *argv[])
{
    Mat srcImage = imread("home.jpg");

    Mat tmpImage, dstImage, sdtImage;

    tmpImage = srcImage;

    imshow("start picture", srcImage);

    resize(tmpImage, dstImage, Size(tmpImage.cols/2, tmpImage.rows/2), (0, 0), (0, 0), 3);
    resize(tmpImage, sdtImage, Size(tmpImage.cols*2, tmpImage.rows*2), (0, 0), (0, 0), 3);

    imshow("xiaoguo tu1", dstImage);
    imshow("xiaoguo tu2", sdtImage);
    waitKey(0);

    return 0;
}*/

/*
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

int main(int argc, char * argv[])
{
    Mat srcImage = imread("home.jpg");

    Mat tmpImage, dstImage;

    tmpImage = srcImage;

    imshow("logo", srcImage);

    pyrUp(tmpImage, dstImage, Size(tmpImage.cols*2, tmpImage.rows*2));
    imshow("pro", dstImage);

    waitKey(0);
    return 0;
}*/

/*
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;

#define WINDOW_NAME "DKS"

int g_nThresholdValue = 100;
int g_nThresholdType = 3;
Mat g_srcImage, g_dstImage, g_grayImage ;

static void ShowHelpText();
void on_Threshold(int, void*);

int main(int argc, char *argv)
{
    g_srcImage = imread("home.jpg");
}

*/
/*
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

int main(int argc, char *argv[])
{
    Mat src = imread("girl.jpg");
    Mat srcl = src.clone();
    Mat srcImage;
    imshow("before canny", src);

    Canny(src, srcImage, 150, 100, 3);

    imshow("after canny", srcImage);

    Mat dst, edge, gray;
    dst.create(srcl.size(), srcl.type());
    cvtColor(srcl, gray, COLOR_BGR2GRAY);

    blur(gray, edge, Size(3,3));
    Canny(edge, edge, 3, 9, 3);

    dst = Scalar::all(0);
    srcl.copyTo(dst, edge);

    imshow("filter canny", dst);
    waitKey(0);
    return 0;
}
*/
/*
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

int main(int argc, char * argv[])
{
    Mat grad_x, grad_y;

    Mat abs_grad_x, abs_grad_y, dst;

    Mat src = imread("girl.jpg");
    imshow("sobel", src);

    Sobel(src, grad_x, CV_16S, 1, 0, 3, 1, 1, BORDER_DEFAULT);

    convertScaleAbs(grad_x, abs_grad_x);
    imshow("x dir sobel", abs_grad_x);

    //y
    Sobel(src, grad_y, CV_16S, 0, 1, 3, 1, 1, BORDER_DEFAULT);
    convertScaleAbs(grad_y, abs_grad_y);
    imshow("y dir sobel", abs_grad_y);
    //
    addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, dst);
    imshow("add", dst);

    waitKey(0);
    return 0;
}*/
/*
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

int main(int argc, char *argv[])
{
    Mat src, src_gray, dst, abs_dst;
    src = imread("girl.jpg");

    imshow("before pro", src);

    GaussianBlur(src, src, Size(5,5), 0, 0, BORDER_DEFAULT);

    cvtColor(src, src_gray, COLOR_RGB2GRAY);

    Laplacian(src_gray, dst, CV_16S, 3, 1, 0, BORDER_DEFAULT);

    convertScaleAbs(dst, abs_dst);

    imshow("after lap", abs_dst);
    waitKey(0);
    return 0;
}*/
/*
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

int main(int argc, char *argv[])
{
    Mat grad_x, grad_y;
    Mat abs_grad_x, abs_grad_y, dst;

    Mat src = imread("girl.jpg");

    imshow("before pro", src);

    Scharr(src, grad_x, CV_16S, 1, 0, 1, 0, BORDER_DEFAULT);

    convertScaleAbs(grad_x, abs_grad_x);
    imshow("x dir", abs_grad_x);

    Scharr(src, grad_y, CV_16S, 0,1,1,0, BORDER_DEFAULT);
    convertScaleAbs(grad_y, abs_grad_y);
    imshow("y dir", abs_grad_y);

    addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, dst);
    imshow("add", dst);
    waitKey(0);
    return 0;
}*/

/*
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

int main(int argc, char *argv[])
{
    Mat srcImage = imread("home.jpg");

    Mat midImage, dstImage;

    Canny(srcImage, midImage, 50, 200, 3);
    cvtColor(midImage, dstImage, CV_GRAY2BGR);

    vector<Vec4i>lines;

    HoughLinesP(midImage, lines, 1, CV_PI/180, 80, 50, 10);

    for(size_t i = 0; i < lines.size(); i ++)
    {

//        float rho = lines[i][0], theta = lines[i][1];
//        Point pt1, pt2;
//        double a = cos(theta), b = sin(theta);
//        double x0 = a*rho, y0 = b *rho;
//        pt1.x = cvRound(x0 + 1000*(-b));
//        pt1.y = cvRound(y0 + 1000 * a);
//        pt2.x = cvRound(x0 - 1000 *(-b));
//        pt2.y = cvRound(y0 - 1000 * a);

//        line(dstImage, pt1, pt2, Scalar(55, 100, 195), 1, CV_AA);

        Vec4i l = lines[i];
        line(dstImage, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(186, 88, 255), 1, CV_AA);
    }
    imshow("before", srcImage);
    imshow("canny", midImage);
    imshow("after", dstImage);
    waitKey(0);
    return 0;
}*/
/*
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

int main(int argc, char* argv[])
{
    Mat srcImage, dstImage;
    Mat map_x, map_y;

    srcImage = imread("girl.jpg");

    imshow("before", srcImage);

    dstImage.create(srcImage.size(), srcImage.type());
    map_x.create(srcImage.size(), CV_32FC1);
    map_y.create(srcImage.size(), CV_32FC1);

    for(int j = 0; j < srcImage.rows; j ++)
    {
        for(int i = 0; i < srcImage.cols; i ++)
        {
            map_x.at<float>(j, i) = static_cast<float>(i);
            map_y.at<float>(j, i) = static_cast<float>(srcImage.rows - j);
        }
    }
    remap(srcImage, dstImage, map_x, map_y, CV_INTER_LINEAR, BORDER_CONSTANT, Scalar(0,0,0));
    imshow("after", dstImage);
    waitKey(0);
    return 0;
}*/

/*
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

int main(int argc, char* argv[])
{
    Mat srcImage, dstImage;

    srcImage = imread("girl.jpg");
    cvtColor(srcImage, srcImage, COLOR_BGR2GRAY);
    imshow("before", srcImage);

    equalizeHist(srcImage, dstImage);
    imshow("after", dstImage);
    waitKey(0);
    return 0;
}
*/

/*
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

int main(int argc, char * argv[])
{
    Mat srcImage = imread("girl.jpg", 0);
    imshow("before", srcImage);

    Mat dstImage = Mat::zeros(srcImage.rows, srcImage.cols, CV_8UC3);

    srcImage = srcImage > 119;
    imshow("after", srcImage);

    //
    vector<vector<Point>>contours;
    vector<Vec4i>hierarchy;

    //
    findContours(srcImage, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
    int index = 0;
    for(; index >= 0; index = hierarchy[index][0])
    {
        Scalar color(rand()&255, rand()&255, rand()&255);
        drawContours(dstImage, contours, index, color, CV_FILLED, 8, hierarchy);

    }
    imshow("luoku", dstImage);
    waitKey(0);
    return 0;
}
*/

/*
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, char *argv[])
{
    Mat image(600, 600, CV_8UC3);
    RNG&rng = theRNG();

    while(1)
    {
        char key;
        int count = (unsigned)rng%100 + 3;
        vector<Point>points;

        for(int i = 0; i < count; i ++)
        {
            Point point;
            point.x = rng.uniform(image.cols/4, image.cols*3/4);
            point.y = rng.uniform(image.rows/4, image.rows*3/4);

            points.push_back(point);

        }

        vector<int> hull;
        convexHull(Mat(points),hull, true);

        image = Scalar::all(0);
        for(int i = 0;  i < count; i ++)
        {
            circle(image, points[i],3,Scalar(rng.uniform(0, 255)), rng.uniform(0,255),
                   rng.uniform(0, 255), FILLED, LINEAA);

        }
    }
}*/
/*
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>

using namespace cv;

int main(int argc, char* argv[])
{
    Mat srcImage, hsvImage;
    srcImage = imread("home.jpg");

    cvtColor(srcImage, hsvImage, COLOR_BGR2HSV);
    int hueBinNum = 30;
    int saturationBinNum = 32;

    int histSize[]={hueBinNum, saturationBinNum};

    float hueRanges[]={0, 180};
    float saturationRanges[]={0, 256};
    const float *ranges[]={hueRanges, saturationRanges};

    MatND dstHist;
    int channels[]={0, 1};

    calcHist(&hsvImage, 1, channels, Mat(), dstHist, 2, histSize, ranges, true, false);

    double maxValue = 0;
    minMaxLoc(dstHist, 0, &maxValue, 0, 0);
    int scale = 10;
    Mat histing = Mat::zeros(saturationBinNum*scale, hueBinNum*10, CV_8UC3);
    for(int hue = 0; hue < hueBinNum; hue ++)
    {
        for(int saturation = 0; saturation < saturationBinNum; saturation ++)
        {
            float binValue = dstHist.at<float>(hue,saturation);
            int intensity = cvRound(binValue*255 / maxValue);
            rectangle(histing, Point(hue*scale, saturation*scale), Point((hue+1)*scale-1, (saturation+1)*scale-1), Scalar::all(intensity), CV_FILLED);
        }
    }
    imshow("before", srcImage);
    imshow("after", dstHist);

    waitKey(0);
    return 0;
}
*/

/*
#include <fstream>
#include <string>
#include <cv.h>
#include <highgui.h>
#include <ml.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "cvaux.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <ctype.h>


using namespace cv;
using namespace std;


int main(int argc, char*argv[])
{
    Mat img0,img1;
    Mat img2;
    //Mat img3;
    int i;
    FILE* f = 0;
    char _filename[1024];

    //imshow("hello",img3);
    //waitKey(0);
    HOGDescriptor hog;
    //CvLatentSvmDetector* detector = cvLoadLatentSvmDetector("result.xml");
    CvSVM svm;
    svm.load("result.xml");
    //hog.load("result.xml");
    //int sv_num= svm.get_support_vector_count();
    hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
    //hog.setSVMDetector(detector);

    Mat img4;
    for(i=46;i<180;i++)
    {
        char file[1024];
        sprintf(file,"D:\\My Documents\\Visual Studio 2008\\Projects\\cc\\CS-%d.bmp",i);
        char file4[1024];
        sprintf(file4,"D:\\My Documents\\Visual Studio 2008\\Projects\\cc\\CS-%d.bmp",i-1);
        Mat img3;
        img3 = imread(file4);
        img4 = imread(file,1);
        //cvSub((CvMat)img2,(CvMat)img3,img);
        //img = img4-img3;
        Mat img(img4.rows,img4.cols,CV_8UC1);
        for(int n=0;n<img4.rows;n++)
        {
            for(int j=0;j<img4.cols;j++)
            {
                short e,d;
                short temp;
                e = *(img4.data+n*img4.step+j*3);
                d = *(img3.data+n*img3.step+j*3);
                //c = *(img2.data+i);
                temp = e-d;
                if(abs(temp)<3)
                temp=0;
                *(img.data+n*img.step+j) = (uchar)e;
            }
        }
        //cv::subtract
        //char* filename = _filename;
        char* filename = file;
        if(0)
        {
            if(!fgets(filename, (int)sizeof(_filename)-2, f))
            break;
            //while(*filename && isspace(*filename))
            // ++filename;
            if(filename[0] == '#')
            continue;
            int l = strlen(filename);
            while(l > 0 && isspace(filename[l-1]))
            --l;
            filename[l] = '\0';
            img = imread(filename);
        }
        printf("%s:\n", filename);
        if(!img.data)
        continue;


        fflush(stdout);
        vector<Rect> found, found_filtered;
        double t = (double)getTickCount();
        // run the detector with default parameters. to get a higher hit-rate
        // (and more false alarms, respectively), decrease the hitThreshold and
        // groupThreshold (set groupThreshold to 0 to turn off the grouping completely).
        int can = img.channels();
        hog.detectMultiScale(img, found, 0, Size(8,8), Size(32,32), 1.05, 2);
        t = (double)getTickCount() - t;
        printf("tdetection time = %gms\n", t*1000./cv::getTickFrequency());
        size_t i, j;
        for( i = 0; i < found.size(); i++ )
        {
            Rect r = found[i];
            for( j = 0; j < found.size(); j++ )
            if( j != i && (r & found[j]) == r)
            break;
            if( j == found.size() )
            found_filtered.push_back(r);
        }
        for( i = 0; i < found_filtered.size(); i++ )
        {
            Rect r = found_filtered[i];
            // the HOG detector returns slightly larger rectangles than the real objects.
            // so we slightly shrink the rectangles to get a nicer output.
            r.x += cvRound(r.width*0.1);
            r.width = cvRound(r.width*1);
            r.y += cvRound(r.height*0.07);
            r.height = cvRound(r.height*0.8);
            rectangle(img, r.tl(), r.br(), cv::Scalar(0,255,0), 3);
        }
        imshow("people detector", img);
        //int c = waitKey(0) & 255;
        //if( c == 'q' || c == 'Q' || !f)
        //break;
        waitKey(1);
        //cvDestoryWindow("people detector");
    }
    if(f)
    fclose(f);
    return 0;
}*/
//对运动物体的跟踪：
//如果背景固定,可用帧差法 然后在计算下连通域 将面积小的去掉即可
//如果背景单一,即你要跟踪的物体颜色和背景色有较大区别 可用基于颜色的跟踪 如CAMSHIFT 鲁棒性都是较好的
//如果背景复杂,如背景中有和前景一样的颜色 就需要用到一些具有预测性的算法 如卡尔曼滤波等 可以和CAMSHIFT结合
/*
#ifdef _CH_
#pragma package <opencv>
#endif

#ifndef _EiC
#include "cv.h"
#include "highgui.h"
#include <stdio.h>
#include <ctype.h>
#endif

IplImage *image = 0, *hsv = 0, *hue = 0, *mask = 0, *backproject = 0, *histimg = 0;
//用HSV中的Hue分量进行跟踪
CvHistogram *hist = 0;
//直方图类
int backproject_mode = 0;
int select_object = 0;
int track_object = 0;
int show_hist = 1;
CvPoint origin;
CvRect selection;
CvRect track_window;
CvBox2D track_box;
//Meanshift跟踪算法返回的Box类
//typedef struct CvBox2D{
//CvPoint2D32f center; // 盒子的中心
//CvSize2D32f size; /// 盒子的长和宽
//float angle; // 水平轴与第一个边的夹角，用弧度表示
//}CvBox2D;
CvConnectedComp track_comp;
//连接部件
//typedef struct CvConnectedComp{
//double area; // 连通域的面积
//float value; // 分割域的灰度缩放值
//CvRect rect; // 分割域的 ROI
//} CvConnectedComp;
int hdims = 16;
//划分直方图bins的个数，越多越精确
float hranges_arr[] = {0,180};
//像素值的范围
float* hranges = hranges_arr;
//用于初始化CvHistogram类
int vmin = 10, vmax = 256, smin = 30;
//用于设置滑动条

void on_mouse( int event, int x, int y, int flags, void* param )
//鼠标回调函数,该函数用鼠标进行跟踪目标的选择
{
    if( !image )
        return;

    if( image->origin )
        y = image->height - y;
    //如果图像原点坐标在左下,则将其改为左上

    if( select_object )
    //select_object为1,表示在用鼠标进行目标选择
    //此时对矩形类selection用当前的鼠标位置进行设置
    {
        selection.x = MIN(x,origin.x);
        selection.y = MIN(y,origin.y);
        selection.width = selection.x + CV_IABS(x - origin.x);
        selection.height = selection.y + CV_IABS(y - origin.y);

        selection.x = MAX( selection.x, 0 );
        selection.y = MAX( selection.y, 0 );
        selection.width = MIN( selection.width, image->width );
        selection.height = MIN( selection.height, image->height );
        selection.width -= selection.x;
        selection.height -= selection.y;
    }

    switch( event )
    {
    case CV_EVENT_LBUTTONDOWN:
        //鼠标按下,开始点击选择跟踪物体
        origin = cvPoint(x,y);
        selection = cvRect(x,y,0,0);
        select_object = 1;
        break;
    case CV_EVENT_LBUTTONUP:
        //鼠标松开,完成选择跟踪物体
        select_object = 0;
        if( selection.width > 0 && selection.height > 0 )
            //如果选择物体有效，则打开跟踪功能
            track_object = -1;
        break;
    }
}


CvScalar hsv2rgb( float hue )
//用于将Hue量转换成RGB量
{
    int rgb[3], p, sector;
    static const int sector_data[][3]=
        {{0,2,1}, {1,2,0}, {1,0,2}, {2,0,1}, {2,1,0}, {0,1,2}};
    hue *= 0.033333333333333333333333333333333f;
    sector = cvFloor(hue);
    p = cvRound(255*(hue - sector));
    p ^= sector & 1 ? 255 : 0;

    rgb[sector_data[sector][0]] = 255;
    rgb[sector_data[sector][1]] = 0;
    rgb[sector_data[sector][2]] = p;

    return cvScalar(rgb[2], rgb[1], rgb[0],0);
}

int main( int argc, char** argv )
{
    CvCapture* capture = 0;

    if( argc == 1 || (argc == 2 && strlen(argv[1]) == 1 && isdigit(argv[1][0])))
        //打开摄像头
        capture = cvCaptureFromCAM( argc == 2 ? argv[1][0] - '0' : 0 );
    else if( argc == 2 )
        //打开avi
        capture = cvCaptureFromAVI( argv[1] );

    if( !capture )
    //打开视频流失败
    {
        fprintf(stderr,"Could not initialize capturing...\n");
        return -1;
    }

    printf( "Hot keys: \n"
        "\tESC - quit the program\n"
        "\tc - stop the tracking\n"
        "\tb - switch to/from backprojection view\n"
        "\th - show/hide object histogram\n"
        "To initialize tracking, select the object with mouse\n" );
    //打印程序功能列表

    cvNamedWindow( "Histogram", 1 );
    //用于显示直方图
    cvNamedWindow( "CamShiftDemo", 1 );
    //用于显示视频
    cvSetMouseCallback( "CamShiftDemo", on_mouse, 0 );
    //设置鼠标回调函数
    cvCreateTrackbar( "Vmin", "CamShiftDemo", &vmin, 256, 0 );
    cvCreateTrackbar( "Vmax", "CamShiftDemo", &vmax, 256, 0 );
    cvCreateTrackbar( "Smin", "CamShiftDemo", &smin, 256, 0 );
    //设置滑动条

    for(;;)
    //进入视频帧处理主循环
    {
        IplImage* frame = 0;
        int i, bin_w, c;

        frame = cvQueryFrame( capture );
        if( !frame )
            break;

        if( !image )
        //image为0,表明刚开始还未对image操作过,先建立一些缓冲区
        {
            image = cvCreateImage( cvGetSize(frame), 8, 3 );
            image->origin = frame->origin;
            hsv = cvCreateImage( cvGetSize(frame), 8, 3 );
            hue = cvCreateImage( cvGetSize(frame), 8, 1 );
            mask = cvCreateImage( cvGetSize(frame), 8, 1 );
            //分配掩膜图像空间
            backproject = cvCreateImage( cvGetSize(frame), 8, 1 );
            //分配反向投影图空间,大小一样,单通道
            hist = cvCreateHist( 1, &hdims, CV_HIST_ARRAY, &hranges, 1 );
            //分配直方图空间
            histimg = cvCreateImage( cvSize(320,200), 8, 3 );
            //分配用于直方图显示的空间
            cvZero( histimg );
            //置背景为黑色
        }

        cvCopy( frame, image, 0 );
        cvCvtColor( image, hsv, CV_BGR2HSV );
        //把图像从RGB表色系转为HSV表色系

        if( track_object )
        //track_object非零,表示有需要跟踪的物体
        {
            int _vmin = vmin, _vmax = vmax;

            cvInRangeS( hsv, cvScalar(0,smin,MIN(_vmin,_vmax),0),
                        cvScalar(180,256,MAX(_vmin,_vmax),0), mask );
            //制作掩膜板，只处理像素值为H：0~180，S：smin~256，V：vmin~vmax之间的部分
            cvSplit( hsv, hue, 0, 0, 0 );
            //分离H分量

            if( track_object < 0 )
            //如果需要跟踪的物体还没有进行属性提取，则进行选取框类的图像属性提取
            {
                float max_val = 0.f;
                cvSetImageROI( hue, selection );
                //设置原选择框为ROI
                cvSetImageROI( mask, selection );
                //设置掩膜板选择框为ROI
                cvCalcHist( &hue, hist, 0, mask );
                //得到选择框内且满足掩膜板内的直方图
                cvGetMinMaxHistValue( hist, 0, &max_val, 0, 0 );
                cvConvertScale( hist->bins, hist->bins, max_val ? 255. / max_val : 0., 0 );
                // 对直方图的数值转为0~255
                cvResetImageROI( hue );
                //去除ROI
                cvResetImageROI( mask );
                //去除ROI
                track_window = selection;
                track_object = 1;
                //置track_object为1,表明属性提取完成
                cvZero( histimg );
                bin_w = histimg->width / hdims;
                for( i = 0; i < hdims; i++ )
                //画直方图到图像空间
                {
                    int val = cvRound( cvGetReal1D(hist->bins,i)*histimg->height/255 );
                    CvScalar color = hsv2rgb(i*180.f/hdims);
                    cvRectangle( histimg, cvPoint(i*bin_w,histimg->height),
                                 cvPoint((i+1)*bin_w,histimg->height - val),
                                 color, -1, 8, 0 );
                }
            }

            cvCalcBackProject( &hue, backproject, hist );
            //计算hue的反向投影图
            cvAnd( backproject, mask, backproject, 0 );
            //得到掩膜内的反向投影
            cvCamShift( backproject, track_window,
                        cvTermCriteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1 ),
                        &track_comp, &track_box );
            //使用MeanShift算法对backproject中的内容进行搜索,返回跟踪结果
            track_window = track_comp.rect;
            //得到跟踪结果的矩形框

            if( backproject_mode )
                cvCvtColor( backproject, image, CV_GRAY2BGR );

            if( image->origin )
                track_box.angle = -track_box.angle;
            cvEllipseBox( image, track_box, CV_RGB(255,0,0), 3, CV_AA, 0 );
            //画出跟踪结果的位置
        }

        if( select_object && selection.width > 0 && selection.height > 0 )
        //如果正处于物体选择，画出选择框
        {
            cvSetImageROI( image, selection );
            cvXorS( image, cvScalarAll(255), image, 0 );
            cvResetImageROI( image );
        }

        cvShowImage( "CamShiftDemo", image );
        cvShowImage( "Histogram", histimg );

        c = cvWaitKey(10);
        if( (char) c == 27 )
            break;
        switch( (char) c )
        //按键切换功能
        {
        case 'b':
            backproject_mode ^= 1;
            break;
        case 'c':
            track_object = 0;
            cvZero( histimg );
            break;
        case 'h':
            show_hist ^= 1;
            if( !show_hist )
                cvDestroyWindow( "Histogram" );
            else
                cvNamedWindow( "Histogram", 1 );
            break;
        default:
            ;
        }
    }

    cvReleaseCapture( &capture );
    cvDestroyWindow("CamShiftDemo");

    return 0;
}*/

/*
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

int main(int argc, char *argv[])
{
    Mat srcImage;
    srcImage = imread("home.jpg", 0);
    imshow("before", srcImage);

    MatND dstHist;  //typedef Mat -> MatND
    int dims = 1;
    float hranges[] = {0, 255};

    const float *ranges[] = {hranges};

    int size = 256;
    int channels = 0;

    calcHist(&srcImage, 1, &channels, Mat(), dstHist, dims, &size, ranges);

    int scale = 1;
    Mat dstImage(size *scale, size, CV_8U, Scalar(0));
    //最大值也最小值
    double minValue = 0;
    double maxValue = 0;
    minMaxLoc(dstHist, &minValue, &maxValue, 0, 0);

    int hpt = saturate_cast<int>(0.9*size);
    for(int i = 0; i < 256; i ++)
    {
        float binValue = dstHist.at<float>(i);
        cout << binValue << endl;
        int realValue = saturate_cast<int>(binValue*hpt/maxValue);
        rectangle (dstImage, Point(i*scale, size - 1), Point((i + 1)* scale,  size - realValue), Scalar(255));

    }
    imshow("after", dstImage);
    waitKey(0);
    return 0;
}
*/
/*
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

int main(int argc, char *argv[])
{
    Mat srcImage_base,  hsvImage_base;
    Mat srcImage_test1, hsvImage_test1;
    Mat srcImage_test2, hsvImage_test2;

    Mat hsvImage_halfDown;

    srcImage_base = imread("home.jpg", 1);
    srcImage_test1 = imread("girl.jpg", 1);
    srcImage_test2 = imread("lou.jpg", 1);
    //显示3张凸显
    imshow("init picture", srcImage_base);
    imshow("test1", srcImage_test1);
    imshow("test2", srcImage_test2);

    cvtColor(srcImage_base, hsvImage_base, COLOR_BGR2HSV);
    cvtColor(srcImage_test1, hsvImage_test1, COLOR_BGR2HSV);
    cvtColor(srcImage_test2, hsvImage_test2, COLOR_BGR2HSV);

    //创建包含基准图像下半部图像
    hsvImage_halfDown = hsvImage_base(Range(hsvImage_base.rows/2, hsvImage_base.rows -1), Range(0, hsvImage_base.cols - 1));
    //初始化计算直方图需要的实参
    int h_bins = 50; int s_bins = 60;
    int histSize[] = {h_bins, s_bins};
    float h_ranges[] = {0, 256};
    float s_ranges[] = {0, 180};
    const float* ranges[] = {h_ranges, s_ranges};

    int channels = {0, 1};

    MatND baseHist, halfDownHist, testHist1, testHist2;


    return 0;
}*/

/*
//反向投影
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

Mat g_srcImage, g_hsvImage, g_hueImage;
int g_bins = 30;

void on_BinChange(int, void*);

//------------------------------//

int main(int argc, char *argv[])
{
    g_srcImage = imread("hand.jpg", 1);

    cvtColor(g_srcImage, g_hsvImage, COLOR_BGR2HSV);

    //
    g_hueImage.create(g_hsvImage.size(), g_hsvImage.depth());
    int ch[] = {0, 0};
    mixChannels(&g_hsvImage, 1, &g_hueImage, 1, ch, 1);
    //创建工具条来调节
    namedWindow("tools bar", WINDOW_AUTOSIZE);
    createTrackbar("color distance", "tools bar", &g_bins, 180, on_BinChange);
    on_BinChange(0, 0);

    //
    imshow("after", g_srcImage);
    waitKey(0);
    return 0;
}

void on_BinChange(int, void *)
{
    MatND hist;
    int histSize = MAX(g_bins, 2);
    float hue_range[] = {0, 180};
    const float* ranges = {hue_range};

    calcHist(&g_hueImage, 1, 0, Mat(), hist, 1, &histSize, &ranges, true, false);
    normalize(hist, hist, 0, 255, NORM_MINMAX, -1, Mat());
    //------------------------------//
    MatND backproj;
    calcBackProject(&g_hueImage, 1, 0, hist, backproj, &ranges, 1, true);
    //显示反向投影图
    imshow("back picture", backproj);
    //
    int w = 400, h = 400;
    int bin_w = cvRound((double)w / histSize);
    Mat histImg = Mat::zeros(w,h,CV_8UC3);

    for(int i = 0; i < g_bins; i ++)
    {
        rectangle(histImg, Point(i*bin_w, h), Point((i+1)*bin_w, h - cvRound(hist.at<float>(i)*h/255.0)), Scalar(100, 123, 255), -1);
    }
    imshow("zhi", histImg);
}*/
/*
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

int main(int argc, char *argv[])
{
    Mat srcImage = imread("girl.jpg", 0);
    imshow("before", srcImage);

    Mat cornerStrength;
    cornerHarris(srcImage, cornerStrength, 2, 3, 0.01);
    imshow("corner", cornerStrength);
    Mat harrisCorner;
    threshold(cornerStrength, harrisCorner, 0.00001, 255, THRESH_BINARY);
    imshow("after", harrisCorner);

    waitKey(0);
    return 0;
}*/
/*
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

//define windows

#define     WINDOW_NAME1  "before"
#define     WINDOW_NAME2  "after pro"

//--------------------------------//
Mat g_srcImage, g_srcImage1, g_grayImage;
int thresh = 30;
int max_thresh = 175;

//函数声明
void on_CornerHarris(int, void *);

int main(int argc, char *argv[])
{
    g_srcImage = imread("home.jpg");
    imshow("src", g_srcImage);

    g_srcImage1 = g_srcImage.clone();

    //保存一张灰度图
    cvtColor(g_srcImage1, g_grayImage, COLOR_BGR2GRAY);
    namedWindow(WINDOW_NAME1, WINDOW_AUTOSIZE);
    //进度条最大值， 改变一次回调函数，变量值
    createTrackbar("set value:", WINDOW_NAME1, &thresh, max_thresh, on_CornerHarris);
    //
    on_CornerHarris(0,0);
    waitKey(0);
    return 0;
}

//------------------------------//
void on_CornerHarris(int, void *)
{
    Mat dstImage, normalImage, scaledImage;

    dstImage = Mat::zeros(g_srcImage.size(), CV_32FC1);
    g_srcImage1 = g_srcImage.clone();

    //进行角点检测
    cornerHarris(g_grayImage, dstImage, 2, 3, 0.04, BORDER_DEFAULT);
    //归一化处理
    normalize(dstImage, normalImage, 0, 255, NORM_MINMAX, CV_32FC1, Mat());
    //转化数据类型
    convertScaleAbs(normalImage, scaledImage);

    for(int j = 0; j < normalImage.rows; j ++)
    {
        for(int i = 0; i < normalImage.cols; i ++)
        {
            if((int)normalImage.at<float>(j,i) > thresh + 80)
            {
                circle(g_srcImage1, Point(i,j), 5,Scalar(10,10,255), 2, 8,0);
                circle(scaledImage, Point(i,j), 5,Scalar(0,10,255), 2,8,0);
            }
        }
    }
    imshow(WINDOW_NAME1, g_srcImage1);
    imshow(WINDOW_NAME2, scaledImage);
}
*/
/*
#include <stdio.h>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

int main( int argc, char** argv )
{
  //声明IplImage指针
  IplImage* pFrame = NULL;
  IplImage* pFrImg = NULL;
  IplImage* pBkImg = NULL;

  CvMat* pFrameMat = NULL;
  CvMat* pFrMat = NULL;
  CvMat* pBkMat = NULL;

  CvCapture* pCapture = NULL;

  int nFrmNum = 0;

  //创建窗口
  cvNamedWindow("video", 1);
  cvNamedWindow("background",1);
  cvNamedWindow("foreground",1);
  //使窗口有序排列
  cvMoveWindow("video", 30, 0);
  cvMoveWindow("background", 360, 0);
  cvMoveWindow("foreground", 690, 0);



  if( argc > 2 )
    {
      fprintf(stderr, "Usage: bkgrd [video_file_name]\n");
      return -1;
    }

  //打开摄像头
  if (argc ==1)
    if( !(pCapture = cvCaptureFromCAM(-1)))
      {
    fprintf(stderr, "Can not open camera.\n");
    return -2;
      }

  //打开视频文件
  if(argc == 2)
    if( !(pCapture = cvCaptureFromFile(argv[1])))
      {
    fprintf(stderr, "Can not open video file %s\n", argv[1]);
    return -2;
      }

  //逐帧读取视频
  while(pFrame = cvQueryFrame( pCapture ))
    {
      nFrmNum++;

      //如果是第一帧，需要申请内存，并初始化
      if(nFrmNum == 1)
    {
      pBkImg = cvCreateImage(cvSize(pFrame->width, pFrame->height),  IPL_DEPTH_8U,1);
      pFrImg = cvCreateImage(cvSize(pFrame->width, pFrame->height),  IPL_DEPTH_8U,1);

      pBkMat = cvCreateMat(pFrame->height, pFrame->width, CV_32FC1);
      pFrMat = cvCreateMat(pFrame->height, pFrame->width, CV_32FC1);
      pFrameMat = cvCreateMat(pFrame->height, pFrame->width, CV_32FC1);

      //转化成单通道图像再处理
      cvCvtColor(pFrame, pBkImg, CV_BGR2GRAY);
      cvCvtColor(pFrame, pFrImg, CV_BGR2GRAY);

      cvConvert(pFrImg, pFrameMat);
      cvConvert(pFrImg, pFrMat);
      cvConvert(pFrImg, pBkMat);
    }
      else
    {
      cvCvtColor(pFrame, pFrImg, CV_BGR2GRAY);
      cvConvert(pFrImg, pFrameMat);
      //高斯滤波先，以平滑图像
      //cvSmooth(pFrameMat, pFrameMat, CV_GAUSSIAN, 3, 0, 0);

      //当前帧跟背景图相减
      cvAbsDiff(pFrameMat, pBkMat, pFrMat);

      //二值化前景图
      cvThreshold(pFrMat, pFrImg, 60, 255.0, CV_THRESH_BINARY);

      //进行形态学滤波，去掉噪音
      //cvErode(pFrImg, pFrImg, 0, 1);
      //cvDilate(pFrImg, pFrImg, 0, 1);

      //更新背景
      cvRunningAvg(pFrameMat, pBkMat, 0.003, 0);
      //将背景转化为图像格式，用以显示
      cvConvert(pBkMat, pBkImg);

      //显示图像
      cvShowImage("video", pFrame);
      cvShowImage("background", pBkImg);
      cvShowImage("foreground", pFrImg);

      //如果有按键事件，则跳出循环
      //此等待也为cvShowImage函数提供时间完成显示
      //等待时间可以根据CPU速度调整
      if( cvWaitKey(2) >= 0 )
        break;


    }

    }




  //销毁窗口
  cvDestroyWindow("video");
  cvDestroyWindow("background");
  cvDestroyWindow("foreground");

  //释放图像和矩阵
  cvReleaseImage(&pFrImg);
  cvReleaseImage(&pBkImg);

  cvReleaseMat(&pFrameMat);
  cvReleaseMat(&pFrMat);
  cvReleaseMat(&pBkMat);

  cvReleaseCapture(&pCapture);

  return 0;
}*/

/*
#include <cv.h>
#include <highgui.h>
#include <iostream>

using namespace cv;
using namespace std;

void intial(Mat src);
void accbackgound(Mat src,Mat pre);
void backgound(int count);
void foregound(Mat src,Mat pre);
void skin(Mat src);

Mat bg,Th,mask0;
Mat bglow0,bglow1,bglow2;
Mat bghigh0,bghigh1,bghigh2;
Mat mask;
int high=15,low=15;

int main(int argc, char *argv[])
{
    int count=0;
    VideoCapture capture;
    capture.open(0);
    Mat fram,prefram,result,fg;
    int framNum=0;


    while(capture.isOpened())
    {
        capture>>fram;

        fram.convertTo(fram,CV_32FC3);
        normalize(fram,fram,1,0,CV_MINMAX);
        imshow("src",fram);

        if(framNum==0)
        {
            intial(fram);
        }
        else if(framNum<30)
        {
            ++count;
            accbackgound(fram,prefram);
        }
        else if(framNum==30)
            backgound(count);
        else
        {
            foregound(fram,prefram);
            skin(fram);
        }
        fram.copyTo(prefram);
        framNum++;

        char key=(char)waitKey(2);
        switch(key)
        {
        case 27:
            return 0;
            break;

        }
    }
    return 0;
}

void intial(Mat src)
{
    src.copyTo(bg);
}

void accbackgound(Mat src,Mat pre)
{
    Mat temp;
    accumulate(src,bg);
    absdiff(src,pre,temp);

    if (Th.data==NULL)
    {
        temp.copyTo(Th);
    }
    else
        accumulate(temp,Th);
}

void backgound(int count)
{
    bg=bg/count;
    Th=Th/count;

    normalize(bg,bg,1,0,CV_MINMAX);
    imshow("backgound",bg);

    Mat t[3];
    Mat b[3];
    split(Th,t);
    split(bg,b);
    //-------------这几步操作不懂-------------//
    bglow0=b[0]-t[0]*low;  //可不可理解为在手的颜色范围之内
    bglow1=b[1]-t[1]*low;
    bglow2=b[2]-t[2]*low;
    bghigh0=b[0]+t[0]*high;
    bghigh1=b[1]+t[1]*high;
    bghigh2=b[2]+t[2]*high;
    //--------------------------------------//
    cout<<"Start Traclking"<<endl;
}

void foregound(Mat src,Mat pre)
{
    Mat temp0,temp1,temp2;
    Mat framNow[3];
    Mat frampre[3];
    framNow[0].setTo(Scalar(0,0,0));
    framNow[1].setTo(Scalar(0,0,0));
    framNow[2].setTo(Scalar(0,0,0));
    temp0.setTo(Scalar(0,0,0));
    temp1.setTo(Scalar(0,0,0));
    temp2.setTo(Scalar(0,0,0));

//    split(pre,frampre);
//    accumulateWeighted(frampre[0],bglow0,0.1);
//    accumulateWeighted(frampre[0],bghigh0,0.1);
//    accumulateWeighted(frampre[1],bglow1,0.1);
//    accumulateWeighted(frampre[1],bghigh1,0.1);
//    accumulateWeighted(frampre[2],bglow2,0.1);
//    accumulateWeighted(frampre[2],bglow2,0.1);

    split(src,framNow);
    inRange(framNow[0],bglow0,bghigh0,temp0);
    inRange(framNow[1],bglow1,bghigh1,temp1);
    inRange(framNow[2],bglow2,bghigh2,temp2);
    bitwise_or(temp0,temp1,temp0);
    bitwise_or(temp0,temp2,temp0);
    bitwise_not(temp0,temp0);

    imshow("Show",temp0);
    temp0.copyTo(mask0);
}

void skin(Mat src)
{
    src.convertTo(src,CV_8UC3,255);
    Mat yuv,dst;
    cvtColor(src,yuv,CV_BGR2YCrCb);
    Mat dstTemp1(src.rows, src.cols, CV_8UC1);
    Mat dstTemp2(src.rows, src.cols, CV_8UC1);
    // 对YUV空间进行量化，得到2值图像，亮的部分为手的形状
    inRange(yuv, Scalar(0,133,0), Scalar(256,173,256), dstTemp1);
    inRange(yuv, Scalar(0,0,77), Scalar(256,256,127), dstTemp2);
    bitwise_and(dstTemp1, dstTemp2, mask);
    dst.setTo(Scalar::all(0));

    bitwise_and(mask,mask0,mask);
    src.copyTo(dst,mask);

    vector< vector<Point> > contours;   // 轮廓
    vector< vector<Point> > filterContours; // 筛选后的轮廓
    vector< Vec4i > hierarchy;    // 轮廓的结构信息
    vector< Point > hull; // 凸包络的点集
    contours.clear();
    hierarchy.clear();
    filterContours.clear();

    // 得到手的轮廓
    findContours(mask, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
            // 去除伪轮廓
    for (size_t i = 0; i < contours.size(); i++)
    {
        //approxPolyDP(Mat(contours[i]), Mat(approxContours[i]), arcLength(Mat(contours[i]), true)*0.02, true);
        if (fabs(contourArea(Mat(contours[i]))) > 1000&&fabs(arcLength(Mat(contours[i]),true))<2000)  //判断手进入区域的阈值
        {
            filterContours.push_back(contours[i]);
        }
    }
    // 画轮廓
    drawContours(src, filterContours, -1, Scalar(0,0,255), 2); //8, hierarchy);
    imshow("traclking",src);
}
*/
