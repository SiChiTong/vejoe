#ifndef DEVICE_H
#define DEVICE_H

#include "Config.h"

//-----------------------内存管理函数---------------------------------------------
#ifdef USE_MEM_MANAGER
struct MY_MEM
{
	UINT8* _memBlock;
	UINT16 _memSize;
	UINT16 _memLeft;
	BOOL _memEmpty;
};

void CreatBuffer(UINT8* src, UINT16 len);
void *My_malloc(UINT16 len);
void My_memset(UINT8* src, UINT16 len, UINT8 value);
void My_free(void* p);
struct MY_MEM Get_Mem_Info(void);
#endif
//-----------------------End of 内存管理函数---------------------------------------

//-----------------------GPIO---------------------------------------------
#ifdef USE_GPIO
void SET_GPIO_OUTPUT01(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void SET_GPIO_INPUT01(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
#endif 
//-----------------------End of GPIO----------------------------------------	

//-----------------------SYS_TICK---------------------------------------------
#ifdef USE_SYS_TICK
#define SYSTICK_CALLBACK_NUM			15		//系统定时器最大常规任务数
#define SYS_TIMER									0xff
struct sysTick_Info
{
	UINT32 _frequency;
	UINT8 _callback_Num;
	CB_VOID _callback[SYSTICK_CALLBACK_NUM];
	BOOL _isInit;
};

void Config_SysTick(UINT32 frequency);   //配置系统时钟中断
void SysTick_Register(CB_VOID function); //回调函数注册
struct sysTick_Info *Get_SysTickInfo(void);

void SysTick_Handler(void);              //中断函数
#endif  
//-----------------------End of SYS_TICK----------------------------------------	

//-----------------------TIMER_TICK---------------------------------------------
#ifdef USE_TIMER_TICK
#define TIMERTICK_CALLBACK_NUM			15	//系统定时器最大常规任务数
enum
{
		TIMER_2 = 0,
		TIMER_3,
		TIMER_4,
		TIMER_NUM
}; 

struct timer_Info
{		
	BOOL    _isInit;
	UINT8   _callback_Num;
	UINT32  _frequency;
	CB_VOID _callback[TIMERTICK_CALLBACK_NUM];
};

void Config_TIMER(UINT8 timer, UINT8 pri, UINT32 fcy);
void Timer_Register(UINT8 timer, CB_VOID function);
struct timer_Info *Get_TimerInfo(UINT8 timer);

//interrupt.
#ifdef USE_TIMER2_TICK
void TIM2_IRQHandler(void);              //中断函数
#endif

#ifdef USE_TIMER3_TICK
void TIM3_IRQHandler(void);              //中断函数
#endif  

#ifdef USE_TIMER4_TICK
void TIM4_IRQHandler(void);              //中断函数
#endif  

#endif  
//-----------------------End of TIMERTICK----------------------------------------	

//-----------------------TIMER1_CPWM---------------------------------------------
#ifdef USE_TIMER1_CPWM
void Set_T1PWM1L_Pulse(UINT16 width);
void Set_T1PWM2L_Pulse(UINT16 width);
void Set_T1PWM1L_Percent(UINT8 percent);
void Set_T1PWM2L_Percent(UINT8 percent);
void Init_T1ComplementaryPWM(UINT32 pwm_fcy);
UINT16 Get_T1PWMResolution(void);
#endif 
//-----------------------End of TIMER1_CPWM----------------------------------------	

//-----------------------USE_TIMER2/TIMER4_ENCODER-----------------------------------
#ifdef USE_TIMER2_ENCODER
void Encoder_Init_TIM2(void);
INT32 Read_Encoder_TIM2(void);
void TIM2_IRQHandler(void);
#endif 

#ifdef USE_TIMER4_ENCODER
void Encoder_Init_TIM4(void);
INT32 Read_Encoder_TIM4(void);
void TIM4_IRQHandler(void);
#endif 
//-----------------------End of USE_TIMER2/TIMER4_ENCODER------------------------------	

//-----------------------USE_ADC-------------------------------------------------------
#ifdef USE_ADC
struct ADC_Info
{
	GPIO_TypeDef* _GPIOx;
	uint16_t _GPIO_Pin;
	uint8_t _ADC_Channel;
	UINT16 _adc_value_buffer[8];
	UINT16 _adc_value;
	UINT8 _index;
};

typedef struct
{
	u16 GPIOPin;
	u8 	ADCChannel;
	GPIO_TypeDef* GPIOx;
} StructAdcChannelInfo;

void DeviceADCInitial(StructAdcChannelInfo channelInfo[],u8 channelCount);
//void Add_ADC_CH(UINT8 index, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint8_t ADC_Channel);
//void Enable_ADC(void);
UINT16 Get_ADC_Value(UINT8 index);
void DMA1_Channel1_IRQHandler(void);
#endif 
//-----------------------End of USE_ADC--------------------------------------------------	
#endif

