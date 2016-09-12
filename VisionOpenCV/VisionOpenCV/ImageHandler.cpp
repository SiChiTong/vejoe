#include "ImageHandler.h"
#include <numeric>

using namespace std;

#define  AREAS_MOTION	100
#define  MIN_TARGET_AREAR		500
#define  MAX_TARGET_AREAR		10000 
#define  FILTER_MIDDLE_COUNT	5	// 中值滤波数据量
#define	 FILTER_MEAN_COUNT		3	// 均值滤波数据量

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

	shapeOperateKernal = getStructuringElement(MORPH_RECT, Size(5, 5));
	string faceCascadeName = "haarcascade_frontalface_alt.xml";
	if(!faceCascade.load(faceCascadeName)){printf("--(!)Error loading\n");}
}


ImageHandler::~ImageHandler(void)
{
}

//使用CamShift算法捕获运动并标记
bool ImageHandler::TrackCamShift(Mat souceFrame,Mat foreground)
{
	//跟踪目标消失标志
	bool targetExistsFlag = false;
	static enumReconStatus state_fsm = SET_TARGET;
	Mat dstImage;
	//使用中值滤波器进行模糊操作（平滑处理）：中值滤波将图像的每个像素用邻域 (以当前像素为中心的正方形区域)像素的中值代替
	medianBlur(souceFrame, dstImage, 3);
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
			RecognitionMotionTarget(foreground);
			int areaObject = moveRange.height * moveRange.width;
			if(areaObject >= MIN_TARGET_AREAR && areaObject <= MAX_TARGET_AREAR)
			{
				selection = moveRange;
				state_fsm = TARGET_CAMSHIFT;
			}

			//计算直方图
			Mat roi(hue, selection), maskroi(mask, selection);
			//计算直方图，放入hist.
			calcHist(&roi, 1, 0, maskroi, hist, 1, &hsize, &phranges);
			//归一化处理
			normalize(hist, hist, 0, 255, CV_MINMAX);

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
			//imshow("Move Obj", dstImage);
			//moveWindow("Move Obj",700,500);

			//判断跟踪的目标位置是否发生改变，若没有则开始计时
			RecognitionMotionTarget(foreground);
			//该计算只是每帧之间的移动面积差，若不行估计要移动多帧后在计算。
			if(moveRange.height * moveRange.width <= AREAS_MOTION) //说明物体基本没有移动
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

//仅锁定运动物体，停止运动时根据最后运动物体色块追踪
int ImageHandler::TrackMotionTarget(Mat souceFrame,Mat foreground)
{
	//框出运动目标
	RecognitionMotionTarget(foreground);
	if(moveRange.area() < MIN_RECT_AREA) return -1;//运动物体太小则忽略
	//Mat dstImage;
	////使用中值滤波器进行模糊操作（平滑处理）：中值滤波将图像的每个像素用邻域 (以当前像素为中心的正方形区域)像素的中值代替
	//medianBlur(souceFrame, dstImage, 3);
	////高斯滤波：这个像素滤波后的值是根据其相邻像素（包括自己那个点）与一个滤波模板进行相乘
	//GaussianBlur(dstImage, dstImage, Size(3,3), 0,0);
	////将彩色图像转换为HSV格式，保存到hsv中
	//cvtColor(dstImage, hsv, COLOR_BGR2HSV);
	////将数据设定在规定的范围内
	//inRange(hsv, Scalar(0, smin, MIN(vmin,vmax)),Scalar(180, 256, MAX(vmin, vmax)), mask);
	////取出 H通道放入hue中
	//hue.create(hsv.size(), hsv.depth());
	//mixChannels(&hsv, 1, &hue, 1, ch, 1);
	////计算直方图
	//Mat roi(hue, moveRange), maskroi(mask, moveRange);
	////计算直方图，放入hist.
	//calcHist(&roi, 1, 0, maskroi, hist, 1, &hsize, &phranges);
	////归一化处理
	//normalize(hist, hist, 0, 255, CV_MINMAX);
	////反向投影图,放入backproj, H通道的范围在0~180的范围内
	//calcBackProject(&hue, 1, 0, hist, backproj, &phranges);
	//backproj &= mask;
	////camshift算法
	//trackBox = CamShift(backproj, moveRange,TermCriteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 15, 2.0));
	//double xValue = trackBox.center.x;	

	//中值 + 均值 过滤
	double xValue = moveRange.x + moveRange.width/2.0;
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
	xValue =std::accumulate(meanFiltArray.begin(),meanFiltArray.end(),0)/meanFiltArray.size(); 
	
	return xValue;

	//跳帧 检测过滤
	//if(!findTargetFlag){//首次没用多帧取均值
	//	nextTargetRotate = trackBox;
	//	findTargetFlag = true;
	//	return nextTarget.x;
	//}
	//if(abs(trackBox.center.x - nextTargetRotate.center.x) > CHANGE_FACE_JUMP_FALG || abs(trackBox.center.y - nextTargetRotate.center.y) > CHANGE_FACE_JUMP_FALG || 
	//	abs(trackBox.size.width - nextTargetRotate.size.width) > CHANGE_FACE_JUMP_FALG || abs(trackBox.size.height - nextTargetRotate.size.height) > CHANGE_FACE_JUMP_FALG)
	//{//跳帧检查
	//	jumpFrameCount++;
	//	if(jumpFrameCount >= CHANGE_FACE_MIN_COUNT)//没对跳帧处理，只是取了最后一帧
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

//确定运动区域
void ImageHandler::RecognitionMotionTarget(Mat foreground)
{
	//开闭操作
	morphologyEx(foreground,tmpImage,MORPH_OPEN,shapeOperateKernal);	
	morphologyEx(tmpImage,srcImage,MORPH_CLOSE,shapeOperateKernal);
	
	//提取边界
	//Canny(srcImage, srcImage, 50, 150, 3);	
	//找到所有轮廓
	findContours(srcImage, contourAll, hierarchy, RETR_EXTERNAL , CHAIN_APPROX_SIMPLE);
	int shapeCount = contourAll.size();
	vector<vector<Point> >contoursAppr(shapeCount);
	vector<Rect> boundRect(shapeCount);
	vector<int>array_x, array_y, array_x2, array_y2;
	//找到最大连通域
	for(int i = 0; i < shapeCount; i ++)
	{//找到所有物体
		approxPolyDP(Mat(contourAll[i]), contoursAppr[i], 5, true);
		boundRect[i] = boundingRect(Mat(contoursAppr[i]));
		//填充空洞
		drawContours(srcImage,contourAll,i,Scalar(255), CV_FILLED);
		if(boundRect[i].area() < MIN_RECT_AREA) continue;
		array_x.push_back(boundRect[i].x); array_x2.push_back(boundRect[i].x + boundRect[i].width);
		array_y.push_back(boundRect[i].y); array_y2.push_back(boundRect[i].y + boundRect[i].height);
	}
	//找到最大\最小值
	if(array_x.size() == 0) return;//没有捕捉到运动物体
	moveRange.x = (int)(*std::min_element(array_x.begin(),array_x.end())); 
	moveRange.y = (int)(*std::min_element(array_y.begin(),array_y.end())); 
	moveRange.width = (int)(*std::max_element(array_x2.begin(),array_x2.end())) - moveRange.x; 
	moveRange.height = (int)(*std::max_element(array_y2.begin(),array_y2.end())) - moveRange.y;
	
	if(moveRange.area() >= MIN_RECT_AREA)
	{
		rectangle(srcImage, Point(moveRange.x, moveRange.y), Point(moveRange.x + moveRange.width, moveRange.y + moveRange.height), Scalar(255,0,0), 2);
		circle(srcImage, Point(moveRange.x + moveRange.width / 2, moveRange.y + moveRange.height /2 ),7, Scalar(255,0,0),2);
		imshow("Move", srcImage);
		moveWindow("Move",0,400);
	}
}

int findMostSimilarRect(Rect target, vector<Rect> selectList);
void FindTheFirstFace(vector<vector<Rect> > , int , Rect&);

//人脸识别
int ImageHandler::RecognitionHumanFace(Mat sourceFrame){
	vector<Rect> faces;
	Mat faceGray;
	//灰度处理（彩色图像变为黑白）
	cvtColor(sourceFrame, faceGray, CV_RGB2GRAY);
	//灰度图象直方图均衡化（归一化图像亮度和增强对比度）
	equalizeHist( faceGray, faceGray );
	//人脸识别
	faceCascade.detectMultiScale(faceGray, faces, 1.1, 3, 0|CV_HAAR_SCALE_IMAGE, Size(60,60));
	if(faces.size() < 1) return -1;
	if(!findTargetFlag)
	{//首次识别
		int faceCollectCount = allFaceLatest.size();
		if(faceCollectCount < FIRST_FRAME_COUNT)
		{//首次采集
			allFaceLatest.push_back(faces);
		}
		if(faceCollectCount >= FIRST_FRAME_COUNT)
		{//首次识别：最多脸 -> 最大脸
			FindTheFirstFace(allFaceLatest,MIN_SIZE_PIXEL,nextTarget);
			findTargetFlag = true;
		}
		return -1;
	}
	//距离上次最近的脸
	int similarIdx=findMostSimilarRect(nextTarget , faces);
	if(abs(faces[similarIdx].x - nextTarget.x) > CHANGE_FACE_JUMP_FALG || abs(faces[similarIdx].y - nextTarget.y) > CHANGE_FACE_JUMP_FALG || 
		abs(faces[similarIdx].width - nextTarget.width) > CHANGE_FACE_JUMP_FALG || abs(faces[similarIdx].height - nextTarget.height) > CHANGE_FACE_JUMP_FALG)
	{//跳帧检查
		jumpFrameCount++;
		if(jumpFrameCount >= CHANGE_FACE_MIN_COUNT){
			findTargetFlag = false;
			allFaceLatest.clear();
		}
	}else{
		jumpFrameCount = 0;
		nextTarget = faces[similarIdx];
	}
	//图像绘制
	Point center(nextTarget.x +nextTarget.width/2,nextTarget.y + nextTarget.height/2 );
	ellipse(sourceFrame,center,Size(nextTarget.width/2,nextTarget.height/2),0,0,360,Scalar( 255, 0, 255 ), 2, 8, 0 );	
	imshow("Hunman Face",sourceFrame);
	moveWindow("Hunman Face",700,0);

	return nextTarget.x +nextTarget.width/2;
}

//找到初始化的第一张脸
void FindTheFirstFace(vector<vector<Rect> > allFrameFaces, int maxInter,Rect &faceNearest)
{//首次识别：最多脸 -> 最大脸
	char* strRectFormat = "%d_%d_%d_%d", *tmpStrRect=new char[100];
	map<string,int> faceStrCountList;
	map<string,Rect> faceStrRectList;
	vector<Rect> faceList = allFrameFaces[0];
	for(vector<Rect>::iterator istep=faceList.begin();istep != faceList.end();++istep)
	{//存储第一帧中的人脸
		sprintf(tmpStrRect, strRectFormat,istep->x,istep->y,istep->width,istep->height);
		faceStrCountList[tmpStrRect]=1;
		faceStrRectList[tmpStrRect] = *istep;
	}
	int frameCount = allFrameFaces.size(), faceNum;
	for(int i=1;i<frameCount;i++)
	{//搜集所有位置脸的数量
		faceNum = allFrameFaces[i].size();
		for(int j=0;j<faceNum;j++)
		{
			sprintf(tmpStrRect, strRectFormat,allFrameFaces[i][j].x,allFrameFaces[i][j].y,allFrameFaces[i][j].width,allFrameFaces[i][j].height);
			int nearIdx = findMostSimilarRect(allFrameFaces[i][j],faceList);
			if(abs(allFrameFaces[i][j].x - faceList[nearIdx].x) > maxInter || abs(allFrameFaces[i][j].y - faceList[nearIdx].y) > maxInter|| 
				abs(allFrameFaces[i][j].width - faceList[nearIdx].width) > maxInter|| abs(allFrameFaces[i][j].height - faceList[nearIdx].height) > maxInter)
			{//新发现的矩形（最近的矩形不是同一个）
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
	{//找所有帧图像中，最多脸的矩形位置（识别度最高的认为是人脸）
		if(istep->second > maxRectNum)
			maxRectNum = istep->second;
	}
	int maxFaceSize = 0;
	for(map<string,int>::iterator istep = faceStrCountList.begin();istep != faceStrCountList.end();istep++)
	{//找最多脸 中的最大脸（几个一样多数量的脸，找距离摄像头最近的）
		if(istep->second < maxRectNum) continue;
		if(faceStrRectList[istep->first].height * faceStrRectList[istep->first].width > maxFaceSize)
		{
			faceNearest = faceStrRectList[istep->first];
			maxFaceSize = faceNearest.height * faceNearest.width;
		}		
	}
}

//计算两矩形的相似值
int calcTwoRectSimilar(Rect one, Rect two)
{
	//相似计算公式（最小值）：面积差 + 中心坐标距离平方
	return abs(one.width * one.height - two.height*two.width) + 
		   (int)abs(pow(one.x + one.width/2 ,2) + pow(one.y + one.height/2,2) -pow(two.x + two.width/2,2)-pow(two.y+two.height/2,2));
}

//找最相似目标
int findMostSimilarRect(Rect target, vector<Rect> selectList)
{
	int faceCount = selectList.size(),similarIdx=0, mostSimilar = 0xFFFFFF, tmpSimilar;
	for(int i=1;i<faceCount;i++)
	{//找到最相似人脸
		tmpSimilar = calcTwoRectSimilar(selectList[i], target);
		if(tmpSimilar < mostSimilar)
		{
			similarIdx = i;
			mostSimilar = tmpSimilar;
		}
	}
	return similarIdx;
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