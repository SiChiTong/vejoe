/*
传感器测试，主要有温度传感器(AD 转换)
超声波传感器   -> 定时器输入捕获
红外测距传感器 -> AD转换
压电传感器	   -> 脉冲读取
//把BMX055模块去掉后，然后加上线性CCD模块。
*/

#include "Sensor_Test.h"
#include "oled.h"

/*
io config.
*/
void IO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
}
//adc

struct adc_info _adc_info[ADC_NUM];

void Move_Array(uint8_t array_bit, u16 *array);
/*
config adc.
*/
void Config_ADC(uint8_t adc, uint16_t channel, HANDLER08 _callback)
{
	ADC_InitTypeDef   ADC_InitStructure;
	GPIO_InitTypeDef  ADC_GPIO_InitStructure;
	_adc_info[HardWare_ADC1].callback = _callback;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;       //单通道与多通道采样转换模式
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel= 6;
	//--------------------------select channel---------------------------//
	/* Route the GPIO */  
	ADC_GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	ADC_GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	/* adc 1, 2  pin configuration */
	if(adc >= HardWare_ADC2)return;
	
	ADC_GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6|
	                                  GPIO_Pin_7; 			
	GPIO_Init(GPIOA, &ADC_GPIO_InitStructure);
	//
	ADC_GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_Init(GPIOC, &ADC_GPIO_InitStructure);	
	//	
	ADC_Init(ADC1, &ADC_InitStructure); 	
	//set sample cycle.
	ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 1, ADC_SampleTime_239Cycles5);	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 2, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 3, ADC_SampleTime_239Cycles5);	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 4, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 5, ADC_SampleTime_239Cycles5);	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_14,6, ADC_SampleTime_239Cycles5);	
	//ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 3, ADC_SampleTime_239Cycles5);	
	/* Enable ADC1 */
	ADC_DMAConfig();
	
	ADC_DMACmd(ADC1, ENABLE);
	ADC_Cmd(ADC1, ENABLE);
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	//START
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
	//start adc convert.
//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

/*
DMA config.
*/
u16 ADC_DataBuf[6];
DMA_InitTypeDef DMA_InitStructure;

void ADC_DMAConfig(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	//enable clock.
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr =(u32)( &(ADC1->DR));		//ADC1数据寄存器
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)ADC_DataBuf;			//获取ADC的数组
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;					//片内外设作源头
	DMA_InitStructure.DMA_BufferSize = 6;								//每次DMA16个数据
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	//外设地址不增加
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;				//内存地址增加
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	//半字
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;			//半字
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;						//普通模式循环
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;					//高优先级
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;						//非内存到内存
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	
	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);						//DMA通道1传输完成中断
	
	/* Enable DMA1 channel1 */
	DMA_Cmd(DMA1_Channel1, ENABLE);
}

/*
DMA channel sample interrupt.
*/
void DMA1_Channel1_IRQHandler(void)
{
	uint8_t index = 0;
	if(DMA_GetITStatus(DMA1_IT_TC1))
	{
		DMA_ClearITPendingBit(DMA1_IT_GL1);
		for(index = 0; index < 6; index ++)
		_adc_info[HardWare_ADC1].adc_value[index] = ADC_DataBuf[index];
	}
}
/*

*/
uint16_t Filter_ADValue[6][16];

