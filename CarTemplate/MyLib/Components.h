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
	
	//编码器索引
	typedef enum
	{
		HallEncoderLeftWheel = 0x00,
		HallEncoderRightWheel
	}HallEncoderIndex;

	void TIM4_IRQHandler(void);
	void TIM2_IRQHandler(void);
	
	void HallEncoderInit(GPIOConfigStruct * channelInfo, u8 channelCount, HallEncoderIndex encoderIdx);
	int Read_ABS_Value(HallEncoderIndex);
	//速度计算
	void HallSpeedInitial(void);
	int getHallChangeSpeed(HallEncoderIndex encoderIdx);
	
#endif
//-----------------------end of Hall------------------------------------------------------

//----------------------- 电机 ------------------------------------------------------
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
//-----------------------end of 电机 ------------------------------------------------------

//----------------------- 设备安全检查 ------------------------------------------------------
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
		u8  overload_flag;				//过载标志
		u8	overcurrent_flag;			//过流标志
		u8  overvoltage_flag;			//过压标志
		u8  undervoltage_flag;			//欠压标志
		u8  overload_time_flag;		//过载计时开始
		u8  overcurrent_time_flag;		//过流计时
		u8  overvoltage_time_flag;
		u8  undervoltage_time_flag;
		
		u8  weightFilterIdx;		//权值滤波索引
		
		u8  offset_delay_id;			
		u8  tolerant_cnt;				//
		u16 load_error_time;			//错误时间
		u16 free_load_error_time;		//free error 时间
		u16 free_cur_error_time;
		u16 current_error_time;
		u16 overvoltage_error_time;		//
		u16 undervoltage_error_time;
		//堵转保护
		u16 stall_current;			//堵转电流
		u16 stall_time;				  //堵转时间
		u16 free_time;					//释放时间
		u16 stall_cmp;					//堵转电流累计差产生的积分和
		u16 stall_sum;					//堵转电流 + 中间偏置电压值
		
		u16 voltage_adc;				//电压ADC的值
		u16 current_adc;				//电流ADC的值
		u16 encode_value;				//编码器的实时读取值
		u16 cur_diff_default;			//电流绝对偏差值
		u16 max_voltage;				//最小电压值
		u16 min_voltage;				//最大电压值
		//---------------------//
		u16 cur_waring_value;			//最大电流值
		u16 cur_last_value;			//上一次电流值
		u16 cur_offset_dc;				//电流最大偏差之和
		int 	 cur_offset_sum;			//
		int		 cur_offset;				//  
		u16 cur_adc_buffer[ADC_BUF_SIZE];//电流ADC buf
		//---------------------//
		float    voltage_ratio;				//电压的分压比率
	}StructCheckOverload;
	
	typedef struct
	{
		//休息一定次数后恢复
		u16 freeTime;
		//超出次数表示异常
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
//-----------------------end of 设备安全检查 ------------------------------------------------------

#endif   // _COMPONENTS_H

