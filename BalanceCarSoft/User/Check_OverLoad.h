/*
check overload or overcurrent, overvoltage...
author： Kernel_007
 每当舵机出现过载的现象时，会对过载次数做一个计数，若在一段时间内，过载次数大于一定的
 次数则，直接报警提示
*/

#ifndef CHECK_OVERLOAD_H
#define CHECK_OVERLOAD_H

	#include "timer.h"
	#include "Sensor_Test.h"
	
	#define		ADC_12_BIT
	//所有ADC值默认的参考电压为3.3v
	#define		ADC_RESOLUTION_10		1024		//10位AD的分辨率
	#define  	ADC_RESOLUTION_12		4096		//12位AD的分辨率
	#define		ADC_BUF_SIZE			8		
	//实际测试得出舵机堵转时，会产生一个电压差出来，差值将近500mv,
	//不同的电机参数
 	#define 	ERROR_TIME				200			//200MS
	//#ifdef 		ADC_10_BIT
	//#define		ADC_MAX_ERROR_VALUE		
	//struct 
	struct check_overload
	{
		uint8_t  overload_flag;				//过载标志
		uint8_t	 overcurrent_flag;			//过流标志
		uint8_t  overvoltage_flag;			//过压标志
		uint8_t  undervoltage_flag;			//欠压标志
		uint8_t  overload_time_flag;		//过载计时开始
		uint8_t  overcurrent_time_flag;		//过流计时
		uint8_t  overvoltage_time_flag;
		uint8_t  undervoltage_time_flag;
		
		uint8_t  offset_delay_id;			
		uint8_t  tolerant_cnt;				//
		uint16_t load_error_time;			//错误时间
		uint16_t free_load_error_time;		//free error 时间
		uint16_t free_cur_error_time;
		uint16_t current_error_time;
		uint16_t overvoltage_error_time;		//
		uint16_t undervoltage_error_time;
		//堵转保护
		uint16_t stall_current;				//堵转电流
		uint16_t stall_time;				//堵转时间
		uint16_t free_time;					//释放时间
		uint16_t stall_cmp;					//堵转电流累计差产生的积分和
		uint16_t stall_sum;					//堵转电流 + 中间偏置电压值
		
		uint16_t voltage_adc;				//电压ADC的值
		uint16_t current_adc;				//电流ADC的值
		uint16_t encode_value;				//编码器的实时读取值
		uint16_t cur_diff_default;			//电流绝对偏差值
		uint16_t max_voltage;				//最小电压值
		uint16_t min_voltage;				//最大电压值
		//---------------------//
		uint16_t cur_waring_value;			//最大电流值
		uint16_t cur_last_value;			//上一次电流值
		uint16_t cur_offset_dc;				//电流最大偏差之和
		int 	 cur_offset_sum;			//
		int		 cur_offset;				//  
		uint16_t cur_adc_buffer[ADC_BUF_SIZE];//电流ADC buf
		//---------------------//
		float    voltage_ratio;				//电压的分压比率
	};		
	//function state;
	void Init_CheckMotorParameter(void);
	void Config_MotorParameter(uint8_t index, uint16_t stall_time, uint16_t free_time, uint16_t stall_current);
	void Get_CheckSampleValue(uint8_t index, uint16_t voltage, uint16_t current, uint16_t encode);
	void Output_FeedbackSig(uint8_t index, uint8_t *overload, uint8_t *overcurrent, uint8_t *overvoltage, uint8_t *undervoltage);
	void Set_HardwareParameter(uint8_t index, float voltage_ratio, uint16_t max_work_voltage, uint16_t min_work_voltage);
	//每次启动时，需要读取电机没有工作时的电流的直流偏置值
	uint8_t Read_OffsetCurValue(uint8_t temp, uint16_t offset_cur);
	void Check_DeviceApp(void);
	void Check_ErrorTimer(void);
	
#endif

/*
end of file.
*/

