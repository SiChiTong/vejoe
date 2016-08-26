#include <iostream>
#include <core/core.hpp>
#include <highgui/highgui.hpp>

using namespace cv;
using namespace std;

void DemoImage(void);

int main()
{
	DemoImage();
	return 0;
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