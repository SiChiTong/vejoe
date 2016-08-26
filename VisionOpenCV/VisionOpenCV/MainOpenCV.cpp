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
//���Hͨ������
vector <Mat> channels, back_channels;
//------------------------------------//
Mat grad_x, grad_y, grad_xy, grad_abx, grad_aby;
Mat frame,srcImage, element, dilates, rect;
Mat foreground;
//�ɱ�ռ�����
vector<vector<Point>> contours;
vector<Vec4i>hierarchy;
//
double x_max_value, x_min_value = 0, y_max_value, y_min_value = 0;
int rect_x, rect_y, rect_h, rect_w;
//hsvת�����ݷ�Χ�˲�
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

//����������
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
                    //�����һ���ķ�Χ�ڲŽ���Ŀ����ѡ����
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
        //Ȼ���ٻ����ο�
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
//����Ŀ����ʧ��־
unsigned int target_flag = 0;
int hsize = 16;
float hranges[] = {0,180};
const float* phranges = hranges;
Rect trackWindow;
//�м����
Mat hsv, hue, mask, hist, backproj;
int _vmin = vmin, _vmax = vmax;
int ch[] = {0, 0};
//��¼����Ŀ���ʵʱλ��
vector<Point>Position_Buf;
Point g_cur_position, g_last_position, g_offset_postion;  //��¼����λ������
//qt�еĶ�ʱ��
Timer_Motion timer;
//---------------------------------------------//
//
//---------------------------------------------//
int Camshift_cal(void)
{
    static unsigned int state_fsm = SET_TARGET;
    Mat dstImage;      
    //-----------------------------------------//
    //����ѡ��������ѡ�����Ŀ��, ��RGBת��ΪHSV��ɫ�ռ�
    medianBlur(frame, dstImage, 3);
    GaussianBlur(dstImage, dstImage, Size(3,3), 0,0);
    cvtColor(dstImage, hsv, COLOR_BGR2HSV);//
    //�������趨�ڹ涨�ķ�Χ��
    inRange(hsv, Scalar(0, smin, MIN(_vmin,_vmax)),
    Scalar(180, 256, MAX(_vmin, _vmax)), mask);
    //
    hue.create(hsv.size(), hsv.depth());
    //ȡ��Hͨ������hue��
    mixChannels(&hsv, 1, &hue, 1, ch, 1);

    switch(state_fsm)
    {
        case SET_TARGET:
            {
                target_flag = 0;   
                //----------------------------------//����ֱ��ͼ
                Mat roi(hue, selection), maskroi(mask, selection);
                //����ֱ��ͼ������hist.
                calcHist(&roi, 1, 0, maskroi, hist, 1, &hsize, &phranges);
                //��һ������
                normalize(hist, hist, 0, 255, CV_MINMAX);
                //��ʼ������
                trackWindow = selection;

                state_fsm = TARGET_CAMSHIFT;
                break;
            }

        case TARGET_CAMSHIFT :
            {
                //����ͶӰͼ,����backproj, Hͨ���ķ�Χ��0~180�ķ�Χ��
                calcBackProject(&hue, 1, 0, hist, backproj, &phranges);
                backproj &= mask;
                //camshift�㷨
                RotatedRect trackBox = CamShift(backproj, trackWindow,
                                    TermCriteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 15, 2.0));

                //�ж�camshiftĿ��׷Ѱ����, ͨ�����صľ��ο��������ж�
                ellipse(dstImage, trackBox, Scalar(0,0,255), 3, CV_AA);
                imshow("motion", dstImage);

                //�жϸ��ٵ�Ŀ��λ���Ƿ����ı䣬��û����ʼ��ʱ
                Select_MotionTarget();
                Rect roud(rect_x, rect_y, rect_h, rect_w);
                //----------------------------------------//
                //�ü���ֻ��ÿ֮֡����ƶ����������й���Ҫ�ƶ���֡���ڼ��㡣
                if(roud.area() <= AREAS_MOTION) //˵���������û���ƶ�
                {
                    timer.start_timer(); //��ʱ7S��
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
    //��ֵ������
    threshold(srcImage, srcImage, 180, 255, THRESH_BINARY);
    //------------------------------//
    Canny(srcImage, srcImage, 3, 9, 3);
    //�ҵ���������
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
    //�ҵ����ֵ,��Сֵ
    minMaxLoc(array_x, &x_min_value, &x_max_value, 0, 0);
    minMaxLoc(array_y, &y_min_value, &y_max_value, 0, 0);
    //�������
    rect_x = x_min_value; rect_y = y_min_value;
    rect_h = x_max_value - x_min_value;
    rect_w = y_max_value - y_min_value;
    //------------------------------------//
}


//һ��DemoͼƬ
void DemoImage(void){
		//����ͼƬ��ע��ͼƬ·��
	Mat image=imread("girl.jpg");


	//ͼƬ����ɹ�����ж�
	if(!image.data)
	{
		cout<<"you idiot��where did you hide lena��"<<endl;


		//�ȴ�����
		system("pause");
		return ;
	}


	//����һ������Ϊ��Lena����ͼ����ʾ���ڣ�������ǰ����Ҳ���ԣ�
	namedWindow("Lena",1);


	//��ʾͼ��
	imshow("Lena",image);


	//�ȴ�����
	waitKey();
}