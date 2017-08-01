#include "timer.h"

/**************************************************************************
�������ܣ���ʱ��3ͨ��3���벶���ʼ��
��ڲ�������ڲ�����arr���Զ���װֵ  psc��ʱ��Ԥ��Ƶ�� 
����  ֵ����
**************************************************************************/
TIM_ICInitTypeDef  TIM3_ICInitStructure;
void TIM3_Cap_Init(u16 arr,u16 psc)	
{	 
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//ʹ��TIM3ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE);  //ʹ��GPIOBʱ��

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 	//PB0 ����  
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1;     
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;    
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//��ʼ����ʱ��3 TIM3	 
	TIM_TimeBaseStructure.TIM_Period = arr; 	//�趨�������Զ���װֵ 
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 	//Ԥ��Ƶ��   
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

	//��ʼ��TIM3���벶�����
	TIM3_ICInitStructure.TIM_Channel = TIM_Channel_3; //CC1S=03 	ѡ������� IC3ӳ�䵽TI1��
	TIM3_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ز���
	TIM3_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM3_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //���������Ƶ,����Ƶ 
	TIM3_ICInitStructure.TIM_ICFilter = 0x00;//���������˲��� ���˲�
	TIM_ICInit(TIM3, &TIM3_ICInitStructure);

	//�жϷ����ʼ��
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //��ռ���ȼ�2��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //�����ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ��� 	
	TIM_ITConfig(TIM3,TIM_IT_Update|TIM_IT_CC3,ENABLE);//��������ж� ,����CC3IE�����ж�	
	TIM_Cmd(TIM3,ENABLE ); 	//ʹ�ܶ�ʱ��3
}
/**************************************************************************
�������ܣ����������ջز�����
��ڲ�������
����  ֵ����
**************************************************************************/
u16 TIM3CH3_CAPTURE_STA,TIM3CH3_CAPTURE_VAL;
void Read_Distane(void)
{   
	PBout(1)=1;
	delay_us(15);  
	PBout(1)=0;	
	if(TIM3CH3_CAPTURE_STA&0X80)//�ɹ�������һ�θߵ�ƽ
	{
		Distance=TIM3CH3_CAPTURE_STA&0X3F;
		Distance*=65536;					        //���ʱ���ܺ�
		Distance+=TIM3CH3_CAPTURE_VAL;		//�õ��ܵĸߵ�ƽʱ��
		Distance=Distance*170/1000;
		//	printf("%d \r\n",Distance);
		TIM3CH3_CAPTURE_STA=0;			//������һ�β���
	}				
}
/**************************************************************************
�������ܣ��������ز������ȡ�ж�
��ڲ�������
����  ֵ����
��    �ߣ�ƽ��С��֮��
**************************************************************************/
void TIM3_IRQHandler(void)
{ 		    		  			    
	u16 tsr;
	tsr=TIM3->SR;
	if((TIM3CH3_CAPTURE_STA&0X80)==0)//��δ�ɹ�����	
	{
		if(tsr&0X01)//���
		{	    
			if(TIM3CH3_CAPTURE_STA&0X40)//�Ѿ����񵽸ߵ�ƽ��
			{
				if((TIM3CH3_CAPTURE_STA&0X3F)==0X3F)//�ߵ�ƽ̫����
				{
					TIM3CH3_CAPTURE_STA|=0X80;//��ǳɹ�������һ��
					TIM3CH3_CAPTURE_VAL=0XFFFF;
				}else TIM3CH3_CAPTURE_STA++;
			}	 
		}
		if(tsr&0x08)//����3���������¼�
		{	
			if(TIM3CH3_CAPTURE_STA&0X40)		//����һ���½��� 		
			{	  			
			TIM3CH3_CAPTURE_STA|=0X80;		//��ǳɹ�����һ�θߵ�ƽ����
			TIM3CH3_CAPTURE_VAL=TIM3->CCR3;	//��ȡ��ǰ�Ĳ���ֵ.
			TIM3->CCER&=~(1<<9);			//CC1P=0 ����Ϊ�����ز���
			}else  								//��δ��ʼ,��һ�β���������
			{
				TIM3CH3_CAPTURE_STA=0;			//���
				TIM3CH3_CAPTURE_VAL=0;
				TIM3CH3_CAPTURE_STA|=0X40;		//��ǲ�����������
				TIM3->CNT=0;					//���������
				TIM3->CCER|=1<<9; 				//CC1P=1 ����Ϊ�½��ز���
			}		    
		}			     	    					   
	}
	TIM3->SR=0;//����жϱ�־λ 	     
}



