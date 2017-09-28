#ifndef _COMPONENTS_H
#define _COMPONENTS_H

#include "Config.h"

//-----------------------OLED------------------------------------------------------
#ifdef COMPONENTS_OLED

	#define IO_ADDRES_CONFIGURATION	
	#define GPIO_CONFIGURATION
	#include "Tools.h"
	
	#define OLED_CMD  0	//д����
	#define OLED_DATA 1	//д����
	
	//OLED�����ú���
	void OLED_Display_On(void);
	void OLED_Display_Off(void);
	void OLED_Refresh_Gram(void);		   				   		    
	void OLED_Init(GPIOChannelType channel, u8 portRst,u8 portDc,u8 portScl,u8 portSda);
	void OLED_Clear(void);
	void OLED_DrawPoint(u8 x,u8 y,u8 t);
	void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode);
	void OLED_ShowNumber(u8 x,u8 y,u32 num,u8 len,u8 size);
	void OLED_ShowNumber2(u8 x,u8 y,int num,u8 len,u8 size);
	void OLED_ShowString(u8 x,u8 y,const u8 *p);	

#endif
//-----------------------end of OLED------------------------------------------------------

//-----------------------Hall------------------------------------------------------
#ifdef COMPONENTS_HALL	
	
	#define GPIO_CONFIGURATION
	#define USE_FILTER
	#define DATA_ARRAY_HANDLE
	#include "Tools.h"
	#include "Device.h"
	
	//����������
	typedef enum
	{
		HallEncoderLeftWheel = 0x00,
		HallEncoderRightWheel
	}HallEncoderIndex;

	void TIM4_IRQHandler(void);
	void TIM2_IRQHandler(void);
	
	void HallEncoderInit(GPIOConfigStruct * channelInfo, u8 channelCount, HallEncoderIndex encoderIdx);
	int Read_ABS_Value(HallEncoderIndex);
	//�ٶȼ���
	void HallSpeedInitial(void);
	int getHallChangeSpeed(HallEncoderIndex encoderIdx);
	
#endif
//-----------------------end of Hall------------------------------------------------------

//----------------------- ��� ------------------------------------------------------
#ifdef COMPONENTS_MOTOR	
	#define PWM_EXTREME_VALUE 				6900	
	typedef struct
	{		
		GPIOChannelType forwardType;
		u8 forwardGPIOPin;
		GPIOChannelType backwardType;
		u8 backwardGPIOPin;		
	} WheelGPIOInfo;
	void PWMBalanceCarInitial(GPIOConfigStruct channelsMotor[],u8 channelMotorCount,WheelGPIOInfo wheelConfig[],GPIOConfigStruct channelsPwm[],u8 channelPwmCount,u16 period,u16 prescaler);
	void SetPwmValue(int leftPwm,int rightPwm);
#endif
//-----------------------end of ��� ------------------------------------------------------

//----------------------- �豸��ȫ��� ------------------------------------------------------
#ifdef DEVICE_SAFETY_CHECK
	#define		ADC_BUF_SIZE			8
	#define 	ADC_VALUE_COUNT		6
	
	typedef struct 
	{
		u8 index;
		u8 weightFilterIdxArray[ADC_VALUE_COUNT];
		u8 averageFilterIdxArray[ADC_VALUE_COUNT];
		u16  adcSourceValuesArray[ADC_VALUE_COUNT];
		u16  adcWeightFilterValuesArray[ADC_VALUE_COUNT];
		u16  adcFilterResultValuesArray[ADC_VALUE_COUNT];
		
		HANDLER08 callback;
	}StructAdcInfo;
	
	enum
	{
		HardWare_ADC1 = 0,
		HardWare_ADC2,
		ADC_NUM
	};	
	typedef struct 
	{
		u8  overload_flag;				//���ر�־
		u8	overcurrent_flag;			//������־
		u8  overvoltage_flag;			//��ѹ��־
		u8  undervoltage_flag;			//Ƿѹ��־
		u8  overload_time_flag;		//���ؼ�ʱ��ʼ
		u8  overcurrent_time_flag;		//������ʱ
		u8  overvoltage_time_flag;
		u8  undervoltage_time_flag;
		
		u8  weightFilterIdx;		//Ȩֵ�˲�����
		
		u8  offset_delay_id;			
		u8  tolerant_cnt;				//
		u16 load_error_time;			//����ʱ��
		u16 free_load_error_time;		//free error ʱ��
		u16 free_cur_error_time;
		u16 current_error_time;
		u16 overvoltage_error_time;		//
		u16 undervoltage_error_time;
		//��ת����
		u16 stall_current;			//��ת����
		u16 stall_time;				  //��תʱ��
		u16 free_time;					//�ͷ�ʱ��
		u16 stall_cmp;					//��ת�����ۼƲ�����Ļ��ֺ�
		u16 stall_sum;					//��ת���� + �м�ƫ�õ�ѹֵ
		
		u16 voltage_adc;				//��ѹADC��ֵ
		u16 current_adc;				//����ADC��ֵ
		u16 encode_value;				//��������ʵʱ��ȡֵ
		u16 cur_diff_default;			//��������ƫ��ֵ
		u16 max_voltage;				//��С��ѹֵ
		u16 min_voltage;				//����ѹֵ
		//---------------------//
		u16 cur_waring_value;			//������ֵ
		u16 cur_last_value;			//��һ�ε���ֵ
		u16 cur_offset_dc;				//�������ƫ��֮��
		int 	 cur_offset_sum;			//
		int		 cur_offset;				//  
		u16 cur_adc_buffer[ADC_BUF_SIZE];//����ADC buf
		//---------------------//
		float    voltage_ratio;				//��ѹ�ķ�ѹ����
	}StructCheckOverload;
	
	typedef struct
	{
		//��Ϣһ��������ָ�
		u16 freeTime;
		//����������ʾ�쳣
		u16 stallTime;
		u16 stallCurrent;
		float voltageRatio;
		u16 maxWorkVoltage;
		u16 minWorkVoltage;
	}StructMotorSafeInfo;	
	
	typedef	struct 
	{
		u8 StartFlag;
		u8 Success;
		u16 Time;
		u16 TimeCount;
	}StructAdcDelayInfo;
	
	void FilterADCValue(void);
	void GetVolCurValue(u16 * batteryVoltage, u16 * leftCurrent, u16 * rightCurrent);
	void UpdateVolCurValue(u8 voltIdx, u8 leftCurIdx,u8 rightCurIdx);
	void updateCurrentZeroDrift(u8 leftCurrentAdcIdx, u8 rightCurrentAdcIdx);
	void ReadOffsetCurrentValue(u8 chkIdx,u8 adcInfoIdx);
	void motorSafetyCheckInitital(StructMotorSafeInfo initialInfo[],u8 infoCount);
	void GeneralSafetyCheck(void);
#endif
//-----------------------end of �豸��ȫ��� ------------------------------------------------------

#endif   // _COMPONENTS_H

