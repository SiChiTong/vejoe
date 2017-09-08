#include "stm32f10x.h"
#include "sys.h"
#include "Check_OverLoad.h"
#include "control.h"

int encoderLeft = 0;
int encoderRight = 0;

uint16_t adc_channel_value[6];  //adc sample value.
uint16_t adc_filter_v[6];

void Filter_ADCValue(void);
void Get_Adcs(void);

u8 Way_Angle=2;                             //��ȡ�Ƕȵ��㷨��1����Ԫ��  2��������  3�������˲� 
u8 Flag_Qian,Flag_Hou,Flag_Left,Flag_Right,Flag_sudu=2; //����ң����صı���
u8 Flag_Stop=0, Flag_Show=0, Flag_Hover=0;    //ֹͣ��־λ�� ��ʾ��־λ Ĭ��ֹͣ ��ʾ��
int Encoder_Left,Encoder_Right;             //���ұ��������������
int Moto1,Moto2;                            //���PWM���� Ӧ��Motor�� 
int Temperature;                            //��ʾ�¶�
int Voltage;                                //��ص�ѹ������صı���
float Angle_Balance,Gyro_Balance,Gyro_Turn; //ƽ����� ƽ�������� ת��������
//ˮƽת�ǹ���
float Yawn = 0;
float Show_Data_Mb;                         //ȫ����ʾ������������ʾ��Ҫ�鿴������
u32 Distance;                               //���������
u8 delay_50,delay_flag,Bi_zhang=0,PID_Send,Flash_Send; //��ʱ�͵��εȱ���
float Acceleration_Z;                       //Z����ٶȼ�  
float Balance_Kp = 300,Balance_Kd=1.0,Velocity_Kp=80,Velocity_Ki=0.4;//PID���� 80 ,0.4
u16 PID_Parameter[10],Flash_Parameter[10];  //Flash�������

float Zhongzhi = 0.5;
u32 Remoter_Ch1=1500,Remoter_Ch2=1500;      //��ģң�ؽ��ձ���

int hallDiffCount = 0;
int targetPulseForTurn;
u8 labyrinthCanRunning = 0;

void MotorControl(void);
 
int main(void)
{ 
	delay_init();	    	            //=====��ʱ������ʼ��	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,  ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	
	MY_NVIC_PriorityGroupConfig(2);	//=====�����жϷ���
	MiniBalance_PWM_Init(7199,0);   //=====��ʼ��PWM 10KHZ������������� �����ʼ������ӿ� 

	Encoder_Init_TIM2();            //=====�������ӿ�
	Encoder_Init_TIM4();            //=====��ʼ��������         
	
	Config_ADC(HardWare_ADC1, 0, 0);//=====adc��ʼ��
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	//------------------------------//
	
	Config_TIMER(TIMER_5, 3, 1000);
	Timer_Register(TIMER_5, MotorControl);
	
	Config_MotorParameter(1, 500, 2500, 1400);
	Set_HardwareParameter(1, 0.001, 14, 9);	

	OLED_Init();     
	
	while(1)
	{
		encoderLeft = Read_ABS_Value_TIM2();
		encoderRight = Read_ABS_Value_TIM4();
		
		OLED_ShowString(00,20,"EncoLEFT");
		if( encoderLeft<0)		OLED_ShowString(80,20,"-"),
		  OLED_ShowNumber(95,20,-encoderLeft,4,12);
		else  OLED_ShowString(80,20,"+"),
		  OLED_ShowNumber(95,20, encoderLeft,4,12);
		
		 OLED_ShowString(00,30,"EncoRIGHT");
		if(encoderRight<0)		  OLED_ShowString(80,30,"-"),
		  OLED_ShowNumber(95,30,-encoderRight,4,12);
		else  OLED_ShowString(80,30,"+"),
		  OLED_ShowNumber(95,30,encoderRight,4,12);	
		
		//=============ˢ��=======================//
		OLED_Refresh_Gram();	
		
		/*if(Flash_Send==1)        //д��PID������Flash,��app���Ƹ�ָ��
		{
			Flash_Write();	
			Flash_Send=0;	
		}	
		if(Flag_Show==0)        	  //ʹ��MiniBalance APP��OLED��ʾ��
		{
		//	if(g_bluetooth_flag)
		//	APP_Show();	
			oled_show();          //===��ʾ����
		}
		else                      //ʹ��MiniBalance��λ�� ��λ��ʹ�õ�ʱ����Ҫ�ϸ��ʱ�򣬹ʴ�ʱ�ر�app��ز��ֺ�OLED��ʾ��
		{
			DataScope();          //����MiniBalance��λ��
		}	
		//----------------------//
		Get_Adcs();
		//��������ص�ѹ�͵��������������ں���Χ��ֱ�ӹرյ��
		Get_CheckSampleValue(0, adc_filter_v[1], adc_filter_v[2], 0);
		Get_CheckSampleValue(1, adc_filter_v[1], adc_filter_v[3], 0);
		
		Get_SampleValue(adc_filter_v[1]);
		Temp_Cal(adc_filter_v[4]);
		Infrared_SensorTest(adc_filter_v[5]);
		calcUltrasonic_SensorTest();
		Piezoelectric_WeighingSensor();
		Get_Weight();
		
		Display_SampleValue();*/
	} 
}


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

void MotorControl(void)
{
}


