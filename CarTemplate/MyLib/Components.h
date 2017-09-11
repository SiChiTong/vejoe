#ifndef _COMPONENTS_H
#define _COMPONENTS_H

//-----------------------OLED------------------------------------------------------
#ifdef COMPONENTS_OLED
	#define OLED_CMD  0	//写命令
	#define OLED_DATA 1	//写数据
	
	private:
	
	void OLED_RST_Clr();   //RST
	void OLED_RST_Set();   //RST
	void OLED_RS_Clr();    //DC
	void OLED_RS_Set();    //DC
	void OLED_SCLK_Clr();  //SCL
	void OLED_SCLK_Set();  //SCL
	void OLED_SDIN_Clr();  //SDA
	void OLED_SDIN_Set();  //SDA
	
	void OLED_WR_Byte(u8 dat,u8 cmd);	    
	
	public:
	void OLED_Config(char channel, u8 portRst,u8 portDc,u8 portScl,u8 portSda);
	//OLED控制用函数
	void OLED_Display_On(void);
	void OLED_Display_Off(void);
	void OLED_Refresh_Gram(void);		   				   		    
	void OLED_Init(void);
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
	
	//编码器索引
	typedef enum
	{
		First = 0x02,
		Second = 0x04
	}HallEncoderIndex;
	
	private:
	void TIM4_IRQHandler(void);
	void TIM2_IRQHandler(void);
	
	public:
	void Hall_Encoder_Init(GPIOChannelType channel, HallEncoderIndex encoderIdx, u8 portOne,u8 portOther));
	int Read_ABS_Value(HallEncoderIndex);
	
#endif
//-----------------------end of Hall------------------------------------------------------
#endif   // _COMPONENTS_H

