#include "Switch.h"

//#define	GPIO_Remap_SWJ_JTAGDisable ((uint32_t)0x00300200)
void InitialSwitch(void)
{
	//配置LED
	InitialLED();
//	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	//使能GPIO对应的外设时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	
	//声明一个GPIO_InitStructure结构体
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//选择待设置的GPIOC管脚3（对应SW_IN 3）
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	
	//设置选中GPIO管脚的模式（上拉输入模式）
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	
	//根据GPIO_InitStructure中指定的参数初始化外设GPIO
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
