#include "ImageHandler.h"
#include <iostream>
#include <numeric>
#include <fstream>

using namespace std;

#define  AREAS_MOTION	100
#define  MIN_TARGET_AREAR		500
#define  MAX_TARGET_AREAR		10000 
#define	 DEMO_RESULT_RADIUS		500 // ָ�� �뾶
#define  FILTER_MIDDLE_COUNT	5	// ��ֵ�˲�������
#define	 FILTER_MEAN_COUNT		3	// ��ֵ�˲�������


Scalar ImageHandler::colorDemoResult;
Point ImageHandler::camPosDemoResult, ImageHandler::objPosDemoResult;


ofstream fOrigin("data.txt");
ofstream fMid("inData.txt");

ImageHandler::ImageHandler(void):FIRST_FRAME_COUNT(10),MIN_SIZE_PIXEL(10),CHANGE_FACE_JUMP_FALG(200), CHANGE_FACE_MIN_COUNT(5),MIN_RECT_AREA(200)
{
	vmin = 10;
	vmax = 256;
	smin = 30;

	findTargetFlag = false;
	jumpFrameCount=0;
	hsize = 16;
	hranges[0]=0;
	hranges[1]=180;
	phranges = hranges;
	stayCount = 0;
	stayMaxCount = 100;
	ch[0]=0;
	ch[1] =0;

	camPosDemoResult = Point(DEMO_RESULT_RADIUS/2,0);
	objPosDemoResult = Point(DEMO_RESULT_RADIUS/2,DEMO_RESULT_RADIUS/2);
	colorDemoResult = Scalar(255,255,255);

	shapeOperateKernal = getStructuringElement(MORPH_RECT, Size(5, 5));
	string faceCascadeName = "haarcascade_frontalface_alt.xml";
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
	//ʹ����ֵ�˲�������ģ��������ƽ����������ֵ�˲���ͼ���ÿ������������ (�Ե�ǰ����Ϊ���ĵ�����������)���ص���ֵ����
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
			//imshow("Move Obj", dstImage);
			//moveWindow("Move Obj",700,500);

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

//�������˶����壬ֹͣ�˶�ʱ��������˶�����ɫ��׷��
int ImageHandler::TrackMotionTarget(Mat souceFrame,Mat foreground)
{
	//����˶�Ŀ��
	RecognitionMotionTarget(foreground);
	if(moveRange.area() < MIN_RECT_AREA) return -1;//�˶�����̫С�����
	//Mat dstImage;
	////ʹ����ֵ�˲�������ģ��������ƽ����������ֵ�˲���ͼ���ÿ������������ (�Ե�ǰ����Ϊ���ĵ�����������)���ص���ֵ����
	//medianBlur(souceFrame, dstImage, 3);
	////��˹�˲�����������˲����ֵ�Ǹ������������أ������Լ��Ǹ��㣩��һ���˲�ģ��������
	//GaussianBlur(dstImage, dstImage, Size(3,3), 0,0);
	////����ɫͼ��ת��ΪHSV��ʽ�����浽hsv��
	//cvtColor(dstImage, hsv, COLOR_BGR2HSV);
	////�������趨�ڹ涨�ķ�Χ��
	//inRange(hsv, Scalar(0, smin, MIN(vmin,vmax)),Scalar(180, 256, MAX(vmin, vmax)), mask);
	////ȡ�� Hͨ������hue��
	//hue.create(hsv.size(), hsv.depth());
	//mixChannels(&hsv, 1, &hue, 1, ch, 1);
	////����ֱ��ͼ
	//Mat roi(hue, moveRange), maskroi(mask, moveRange);
	////����ֱ��ͼ������hist.
	//calcHist(&roi, 1, 0, maskroi, hist, 1, &hsize, &phranges);
	////��һ������
	//normalize(hist, hist, 0, 255, CV_MINMAX);
	////����ͶӰͼ,����backproj, Hͨ���ķ�Χ��0~180�ķ�Χ��
	//calcBackProject(&hue, 1, 0, hist, backproj, &phranges);
	//backproj &= mask;
	////camshift�㷨
	//trackBox = CamShift(backproj, moveRange,TermCriteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 15, 2.0));
	//double xValue = trackBox.center.x;	

	//��ֵ + ��ֵ ����
	double xValue = moveRange.x + moveRange.width/2.0;
	fOrigin << xValue << " ";	
	cout <<xValue<< "========================" ; 
	midFiltArray.push_back(xValue);
	sourceFiltArray.push_back(xValue);
	if(midFiltArray.size() < FILTER_MIDDLE_COUNT) return -1;
	sort(midFiltArray.begin(), midFiltArray.end());
	meanFiltArray.push_back(midFiltArray[FILTER_MIDDLE_COUNT/2]);
	if(meanFiltArray.size() > FILTER_MEAN_COUNT)
	{
		meanFiltArray.erase(meanFiltArray.begin ());
	}
	midFiltArray.erase(std::find(midFiltArray.begin(),midFiltArray.end(),sourceFiltArray[0]));
	sourceFiltArray.erase(sourceFiltArray.begin());
	xValue =std::accumulate(std::begin(meanFiltArray),end(meanFiltArray),0)/meanFiltArray.size(); 
	
	fMid << xValue << " ";
	cout<< xValue<<endl;

	return xValue;

	//��֡ ������
	//if(!findTargetFlag){//�״�û�ö�֡ȡ��ֵ
	//	nextTargetRotate = trackBox;
	//	findTargetFlag = true;
	//	return nextTarget.x;
	//}
	//if(abs(trackBox.center.x - nextTargetRotate.center.x) > CHANGE_FACE_JUMP_FALG || abs(trackBox.center.y - nextTargetRotate.center.y) > CHANGE_FACE_JUMP_FALG || 
	//	abs(trackBox.size.width - nextTargetRotate.size.width) > CHANGE_FACE_JUMP_FALG || abs(trackBox.size.height - nextTargetRotate.size.height) > CHANGE_FACE_JUMP_FALG)
	//{//��֡���
	//	jumpFrameCount++;
	//	if(jumpFrameCount >= CHANGE_FACE_MIN_COUNT)//û����֡����ֻ��ȡ�����һ֡
	//		nextTargetRotate = trackBox;
	//}else{
	//	jumpFrameCount = 0;
	//	nextTargetRotate = trackBox;
	//}
	////ellipse(dstImage,nextTargetRotate, Scalar(0,0,255), 3, CV_AA);
	////imshow("Move Obj", dstImage);
	////moveWindow("Move Obj",700,500);
	//return nextTargetRotate.center.x + nextTargetRotate.size.width / 2.0;
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
	vector<vector<Point> >contoursAppr(shapeCount);
	vector<Rect> boundRect(shapeCount);
	vector<int>array_x, array_y, array_x2, array_y2;
	//�ҵ������ͨ��
	for(int i = 0; i < shapeCount; i ++)
	{//�ҵ���������
		approxPolyDP(Mat(contourAll[i]), contoursAppr[i], 5, true);
		boundRect[i] = boundingRect(Mat(contoursAppr[i]));
		//���ն�
		drawContours(srcImage,contourAll,i,Scalar(255), CV_FILLED);
		if(boundRect[i].area() < MIN_RECT_AREA) continue;
		array_x.push_back(boundRect[i].x); array_x2.push_back(boundRect[i].x + boundRect[i].width);
		array_y.push_back(boundRect[i].y); array_y2.push_back(boundRect[i].y + boundRect[i].height);
	}
	//�ҵ����\��Сֵ
	if(array_x.size() == 0) return;//û�в�׽���˶�����
	moveRange.x = (int)(*std::min_element(array_x.begin(),array_x.end())); 
	moveRange.y = (int)(*std::min_element(array_y.begin(),array_y.end())); 
	moveRange.width = (int)(*std::max_element(array_x2.begin(),array_x2.end())) - moveRange.x; 
	moveRange.height = (int)(*std::max_element(array_y2.begin(),array_y2.end())) - moveRange.y;
	
	if(moveRange.area() >= MIN_RECT_AREA)
	{
		rectangle(srcImage, Point(moveRange.x, moveRange.y), Point(moveRange.x + moveRange.width, moveRange.y + moveRange.height), Scalar(255,0,0), 2);
		circle(srcImage, Point(moveRange.x + moveRange.width / 2, moveRange.y + moveRange.height /2 ),7, Scalar(255,0,0),2);
		imshow("Move", srcImage);
		moveWindow("Move",0,500);
	}
}

//��ʾ��ǰ�Ƕ�
void ImageHandler::ShowDemoInfo(double degree,int xValue)
{
	Mat demoResultInfo = Mat::zeros(DEMO_RESULT_RADIUS/2,DEMO_RESULT_RADIUS,CV_8UC1);
	//��ע����ͷλ��
	circle(demoResultInfo,camPosDemoResult,6,colorDemoResult,5);
	//���㵱ǰ�Ƕ�
	objPosDemoResult.x = DEMO_RESULT_RADIUS / 2.0 * (1.0 - sin(degree));
	objPosDemoResult.y = DEMO_RESULT_RADIUS / 2.0 * cos(degree);
	line(demoResultInfo, camPosDemoResult, objPosDemoResult,colorDemoResult,3);
	if(xValue > 0)
	{//-1Ϊû��׽���˶�����
		circle(demoResultInfo,Point(xValue * 1.2,0), 7,colorDemoResult,2);
	}
	cout << xValue << "****" << degree<<endl;

	imshow("Result", demoResultInfo);
	moveWindow("Result",700,0);
}

int findMostSimilarRect(Rect target, vector<Rect> selectList);
void FindTheFirstFace(vector<vector<Rect> > , int , Rect&);

//����ʶ��
int ImageHandler::RecognitionHumanFace(Mat sourceFrame){
	vector<Rect> faces;
	Mat faceGray;
	//�Ҷȴ�����ɫͼ���Ϊ�ڰף�
	cvtColor(sourceFrame, faceGray, CV_RGB2GRAY);
	//�Ҷ�ͼ��ֱ��ͼ���⻯����һ��ͼ�����Ⱥ���ǿ�Աȶȣ�
	equalizeHist( faceGray, faceGray );
	//����ʶ��
	faceCascade.detectMultiScale(faceGray, faces, 1.1, 3, 0|CV_HAAR_SCALE_IMAGE, Size(60,60));
	if(faces.size() < 1) return -1;
	if(!findTargetFlag)
	{//�״�ʶ��
		int faceCollectCount = allFaceLatest.size();
		if(faceCollectCount < FIRST_FRAME_COUNT)
		{//�״βɼ�
			allFaceLatest.push_back(faces);
		}
		if(faceCollectCount >= FIRST_FRAME_COUNT)
		{//�״�ʶ������� -> �����
			FindTheFirstFace(allFaceLatest,MIN_SIZE_PIXEL,nextTarget);
			findTargetFlag = true;
		}
		return -1;
	}
	//�����ϴ��������
	int similarIdx=findMostSimilarRect(nextTarget , faces);
	if(abs(faces[similarIdx].x - nextTarget.x) > CHANGE_FACE_JUMP_FALG || abs(faces[similarIdx].y - nextTarget.y) > CHANGE_FACE_JUMP_FALG || 
		abs(faces[similarIdx].width - nextTarget.width) > CHANGE_FACE_JUMP_FALG || abs(faces[similarIdx].height - nextTarget.height) > CHANGE_FACE_JUMP_FALG)
	{//��֡���
		jumpFrameCount++;
		if(jumpFrameCount >= CHANGE_FACE_MIN_COUNT){
			findTargetFlag = false;
			allFaceLatest.clear();
		}
	}else{
		jumpFrameCount = 0;
		nextTarget = faces[similarIdx];
	}
	//ͼ�����
	Point center(nextTarget.x +nextTarget.width/2,nextTarget.y + nextTarget.height/2 );
	ellipse(sourceFrame,center,Size(nextTarget.width/2,nextTarget.height/2),0,0,360,Scalar( 255, 0, 255 ), 2, 8, 0 );	
	imshow("Hunman Face",sourceFrame);
	moveWindow("Hunman Face",700,0);

	return nextTarget.x +nextTarget.width/2;
}

//�ҵ���ʼ���ĵ�һ����
void FindTheFirstFace(vector<vector<Rect> > allFrameFaces, int maxInter,Rect &faceNearest)
{//�״�ʶ������� -> �����
	char* strRectFormat = "%d_%d_%d_%d", *tmpStrRect=new char[100];
	map<string,int> faceStrCountList;
	map<string,Rect> faceStrRectList;
	vector<Rect> faceList = allFrameFaces[0];
	for(vector<Rect>::iterator istep=faceList.begin();istep != faceList.end();++istep)
	{//�洢��һ֡�е�����
		sprintf(tmpStrRect, strRectFormat,istep->x,istep->y,istep->width,istep->height);
		faceStrCountList[tmpStrRect]=1;
		faceStrRectList[tmpStrRect] = *istep;
	}
	int frameCount = allFrameFaces.size(), faceNum;
	for(int i=1;i<frameCount;i++)
	{//�Ѽ�����λ����������
		faceNum = allFrameFaces[i].size();
		for(int j=0;j<faceNum;j++)
		{
			sprintf(tmpStrRect, strRectFormat,allFrameFaces[i][j].x,allFrameFaces[i][j].y,allFrameFaces[i][j].width,allFrameFaces[i][j].height);
			int nearIdx = findMostSimilarRect(allFrameFaces[i][j],faceList);
			if(abs(allFrameFaces[i][j].x - faceList[nearIdx].x) > maxInter || abs(allFrameFaces[i][j].y - faceList[nearIdx].y) > maxInter|| 
				abs(allFrameFaces[i][j].width - faceList[nearIdx].width) > maxInter|| abs(allFrameFaces[i][j].height - faceList[nearIdx].height) > maxInter)
			{//�·��ֵľ��Σ�����ľ��β���ͬһ����
				faceList.push_back(allFrameFaces[i][j]);
				faceStrCountList[tmpStrRect]=1;
				faceStrRectList[tmpStrRect] = allFrameFaces[i][j];
			}
			else
			{
				faceStrCountList[tmpStrRect] ++;
			}
		}
	}
	int maxRectNum = 0;
	for(map<string,int>::iterator istep = faceStrCountList.begin();istep != faceStrCountList.end();istep++)
	{//������֡ͼ���У�������ľ���λ�ã�ʶ�����ߵ���Ϊ��������
		if(istep->second > maxRectNum)
			maxRectNum = istep->second;
	}
	int maxFaceSize = 0;
	for(map<string,int>::iterator istep = faceStrCountList.begin();istep != faceStrCountList.end();istep++)
	{//������� �е������������һ���������������Ҿ�������ͷ����ģ�
		if(istep->second < maxRectNum) continue;
		if(faceStrRectList[istep->first].height * faceStrRectList[istep->first].width > maxFaceSize)
		{
			faceNearest = faceStrRectList[istep->first];
			maxFaceSize = faceNearest.height * faceNearest.width;
		}		
	}
}

//���������ε�����ֵ
int calcTwoRectSimilar(Rect one, Rect two)
{
	//���Ƽ��㹫ʽ����Сֵ��������� + �����������ƽ��
	return abs(one.width * one.height - two.height*two.width) + 
		   (int)abs(pow(one.x + one.width/2 ,2) + pow(one.y + one.height/2,2) -pow(two.x + two.width/2,2)-pow(two.y+two.height/2,2));
}

//��������Ŀ��
int findMostSimilarRect(Rect target, vector<Rect> selectList)
{
	int faceCount = selectList.size(),similarIdx=0, mostSimilar = 0xFFFFFF, tmpSimilar;
	for(int i=1;i<faceCount;i++)
	{//�ҵ�����������
		tmpSimilar = calcTwoRectSimilar(selectList[i], target);
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
	Mat image=imread("img.jpg");

	//ͼƬ����ɹ�����ж�
	if(!image.data)		return ;

	//��ʾͼ��
	imshow("image1",image);	
	moveWindow("����ʶ��",800,500);
	
	//�ȴ�����
	waitKey();
}