u8 TIM8CH1_CAPTURE_STA = 0;	//ͨ��1���벶���־������λ�������־����6λ�������־		
u16 TIM8CH1_CAPTURE_UPVAL;
u16 TIM8CH1_CAPTURE_DOWNVAL;

u8 TIM8CH2_CAPTURE_STA = 0;	//ͨ��2���벶���־������λ�������־����6λ�������־		
u16 TIM8CH2_CAPTURE_UPVAL;
u16 TIM8CH2_CAPTURE_DOWNVAL;

u8 TIM8CH3_CAPTURE_STA = 0;	//ͨ��3���벶���־������λ�������־����6λ�������־		
u16 TIM8CH3_CAPTURE_UPVAL;
u16 TIM8CH3_CAPTURE_DOWNVAL;

u8 TIM8CH4_CAPTURE_STA = 0;	//ͨ��1���벶���־������λ�������־����6λ�������־		
u16 TIM8CH4_CAPTURE_UPVAL;
u16 TIM8CH4_CAPTURE_DOWNVAL;


u32 TIM8_T1;
u32 TIM8_T2;
u32 TIM8_T3;
u32 TIM8_T4;


/**************************************************************************
�������ܣ���ģң�س�ʼ������
��ڲ�����arr���Զ���װֵ  psc��ʱ��Ԥ��Ƶ�� 
����  ֵ����
**************************************************************************/
void TIM8_Cap_Init(u16 arr, u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseInitStructure; 
	NVIC_InitTypeDef NVIC_InitStructure; 
	TIM_ICInitTypeDef TIM_ICInitStructure; 
	/*RCC Configuration*/ 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8,ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE); 	//�����˸���ʱ�� 
	/*GPIO Configuration*/ 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	 //�������� 
	GPIO_Init(GPIOC,&GPIO_InitStructure); 
	GPIO_ResetBits(GPIOC,GPIO_Pin_6|GPIO_Pin_7); 
	/*Timebase Configuration*/ 
	TIM_InternalClockConfig(TIM8);                                  //ʱ��Դѡ���ڲ�ʱ�����ڼ�ʱ 
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;             //Ԥ��Ƶ������ 
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; //�����ؼ��� 
	TIM_TimeBaseInitStructure.TIM_Period = arr;                //�������� 
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //ʱ�ӷ�Ƶ������Ƶ 
	TIM_TimeBaseInit(TIM8,&TIM_TimeBaseInitStructure); 
	/*PWM_Input Configuration*/ 
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;                //����ͨ��1 
	TIM_ICInitStructure.TIM_ICPolarity= TIM_ICPolarity_Rising;      //�����ز��� 
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //����ӳ��ѡ��Ϊ��Ӧ��Tl1 
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;           //Ԥ��Ƶ1 
	TIM_ICInitStructure.TIM_ICFilter = 0x00;                        //�˲���:���˲� 
	TIM_ICInit(TIM8,&TIM_ICInitStructure); 
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;                //����ͨ��2
	TIM_ICInit(TIM8,&TIM_ICInitStructure); 
	/*NVIC Configuration*/ 
	NVIC_InitStructure.NVIC_IRQChannel = TIM8_CC_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure); 
	NVIC_InitStructure.NVIC_IRQChannel = TIM8_UP_IRQn; 
	NVIC_Init(&NVIC_InitStructure); 
	/*Interrupt Configuration*/ 
	TIM_ClearITPendingBit(TIM8,TIM_IT_Update|TIM_IT_CC1|TIM_IT_CC2); 
	TIM_ITConfig(TIM8,TIM_IT_Update|TIM_IT_CC1|TIM_IT_CC2,ENABLE); 
	TIM_Cmd(TIM8,ENABLE); 
}

