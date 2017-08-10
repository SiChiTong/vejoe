#include "Switch.h"

//#define	GPIO_Remap_SWJ_JTAGDisable ((uint32_t)0x00300200)
void InitialSwitch(void)
{
	//����LED
	InitialLED();
//	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	//ʹ��GPIO��Ӧ������ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	
	//����һ��GPIO_InitStructure�ṹ��
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//ѡ������õ�GPIOC�ܽ�3����ӦSW_IN 3��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	
	//����ѡ��GPIO�ܽŵ�ģʽ����������ģʽ��
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	
	//����GPIO_InitStructure��ָ���Ĳ�����ʼ������GPIO
	GPIO_Init(GPIOD, &GPIO_InitStructure);	
}

BOOL isOnSwitch()
{	
	u8 intStatus= GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_2);
	
	return intStatus == 1;
}

void SwitchTest(void)
{	
	if(isOnSwitch())
		OpenLED(1);
	else
		CloseLED(1);
}
