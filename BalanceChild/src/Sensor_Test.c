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
float d = 0;
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

/*

*/
float Return_InfraredDistance(void)
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
	static uint8_t refresh_flag = 0;
	if(refresh_flag >= 100)  //100ms convert once.
	{
		refresh_flag = 0;
		//判断是否转换完成
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4) == 0)
		{
			Delay_us(10);
			for(index = 0; index < 24; index ++)
			{
				g_piez_temp <<= 0x01;
				GPIO_SetBits(GPIOB, GPIO_Pin_5);
				Delay_us(10);
				GPIO_ResetBits(GPIOB, GPIO_Pin_5);
				g_piez_temp |= GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4);
			}
			//给一个信号脉冲
			GPIO_SetBits(GPIOB, GPIO_Pin_5);
			Delay_us(10);
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
	OLED_ShowNumber(0,10, g_check_temperature/10,2,12);
	OLED_ShowNumber(23,10,g_check_temperature%10,1,12);
	OLED_ShowString(13,10,".");
	OLED_ShowString(35,10,"C");
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

