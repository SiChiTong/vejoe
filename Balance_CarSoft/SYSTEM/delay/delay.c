#include "delay.h"

static volatile u32 TimingDelay=0;  

u16 oneDirectDistance,otherDirectDistance;

void SysTick_Handler(void)
{	
	if (TimingDelay > 0x00)  
  {  
    TimingDelay--;    
  }  
	switch(labCarStatus)
	{
		case labStatusMovingForward:
		{
			Flag_Qian=1;
			Flag_Hou=0;
			Flag_Left=0;
			Flag_Right=0;
			
			if(CheckMovingForwardIsEnd())
			{
				Flag_Qian=0;
				Flag_Left=1;
				labCarStatus = labStatusCheckingOneDirection;
				calcPulseForTurnSemiCircle();
			}
		}
		break;
		case labStatusCheckingOneDirection:
		{
			if(CheckTurningIsEnd())
			{
				oneDirectDistance = Distance;
				labCarStatus = labStatusGetOtherDistantce;
				calcPulseForTurnRound();
			}
		}
		break;
		case labStatusGetOtherDistantce:
		{
			if(CheckTurningIsEnd())
			{
				otherDirectDistance = Distance;
				if(oneDirectDistance > otherDirectDistance + 0xFF)
				{//距离相差0xff毫米，作为误差过滤
					calcPulseForTurnRound();
					labCarStatus = labStatusTurningRound;
				}
				else
				{//当前方向即为目标方向					
					Flag_Qian=1;
					Flag_Left=0;
					labCarStatus = labStatusMovingForward;
				}
			}
		}
		break;
		case labStatusTurningRound:
		{			
			if(CheckTurningIsEnd())
			{
				labCarStatus = labStatusMovingForward;
			}
		}
		break;
		default:break;
	}
}

void delay_init()
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	
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








































