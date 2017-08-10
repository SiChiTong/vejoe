#include "LED.h"

int ledCount = 3;
uint16_t ledArray[3] = {GPIO_Pin_1,GPIO_Pin_2,GPIO_Pin_3};
	
void LEDTest(void)
{
	InitialLED();
	
	openCloseAllLED();
	
	roundLight();
}

void InitialLED(void)
{
	//ʹ��GPIO��Ӧ������ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	//RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOC, DISABLE);

	//����һ��GPIO_InitStructure�ṹ��
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//ѡ������õ�GPIOC�ܽ�1��2��3���ֱ��Ӧ����LED�ƣ�
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	
	//����ѡ��GPIO�ܽ����ʣ�2MHz��
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	
	//����ѡ��GPIO�ܽŵ�ģʽ���������ģʽ��
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	
	//����GPIO_InitStructure��ָ���Ĳ�����ʼ������GPIO
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}


void delaySoft(unsigned int timeStay)
{
	unsigned int timeCount = timeStay * 1e6;
	for(int i=0;i<timeCount;i++);
}

void openCloseAllLED(void)
{
	//��LED�ƣ����˿�GPIOC�ĵ�1��2��3����1���ߵ�ƽ��
	GPIO_SetBits(GPIOC, GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3);
	
	//��ʱ
	delaySoft(2);	
	
	//�ر�LED�ƣ����˿�GPIOC�ĵ�1��2��3����0���͵�ƽ��
	GPIO_ResetBits(GPIOC, GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3);
}

//���ιصƣ�
void roundLight(void)
{
	while(TRUE)
	{
		for(int i=0;i<2;i++)
		{
			delaySoft(1);
			OpenLED(i);
			delaySoft(1);
			CloseLED(i);
		}
	}
}

BOOL checkLedIdxValid(int ledIdx)
{
	return ledIdx < ledCount && ledIdx >= 0;
}
	
void OpenLED(int ledIdx)
{
	if(!checkLedIdxValid(ledIdx)) return;
	
	GPIO_SetBits(GPIOC, ledArray[ledIdx]);
	
}

void CloseLED(int ledIdx)
{
	if(!checkLedIdxValid(ledIdx)) return;
	
	GPIO_ResetBits(GPIOC, ledArray[ledIdx]);
}

u8 GetStatusLED(int ledIdx)
{	
	if(!checkLedIdxValid(ledIdx)) return 0;
	
	return GPIO_ReadOutputDataBit(GPIOC, ledArray[ledIdx]);
}
