#include "Components.h"
#include "Tools.h"


//-----------------------OLED------------------------------------------------------
#if defined COMPONENTS_OLED
	#define GPIO_CONFIGURATION
	
	bool configFinish = false;
	u8 oledRst,oledDc,oledScl,oledSda;
	u8 OLED_GRAM[128][8];
	
	void OLED_Config(char channel, u8 portRst,u8 portDc,u8 portScl,u8 portSda)
	{
		u8 gpioPort = portRst | portDc | portScl | portSda;
		oledRst = portRst;
		oledDc = portDc;
		oledScl = portScl;
		oledSda = portSda;
		
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//使能AFIO时钟
		//推挽输出,2M
		setGPIOConfiguration(channel,gpioPort,GPIO_Mode_Out_PP,GPIO_Speed_2MHz);
		
		configFinish = true;
	}
	
	void OLED_RST_Clr()   //RST
	{
		PCout(oledRst)=0;
	}
	void OLED_RST_Set()   //RST
	{
		PCout(oledRst)=1
	}
	void OLED_RS_Clr()    //DC
	{
		PCout(oledDc)=0;
	}
	void OLED_RS_Set()    //DC
	{
		PCout(oledDc)=1;
	}
	void OLED_SCLK_Clr()  //SCL
	{
		PCout(oledScl)=0;
	}
	void OLED_SCLK_Set()  //SCL
	{
		PCout(oledScl)=1;
	}
	void OLED_SDIN_Clr()  //SDA
	{
		PCout(oledSda)=0;
	}
	void OLED_SDIN_Set()  //SDA
	{
		PCout(oledSda)=1;
	}
	void OLED_Refresh_Gram(void)
	{
		u8 i,n;		    
		for(i=0;i<8;i++)  
		{  
			OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
			OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
			OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
			for(n=0;n<128;n++)OLED_WR_Byte(OLED_GRAM[n][i],OLED_DATA); 
		}   
	}

	//向OLED写入一个字节。
	//dat:要写入的数据/命令
	//cmd:数据/命令标志 0,表示命令;1,表示数据;
	void OLED_WR_Byte(u8 dat,u8 cmd)
	{	
		u8 i;			  
		if(cmd)
			OLED_RS_Set();
		else 
			OLED_RS_Clr();		  
		for(i=0;i<8;i++)
		{			  
			OLED_SCLK_Clr();
			if(dat&0x80)
			OLED_SDIN_Set();
			else 
			OLED_SDIN_Clr();
			OLED_SCLK_Set();
			dat<<=1;   
		}				 		  
		OLED_RS_Set();   	  
	} 

					
	//开启OLED显示    
	void OLED_Display_On(void)
	{
		OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
		OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
		OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
	}
	//关闭OLED显示     
	void OLED_Display_Off(void)
	{
		OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
		OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
		OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
	}		   			 
	//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!	  
	void OLED_Clear(void)  
	{  
		u8 i,n;  
		for(i=0;i<8;i++)for(n=0;n<128;n++)OLED_GRAM[n][i]=0X00;  
		OLED_Refresh_Gram();//更新显示
	}
	//画点 
	//x:0~127
	//y:0~63
	//t:1 填充 0,清空				   
	void OLED_DrawPoint(u8 x,u8 y,u8 t)
	{
		u8 pos,bx,temp=0;
		if(x>127||y>63)return;//超出范围了.
		pos=7-y/8;
		bx=y%8;
		temp=1<<(7-bx);
		if(t)OLED_GRAM[x][pos]|=temp;
		else OLED_GRAM[x][pos]&=~temp;	    
	}

	//在指定位置显示一个字符,包括部分字符
	//x:0~127
	//y:0~63
	//mode:0,反白显示;1,正常显示				 
	//size:选择字体 16/12 
	void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode)
	{      			    
		u8 temp,t,t1;
		u8 y0=y;
		chr=chr-' ';//得到偏移后的值				   
			for(t=0;t<size;t++)
			{   
			if(size==12)temp=oled_asc2_1206[chr][t];  //调用1206字体
			else temp=oled_asc2_1608[chr][t];		 //调用1608字体 	                          
					for(t1=0;t1<8;t1++)
			{
				if(temp&0x80)OLED_DrawPoint(x,y,mode);
				else OLED_DrawPoint(x,y,!mode);
				temp<<=1;
				y++;
				if((y-y0)==size)
				{
					y=y0;
					x++;
					break;
				}
			}  	 
			}          
	}
	//m^n函数
	u32 oled_pow(u8 m,u8 n)
	{
		u32 result=1;	 
		while(n--)result*=m;    
		return result;
	}				  
	//显示2个数字
	//x,y :起点坐标	 
	//len :数字的位数
	//size:字体大小
	//mode:模式	0,填充模式;1,叠加模式
	//num:数值(0~4294967295);	 		  
	void OLED_ShowNumber(u8 x,u8 y,u32 num,u8 len,u8 size)
	{         	
		u8 t,temp;
		u8 enshow=0;						   
		for(t=0;t<len;t++)
		{
			temp=(num/oled_pow(10,len-t-1))%10;
			if(enshow==0&&t<(len-1))
			{
				if(temp==0)
				{
					OLED_ShowChar(x+(size/2)*t,y,' ',size,1);
					continue;
				}else enshow=1; 
				 
			}
			OLED_ShowChar(x+(size/2)*t,y,temp+'0',size,1); 
		}
	} 
	//显示字符串
	//x,y:起点坐标  
	//*p:字符串起始地址
	//用16字体
	void OLED_ShowString(u8 x,u8 y,const u8 *p)
	{
		#define MAX_CHAR_POSX 122
		#define MAX_CHAR_POSY 58          
		while(*p!='\0')
		{       
			if(x>MAX_CHAR_POSX){x=0;y+=16;}
			if(y>MAX_CHAR_POSY){y=x=0;OLED_Clear();}
			OLED_ShowChar(x,y,*p,12,1);	 
			x+=8;
			p++;
		}  
	}	   
	//初始化OLED					    
	void OLED_Init(void)
	{
		if(!configFinish) return;
		
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); //使能PB端口时钟
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//使能AFIO时钟
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;//端口配置
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;      //推挽输出
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;     //2M
		GPIO_Init(GPIOC, &GPIO_InitStructure);					      //根据设定参数初始化GPIO 

		PWR_BackupAccessCmd(ENABLE);//允许修改RTC 和后备寄存器
		RCC_LSEConfig(RCC_LSE_OFF);//关闭外部低速外部时钟信号功能 后，PC13 PC14 PC15 才可以当普通IO用。
		BKP_TamperPinCmd(DISABLE);//关闭入侵检测功能，也就是 PC13，也可以当普通IO 使用
		PWR_BackupAccessCmd(DISABLE);//禁止修改后备寄存器

		OLED_RST_Clr();
		delay_ms(100);
		OLED_RST_Set(); 
							
		OLED_WR_Byte(0xAE,OLED_CMD); //关闭显示
		OLED_WR_Byte(0xD5,OLED_CMD); //设置时钟分频因子,震荡频率
		OLED_WR_Byte(80,  OLED_CMD);   //[3:0],分频因子;[7:4],震荡频率
		OLED_WR_Byte(0xA8,OLED_CMD); //设置驱动路数
		OLED_WR_Byte(0X3F,OLED_CMD); //默认0X3F(1/64) 
		OLED_WR_Byte(0xD3,OLED_CMD); //设置显示偏移
		OLED_WR_Byte(0X00,OLED_CMD); //默认为0

		OLED_WR_Byte(0x40,OLED_CMD); //设置显示开始行 [5:0],行数.
																
		OLED_WR_Byte(0x8D,OLED_CMD); //电荷泵设置
		OLED_WR_Byte(0x14,OLED_CMD); //bit2，开启/关闭
		OLED_WR_Byte(0x20,OLED_CMD); //设置内存地址模式
		OLED_WR_Byte(0x02,OLED_CMD); //[1:0],00，列地址模式;01，行地址模式;10,页地址模式;默认10;
		OLED_WR_Byte(0xA1,OLED_CMD); //段重定义设置,bit0:0,0->0;1,0->127;
		OLED_WR_Byte(0xC0,OLED_CMD); //设置COM扫描方向;bit3:0,普通模式;1,重定义模式 COM[N-1]->COM0;N:驱动路数
		OLED_WR_Byte(0xDA,OLED_CMD); //设置COM硬件引脚配置
		OLED_WR_Byte(0x12,OLED_CMD); //[5:4]配置
			 
		OLED_WR_Byte(0x81,OLED_CMD); //对比度设置
		OLED_WR_Byte(0xEF,OLED_CMD); //1~255;默认0X7F (亮度设置,越大越亮)
		OLED_WR_Byte(0xD9,OLED_CMD); //设置预充电周期
		OLED_WR_Byte(0xf1,OLED_CMD); //[3:0],PHASE 1;[7:4],PHASE 2;
		OLED_WR_Byte(0xDB,OLED_CMD); //设置VCOMH 电压倍率
		OLED_WR_Byte(0x30,OLED_CMD); //[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;

		OLED_WR_Byte(0xA4,OLED_CMD); //全局显示开启;bit0:1,开启;0,关闭;(白屏/黑屏)
		OLED_WR_Byte(0xA6,OLED_CMD); //设置显示方式;bit0:1,反相显示;0,正常显示	    						   
		OLED_WR_Byte(0xAF,OLED_CMD); //开启显示	 
		OLED_Clear();
	}  


