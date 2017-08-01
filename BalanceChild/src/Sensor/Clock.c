#include "Clock.h"

unsigned int timerPeriod = 2000 - 1;

void NVIC_cfg(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//选择中断分组
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	//选择TIM2的中断通道
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	
	//抢占式中断优先级设置为0
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	
	//响应式中断优先级设置为0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	
	//使能中断
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
	//根据NVIC_InitStructure初始化中断优先级
	NVIC_Init(&NVIC_InitStructure);
}

void Timer_cfg(void)
{
	//使能定时器时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	//复位定时器TIM2，进入初始状态
	TIM_DeInit(TIM2);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	//定时周期：时钟中断一次的定时器计数次数
	TIM_TimeBaseStructure.TIM_Period = timerPeriod;
	
	//时钟预分频数：定时器每次计数经过的频率数
	TIM_TimeBaseStructure.TIM_Prescaler = 36000 - 1;
	
	//计数方向（向上计数）
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	
	//初始化时钟TIM2
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);
	
	//清除溢出中断标志
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	
	//使能定时器的更新中断
	TIM_ITConfig(TIM2,TIM_IT_Update, ENABLE);
	
	//使能定时器
	TIM_Cmd(TIM2,ENABLE);
}


void (*pfClockHandler)(void);
void TIM2_IRQHandler(void)
{	
	//主频72MHz
	//定时器的时钟频率 = 主频 / (时钟预分频 + 1) = 72M / (36000 -1 + 1) = 2000
	//中断时间 = (定时周期 + 1) / 时钟频率 = (2000 - 1 + 1) / 2000 = 1
	//         = (定时周期 + 1) * (时钟预分频 + 1) / 主频
	
	//检测是否发生溢出更新事件
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) == RESET) return;
		
	//清除TIM2的中断待处理位
	TIM_ClearITPendingBit(TIM2, TIM_FLAG_Update);
	
	if(pfClockHandler != NULL)
	{
		(*pfClockHandler)();
	}
}

void InitialTimer(void)
{	
	//配置LED
	InitialLED();
	
	//配置中断
	NVIC_cfg();
	
	//配置定时器
	Timer_cfg();
}

	
//定时执行的LED功能
void functionLEDTest(void);
void ClockTest(void)
{
	//定时周期 = 2000 - 1（定时1秒）
	timerPeriod = 2000 - 1;
	
	//指定执行函数
	pfClockHandler = functionLEDTest;
	
	InitialTimer();
}

//待控制的LED灯序号
int ledIdx = 2;
void functionLEDTest(void)
{	
	//读取LED的状态	
	u8 ledStatus = GetStatusLED(ledIdx);
	
	if(ledStatus == 0)
	{
		OpenLED(ledIdx);
	}
	else
	{
		CloseLED(ledIdx);
	}
}



//定时执行的LED渐亮渐灭功能
void functionLEDGradualTest(void);
void ClockGradualTest(void)
{
	//定时周期 = 1（定时1毫秒）
	timerPeriod = 1;
	
	//指定执行函数
	pfClockHandler = functionLEDGradualTest;
	
	InitialTimer();
}

volatile int iLight = 0, iLightCount = 0, iMAX = 20;
BOOL iIsLight = TRUE;
void functionLEDGradualTest(void)
{
	if(iLight >= iMAX)
	{	
		iLight = 0;
		if(iIsLight)			
			iLightCount++;
		else
			iLightCount--;
		if(iLightCount >= iMAX || iLightCount <= 0)
			iIsLight = !iIsLight;
	}
	
	if(iLight < iLightCount)
	{
		OpenLED(ledIdx);
	}
	else
	{
		CloseLED(ledIdx);
	}
	
	iLight++;
}
//void functionLEDGradualTest(void)
//{
//	if(iLight <= 1)
//	{
//		OpenLED(ledIdx);
//	}
//	else
//	{
//		CloseLED(ledIdx);
//	}
//	
//	iLight++;
//	if(iLight>20) iLight=0;
//}
