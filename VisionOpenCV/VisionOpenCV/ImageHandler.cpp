#include "ImageHandler.h"
#include <numeric>
#include <fstream>

using namespace std;

#define  AREAS_MOTION	100
#define  MIN_TARGET_AREAR		500
#define  MAX_TARGET_AREAR		10000 

ImageHandler::ImageHandler(void):FIRST_FRAME_COUNT(10),MIN_SIZE_PIXEL(10),CHANGE_FACE_JUMP_FALG(200), CHANGE_FACE_MIN_COUNT(5)
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
	
	shapeOperateKernal = getStructuringElement(MORPH_RECT, Size(3, 3));

	//使用人脸识别时使用（加载匹配模型）
	//string faceCascadeName = "haarcascade_frontalface_alt.xml";
	//if(!faceCascade.load(faceCascadeName)){printf("--(!)Error loading\n");}

	ifstream fileParam("Config.ini",ios::in);
	if(!fileParam.is_open()){
		cout<<"CANNOT open config file, Params use DEFAULT."<<endl; 	
		MIN_RECT_AREA = 200;
		MAX_RECT_AREA = 30000;
		MIN_X_DISTANCE = 30;
		MAX_X_DISTANCE = 200;
		FILTER_MIDDLE_COUNT = 5;
		FILTER_MEAN_COUNT = 3;
		MAX_VISION = 60;
		MIN_POINT_COUNT = 20;
		MIN_POINT_COUNT = 280;
		VALID_INTERVAL = 15;

		return;
	}
	char tmpChar[256];
	while(!fileParam.eof())
	{
		fileParam.getline(tmpChar,256);
		string tmpString(tmpChar);
		UpdateParams(tmpString);
	}
}


ImageHandler::~ImageHandler(void)
{
}

