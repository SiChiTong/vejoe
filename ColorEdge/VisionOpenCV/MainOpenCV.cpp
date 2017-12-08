#include <iostream>
#include <cv.h>
#include <stdio.h>

#include "ImageHandler.h"
#include "MotionCalc.h"

using namespace std;
void GetParams(int & colorType);

int main()
{
	ImageHandler imageTool;	
	Mat sourceFrame;
	char rootPath[1000];
	string strPath;
	_getcwd_dbg(rootPath,1000,1,NULL,1);
	strPath = (string)rootPath;
	VideoCapture capture("lenovo2011.mp4");//
	if (!capture.isOpened()) return 0;

	bool stopFlag(false);
	int idx=0,color=1;
	GetParams(color);
	while (!stopFlag)
	{
		idx ++;
		if (!capture.read(sourceFrame))
		{
			capture.open(0);
			cout<<endl<<capture.isOpened()<<"Camera Read Fail;"<<endl;
			if (!capture.isOpened() || !capture.read(sourceFrame)) break;
		}

		//显示原始图像
		imshow("Source Image", sourceFrame);
		moveWindow("Source Image",0,0);

		imageTool.recoginceColor(sourceFrame,color);

		//程序结束开关
		if (waitKey(10) == 27)
		{//监听到ESC退出
			stopFlag = true;
		}
	}
	return 0;
}



#include <numeric>
#include <fstream>

//使用配置文件更新参数值
void GetParams(int & colorType){
	ifstream fileParam("Config.ini",ios::in);
	if(!fileParam.is_open()){
		colorType = 1;
	}
	char tmpChar[256];
	while(!fileParam.eof())
	{
		fileParam.getline(tmpChar,256);
		string tmpString(tmpChar);
	
		if(tmpString.substr(0,2) == "//") continue;//注释
		size_t pos= tmpString.find('=');
		if(pos == string::npos) continue;//没找到等号

		string tmpKey = tmpString.substr(0,pos);
		transform(tmpKey.begin(), tmpKey.end(), tmpKey.begin(), (int(*)(int))toupper);
		if(tmpKey == "COLOR"){ //颜色		
			colorType = atoi(tmpString.substr(pos + 1).c_str());
		}
	}
}