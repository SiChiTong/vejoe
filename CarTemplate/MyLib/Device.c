#include "Device.h"

//-----------------------内存管理函数---------------------------------------------
#ifdef USE_MEM_MANAGER
struct MY_MEM _My_mem;

void CreatBuffer(UINT8* src, UINT16 len)
{
	_My_mem._memBlock = src;		
	_My_mem._memSize = len;
	_My_mem._memLeft = len;
	_My_mem._memEmpty = 0;
}

void *My_malloc(UINT16 len)
{
	void* result = 0;
	if(_My_mem._memBlock != 0)
	{
		if((len % 4) != 0) len = len + 4 - (len % 4);	//len不能为奇数
		if(_My_mem._memLeft >= len)
		{
			result = _My_mem._memBlock + _My_mem._memSize - _My_mem._memLeft;
			_My_mem._memLeft -= len;
		}
		else _My_mem._memEmpty = 1;
	}
	return result;
}

void My_memset(UINT8* src, UINT16 len, UINT8 value)
{
	UINT16 i;
	for(i = 0; i < len; i++) *(src + i) = value;
}

void My_free(void* p)
{
	if((UINT8*)p >= _My_mem._memBlock && (UINT8*)p < (_My_mem._memBlock + _My_mem._memSize))
	{
		_My_mem._memLeft = _My_mem._memSize - ((UINT8*)p - _My_mem._memBlock);
	}
}

struct MY_MEM Get_Mem_Info(void) {return _My_mem;}
#endif
//-----------------------End of 内存管理函数---------------------------------------

//-----------------------GPIO---------------------------------------------
#ifdef USE_GPIO

void SET_GPIO_OUTPUT01(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)  //用法 SET_GPIO_OUTPUT01(GPIOC, GPIO_Pin_8);
{
	//使能PC端口时钟
	assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
  if (GPIOx == GPIOA) 			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  else if (GPIOx == GPIOB) 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  else if (GPIOx == GPIOC) 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  else if (GPIOx == GPIOD) 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
  else if (GPIOx == GPIOE) 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE); 
  else if (GPIOx == GPIOF) 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
  else
  {
    if (GPIOx == GPIOG) 		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);
	}
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOx, &GPIO_InitStructure);
}

void SET_GPIO_INPUT01(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)  //用法 SET_GPIO_INPUT01(GPIOC, GPIO_Pin_8);
{
	//使能PC端口时钟
	assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
  if (GPIOx == GPIOA) 			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  else if (GPIOx == GPIOB) 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  else if (GPIOx == GPIOC) 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  else if (GPIOx == GPIOD) 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
  else if (GPIOx == GPIOE) 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE); 
  else if (GPIOx == GPIOF) 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
  else
  {
    if (GPIOx == GPIOG) 		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);
	}
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOx, &GPIO_InitStructure);
}
#endif 
//-----------------------End of GPIO----------------------------------------

//-----------------------SYS_TICK---------------------------------------------
#ifdef USE_SYS_TICK
struct sysTick_Info _sysTick_info;
void Config_SysTick(UINT32 fcy)
{
	RCC_ClocksTypeDef Tick_RCC_Clocks; //clock enable.
	if(_sysTick_info._isInit) return;
	_sysTick_info._frequency = fcy;
                          
	RCC_GetClocksFreq(&Tick_RCC_Clocks);
	SysTick_Config(Tick_RCC_Clocks.HCLK_Frequency / fcy); //
  
	_sysTick_info._isInit = TRUE;
}

void SysTick_Register(CB_VOID function)
{
	UINT8 i;
	if(function == 0) return;
	if(_sysTick_info._callback_Num == SYSTICK_CALLBACK_NUM) return;

	for (i = 0; i < _sysTick_info._callback_Num; i++)
	{
		if(_sysTick_info._callback[i] == function) return;	
	}

	_sysTick_info._callback[_sysTick_info._callback_Num] = function;
	_sysTick_info._callback_Num++;
}