//仅锁定运动物体，停止运动时根据最后运动物体色块追踪
int ImageHandler::TrackMotionTarget(Mat souceFrame,Mat foreground)
{
	//框出运动目标
	RecognitionMotionTarget(foreground);

	if(moveRange.width < MIN_X_DISTANCE || moveRange.width > MAX_X_DISTANCE) return -1;//运动物体太小、太大则忽略
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
	xValue = std::accumulate(meanFiltArray.begin(),meanFiltArray.end(),0)/meanFiltArray.size();
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
	imshow("f",foreground);
	//开闭操作
	morphologyEx(foreground,tmpImage,MORPH_OPEN,shapeOperateKernal);	
	morphologyEx(tmpImage,srcImage,MORPH_CLOSE,shapeOperateKernal);
	imshow("f1",srcImage);
	//找到所有轮廓
	findContours(srcImage, contourAll, hierarchy, RETR_EXTERNAL , CHAIN_APPROX_SIMPLE);
	int shapeCount = contourAll.size(), maxAreaValue=0, maxAreaIdx = -1;
	vector<vector<Point> >contoursAppr(shapeCount);
	vector<Rect> boundRect(shapeCount);
	//找到最大连通域
	for(int i = 0; i < shapeCount; i ++)
	{//找到所有物体
		approxPolyDP(Mat(contourAll[i]), contoursAppr[i], 5, true);
		boundRect[i] = boundingRect(Mat(contoursAppr[i]));
		if(boundRect[i].area() < MIN_RECT_AREA || boundRect[i].area() > MAX_RECT_AREA) continue;
		//填充空洞
		drawContours(srcImage,contourAll,i,Scalar(255), CV_FILLED);
	}	
	int imageWidth = srcImage.cols, imageHeight = srcImage.rows;
	uchar * imageData = srcImage.data, *tmpRow;
	ushort *moveCount = new ushort[imageWidth];
	for(int i=0;i<imageWidth;i++) 
		moveCount[i] = 0;
	for (int i = 0; i < imageHeight; i++)
	{//图像压缩到一维（X轴方向）
		tmpRow = srcImage.ptr<uchar>(i);
		for(int j=0;j<imageWidth;j++){
			//127的灰色区域表示忽略面积边界（忽略）
			moveCount[j] += ((int)tmpRow[j] == 255?1:0);
		}
	}
	bool validFlag = false;
	int invalidCount=0;
	vector<int> validStartEnd;
	for(int i=0;i<imageWidth;i++)
	{//统计有效运动区域
		if(moveCount[i] < MIN_POINT_COUNT || moveCount[i] > MAX_POINT_COUNT)
		{//过滤不合理范围（当前坐标无效）			
			if(validFlag)
			{
				if(invalidCount < VALID_INTERVAL){//小间隔有效
					invalidCount++;
				}
				else
				{
					validStartEnd.push_back(i - VALID_INTERVAL - 1);//终点，减一表示当前也是无效
					validFlag = false;
					invalidCount = 0;
				}
			}
		}
		else if(!validFlag)
		{//新开始记录
			validStartEnd.push_back(i);//起点
			validFlag = true;
			invalidCount = 0;
		}
	}
	if(validStartEnd.size() % 2 != 0)
	{//最后一段保持有效
		validStartEnd.push_back(imageWidth);
	}
	delete(moveCount);
	int segmentCount = validStartEnd.size(),tmpLen;
	int maxValue = 0, maxIdx=-1;
	for(int i=0;i<segmentCount;i+=2)
	{//找最大面积
		tmpLen = validStartEnd[i+1] - validStartEnd[i];
		if(tmpLen > maxValue){
			maxIdx = i;
			maxValue = tmpLen;
		}
	}
	if(maxIdx >= 0)
	{//找到了最大面积
		moveRange.x = validStartEnd[maxIdx]; 
		moveRange.y = 0; 
		moveRange.width = validStartEnd[maxIdx + 1] - validStartEnd[maxIdx]; 
		moveRange.height = imageHeight;

		rectangle(srcImage, Point(moveRange.x, moveRange.y), Point(moveRange.x + moveRange.width, moveRange.y + moveRange.height), Scalar(255,0,0), 2);
		circle(srcImage, Point(moveRange.x + moveRange.width / 2, moveRange.y + moveRange.height /2 ),7, Scalar(255,0,0),2);
		//imshow("Move", srcImage);
		//moveWindow("Move",500,400);
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

//使用配置文件更新参数值
void ImageHandler::UpdateParams(string keyValue){
	if(keyValue.substr(0,2) == "//") return;//注释
	size_t pos= keyValue.find('=');
	if(pos == string::npos) return;//没找到等号

	string tmpKey = keyValue.substr(0,pos);
	transform(tmpKey.begin(), tmpKey.end(), tmpKey.begin(), (int(*)(int))toupper);
	if(tmpKey == "MIDFILTERCOUNT"){ //中值滤波帧数		
		FILTER_MIDDLE_COUNT = atoi(keyValue.substr(pos + 1).c_str());
	}
	else if(tmpKey == "MEANFILTERCOUNT"){//均值滤波帧数
		FILTER_MEAN_COUNT = atoi(keyValue.substr(pos + 1).c_str());
	}
	else if(tmpKey == "IGNOREAREAMIN"){ //忽略面积 最小值
		MIN_RECT_AREA = atoi(keyValue.substr(pos + 1).c_str());
	}
	else if(tmpKey == "IGNOREAREAMAX"){ //忽略面积 最大值
		MAX_RECT_AREA = atoi(keyValue.substr(pos + 1).c_str());
	}
	else if(tmpKey == "MAXVISION"){ //最大视角
		MAX_VISION = atoi(keyValue.substr(pos + 1).c_str());
	}
	else if(tmpKey == "MINPOINTCOUNT"){ //最少运动有效像素数
		MIN_POINT_COUNT = atoi(keyValue.substr(pos + 1).c_str());
	}
	else if(tmpKey == "MAXPOINTCOUNT"){ //最多运动有效像素数
		MAX_POINT_COUNT = atoi(keyValue.substr(pos + 1).c_str());
	}
	else if(tmpKey == "VALIDINTERVAL"){ //最大有效空闲间距
		VALID_INTERVAL = atoi(keyValue.substr(pos + 1).c_str());
	}
	else if(tmpKey == "IGNOREXDISTANCEMIN"){ //最小有效坐标长度
		MIN_X_DISTANCE = atoi(keyValue.substr(pos + 1).c_str());
	}
	else if(tmpKey == "IGNOREXDISTANCEMAX"){ //最大有效坐标长度
		MAX_X_DISTANCE = atoi(keyValue.substr(pos + 1).c_str());
	}
}