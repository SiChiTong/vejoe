#include "Clock.h"

unsigned int timerPeriod = 2000 - 1;

void NVIC_cfg(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//ѡ���жϷ���
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	//ѡ��TIM2���ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	
	//��ռʽ�ж����ȼ�����Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	
	//��Ӧʽ�ж����ȼ�����Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	
	//ʹ���ж�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
	//����NVIC_InitStructure��ʼ���ж����ȼ�
	NVIC_Init(&NVIC_InitStructure);
}

void Timer_cfg(void)
{
	//ʹ�ܶ�ʱ��ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	//��λ��ʱ��TIM2�������ʼ״̬
	TIM_DeInit(TIM2);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	//��ʱ���ڣ�ʱ���ж�һ�εĶ�ʱ����������
	TIM_TimeBaseStructure.TIM_Period = timerPeriod;
	
	//ʱ��Ԥ��Ƶ������ʱ��ÿ�μ���������Ƶ����
	TIM_TimeBaseStructure.TIM_Prescaler = 36000 - 1;
	
	//�����������ϼ�����
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	
	//��ʼ��ʱ��TIM2
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);
	
	//�������жϱ�־
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	
	//ʹ�ܶ�ʱ���ĸ����ж�
	TIM_ITConfig(TIM2,TIM_IT_Update, ENABLE);
	
	//ʹ�ܶ�ʱ��
	TIM_Cmd(TIM2,ENABLE);
}


void (*pfClockHandler)(void);
void TIM2_IRQHandler(void)
{	
	//��Ƶ72MHz
	//��ʱ����ʱ��Ƶ�� = ��Ƶ / (ʱ��Ԥ��Ƶ + 1) = 72M / (36000 -1 + 1) = 2000
	//�ж�ʱ�� = (��ʱ���� + 1) / ʱ��Ƶ�� = (2000 - 1 + 1) / 2000 = 1
	//         = (��ʱ���� + 1) * (ʱ��Ԥ��Ƶ + 1) / ��Ƶ
	
	//����Ƿ�����������¼�
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) == RESET) return;
		
	//���TIM2���жϴ�����λ
	TIM_ClearITPendingBit(TIM2, TIM_FLAG_Update);
	
	if(pfClockHandler != NULL)
	{
		(*pfClockHandler)();
	}
}

void InitialTimer(void)
{	
	//����LED
	InitialLED();
	
	//�����ж�
	NVIC_cfg();
	
	//���ö�ʱ��
	Timer_cfg();
}

	
//��ʱִ�е�LED����
void functionLEDTest(void);
void ClockTest(void)
{
	//��ʱ���� = 2000 - 1����ʱ1�룩
	timerPeriod = 2000 - 1;
	
	//ָ��ִ�к���
	pfClockHandler = functionLEDTest;
	
	InitialTimer();
}

//�����Ƶ�LED�����
int ledIdx = 2;
void functionLEDTest(void)
{	
	//��ȡLED��״̬	
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



//��ʱִ�е�LED����������
void functionLEDGradualTest(void);
void ClockGradualTest(void)
{
	//��ʱ���� = 1����ʱ1���룩
	timerPeriod = 1;
	
	//ָ��ִ�к���
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
