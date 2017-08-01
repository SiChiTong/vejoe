#include "Hall.h"


void InitialMotorEncode(void)
{
	//使能GPIO对应的外设时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//左轮
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	//声明一个GPIO_InitStructure结构体
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//选择待设置的GPIOC管脚0、1（分别对应QEI_BA、QEI_BB）
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	
	//设置选中GPIO管脚速率（2MHz）
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	
	//设置选中GPIO管脚的模式（推挽输出模式）
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	
	//根据GPIO_InitStructure中指定的参数初始化外设GPIO
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
}
