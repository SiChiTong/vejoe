/*
decribe: pcb io port define.
author : houzuping.
data   : 2016-09-05.
*/
#include "pcb.h"

#define	GPIO_Remap_SWJ_JTAGDisable ((uint32_t)0x00300200)
/*

*/
void IO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//-----------------------------------//
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,  ENABLE);
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	//-----------------------------------//
	//外部输入信号. 12路外部输入信号
	//PB5~PB9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	//PC4~PC6
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
	//PA7
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);		
	////////////////////////////////////////////////////////
	//输出端口 PIN1~PIN12 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);	
	//PC1 ~ PC3  LED 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
}

/*
led 输出端口
*/
void Set_LED_IN1(UINT8 state)
{
	if(state)
	{
		GPIO_ResetBits(GPIOC, GPIO_Pin_3);
	}
	else
	{
		GPIO_SetBits(GPIOC, GPIO_Pin_3);
	}
}
/*
led 输出端口
*/
void Set_LED_IN2(UINT8 state)
{
	if(state)
	{
		GPIO_ResetBits(GPIOC, GPIO_Pin_2);
	}
	else
	{
		GPIO_SetBits(GPIOC, GPIO_Pin_2);
	}
}
/*
led 输出端口
*/
void Set_LED_IN3(UINT8 state)
{
	if(state)
	{
		GPIO_ResetBits(GPIOC, GPIO_Pin_1);
	}
	else
	{
		GPIO_SetBits(GPIOC, GPIO_Pin_1);
	}
}

/*
拨码开关信号读入
*/
uint8_t Read_SWIN1(void)
{
	return GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3);
}
/*

*/
uint8_t Read_SWIN2(void)
{
	return GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_2);
}
//


/*
end of file.
*/