void Get_ADCSampleValue(uint8_t channel, uint16_t *value)
{
	if(channel == ADC_Channel_3)
	{
		Move_Array(16, Filter_ADValue[0]);
		Filter_ADValue[0][15] = _adc_info[HardWare_ADC1].adc_value[0];
		*value = Weight_Filter(16, Filter_ADValue[0]);
	}
	else if(channel == ADC_Channel_4)
	{
		Move_Array(16, Filter_ADValue[1]);
		Filter_ADValue[1][15] = _adc_info[HardWare_ADC1].adc_value[1];
		*value = Weight_Filter(16, Filter_ADValue[1]);
	}
	else if(channel == ADC_Channel_5)
	{
		Move_Array(16, Filter_ADValue[2]);
		Filter_ADValue[2][15] = _adc_info[HardWare_ADC1].adc_value[2];
		*value = Weight_Filter(16, Filter_ADValue[2]);
	}
	else if(channel == ADC_Channel_6)
	{
		Move_Array(16, Filter_ADValue[3]);
		Filter_ADValue[3][15] = _adc_info[HardWare_ADC1].adc_value[3];
		*value = Weight_Filter(16, Filter_ADValue[3]);
	}
	else if(channel == ADC_Channel_7)
	{
		Move_Array(16, Filter_ADValue[4]);
		Filter_ADValue[4][15] = _adc_info[HardWare_ADC1].adc_value[4];
		*value = Weight_Filter(16, Filter_ADValue[4]);
	}
	else if(channel == ADC_Channel_14)
	{
		Move_Array(16, Filter_ADValue[5]);
		Filter_ADValue[5][15] = _adc_info[HardWare_ADC1].adc_value[5];
		*value = Weight_Filter(16, Filter_ADValue[5]);
	}
}

/*
adc interrupt.
*/
void ADC1_2_IRQHandler(void)
{	
	uint16_t adc_value = 0;
	//ADC1
	if(ADC_GetITStatus(ADC1, ADC_IT_EOC) != RESET)  
	{ 
		adc_value = ADC_GetConversionValue(ADC1);
		_adc_info[HardWare_ADC1].callback(adc_value);
		ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);  
	}
	//ADC2
	if(ADC_GetITStatus(ADC2, ADC_IT_EOC) != RESET)  
	{ 
		adc_value = ADC_GetConversionValue(ADC2);
		ADC_ClearITPendingBit(ADC2, ADC_IT_EOC); 
		_adc_info[HardWare_ADC2].callback(adc_value);
	}
}

