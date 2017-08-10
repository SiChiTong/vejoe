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
	//使能GPIO对应的外设时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	//RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOC, DISABLE);

	//声明一个GPIO_InitStructure结构体
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//选择待设置的GPIOC管脚1、2、3（分别对应三个LED灯）
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	
	//设置选中GPIO管脚速率（2MHz）
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	
	//设置选中GPIO管脚的模式（推挽输出模式）
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	
	//根据GPIO_InitStructure中指定的参数初始化外设GPIO
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}


void delaySoft(unsigned int timeStay)
{
	unsigned int timeCount = timeStay * 1e6;
	for(int i=0;i<timeCount;i++);
}

void openCloseAllLED(void)
{
	//打开LED灯：将端口GPIOC的第1，2，3脚置1（高电平）
	GPIO_SetBits(GPIOC, GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3);
	
	//延时
	delaySoft(2);	
	
	//关闭LED灯：将端口GPIOC的第1，2，3脚置0（低电平）
	GPIO_ResetBits(GPIOC, GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3);
}

//依次关灯：
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