struct sysTick_Info *Get_SysTickInfo(void)
{	
  return &_sysTick_info;
}

//interrupt function.all callback function use timer not > 1ms
void SysTick_Handler(void)
{
    UINT8 i = _sysTick_info._callback_Num;
    while(i > 0)
    {
        i--;
        _sysTick_info._callback[i]();
    }
}

#endif  
//-----------------------End of SYS_TICK----------------------------------------	

//-----------------------TIMER_TICK---------------------------------------------
#ifdef USE_TIMER_TICK
struct timer_Info _timer_info[TIMER_NUM];
//timer:TIM2~TIM5, pri : timer interrupt 优先级, fcy : timer interrupt cycle.
void Config_TIMER(UINT8 timer, UINT8 pri, UINT32 fcy)
{
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure; 
	TIM_TimeBaseInitTypeDef  timer_struct;
	RCC_ClocksTypeDef RCC_Clocks;
	
	UINT32 CCR1_Val = 0;
	if(timer < TIMER_NUM)
	{
		if(_timer_info[timer]._isInit == 1) return; //说明已经初始化
		else	_timer_info[timer]._isInit = 1; 
	}
	else return;
	
  // Configure one bit for preemption priority
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	if(timer <= TIMER_4) NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn + timer;// interrupt channel.
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = pri;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);  
	
	RCC_GetClocksFreq(&RCC_Clocks); //get system clock.
	//timer config.
	timer_struct.TIM_Prescaler = (RCC_Clocks.HCLK_Frequency / 1000000) - 1;
	_timer_info[timer]._frequency = fcy ;
	timer_struct.TIM_Period = 1000000 / _timer_info[timer]._frequency  - 1;
	timer_struct.TIM_ClockDivision = 0x0;  	                              
	timer_struct.TIM_CounterMode = TIM_CounterMode_Up; 	                  
	// CCR1_Val = fcy >> 1;                                   //中断周期 / 2.
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Inactive;                    
	TIM_OCInitStructure.TIM_Pulse = CCR1_Val;  
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;                
  
	switch(timer)
	{
	  case TIMER_2: 	//TIM clock enable
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
		TIM_TimeBaseInit(TIM2, &timer_struct);
		
		TIM_OC1Init(TIM2, &TIM_OCInitStructure);
		TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Disable);	
		TIM_ARRPreloadConfig(TIM2, ENABLE);
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC1| TIM_IT_Update); 
		TIM_ITConfig(TIM2, TIM_IT_CC1 | TIM_IT_Update, ENABLE);
		TIM_Cmd(TIM2, ENABLE); 
		break;
		
	  case TIMER_3:
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
		TIM_TimeBaseInit(TIM3, &timer_struct);
		
		TIM_OC1Init(TIM3, &TIM_OCInitStructure);
		TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Disable);	//
		TIM_ARRPreloadConfig(TIM3, ENABLE);
		TIM_ClearITPendingBit(TIM3, TIM_IT_CC1| TIM_IT_Update); 
		TIM_ITConfig(TIM3, TIM_IT_CC1 | TIM_IT_Update, ENABLE);
		TIM_Cmd(TIM3, ENABLE); 
		break;
		
	  case TIMER_4:
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
		TIM_TimeBaseInit(TIM4, &timer_struct);
		
		TIM_OC1Init(TIM4, &TIM_OCInitStructure);
		TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Disable);	//
		TIM_ARRPreloadConfig(TIM4, ENABLE);
		TIM_ClearITPendingBit(TIM4, TIM_IT_CC1| TIM_IT_Update); 
		TIM_ITConfig(TIM4, TIM_IT_CC1 | TIM_IT_Update, ENABLE);
		TIM_Cmd(TIM4, ENABLE); 
		break;
	}
}

struct timer_Info *Get_TimerInfo(UINT8 timer)
{
    return &_timer_info[timer];
}