/*
权值滤波
*/
uint16_t Weight_Filter(uint8_t filter_bit, u16 *order_value)
{
	uint8_t index = 0;
	uint16_t sum_check = 0;
	for(index = 0; index < filter_bit; index ++)
	{
		if(&(order_value[index]) != 0)
		sum_check += order_value[index] >> (filter_bit - index);
	}
	return sum_check;
}
/*
average filter.
*/
uint16_t Average_Filter(uint8_t filter_bit, u16 *order_value)
{
	uint8_t index = 0;
	uint32_t sum_check = 0;
	for(index = 0; index < filter_bit; index ++)
	{
		if(&(order_value[index]) != 0)
		sum_check += order_value[index];
	}
	sum_check = sum_check / filter_bit;

	return sum_check;	
}
/*

*/
void Move_Array(uint8_t array_bit, u16 *array)
{
	uint8_t index = 0;
    for(index = 0; index < (array_bit - 1); index ++)
    {
		if((&(array[index]) != 0) && (&(array[index + 1]) != 0))
        array[index] = array[index + 1];
    }
}
/*
温度测试, 温度由NTC传感器，温度与阻值对应关系表
Rt = R * EXP(B * (1 / T1 - 1)/ T2)
Rt 热敏电阻为在T1 下的阻值
R是热敏电阻在T2常温下的标称阻值
B值是热敏电阻的重要参数
EXP是e的n次方
T1 T2都为开尔文温度
*/
//将电阻值转化为ADC的值
const int Temp_ADC[68][2]={
{-5, 3554},{-4, 3392},{-3, 3238},{-2, 3092},{-1, 2953},{0 , 2822}, //所有的值* 100，为热敏电阻值
{1 , 2697},{2 , 2579},{3 , 2467},{4 , 2360},{5 , 2259},{6 , 2163},
{7 , 2071},{8 , 1984},{9 , 1901},{10, 1823},{11, 1748},{12, 1676},
{13, 1608},{14, 1543},{15, 1482},{16, 1423},{17, 1366},{18, 1313},
{19, 1262},{20, 1213},{21, 1166},{22, 1122},{23, 1079},{24, 1038},
{25, 1000},{26,  962},{27,  927},{28,  893},{29,  860},{30,  829},
{31,  799},{32,  771},{33,  743},{34,  717},{35,  692},{36,  668},
{37,  644},{38,  622},{39,  601},{40,  580},{41,  561},{42,  542},
{43,  524},{44,  507},{45,  490},{46,  474},{47,  458},{48,  444},
{49,  429},{50,  416},{51,  402},{52,  390},{53,  377},{54,  366},
{55,  354},{56,  343},{57,  333},{58,  323},{59,  313},{60,  303},//11
{61,  294},{62,  286} //11 * 6 + 2 = 68
};
int g_temp = 0;
/*
将ADC的值转化为电阻值, 根据原理图
x = (4096 *2 / adc - 2) * 1000;
*/	
uint16_t resistance = 0;
int g_check_temperature = 0;
void Temp_Cal(uint16_t temp_adc)
{
	int index = 0;	
	//将ADC的值转化为阻值
	resistance = 8192.0 / temp_adc * 100;
	if(resistance <= Temp_ADC[34][1]) 
	{
		if(resistance <= Temp_ADC[51][1])//51=34+17
		{
			for(index = 51; index < 68; index ++)
			{
				if((resistance < Temp_ADC[index][1]) && (resistance >= Temp_ADC[index + 1][1]))
				{
					g_check_temperature = Temp_ADC[index + 1][0];
					return;
				}
			}
		}
		else
		{
			for(index = 34; index < 51; index ++)
			{
				if((resistance < Temp_ADC[index][1]) && (resistance >= Temp_ADC[index + 1][1]))
				{
					g_check_temperature = Temp_ADC[index + 1][0];
					return;
				}
			}      
		}
	}
	else
	{  
		if(resistance < Temp_ADC[17][1])
		{
			for(index = 17; index < 34; index ++)
			{
				if((resistance < Temp_ADC[index][0]) && (resistance >= Temp_ADC[index + 1][0]))
				{
					g_check_temperature = Temp_ADC[index + 1][0];
					return;
				}
			} 
		}
		else
		{
			for(index = 0; index < 17; index ++)
			{
				if((resistance < Temp_ADC[index][0]) && (resistance >= Temp_ADC[index + 1][0]))
				{
					g_check_temperature = Temp_ADC[index + 1][0];
					return;
				}
			} 
		}    
	}	
}

/*
红外传感器距离测试, 将ADC的值转化为距离,
测试的量程为: 3 ~ 20cm, 看数据手册可以得出,分段来进行模拟
测试方法为标定测试，固定一段距离来测试出传感器的输出电压，通过标定几个坐标后，
来计算出  y = k * x  + b， k,b的值，x 为ADC的采样值; y为测试的距离
--------------------------------分为4段
30~50mm
4cm 3200 2.57v; 4.5cm 2916 2.35v  50mm  2688 
k1 = 0.0176, b1 = 96.32
--------------------------------
50~100mm						//2500
(60mm, 2336) (80, 1824)
k1 = 0.039, b1 = 151
--------------------------------
100~150mm
(100mm , 1424) (150mm, 1000)  //1000
k2 = 0.117, b2 = 266.64
---------------------------------
150~200mm
(160, 1072) (190, 784)		 //750
k2 = 0.104, b2 = 271

*/
uint16_t g_infrared_distance = 0, g_distance_adc = 0;

float k1 = 0.0176, b1 = 96,  k2 = 0.039, b2 = 151;
float k3 = 0.117,  b3 = 266, k4 = 0.104, b4 = 271;
int d = 0;
void Infrared_SensorTest(uint16_t voltage)
{
	if(voltage >= 2500)
	d = b1 - k1 * voltage;
	else if(voltage >= 1600)
	d = b2 - k2 * voltage;
	else if(voltage >= 990)
	d = b3 - k3 * voltage;
	else 
	d = b4 - k4 * voltage;	
}


