#include "led.h"

/**************************************************************************
�������ܣ�LED�ӿڳ�ʼ��
��ڲ������� 
����  ֵ����
PC1 ~ PC3
**************************************************************************/
void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); //ʹ�ܶ˿�ʱ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;	            //�˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;      //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     //50M
	GPIO_Init(GPIOC, &GPIO_InitStructure);					      //�����趨������ʼ��GPIO
}

/**************************************************************************
�������ܣ�LED��˸
��ڲ�������˸Ƶ�� 
����  ֵ����
**************************************************************************/
void Led_Flash(u16 time)
{
	  static int temp;
	  if(0 == time) LED=0;
	  else if(++temp == time)LED=~LED,temp=0;
}

/*

*/


/*
end of file.
*/