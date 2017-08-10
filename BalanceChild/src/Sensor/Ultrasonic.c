#include "Ultrasonic.h"

void InitialUltrasonic(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//ʹ��TIM3ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE);  //ʹ��GPIOBʱ��

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 	//PB0 ����  
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1;     //PB1 ���
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;    
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//��ʼ����ʱ��3 TIM3	 
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = 0XFFFF; 	//�趨�������Զ���װֵ 
	TIM_TimeBaseStructure.TIM_Prescaler =72-1; 	//Ԥ��Ƶ��   
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

	//��ʼ��TIM3���벶�����
	TIM_ICInitTypeDef  TIM3_ICInitStructure;
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

u16 TIM3CH3_CAPTURE_STA,TIM3CH3_CAPTURE_VAL;
u32 readDistance(void)
{
	u32 distance;
	if(TIM3CH3_CAPTURE_STA&0X80)//�ɹ�������һ�θߵ�ƽ
	{
		distance=TIM3CH3_CAPTURE_STA&0X3F;
		distance*=65536;					        //���ʱ���ܺ�
		distance+=TIM3CH3_CAPTURE_VAL;		//�õ��ܵĸߵ�ƽʱ��
		distance=distance*170/1000;				//�����ٶ�340����������2
		//	printf("%d \r\n",Distance);
		TIM3CH3_CAPTURE_STA=0;			//������һ�β���
	}		
	return distance;
}

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
				}else 
				{
					TIM3CH3_CAPTURE_STA++;
				}
			}	 
		}
		if(tsr&0x08)//����3���������¼�
		{	
			if(TIM3CH3_CAPTURE_STA&0X40)		//����һ���½��� 		
			{	  			
				TIM3CH3_CAPTURE_STA|=0X80;		//��ǳɹ�����һ�θߵ�ƽ����
				TIM3CH3_CAPTURE_VAL=TIM3->CCR3;	//��ȡ��ǰ�Ĳ���ֵ.
				TIM3->CCER&=~(1<<9);			//CC1P=0 ����Ϊ�����ز���
			}
			else  								//��δ��ʼ,��һ�β���������
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