void Timer_Register(UINT8 timer, CB_VOID function)
{
	UINT8 i;
	if(function == 0) return;
	if(_timer_info[timer]._callback_Num == TIMERTICK_CALLBACK_NUM) return;

	for (i = 0; i < _timer_info[timer]._callback_Num; i++)
	{
		if(_timer_info[timer]._callback[i] == function) return;	
	}

	_timer_info[timer]._callback[_timer_info[timer]._callback_Num] = function;
	_timer_info[timer]._callback_Num++;
}

#ifdef USE_TIMER2_TICK
void TIM2_IRQHandler(void)
{
	UINT8 i = 0;
	if (TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);
	}  
	else if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update); 
		i = _timer_info[TIMER_2]._callback_Num;
		while(i > 0)
		{
			i--;
			_timer_info[TIMER_2]._callback[i]();
		}      
	}
}
#endif

#ifdef USE_TIMER3_TICK
void TIM3_IRQHandler(void)
{
	UINT8 i = 0;
	if (TIM_GetITStatus(TIM3, TIM_IT_CC1) != RESET)
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);
	}  
	else if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update); 
		i = _timer_info[TIMER_3]._callback_Num;
		while(i > 0)
		{
			i--;
			_timer_info[TIMER_3]._callback[i]();
		}      
	}
}
#endif

#ifdef USE_TIMER4_TICK
void TIM4_IRQHandler(void)
{
	UINT8 i = 0;
	if (TIM_GetITStatus(TIM4, TIM_IT_CC1) != RESET)
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_CC1);
	}  
	else if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update); 
		i = _timer_info[TIMER_4]._callback_Num;
		while(i > 0)
		{
			i--;
			_timer_info[TIMER_4]._callback[i]();
		}      
	}
}
#endif

#endif  
//-----------------------End of TIMER_TICK----------------------------------------	

//-----------------------TIMER1_CPWM---------------------------------------------
#ifdef USE_TIMER1_CPWM
UINT16 g_T1PWM_Resolution = 0;

void Set_T1PWM1LPulse(UINT16 width)
{
	if(width < 2) width = 2;//避免满占空比
	else if(width > g_T1PWM_Resolution) width = g_T1PWM_Resolution;
	TIM1->CCR1 = width;
}

void Set_T1PWM2LPulse(UINT16 width)
{
	if(width < 2) width = 2;
	else if(width > g_T1PWM_Resolution) width = g_T1PWM_Resolution;
	TIM1->CCR2 = width;
}

void Set_T1PWM1L_Percent(UINT8 percent)
{
	Set_T1PWM1LPulse((UINT16)(Get_T1PWMResolution() * percent / 100));
}

void Set_T1PWM2L_Percent(UINT8 percent)
{
	Set_T1PWM2LPulse((UINT16)(Get_T1PWMResolution() * percent / 100));
}

void Init_T1ComplementaryPWM(UINT32 pwm_fcy)
{
	//dath time control set.
	TIM_BDTRInitTypeDef  TIM_BDTRInitStructure;
	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	//PA8, PA9
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//PB13, PB14
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	//Enable Timer1 clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	TIM_DeInit(TIM1);
	
	// Time base configuration
	TIM_TimeBaseStructure.TIM_Period = 4000000 / pwm_fcy - 1; //frc
	g_T1PWM_Resolution = TIM_TimeBaseStructure.TIM_Period;
	TIM_TimeBaseStructure.TIM_Prescaler = 18 - 1;  //预分频1Mhz
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x00;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
 
  // PWM1 Mode configuration: Channel1
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_OutputState  = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_Pulse = (4000000 / pwm_fcy - 1) >> 1;
	//
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;	 //Low
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OCInitStructure.TIM_OCNIdleState= TIM_OCIdleState_Set;
	//需要不同的PWM通道设置不同的占空比
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_Pulse = (4000000 / pwm_fcy - 1) >> 2;	
	TIM_OC2Init(TIM1, &TIM_OCInitStructure);	
	//死区时间段配置
	TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Disable;//TIM_OSSRState_Enable;
	TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Disable;//TIM_OSSIState_Enable;
	//
	TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_OFF;
	TIM_BDTRInitStructure.TIM_DeadTime = 0x20;  //死区时间  a0
	TIM_BDTRInitStructure.TIM_Break = TIM_Break_Disable;  //break disable刹车功能disable.
	TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;  //
	TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
	
	TIM_BDTRConfig(TIM1, &TIM_BDTRInitStructure);
	
	//ENABLE
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);	
	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
	
	//timer1  enable.
	TIM_ARRPreloadConfig(TIM1, ENABLE);
	// TIM1 enable counter 
	TIM_Cmd(TIM1, ENABLE);
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
}

