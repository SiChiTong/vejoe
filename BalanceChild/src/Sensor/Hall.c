#include "Hall.h"


void InitialMotorEncode(void)
{
	//ʹ��GPIO��Ӧ������ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//����
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	//����һ��GPIO_InitStructure�ṹ��
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//ѡ������õ�GPIOC�ܽ�0��1���ֱ��ӦQEI_BA��QEI_BB��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	
	//����ѡ��GPIO�ܽ����ʣ�2MHz��
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	
	//����ѡ��GPIO�ܽŵ�ģʽ���������ģʽ��
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	
	//����GPIO_InitStructure��ָ���Ĳ�����ʼ������GPIO
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
}