#endif
//-----------------------end of OLED------------------------------------------------------

//-----------------------Hall------------------------------------------------------
#ifdef COMPONENTS_HALL
	
	int _TIM4_BaseCounter = 0;
	int _TIM2_BaseCounter = 0;
	int _TIM2_Counter = 0;
	int _TIM4_Counter = 0;

	void Hall_Encoder_Init(GPIOChannelType channel, HallEncoderIndex encoderIdx, u8 portOne,u8 portOther)
	{
		u8 gpioPort = portOne|portOther;
		uint32_t rccChannel = RCC_APB1Periph_TIM2;
		uint8_t IRQChannel = TIM2_IRQn;
		TIM_TypeDef * pTimeType = TIM2;
		if(encoderIdx != First && encoderIdx != Second)
		{
			return;
		}
		else if(encoderIdx == Second)
		{
			rccChannel = RCC_APB1Periph_TIM4;	
			IRQChannel=TIM4_IRQ;
			pTimeType = TIM4;
		}
		setGPIOConfiguration(channel,gpioPort,GPIO_Mode_IN_FLOATING,0);		
		RCC_APB1PeriphClockCmd(rccChannel, ENABLE);
		
		TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  
		TIM_ICInitTypeDef TIM_ICInitStructure;
		NVIC_InitTypeDef  NVIC_InitStructure;
		
		NVIC_InitStructure.NVIC_IRQChannel = IRQChannel;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);

		TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
		TIM_TimeBaseStructure.TIM_Prescaler = 0x0; // 预分频器 
		TIM_TimeBaseStructure.TIM_Period = ENCODER_TIM_PERIOD; //设定计数器自动重装值
		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//选择时钟分频：不分频
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;////TIM向上计数  
		TIM_TimeBaseInit(pTimeType, &TIM_TimeBaseStructure);
		TIM_EncoderInterfaceConfig(pTimeType, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);//使用编码器模式3
		TIM_ICStructInit(&TIM_ICInitStructure);
		TIM_ICInitStructure.TIM_ICFilter = 10;
		TIM_ICInit(pTimeType, &TIM_ICInitStructure);
		TIM_ClearFlag(pTimeType, TIM_FLAG_Update);//清除TIM的更新标志位
		TIM_ITConfig(pTimeType, TIM_IT_Update, ENABLE);
		//Reset counter
		TIM_SetCounter(pTimeType,0);
		TIM_Cmd(pTimeType, ENABLE); 
	}
	
	/**************************************************************************
	函数功能：TIM4中断服务函数
	入口参数：无
	返回  值：无
	**************************************************************************/
	void TIM4_IRQHandler(void)
	{ 		    		  			    
		/*if(TIM4->SR&0X0001)//溢出中断
		{    				   				     	    	
		}				   
		TIM4->SR&=~(1<<0);//清除中断标志位 	
		TIM4 -> CNT=0;*/
		if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
		{
			if(TIM4->CNT <= 0x00FF) _TIM4_BaseCounter += 0xFFFF;
			else if(TIM4->CNT >= 0xFF00) _TIM4_BaseCounter -= 0xFFFF;
			TIM_ClearITPendingBit(TIM4, TIM_IT_Update); 
		}	
	}
	/**************************************************************************
	函数功能：TIM2中断服务函数
	入口参数：无
	返回  值：无
	**************************************************************************/
	void TIM2_IRQHandler(void)
	{ 		    		  			    
		/*if(TIM2->SR&0X0001)//溢出中断
		{    				   				     	    	
		}				   
		TIM2->SR&=~(1<<0);//清除中断标志位 	 
		TIM2 -> CNT=0;	*/
		if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
		{
			if(TIM2->CNT <= 0x00FF) _TIM2_BaseCounter += 0xFFFF;
			else if(TIM2->CNT >= 0xFF00) _TIM2_BaseCounter -= 0xFFFF;
			TIM_ClearITPendingBit(TIM2, TIM_IT_Update); 
		}	
	}

	int Read_ABS_Value(HallEncoderIndex encoderIdx)
	{
		int tempResult = 0;
		if(encoderIdx == First)
		{
			tempResult = _TIM2_Counter = _TIM2_BaseCounter + (int)(TIM2->CNT);
		}
		else if(encoderIdx == Second)
		{
			tempResult = _TIM4_Counter = _TIM4_BaseCounter + (int)(TIM4->CNT);
		}
		return tempResult;
	}
#endif
//-----------------------end of Hall------------------------------------------------------
