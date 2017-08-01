/*
check overload or overcurrent, overvoltage...
author�� Kernel_007
 ÿ��������ֹ��ص�����ʱ����Թ��ش�����һ������������һ��ʱ���ڣ����ش�������һ����
 ������ֱ�ӱ�����ʾ
*/

#ifndef CHECK_OVERLOAD_H
#define CHECK_OVERLOAD_H

	#include "timer.h"
	#include "Sensor_Test.h"
	
	#define		ADC_12_BIT
	//����ADCֵĬ�ϵĲο���ѹΪ3.3v
	#define		ADC_RESOLUTION_10		1024		//10λAD�ķֱ���
	#define  	ADC_RESOLUTION_12		4096		//12λAD�ķֱ���
	#define		ADC_BUF_SIZE			8		
	//ʵ�ʲ��Եó������תʱ�������һ����ѹ���������ֵ����500mv,
	//��ͬ�ĵ������
 	#define 	ERROR_TIME				200			//200MS
	//#ifdef 		ADC_10_BIT
	//#define		ADC_MAX_ERROR_VALUE		
	//struct 
	struct check_overload
	{
		uint8_t  overload_flag;				//���ر�־
		uint8_t	 overcurrent_flag;			//������־
		uint8_t  overvoltage_flag;			//��ѹ��־
		uint8_t  undervoltage_flag;			//Ƿѹ��־
		uint8_t  overload_time_flag;		//���ؼ�ʱ��ʼ
		uint8_t  overcurrent_time_flag;		//������ʱ
		uint8_t  overvoltage_time_flag;
		uint8_t  undervoltage_time_flag;
		
		uint8_t  offset_delay_id;			
		uint8_t  tolerant_cnt;				//
		uint16_t load_error_time;			//����ʱ��
		uint16_t free_load_error_time;		//free error ʱ��
		uint16_t free_cur_error_time;
		uint16_t current_error_time;
		uint16_t overvoltage_error_time;		//
		uint16_t undervoltage_error_time;
		//��ת����
		uint16_t stall_current;				//��ת����
		uint16_t stall_time;				//��תʱ��
		uint16_t free_time;					//�ͷ�ʱ��
		uint16_t stall_cmp;					//��ת�����ۼƲ�����Ļ��ֺ�
		uint16_t stall_sum;					//��ת���� + �м�ƫ�õ�ѹֵ
		
		uint16_t voltage_adc;				//��ѹADC��ֵ
		uint16_t current_adc;				//����ADC��ֵ
		uint16_t encode_value;				//��������ʵʱ��ȡֵ
		uint16_t cur_diff_default;			//��������ƫ��ֵ
		uint16_t max_voltage;				//��С��ѹֵ
		uint16_t min_voltage;				//����ѹֵ
		//---------------------//
		uint16_t cur_waring_value;			//������ֵ
		uint16_t cur_last_value;			//��һ�ε���ֵ
		uint16_t cur_offset_dc;				//�������ƫ��֮��
		int 	 cur_offset_sum;			//
		int		 cur_offset;				//  
		uint16_t cur_adc_buffer[ADC_BUF_SIZE];//����ADC buf
		//---------------------//
		float    voltage_ratio;				//��ѹ�ķ�ѹ����
	};		
	//function state;
	void Init_CheckMotorParameter(void);
	void Config_MotorParameter(uint8_t index, uint16_t stall_time, uint16_t free_time, uint16_t stall_current);
	void Get_CheckSampleValue(uint8_t index, uint16_t voltage, uint16_t current, uint16_t encode);
	void Output_FeedbackSig(uint8_t index, uint8_t *overload, uint8_t *overcurrent, uint8_t *overvoltage, uint8_t *undervoltage);
	void Set_HardwareParameter(uint8_t index, float voltage_ratio, uint16_t max_work_voltage, uint16_t min_work_voltage);
	//ÿ������ʱ����Ҫ��ȡ���û�й���ʱ�ĵ�����ֱ��ƫ��ֵ
	uint8_t Read_OffsetCurValue(uint8_t temp, uint16_t offset_cur);
	void Check_DeviceApp(void);
	void Check_ErrorTimer(void);
	
#endif

/*
end of file.
*/

