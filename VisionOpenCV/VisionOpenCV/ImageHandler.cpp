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

//ʹ��CamShift�㷨�����˶������
bool ImageHandler::RecognitionCamShift(Mat frame)
{
	//����Ŀ����ʧ��־
	bool targetExistsFlag = false;
	static enumReconStatus state_fsm = SET_TARGET;
	Mat dstImage;
	//ʹ����ֵ�˲�������ģ��������ƽ����������ֵ�˲���ͼ���ÿ������������ (�Ե�ǰ����Ϊ���ĵ�����������)���ص���ֵ����
	medianBlur(frame, dstImage, 3);
	//��˹�˲�����������˲����ֵ�Ǹ������������أ������Լ��Ǹ��㣩��һ���˲�ģ��������
	GaussianBlur(dstImage, dstImage, Size(3,3), 0,0);
	//����ɫͼ��ת��ΪHSV��ʽ�����浽hsv��
	cvtColor(dstImage, hsv, COLOR_BGR2HSV);
	//�������趨�ڹ涨�ķ�Χ��
	inRange(hsv, Scalar(0, smin, MIN(vmin,vmax)),
		Scalar(180, 256, MAX(vmin, vmax)), mask);
	//
	hue.create(hsv.size(), hsv.depth());
	//ȡ��Hͨ������hue��
	mixChannels(&hsv, 1, &hue, 1, ch, 1);

	switch(state_fsm)
	{
	case SET_TARGET:
		{
			targetExistsFlag = false;   
			//����ֱ��ͼ
			Mat roi(hue, selection), maskroi(mask, selection);
			//����ֱ��ͼ������hist.
			calcHist(&roi, 1, 0, maskroi, hist, 1, &hsize, &phranges);
			//��һ������
			normalize(hist, hist, 0, 255, CV_MINMAX);

			state_fsm = TARGET_CAMSHIFT;
			break;
		}

	case TARGET_CAMSHIFT :
		{
			//����ͶӰͼ,����backproj, Hͨ���ķ�Χ��0~180�ķ�Χ��
			calcBackProject(&hue, 1, 0, hist, backproj, &phranges);
			backproj &= mask;
			//camshift�㷨
			RotatedRect trackBox = CamShift(backproj, selection,TermCriteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 15, 2.0));

			//�ж�camshiftĿ��׷Ѱ����, ͨ�����صľ��ο��������ж�
			ellipse(dstImage, trackBox, Scalar(0,0,255), 3, CV_AA);
			imshow("�˶����󲹻�", dstImage);

			//�жϸ��ٵ�Ŀ��λ���Ƿ����ı䣬��û����ʼ��ʱ
			SelectMotionTarget();
			//�ü���ֻ��ÿ֮֡����ƶ����������й���Ҫ�ƶ���֡���ڼ��㡣
			if(moveRange.height * moveRange.width <= AREAS_MOTION) //˵���������û���ƶ�
				stayCount++; 
			else
				stayCount=0;

			if(trackBox.size.area() <= 300 || stayCount >= stayMaxCount)
			{
				targetExistsFlag = true;
				state_fsm = SET_TARGET;
				destroyWindow("�˶����󲹻�");
				stayCount=0;
			}
			break;
		}
	}
	return targetExistsFlag;
}

//ȷ���˶�����
void ImageHandler::SelectMotionTarget(void)
{
	medianBlur(foreground, srcImage, 9);
	GaussianBlur(srcImage, srcImage, Size(5,5), 0,0);
	//��ֵ������
	threshold(srcImage, srcImage, 180, 255, THRESH_BINARY);
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
	moveRange.x = (int)x_min_value; 
	moveRange.y = (int)y_min_value;
	moveRange.height = (int)x_max_value - (int)x_min_value;
	moveRange.width = (int)y_max_value - (int)y_min_value;
}

//һ��DemoͼƬ
void ImageHandler::DemoImage(void){
	//����ͼƬ��ע��ͼƬ·��
	Mat image=imread("girl.jpg");
	
	//ͼƬ����ɹ�����ж�
	if(!image.data)		return ;
	
	//����һ������Ϊ��Lena����ͼ����ʾ���ڣ�������ǰ����Ҳ���ԣ�
	namedWindow("Lena",1);
	
	//��ʾͼ��
	imshow("Lena",image);
	
	//�ȴ�����
	waitKey();
}