/**************************************************************************
�������ܣ���ģң�ؽ����ж�
��ڲ�������
����  ֵ����
**************************************************************************/
void TIM8_CC_IRQHandler(void)
{
	if ((TIM8CH1_CAPTURE_STA & 0X80) == 0) 		//��δ�ɹ�����	
	{
		if (TIM_GetITStatus(TIM8, TIM_IT_CC1) != RESET) 		//����1���������¼�
		{
			TIM_ClearITPendingBit(TIM8, TIM_IT_CC1); 		//����жϱ�־λ
			if (TIM8CH1_CAPTURE_STA & 0X40)		//����һ���½���
			{
				TIM8CH1_CAPTURE_DOWNVAL = TIM_GetCapture1(TIM8);//��¼�´�ʱ�Ķ�ʱ������ֵ
				if (TIM8CH1_CAPTURE_DOWNVAL < TIM8CH1_CAPTURE_UPVAL)
				{
				TIM8_T1 = 65535;
				}
				else
				TIM8_T1 = 0;
				Remoter_Ch1 = TIM8CH1_CAPTURE_DOWNVAL - TIM8CH1_CAPTURE_UPVAL
				+ TIM8_T1;		//�õ��ܵĸߵ�ƽ��ʱ��
				TIM8CH1_CAPTURE_STA = 0;		//�����־λ����
				TIM_OC1PolarityConfig(TIM8, TIM_ICPolarity_Rising); //����Ϊ�����ز���		  
			}
			else //��������ʱ�䵫�����½��أ���һ�β��������أ���¼��ʱ�Ķ�ʱ������ֵ
			{
				TIM8CH1_CAPTURE_UPVAL = TIM_GetCapture1(TIM8);		//��ȡ����������
				TIM8CH1_CAPTURE_STA |= 0X40;		//����Ѳ���������
				TIM_OC1PolarityConfig(TIM8, TIM_ICPolarity_Falling);//����Ϊ�½��ز���
			}
		}
	}

	if ((TIM8CH2_CAPTURE_STA & 0X80) == 0)		//��δ�ɹ�����	
	{
		if (TIM_GetITStatus(TIM8, TIM_IT_CC2) != RESET)		//����2���������¼�
		{
			TIM_ClearITPendingBit(TIM8, TIM_IT_CC2);		//����жϱ�־λ
			if (TIM8CH2_CAPTURE_STA & 0X40)		//����һ���½���
			{
				TIM8CH2_CAPTURE_DOWNVAL = TIM_GetCapture2(TIM8);//��¼�´�ʱ�Ķ�ʱ������ֵ
				if (TIM8CH2_CAPTURE_DOWNVAL < TIM8CH2_CAPTURE_UPVAL)
				{
					TIM8_T2 = 65535;
				}
				else
				TIM8_T2 = 0;
				Remoter_Ch2 = TIM8CH2_CAPTURE_DOWNVAL - TIM8CH2_CAPTURE_UPVAL
				+ TIM8_T2;		//�õ��ܵĸߵ�ƽ��ʱ��
				TIM8CH2_CAPTURE_STA = 0;		//�����־λ����
				TIM_OC2PolarityConfig(TIM8, TIM_ICPolarity_Rising); //����Ϊ�����ز���		  
			}
			else //��������ʱ�䵫�����½��أ���һ�β��������أ���¼��ʱ�Ķ�ʱ������ֵ
			{
				TIM8CH2_CAPTURE_UPVAL = TIM_GetCapture2(TIM8);		//��ȡ����������
				TIM8CH2_CAPTURE_STA |= 0X40;		//����Ѳ���������
				TIM_OC2PolarityConfig(TIM8, TIM_ICPolarity_Falling);//����Ϊ�½��ز���
			}
		}
	}
}