UINT16 Get_T1PWMResolution(void) {return g_T1PWM_Resolution;}
#endif 
//-----------------------End of TIMER1_CPWM----------------------------------------	

//-----------------------USE_TIMER2_ENCODER---------------------------------------------
#ifdef USE_TIMER2_ENCODER
INT32 _TIM2_Counter = 0;
INT32 _TIM2_BaseCounter = 0;
void Encoder_Init_TIM2(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);//使能定时器4的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//使能PB端口时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;	//端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);					      //根据设定参数初始化GPIOB
	
	//TIMER4 interrupt
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	//TIMER4  encode input capture module.
	TIM_DeInit(TIM2);
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	//encode interface.
	TIM_EncoderInterfaceConfig(TIM2, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM_ICStructInit(&TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_ICFilter = 6;
	TIM_ICInit(TIM2, &TIM_ICInitStructure);
	//clear interface flag.	
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	//reset counter;
	TIM2->CNT = 0;
	TIM_Cmd(TIM2, ENABLE);
}

INT32 Read_Encoder_TIM2(void)
{
	_TIM2_Counter = _TIM2_BaseCounter + (INT32)(TIM2->CNT);
	return _TIM2_Counter;
}

void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		if(TIM2->CNT <= 0x00FF) _TIM2_BaseCounter += 0xFFFF;
		else if(TIM2->CNT >= 0xFF00) _TIM2_BaseCounter -= 0xFFFF;
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update); 
	}
}
#endif 
//-----------------------End of USE_TIMER2_ENCODER----------------------------------------	

//-----------------------USE_TIMER4_ENCODER---------------------------------------------
#ifdef USE_TIMER4_ENCODER
INT32 _TIM4_Counter = 0;
INT32 _TIM4_BaseCounter = 0;
void Encoder_Init_TIM4(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);//使能定时器4的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//使能PB端口时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;	//端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);					      //根据设定参数初始化GPIOB
	
	//TIMER4 interrupt
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	//TIMER4  encode input capture module.
	TIM_DeInit(TIM4);
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	//encode interface.
	TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM_ICStructInit(&TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_ICFilter = 6;
	TIM_ICInit(TIM4, &TIM_ICInitStructure);
	//clear interface flag.	
	TIM_ClearFlag(TIM4, TIM_FLAG_Update);
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	//reset counter;
	TIM4->CNT = 0;
	TIM_Cmd(TIM4, ENABLE);
}

INT32 Read_Encoder_TIM4(void)
{
	_TIM4_Counter = _TIM4_BaseCounter + (INT32)(TIM4->CNT);
	return _TIM4_Counter;
}

void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
	{
		if(TIM4->CNT <= 0x00FF) _TIM4_BaseCounter += 0xFFFF;
		else if(TIM4->CNT >= 0xFF00) _TIM4_BaseCounter -= 0xFFFF;
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update); 
	}
}
#endif 
//-----------------------End of USE_TIMER4_ENCODER----------------------------------------	

//-----------------------USE_ADC-------------------------------------------------------
#ifdef USE_ADC
UINT8 _ADC_CH_Num = 0;
struct ADC_Info _ADC_Info[18];	//支持18路
UINT16 _ADC_Buffer[2];

