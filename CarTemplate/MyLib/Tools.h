#ifndef _TOOLS_H
#define _TOOLS_H

#include "Config.h"
#include "Device.h"

//-----------------------LED------------------------------------------------------
#if defined USE_LED
	#define LED_OFF					0				//常灭
	#define LED_ON 					1				//常亮
	#define LED_BLINK				2				//闪烁
	#define LED_DIM					3				//渐亮渐灭
	#define LED_CONTROL				4				//外部控制
	
	struct _LED_Info
	{
		UINT8 _LEDState;			//灯的状态
		BOOL _Control;				//决定灯亮还是暗
		CB_SET _LEDhdl;				//控制灯的函数指针
		UINT16 _BlinkCount;			//对应的计数值		
		UINT16 _DimStep;			//对应的计数值
		UINT16 _Tick;				//timer计数值
	};	
	
	void Config_LED(UINT8 number, UINT8 timer);
	void Register_LED(UINT8 index, CB_SET setLED, UINT8 state, UINT16 period);
	void Set_LEDState(UINT8 index, UINT8 state);
	void Set_Control(UINT8 index, BOOL state);
	void Set_LEDFcy(UINT8 index, UINT16 blinkPeriod);
	//用LED状态来显示数字的值，分2进制和4进制两种
	void LED_DisplayBit(UINT16 value);
	void LED_DisplayNumber(UINT16 value);
	
	void _LED_Tick(void);
#endif 
//-----------------------End of LED-----------------------------------------------

//-------------------------------BEEPER-by houzuping------------------------------
#if defined USE_BEEPER
	#define MAX_AUDIO_FREQUENCY		10000 //人类最大音频识别范围是20HZ~20000HZ.
	#define MIN_AUDIO_FREQUENCY     20		

	#define BEEPER_TIME		100 //ms 持续时间，其中一种模式的持续时间.
	#define BEEPER_ON		1
	#define BEEPER_OFF		0

	struct _BEEPER_Info
	{
		UINT8 BeeperState;		//beeper state.
		UINT8 BeeperMode;		//声音模式
		UINT8 StartFlag;		//启动标志
		float Frequency;		//频率
		UINT16 Tick;			//发声时间计数
		UINT16 FcyCount;		//将用户的频率数转化为时间计数
		CB_SET Iohd;			//IO 操作函数
	};
	
	//---------------------------函数声明区--------------------------//
	void Config_Beeper(UINT8 num, UINT8 timer);
	void Config_Beeper_AudioFcy(UINT32 fcy, UINT8 timer, UINT8 pri);
	void RegisterBeeper(UINT8 index, CB_SET IOhd, UINT16 fcy_hz);
	void SetBeeperTime(UINT8 index, UINT16 time);
	void StartBeeper(UINT8 index, UINT8 mode);
	void StopBeeper(UINT8 index);
	
#endif
//-----------------------END of beeper--------------------------------------------

//-----------------------USE_H_BDC_DRIVER---------------------------------------------
#ifdef USE_BDC_H_DRIVER
	void HBDC_Config(UINT8 timer, UINT32 pwm_fcy);
	void HBDC_MoveForward(UINT8 percent);
	void HBDC_MoveBackward(UINT8 percent);
	void HBDC_Stop(void);
#endif 
//-----------------------End of USE_H_BDC_DRIVER----------------------------------------	

//-----------------------USE_FILTER---------------------------------------------
#ifdef USE_FILTER
	struct _Filter_Data16_EX
	{
		UINT8 _filter_type;
		UINT8 _width;
		UINT16 _original_Data;
		UINT32 _sum;
		UINT16 _filtered_Data;
	};
	
	void Filter16_Init(struct _Filter_Data16_EX *filterData, UINT8 type, UINT8 width);
	UINT16 Filter16_GetValue(struct _Filter_Data16_EX *filterData, UINT16 data);
#endif 
//-----------------------End of USE_FILTER----------------------------------------		

//-----------------------USE_CURRENT_MANAGER-------------------------------------------------------
#ifdef USE_CURRENT_MANAGER	
	struct _CUR_Info
	{
		float _coefficient;									//电压与电流的变换系数
		UINT16 _Static_ADC;									//静态时的AD值（电流为0）
		UINT16 _ADC;												//即时的AD值
		UINT16 _Current;										//当前电流值  单位：mA
		UINT16 _Tick;												//timer计数值
		CB_READ_U16 _CurrentHDL;						//读取AD值得函数指针
		UINT8 _FSM;													//模块的状态机
		BOOL _is_Init;											//是否初始化完成
		struct _Filter_Data16_EX _filter;		//滤波方式
	};	
	
	void Config_Current_Manager(UINT8 number, UINT8 timer);
	void Register_Current(UINT8 index, CB_READ_U16 readADC, float coefficient);
	BOOL Start_Current_Manager(void);
	UINT16 Get_Current(UINT8 index);
	
	void _Current_Manager_Tick(void);
#endif 
//-----------------------End of USE_CURRENT_MANAGER--------------------------------------------------	
	
//-----------------------USE_ENCODER_MANAGER---------------------------------------------
#ifdef USE_ENCODER_MANAGER
	struct _Encoder_Manager_Info
	{
		UINT16 _sample_frequency;
		INT32 _buffer[50];
		INT32 _position;
		INT32 _integral;
		INT32 _speed;
		UINT16 _Tick;												//timer计数值
		CB_READ_32 _EncoderHDL;				  		//读取encoder值得函数指针
		UINT8 _FSM;													//模块的状态机
		BOOL _is_Init;											//是否初始化完成
		struct _Filter_Data16_EX _filter;		//滤波方式
	};
	
	void Config_Encoder_Manager(UINT8 number, UINT8 timer);
	void Register_Encoder(UINT8 index, CB_READ_32 readEncoder);
	BOOL Start_Encoder_Manager(void);
	INT32 Get_Position(UINT8 index);
	INT32 Get_Speed(UINT8 index);
	void _Encoder_Manager_Tick(void);
#endif 
//-----------------------End of USE_ENCODER_MANAGER----------------------------------------	
	
//-----------------------USE_PID---------------------------------------------
#ifdef USE_PID
	struct _PID_Info
	{
		float _Kp;
		float _Ki;
		float _Kd;
		float _error;
		float _error_proportion;
		float _error_integral;
		float _error_differential;
		float _control_output;
		float _upper_bound;
		float _lower_bound;
	};
	
	void Config_PID(struct _PID_Info *PID_obj, float kp, float ki, float kd, float ub, float lb);
	float Get_PID_Output(struct _PID_Info *PID_obj, float error);
#endif 
//-----------------------End of USE_PID----------------------------------------	


#endif   // _TOOLS_H