void TIM8_UP_IRQHandler(void) 
{ 
// 
	TIM_ClearITPendingBit(TIM8,TIM_IT_Update);  
}
/////////////////////////////////////////////////////
struct timer_Info _timer_info[TIMER_NUM];

void Config_TIMER(uint8_t timer, uint8_t pri, uint32_t fry)
{
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure; 
	TIM_TimeBaseInitTypeDef  timer_struct;
	RCC_ClocksTypeDef RCC_Clocks;
	
	uint32_t CCR1_Val = 0;
	
	if(timer < TIMER_NUM)
	{
		if(_timer_info[timer]._isInit == 1)  //˵���Ѿ���ʼ��
		return;
		else
		_timer_info[timer]._isInit = 1;  
	}
	else
	return;
  /* Configure one bit for preemption priority */
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	if(timer <= TIMER_4)
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn + timer;  // interrupt channel.
	else if(timer == TIMER_5)
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
	else if(timer == TIMER_7)
	NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);  
	//get system clock.
	RCC_GetClocksFreq(&RCC_Clocks); 
	//timer config.
	_timer_info[timer]._frequency = fry ;
	timer_struct.TIM_Period = 1000000 / _timer_info[timer]._frequency  - 1; //
	timer_struct.TIM_Prescaler = (RCC_Clocks.HCLK_Frequency / 1000000) - 1;   //
	timer_struct.TIM_ClockDivision = 0x0;  	                                //
	timer_struct.TIM_CounterMode = TIM_CounterMode_Up; 	                    //
	//oc  
	// CCR1_Val = fry >> 1;                                   //�ж����� / 2.
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Inactive;                     //
	TIM_OCInitStructure.TIM_Pulse = CCR1_Val;  
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;                 //
  
	switch(timer)
	{		
		case TIMER_5:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
			TIM_TimeBaseInit(TIM5, &timer_struct);
			
			TIM_OC1Init(TIM5, &TIM_OCInitStructure);
			TIM_OC1PreloadConfig(TIM5, TIM_OCPreload_Disable);	//
			TIM_ARRPreloadConfig(TIM5, ENABLE);
			TIM_ClearITPendingBit(TIM5, TIM_IT_CC1| TIM_IT_Update); 
			TIM_ITConfig(TIM5, TIM_IT_CC1 | TIM_IT_Update, ENABLE);
			TIM_Cmd(TIM5, ENABLE); 
			break;	
			
		case TIMER_7:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
			TIM_TimeBaseInit(TIM7, &timer_struct);
			
			TIM_OC1Init(TIM7, &TIM_OCInitStructure);
			TIM_OC1PreloadConfig(TIM7, TIM_OCPreload_Disable);	//
			TIM_ARRPreloadConfig(TIM7, ENABLE);
			TIM_ClearITPendingBit(TIM7, TIM_IT_CC1| TIM_IT_Update); 
			TIM_ITConfig(TIM7, TIM_IT_CC1 | TIM_IT_Update, ENABLE);
			TIM_Cmd(TIM7, ENABLE); 
			break;	
	}
}
/*
get timer info.
*/
struct timer_Info *Get_TimerInfo(uint8_t timer)
{
    return &_timer_info[timer];
}
/*

*/
void Timer_Register(uint8_t timer, CB_VOID function)
{
	uint8_t i;
	if(function == 0) return;
	if(_timer_info[timer]._callback_Num == TIMER_CALLBACK_NUM) return;

	for (i = 0; i < _timer_info[timer]._callback_Num; i++)
	{
		if(_timer_info[timer]._callback[i] == function) return;	
	}

	_timer_info[timer]._callback[_timer_info[timer]._callback_Num] = function;
	_timer_info[timer]._callback_Num++;
}
/*
timer5 interrupt function.
*/
void TIM5_IRQHandler(void)
{
	uint8_t i = 0;
	if (TIM_GetITStatus(TIM5, TIM_IT_CC1) != RESET)
	{
		TIM_ClearITPendingBit(TIM5, TIM_IT_CC1);
	}  
	else if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM5, TIM_IT_Update); 
		i = _timer_info[TIMER_5]._callback_Num;
		while(i > 0)
		{
			i--;
			_timer_info[TIMER_5]._callback[i]();
		}      
	}
}

