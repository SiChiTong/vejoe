#ifndef _COMPONENTS_H
#define _COMPONENTS_H

#include "Config.h"

//-----------------------OLED------------------------------------------------------
#ifdef COMPONENTS_OLED

	#define IO_ADDRES_CONFIGURATION	
	#define GPIO_CONFIGURATION
	#include "Tools.h"
	
	#define OLED_CMD  0	//写命令
	#define OLED_DATA 1	//写数据
	
	//OLED控制用函数
	void OLED_Display_On(void);
	void OLED_Display_Off(void);
	void OLED_Refresh_Gram(void);		   				   		    
	void OLED_Init(GPIOChannelType channel, u8 portRst,u8 portDc,u8 portScl,u8 portSda);
	void OLED_Clear(void);
	void OLED_DrawPoint(u8 x,u8 y,u8 t);
	void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode);
	void OLED_ShowNumber(u8 x,u8 y,u32 num,u8 len,u8 size);
	void OLED_ShowString(u8 x,u8 y,const u8 *p);	

#endif
//-----------------------end of OLED------------------------------------------------------

//-----------------------Hall------------------------------------------------------
#ifdef COMPONENTS_HALL	
	
	#define GPIO_CONFIGURATION
	#define DATA_ARRAY_HANDLE
	#include "Tools.h"
	#include "Device.h"
	
	//编码器索引
	typedef enum
	{
		First = 0x02,
		Second = 0x04
	}HallEncoderIndex;

	void TIM4_IRQHandler(void);
	void TIM2_IRQHandler(void);
	
	void Hall_Encoder_Init(GPIOChannelType channel, HallEncoderIndex encoderIdx, u8 portOne,u8 portOther);
	int Read_ABS_Value(HallEncoderIndex);
	
	void HallSpeedInitial(u8 timesFor5ms);
	int getHallChangeSpeed(HallEncoderIndex encoderIdx);
	
#endif
//-----------------------end of Hall------------------------------------------------------
	
#endif   // _COMPONENTS_H

