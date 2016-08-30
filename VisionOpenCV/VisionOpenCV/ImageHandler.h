#pragma once
#include <iostream>
#include <core/core.hpp>
#include <highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv.hpp>
#include <cv.h>

using namespace cv;

class ImageHandler
{
public:
	ImageHandler(void);
	~ImageHandler(void);
	
	//��ɫ׷��
	bool TrackCamShift(Mat souceFrame,Mat foreground);
	//ʶ���˶�����
	void RecognitionMotionTarget(Mat foreground);
	//����ʶ��
	void RecognitionHumanFace(Mat sourceFrame);

	//ͼ�����
	void DemoImage(void);

	enum enumReconStatus
	{
		SET_TARGET = 0,
		TARGET_CAMSHIFT
	};
	Mat foreground;
	Rect selection, moveRange;
	double x_max_value, x_min_value , y_max_value, y_min_value ;

private:
	bool findTargetFlag;
	double currentAngle;
	//�״�ʶ��������Ҫ��Ƶͼ��֡����ͬһ�������ľ�����Χ���ı�Ŀ����Ϊ��֡����ֵ�����¶�λǰ����������֡����
	const int FIRST_FRAME_COUNT, MIN_SIZE_PIXEL, CHANGE_FACE_JUMP_FALG, CHANGE_FACE_MIN_COUNT;
	//ͼ����
	int imageAllWidth, imageTargetX;
	//��֡ͳ��
	int jumpFrameCount;
	int hsize;
	//���������涨ʱ��
	int stayCount, stayMaxCount ;
	//hsvת�����ݷ�Χ�˲�
	int vmin , vmax , smin ;
	float hranges[2] ;
	const float* phranges;
	int ch[2] ;
	//��ǰ׷�ٵ�Ŀ��
	Rect currentTarget,nextTarget;
	//�м����
	Mat hsv, hue, mask, hist, backproj,srcImage,tmpImage,shapeOperateKernal;
	//�ɱ�ռ�����
	vector<vector<Point>> contourAll;
	vector<Vec4i>hierarchy;	
	//����ʶ��
	CascadeClassifier faceCascade;
	vector<vector<Rect>> allFaceLatest;
};