/*
timer7 interrupt function.
*/
void TIM7_IRQHandler(void)
{
	uint8_t i = 0;
	if (TIM_GetITStatus(TIM7, TIM_IT_CC1) != RESET)
	{
		TIM_ClearITPendingBit(TIM7, TIM_IT_CC1);
	}  
	else if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM7, TIM_IT_Update); 
		i = _timer_info[TIMER_7]._callback_Num;
		while(i > 0)
		{
			i--;
			_timer_info[TIMER_7]._callback[i]();
		}      
	}
}
/////////////////////////////////////////////////////
//timer delay
uint8_t d_module_num = 0, _delay_count_id = 0;

struct delay_info _delay_module[5];

//��������
void DelayLoop(void);
/*

*/
void ConfigDelayModuleNum(uint8_t num, uint8_t timer)
{
	d_module_num = num;
	Timer_Register(timer, DelayLoop);
}

/*
add delay id.
*/
uint8_t GetDelayIdFunction(uint8_t num, uint8_t *return_id)
{
	uint8_t add_success_flag = 0, index = 0;
	if((_delay_count_id + num) < d_module_num)
	{
		for(index = 0; index < num; index ++)
		{
			if(return_id != 0)
			{
				*return_id = _delay_count_id;
				return_id ++;
				_delay_count_id ++;
			}
			else
			return 0;
		}
		add_success_flag = 1;
	}
	else
	return 0;
	return add_success_flag;
}
/*

*/
char SetCountTime(uint8_t id_number, uint16_t time)
{
	if(id_number < d_module_num)
	{
		_delay_module[id_number].Time = time;
		return 1;
	}
	return 0;
}

/*

*/
char StartDelayCount(uint8_t id_number)
{
	if(id_number < d_module_num)
	{
		_delay_module[id_number].StartFlag = 1;
		return 1;
	}
	return 0;
}

char StopDelayCount(uint8_t id_number)
{
	if(id_number < d_module_num)
	{
		_delay_module[id_number].StartFlag = 0;
		return 1;
	}
	return 0;
}

char ReturnSuccess(uint8_t id_number)
{
	if(id_number < d_module_num)
	return _delay_module[id_number].Success;

	return 0;
}

/*

*/
unsigned int ReturnCurCount(uint8_t id_number)
{
	if(id_number < d_module_num)
	return _delay_module[id_number].TimeCount;

	return 0;
}

/*

*/
char ClearDelaySucFlag(uint8_t id_number)
{
	if(id_number < d_module_num)
	{
		_delay_module[id_number].Success = 0;
		_delay_module[id_number].TimeCount = 0;
		return 1;
	}

	return 0;
}

/*

*/
void DelayLoop(void)
{
	uint8_t index = 0;
	for(index = 0; index < d_module_num; index ++)
	{
		if(_delay_module[index].StartFlag)
		{
			_delay_module[index].TimeCount ++;
			if(_delay_module[index].TimeCount >= _delay_module[index].Time)
			{
				_delay_module[index].TimeCount = 0;
				_delay_module[index].Success = 1;
				_delay_module[index].StartFlag = 0; //????
			}
		}
	}
}

/*
end of file.
*/


