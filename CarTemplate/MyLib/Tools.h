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

	
//-----------------------GPIO Config---------------------------------------------
#ifdef GPIO_CONFIGURATION
	//通道类型
	typedef enum 
	{
		ChannelA,
		ChannelB,
		ChannelC,
		ChannelD,	
		ChannelE,
		ChannelF,
		ChannelG
	} GPIOChannelType;
	u16 getGPIOPortByNumber(u8);
	void setGPIOConfiguration(GPIOChannelType channel,u16 port,GPIOMode_TypeDef mode, GPIOSpeed_TypeDef speed);
#endif
//-----------------------End of GPIO Config----------------------------------------	
	

//-----------------------IO Config---------------------------------------------
#ifdef IO_ADDRES_CONFIGURATION
	#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
	#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
	#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
	//IO口地址映射
	#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C 
	#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C 
	#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C 
	#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C 
	#define GPIOE_ODR_Addr    (GPIOE_BASE+12) //0x4001180C 
	#define GPIOF_ODR_Addr    (GPIOF_BASE+12) //0x40011A0C    
	#define GPIOG_ODR_Addr    (GPIOG_BASE+12) //0x40011E0C
	
	#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808 
	#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08 
	#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008 
	#define GPIOD_IDR_Addr    (GPIOD_BASE+8) //0x40011408 
	#define GPIOE_IDR_Addr    (GPIOE_BASE+8) //0x40011808 
	#define GPIOF_IDR_Addr    (GPIOF_BASE+8) //0x40011A08 
	#define GPIOG_IDR_Addr    (GPIOG_BASE+8) //0x40011E08 
	
	//IO口操作,只对单一的IO口!
	//确保n的值小于16!
	#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出 
	#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入 
	#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
	#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 
	#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 
	#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 
	#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出 
	#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入 
	#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出 
	#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入
	#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出 
	#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入
	#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出 
	#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入
#endif

//-----------------------End of IO Config----------------------------------------	

//----------------------- 坐标系变换 ---------------------------------------------
#ifdef EXCHANGE_COORDINATE
	//将霍尔传感器的数值转换为小车的运动距离
	void Hall_2_CarInitial(double ratio,double diameter);
	u16 SpeedHall_2_Car(u16 hallValue);
#endif
//-----------------------End of  坐标系变换 ----------------------------------------	

//----------------------- 滤波 ---------------------------------------------
#ifdef DATA_FILTER
	u16 filterByMean(u8 dataIdx,u16 sourceData);
	u16 filterByMedium(u8 dataIdx,u16 sourceData);
#endif
//-----------------------End of 滤波 ----------------------------------------	

#endif   // _TOOLS_H
