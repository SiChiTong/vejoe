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