int ultrasonicDistance = 0;
void calcUltrasonic_SensorTest(void)
{
	Read_Distane();
	ultrasonicDistance = Distance;
}


/*

*/
int Return_InfraredDistance(void)
{
	return d;
}
/*
压电称重传感器, 读取时序为HX711 IC的数据
PB5 -> SCK	 ->OUTPUT
PB4 -> DOUT  ->INPUT,
放入定时器中扫描,
*/
void Delay_us(uint16_t us_number);
/*
io init.
*/
void Piezoelectric_IOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//PB4
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	//
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}
/*

*/
uint32_t g_piez_value = 0, g_piez_temp = 0;
void Piezoelectric_WeighingSensor(void)
{
	uint8_t index = 0;
	static uint16_t refresh_flag = 0;
	if(refresh_flag >= 5)  //100ms convert once.
	{
		refresh_flag = 0;
		//判断是否转换完成
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4) == 0)
		{
			Delay_us(5);
			for(index = 0; index < 24; index ++)
			{
				g_piez_temp <<= 0x01;
				GPIO_SetBits(GPIOB, GPIO_Pin_5);
				Delay_us(5);
				GPIO_ResetBits(GPIOB, GPIO_Pin_5);
				g_piez_temp |= GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4);
			}
			//给一个信号脉冲
			GPIO_SetBits(GPIOB, GPIO_Pin_5);
			Delay_us(5);
			g_piez_value = g_piez_temp ^ 0x800000;
			GPIO_ResetBits(GPIOB, GPIO_Pin_5);
			g_piez_temp = 0;
		}
	}
	else
	GPIO_ResetBits(GPIOB, GPIO_Pin_5);
	
	refresh_flag ++;
}
/*
 将读取的ADC的值转化为重量, 
*/
uint32_t g_weight_init = 0x7ccf50;
int weight = 0;
void Get_Weight(void)
{
	if(g_piez_value > g_weight_init)
	weight = (g_piez_value - g_weight_init) * 1.0 / 104.5; 
	else 
	weight = 0;
}
/*
delay us
*/
void Delay_us(uint16_t us_number)
{
	while(us_number--);
}
/*

*/
void Display_SampleValue(void)
{
//	OLED_ShowNumber(0,0, g_check_temperature/10, 2, 12);
//	OLED_ShowNumber(13,0,g_check_temperature%10, 1, 12);
//	OLED_ShowString(23,0,"C");
//	//红外distance.
//	OLED_ShowNumber(40,0, d/100,    1, 12);
//	OLED_ShowNumber(48,0, d%100/10, 1, 12);
//	OLED_ShowNumber(56,0, d%10,     1, 12);
//	OLED_ShowString(64,0, "MM");
//	//wieght
//	OLED_ShowNumber(90, 0, weight/100, 1, 12);
//	OLED_ShowNumber(98, 0, weight%100/10, 1, 12);
//	OLED_ShowNumber(106,0, weight%10, 1,  12);
//	OLED_ShowString(114,0, "g");	
	
	//当前状态标志
	OLED_ShowNumber(0,0, (int)labCarStatus, 2, 12);
	//超声波距离	
	OLED_ShowNumber(40,0, ultrasonicDistance/1000,    1, 12);
	OLED_ShowNumber(48,0, ultrasonicDistance%1000/100, 1, 12);
	OLED_ShowNumber(56,0, ultrasonicDistance%100/10,     1, 12);	
	OLED_ShowNumber(64,0, ultrasonicDistance%10,     1, 12);
	//前后左右标志
	OLED_ShowNumber(90, 0, Flag_Qian, 1, 12);
	OLED_ShowNumber(98, 0, Flag_Hou, 1, 12);
	OLED_ShowNumber(106,0, Flag_Left, 1,  12);
	OLED_ShowNumber(114,0, Flag_Right, 1,  12);		
}
/*
FFT test.
*/
void FFT_Test(void)
{
	
}

/*
end of file.
*/