void Add_ADC_CH(UINT8 index, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint8_t ADC_Channel)
{
	_ADC_CH_Num++;
	_ADC_Info[index]._GPIOx = GPIOx;
	_ADC_Info[index]._GPIO_Pin = GPIO_Pin;
	_ADC_Info[index]._ADC_Channel = ADC_Channel;
	_ADC_Info[index]._index = 0;
}
void Enable_ADC(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	
	ADC_InitTypeDef   ADC_InitStructure;
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;       	//单通道与多通道采样转换模式
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel= _ADC_CH_Num;		//ADC的通道数
	//--------------------------select channel---------------------------//
	// Route the GPIO 
	GPIO_InitTypeDef  ADC_GPIO_InitStructure;
	ADC_GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	ADC_GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	for(UINT8 i = 0; i < _ADC_CH_Num; i++)
	{		
		ADC_GPIO_InitStructure.GPIO_Pin = _ADC_Info[i]._GPIO_Pin; 			
		GPIO_Init(_ADC_Info[i]._GPIOx, &ADC_GPIO_InitStructure);
	}
	
	ADC_Init(ADC1, &ADC_InitStructure); 	
	for(UINT8 i = 0; i < _ADC_CH_Num; i++)  //set sample cycle.
	{
		ADC_RegularChannelConfig(ADC1, _ADC_Info[i]._ADC_Channel, i + 1, ADC_SampleTime_239Cycles5);	
	}

	//config_ADC_DMA
	DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	//enable clock.
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr =(u32)( &(ADC1->DR));		//ADC1数据寄存器
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)_ADC_Buffer;					//获取ADC的数组
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;								//片内外设作源头
	DMA_InitStructure.DMA_BufferSize = _ADC_CH_Num;										//每次DMA的数据个数
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	//外设地址不增加
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;						//内存地址增加
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	//半字
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;			//半字
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;										//普通模式循环
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;								//高优先级
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;											//非内存到内存
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	
	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);						//DMA通道1传输完成中断
	DMA_Cmd(DMA1_Channel1, ENABLE);		//Enable DMA1 channel1
	
	//enable ADC
	ADC_DMACmd(ADC1, ENABLE);
	ADC_Cmd(ADC1, ENABLE);
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);		//START
	while(ADC_GetCalibrationStatus(ADC1));
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}


void DeviceADCInitial(StructAdcChannelInfo channelInfo[],u8 channelCount)
{
	for(u8 i=0;i<channelCount;i++)
	{
		Add_ADC_CH(i,channelInfo[i].GPIOx,channelInfo[i].GPIOPin,channelInfo[i].ADCChannel);
	}
	Enable_ADC();
}

UINT16 Get_ADC_Value(UINT8 index)
{
	return _ADC_Info[index]._adc_value;
}

void DMA1_Channel1_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC1))
	{
		for(UINT8 i = 0; i < _ADC_CH_Num; i++)
		{
			_ADC_Info[i]._adc_value = _ADC_Buffer[i];
//			//之前的版本：默认做了一次均值滤波，现在改为独立做滤波
//			_ADC_Info[i]._adc_value_buffer[_ADC_Info[i]._index] = _ADC_Buffer[i];
//			_ADC_Info[i]._index++;
//			if(_ADC_Info[i]._index == 8) _ADC_Info[i]._index = 0;
//			_ADC_Info[i]._adc_value = 0;
//			for(UINT8 j = 0; j < 8; j++)
//			{
//				_ADC_Info[i]._adc_value += _ADC_Info[i]._adc_value_buffer[j];
//			}
//			_ADC_Info[i]._adc_value = (_ADC_Info[i]._adc_value >> 3);
		}
		
		DMA_ClearITPendingBit(DMA1_IT_GL1);
	}
}
#endif 
//-----------------------End of USE_ADC--------------------------------------------------	


