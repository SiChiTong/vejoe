/*

*/
#include "stm32f10x.h"
#include "sys.h"
#include "Sensor_Test.h"
#include "Check_OverLoad.h"


#define	GPIO_Remap_SWJ_JTAGDisable ((uint32_t)0x00300200)

uint16_t adc_channel_value[6];  //adc sample value.
uint16_t adc_filter_v[6];

void Filter_ADCValue(void);
void Get_Adcs(void);
void Read_BalanceCarMode(void);
void Init_SwitchMode(void);

/**************************************************************************

**************************************************************************/
u8 Way_Angle=2;                             //获取角度的算法，1：四元数  2：卡尔曼  3：互补滤波 
u8 Flag_Qian,Flag_Hou,Flag_Left,Flag_Right,Flag_sudu=2; //蓝牙遥控相关的变量
u8 Flag_Stop=0, Flag_Show=0, Flag_Hover=0;    //停止标志位和 显示标志位 默认停止 显示打开
int Encoder_Left,Encoder_Right;             //左右编码器的脉冲计数
int Moto1,Moto2;                            //电机PWM变量 应是Motor的 
int Temperature;                            //显示温度
int Voltage;                                //电池电压采样相关的变量
float Angle_Balance,Gyro_Balance,Gyro_Turn; //平衡倾角 平衡陀螺仪 转向陀螺仪
float Show_Data_Mb;                         //全局显示变量，用于显示需要查看的数据
u32 Distance;                               //超声波测距
u8 delay_50,delay_flag,Bi_zhang=0,PID_Send,Flash_Send; //延时和调参等变量
float Acceleration_Z;                       //Z轴加速度计  
float Balance_Kp = 300,Balance_Kd=1.0,Velocity_Kp=80,Velocity_Ki=0.4;//PID参数 80 ,0.4
u16 PID_Parameter[10],Flash_Parameter[10];  //Flash相关数组

float Zhongzhi = 0.5;
u32 Remoter_Ch1=1500,Remoter_Ch2=1500;      //航模遥控接收变量

MovementDirection carMoveDirect;

int main(void)
{ 
	//delay_init();	    	            //=====延时函数初始化	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,  ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	Piezoelectric_IOInit();
	Init_SwitchMode();
	uart_init(128000);	            //=====串口初始化为
	LED_Init();                     //=====初始化与 LED 连接的硬件接口
	KEY_Init();                     //=====按键初始化
	MY_NVIC_PriorityGroupConfig(2);	//=====设置中断分组
	MiniBalance_PWM_Init(7199,0);   //=====初始化PWM 10KHZ，用于驱动电机 如需初始化电调接口 
	uart1_init(9600);               //=====串口初始化
	Encoder_Init_TIM2();            //=====编码器接口
	Encoder_Init_TIM4();            //=====初始化编码器                   
	Config_ADC(HardWare_ADC1, 0, 0);//=====adc初始化
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	//------------------------------//
	Config_TIMER(TIMER_5, 3, 1000);
	ConfigDelayModuleNum(5, TIMER_5);	
	//过流保护
	Init_CheckMotorParameter();
	Config_MotorParameter(0, 500, 2500, 1400);
	Set_HardwareParameter(0, 0.001, 14, 9);
	
	Config_MotorParameter(1, 500, 2500, 1400);
	Set_HardwareParameter(1, 0.001, 14, 9);	
	//采样通道，采样过流端的中间值
	while(!Read_OffsetCurValue(0, adc_filter_v[2]))
	{
		Get_Adcs();
	}
	while(!Read_OffsetCurValue(1, adc_filter_v[3]))
	{
		Get_Adcs();
	}
	//-----------------------------//
	delay_ms(500);                  //=====延时
	IIC_Init();                     //=====IIC初始化
	MPU6050_initialize();           //=====MPU6050初始化	
	DMP_Init();                     //=====初始化DMP 
	OLED_Init();                    //=====OLED初始化	    
	TIM3_Cap_Init(0XFFFF,72-1);	    //=====超声波初始化
//	TIM8_Cap_Init(0XFFFF,72-1);	    //=====航模遥控接收初始化
	CAN1_Mode_Init(1,2,3,6,0);		//=====CAN初始化,波特率1Mbps   
	MiniBalance_EXTI_Init();        //=====MPU6050 5ms定时中断初始化
	initialLabyrinthConfig();
	while(1)
	{
		if(Flash_Send==1)        //写入PID参数到Flash,由app控制该指令
		{
			Flash_Write();	
			Flash_Send=0;	
		}	
		if(Flag_Show==0)        	  //使用MiniBalance APP和OLED显示屏
		{
		//	if(g_bluetooth_flag)
		//	APP_Show();	
			oled_show();          //===显示屏打开
		}
		else                      //使用MiniBalance上位机 上位机使用的时候需要严格的时序，故此时关闭app监控部分和OLED显示屏
		{
			DataScope();          //开启MiniBalance上位机
		}	
		//----------------------//
		Get_Adcs();
		//采样出电池电压和电机过流，如果不在合理范围内直接关闭电机
		Get_CheckSampleValue(0, adc_filter_v[1], adc_filter_v[2], 0);
		Get_CheckSampleValue(1, adc_filter_v[1], adc_filter_v[3], 0);
		
		Get_SampleValue(adc_filter_v[1]);
		Temp_Cal(adc_filter_v[4]);
		Infrared_SensorTest(adc_filter_v[5]);
		Piezoelectric_WeighingSensor();
		Get_Weight();
		
		Display_SampleValue();
		Read_BalanceCarMode();
		//过流过载检测s
		Check_DeviceApp();
	//	delay_flag=1;	
	//	delay_50=0;
	//	while(delay_flag);	     //通过MPU6050的INT中断实现的50ms精准延时	
		
		labyrinthMain();	//迷宫算法
	} 
}

/*

*/
void Get_Adcs(void)
{
	Get_ADCSampleValue(ADC_Channel_3, &adc_channel_value[0]);
	Get_ADCSampleValue(ADC_Channel_4, &adc_channel_value[1]);
	Get_ADCSampleValue(ADC_Channel_5, &adc_channel_value[2]);
	Get_ADCSampleValue(ADC_Channel_6, &adc_channel_value[3]);
	Get_ADCSampleValue(ADC_Channel_7, &adc_channel_value[4]);
	Get_ADCSampleValue(ADC_Channel_14,&adc_channel_value[5]);	
	Filter_ADCValue();
}
/*

*/
uint16_t adc_sample_value[6][32];

void Filter_ADCValue(void)
{
	uint8_t index = 0;
	for(index = 0; index < 6; index ++)
	{
		adc_sample_value[index][31] = adc_channel_value[index];
		Move_Array(32, adc_sample_value[index]);
		adc_filter_v[index] = Average_Filter(32, adc_sample_value[index]);
	}
}
/*
读取拨码开关，PD3 PB2
*/
void Init_SwitchMode(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //使能端口时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;	            //端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;         //上拉输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);	

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //使能端口时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	            //端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;         //上拉输入
	GPIO_Init(GPIOD, &GPIO_InitStructure);		
}

/*

*/
void Read_BalanceCarMode(void)
{
	//普通模式
	if((GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3)) && (GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_2)))
	{
		Bi_zhang = 0;
	}
	//跟随模式
	else if((GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3) == 0) && (GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_2)))
	{
		Bi_zhang = 2;
	}
	//蔽障模式
	else if((GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3)) && (GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_2) == 0))
	{
		Bi_zhang = 1;
	}
}
/*
end of file.
*/

