#include "ImageHandler.h"


using namespace std;

#define  AREAS_MOTION  100  //
#define    MIN_TARGET_AREAR     500
#define    MAX_TARGET_AREAR     10000


ImageHandler::ImageHandler(void):FACE_COUNT(10),MIN_SIZE_PIXEL(10)
{
	vmin = 10;
	vmax = 256;
	smin = 30;
	x_min_value = 0;
	y_min_value = 0;

	findTargetFlag = false;

	hsize = 16;
	hranges[0]=0;
	hranges[1]=180;
	phranges = hranges;
	stayCount = 0;
	stayMaxCount = 100000;
	ch[0]=0;
	ch[1] =0;


	shapeOperateKernal = getStructuringElement(MORPH_RECT, Size(5, 5));
	string faceCascadeName = "haarcascade_frontalface_default.xml";
	if(!faceCascade.load(faceCascadeName)){printf("--(!)Error loading\n");}
}


ImageHandler::~ImageHandler(void)
{
}

//ʹ��CamShift�㷨�����˶������
bool ImageHandler::TrackCamShift(Mat souceFrame,Mat foreground)
{
	//����Ŀ����ʧ��־
	bool targetExistsFlag = false;
	static enumReconStatus state_fsm = SET_TARGET;
	Mat dstImage;
	//ʹ����ֵ�˲�������ģ��������ƽ������������ֵ�˲���ͼ���ÿ������������ (�Ե�ǰ����Ϊ���ĵ�����������)���ص���ֵ����
	medianBlur(souceFrame, dstImage, 3);
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
			RecognitionMotionTarget(foreground);
			int areaObject = moveRange.height * moveRange.width;
			if(areaObject >= MIN_TARGET_AREAR && areaObject <= MAX_TARGET_AREAR)
			{
				selection = moveRange;
				state_fsm = TARGET_CAMSHIFT;
			}

			//����ֱ��ͼ
			Mat roi(hue, selection), maskroi(mask, selection);
			//����ֱ��ͼ������hist.
			calcHist(&roi, 1, 0, maskroi, hist, 1, &hsize, &phranges);
			//��һ������
			normalize(hist, hist, 0, 255, CV_MINMAX);

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
			imshow("�˶����󲶻�", dstImage);
			moveWindow("�˶����󲶻�",700,500);

			//�жϸ��ٵ�Ŀ��λ���Ƿ����ı䣬��û����ʼ��ʱ
			RecognitionMotionTarget(foreground);
			//�ü���ֻ��ÿ֮֡����ƶ����������й���Ҫ�ƶ���֡���ڼ��㡣
			if(moveRange.height * moveRange.width <= AREAS_MOTION) //˵���������û���ƶ�
				stayCount++; 
			else
				stayCount=0;

			if(trackBox.size.area() <= 300 || stayCount >= stayMaxCount)
			{
				targetExistsFlag = true;
				state_fsm = SET_TARGET;
				stayCount=0;
			}
			break;
		}
	}
	return targetExistsFlag;
}

//ȷ���˶�����
void ImageHandler::RecognitionMotionTarget(Mat foreground)
{
	//���ղ���
	morphologyEx(foreground,tmpImage,MORPH_OPEN,shapeOperateKernal);
	morphologyEx(tmpImage,srcImage,MORPH_CLOSE,shapeOperateKernal);
	
	//��ȡ�߽�
	//Canny(srcImage, srcImage, 50, 150, 3);	
	//�ҵ���������
	findContours(srcImage, contourAll, hierarchy, RETR_EXTERNAL , CHAIN_APPROX_SIMPLE);
	int shapeCount = contourAll.size();
	vector<vector<Point>>contoursAppr(shapeCount);
	vector<Rect> boundRect(shapeCount);
	vector<int>array_x(shapeCount), array_y(shapeCount);
	//�ҵ������ͨ��
	for(int i = 0; i < shapeCount; i ++)
	{//�ҵ���������
		approxPolyDP(Mat(contourAll[i]), contoursAppr[i], 5, true);
		boundRect[i] = boundingRect(Mat(contoursAppr[i]));
		array_x[i] = boundRect[i].x;
		array_y[i] = boundRect[i].y;
		//���ն�
		drawContours(srcImage,contourAll,i,Scalar(255), CV_FILLED);
	}	
	imshow("�˶�����", srcImage);
	moveWindow("�˶�����",700,0);
	//�ҵ����ֵ,��Сֵ
	minMaxLoc(array_x, &x_min_value, &x_max_value, 0, 0);
	minMaxLoc(array_y, &y_min_value, &y_max_value, 0, 0);
	//�������
	moveRange.x = (int)x_min_value; 
	moveRange.y = (int)y_min_value;
	moveRange.height = (int)x_max_value - (int)x_min_value;
	moveRange.width = (int)y_max_value - (int)y_min_value;
}

int findMostSimilarRect(Rect target, vector<Rect> selectList);
//����ʶ��
void ImageHandler::RecognitionHumanFace(Mat sourceFrame){
	
	vector<Rect> faces;
	Mat faceGray;
	//�Ҷȴ�������ɫͼ���Ϊ�ڰף�
	cvtColor(sourceFrame, faceGray, CV_RGB2GRAY);
	//�Ҷ�ͼ��ֱ��ͼ���⻯����һ��ͼ�����Ⱥ���ǿ�Աȶȣ�
	equalizeHist( faceGray, faceGray );
	//����ʶ��
	faceCascade.detectMultiScale(faceGray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30,30));
	if(!findTargetFlag)
	{//�״�ʶ��
		if(allFaceLatest.size() < FACE_COUNT)
		{//�״βɼ�
			allFaceLatest.push_back(faces);
		}
		if(allFaceLatest.size() >= FACE_COUNT)
		{//�״�ʶ������� -> �����
			findTargetFlag = true;
		}
		return;
	}
	int similarIdx=findMostSimilarRect(currentTarget,faces );
	currentTarget = faces[similarIdx];
	//ͼ�����
	Point center(currentTarget.x +currentTarget.width/2,currentTarget.y + currentTarget.height/2 );
	ellipse(sourceFrame,center,Size(currentTarget.width/2,currentTarget.height/2),0,0,360,Scalar( 255, 0, 255 ), 2, 8, 0 );	
	imshow("����ʶ��",sourceFrame);
	moveWindow("����ʶ��",0,500);
}

//��������Ŀ��
int findMostSimilarRect(Rect target, vector<Rect> selectList)
{
	int faceCount = selectList.size(),similarIdx=0, mostSimilar =0xFFFFFF, tmpSimilar;
	for(int i=1;i<faceCount;i++)
	{//�ҵ�����������
		//�����Ƽ��㹫ʽ����Сֵ��������� + �������ƽ��
		tmpSimilar = abs(selectList[0].width * selectList[0].height - target.height*target.width) + (int)abs(pow(selectList[0].x,2) + pow(selectList[0].y,2) -pow(target.x,2)-pow(target.y,2));
		if(tmpSimilar < mostSimilar)
		{
			similarIdx = i;
			mostSimilar = tmpSimilar;
		}
	}
	return similarIdx;
}

//һ��DemoͼƬ
void ImageHandler::DemoImage(void){
	//����ͼƬ��ע��ͼƬ·��
	Mat image=imread("girl.jpg");

	//ͼƬ����ɹ�����ж�
	if(!image.data)		return ;

	//��ʾͼ��
	imshow("image1",image);
	
	//�ȴ�����
	waitKey();
}