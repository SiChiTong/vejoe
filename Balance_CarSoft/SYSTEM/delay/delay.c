#include "delay.h"

static volatile u32 TimingDelay=0;  


void SysTick_Handler(void)
{	
	if (TimingDelay > 0x00)  
  {  
    TimingDelay--;    
  }  
}

void delay_init()
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	
//	NVIC_InitTypeDef NVIC_InitStructure;
//	NVIC_InitStructure.NVIC_IRQChannel = SysTick_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
	
	while( SysTick_Config( 72000 )  != 0  );
}								    

void delay_us(u32 nus)
{
	int i=72;
	while(i>0) i--;
}

void delay_ms(u32 nms)
{
	TimingDelay = nms; 
  while(TimingDelay != 0);   
}








































