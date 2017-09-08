/*
check overload 

*/
#include "Check_OverLoad.h"

#define CHECK_NUMBER 		2
struct check_overload g_check_info[CHECK_NUMBER];

/*
config motor parameter.
*/
void Config_MotorParameter(uint8_t index, uint16_t stall_time, uint16_t free_time, uint16_t stall_current)
{
	g_check_info[index].free_time = free_time;
	g_check_info[index].stall_time = stall_time;
	g_check_info[index].stall_current = stall_current;
	//-------------------------------//
	g_check_info[index].stall_cmp = g_check_info[index].stall_current * 0.373;//0.01 * 6) / 3.3) *4096 / 1000; unit is mA.
}

/*
init parameter.
*/
void Init_CheckMotorParameter(void)
{
	uint8_t index = 0;
	for(index = 0; index < CHECK_NUMBER; index ++)
	{
		g_check_info[index].stall_time = 500;			//time is 500ms.
		g_check_info[index].stall_current = 4000;		//current is 4000ma.
		g_check_info[index].free_time = 3000;			//等待3S后，电机会恢复
		g_check_info[index].current_adc = 0;
		g_check_info[index].current_error_time = 0;
		g_check_info[index].stall_cmp = g_check_info[index].stall_current * 0.744;
		GetDelayIdFunction(1, &g_check_info[index].offset_delay_id);
	}
	Timer_Register(TIMER_5, Check_ErrorTimer);
}
/*

*/
void Get_CheckSampleValue(uint8_t index, uint16_t voltage, uint16_t current, uint16_t encode)
{
	g_check_info[index].current_adc = current;
	g_check_info[index].encode_value = encode;
	g_check_info[index].voltage_adc = voltage;
}
/*

*/
void Output_FeedbackSig(uint8_t index, uint8_t *overload, uint8_t *overcurrent, uint8_t *overvoltage, uint8_t *undervoltage)
{
	if(overload != 0)
	*overload = g_check_info[index].overload_flag;
	if(overcurrent != 0)
	*overcurrent = g_check_info[index].overcurrent_flag;
	if(overvoltage != 0)
	*overvoltage = g_check_info[index].overvoltage_flag;
}
/*

*/
void Set_HardwareParameter(uint8_t index, float voltage_ratio, uint16_t max_work_voltage, uint16_t min_work_voltage)
{
	#ifdef	ADC_12_BIT
	g_check_info[index].max_voltage = max_work_voltage * voltage_ratio * ADC_RESOLUTION_12 / 3.3;
	g_check_info[index].min_voltage = min_work_voltage * voltage_ratio * ADC_RESOLUTION_12 / 3.3;
	#else
	g_check_info[index].max_voltage = max_work_voltage * voltage_ratio * ADC_RESOLUTION_10 / 3.3;
	g_check_info[index].min_voltage = min_work_voltage * voltage_ratio * ADC_RESOLUTION_10 / 3.3;
	#endif	
}

/*
//每次启动时，需要读取电机没有工作时的电流的直流偏置值,采集时间为200~300ms
*/
enum
{
	READY_SAMPLE = 0,
	START_SAMPLE,
	GET_SAMPLE_VALUE
};
uint8_t Read_OffsetCurValue(uint8_t temp, uint16_t offset_cur)
{
	uint8_t sample_end = 0;
	static uint8_t Offset_FSM[CHECK_NUMBER], index = 0;
	switch(Offset_FSM[temp])
	{
		case READY_SAMPLE:
			SetCountTime(g_check_info[temp].offset_delay_id, 200);//200ms
			StartDelayCount(g_check_info[temp].offset_delay_id);
			Offset_FSM[temp] = START_SAMPLE;
			break;
		
		case START_SAMPLE:
			if(ReturnSuccess(g_check_info[temp].offset_delay_id))
			{
				StopDelayCount(g_check_info[temp].offset_delay_id);
				ClearDelaySucFlag(g_check_info[temp].offset_delay_id);
				
				Offset_FSM[temp] = GET_SAMPLE_VALUE;
			}
			else
			{
				//最新的数据都在最后一个缓冲数据
				for(index = 0; index < (ADC_BUF_SIZE - 1); index ++)
				{
					g_check_info[temp].cur_adc_buffer[index] = g_check_info[temp].cur_adc_buffer[index + 1];
				}
				g_check_info[temp].cur_adc_buffer[ADC_BUF_SIZE - 1] = offset_cur;
				g_check_info[temp].cur_offset_dc = Weight_Filter(ADC_BUF_SIZE, g_check_info[temp].cur_adc_buffer);	
			}
			break;
		
		case GET_SAMPLE_VALUE:
			sample_end = 1;
			//电流峰值时所产生的电压值
			g_check_info[temp].stall_sum = g_check_info[temp].cur_offset_dc + g_check_info[temp].stall_cmp;
			g_check_info[temp].cur_last_value = g_check_info[temp].cur_offset_dc;
			Offset_FSM[temp] = READY_SAMPLE; 
			break;
	}
	return sample_end;
}
/*

*/
void Check_DeviceApp(void)
{
	uint8_t index = 0;
	//overcurrent.  sample current .
	for(index = 0; index < CHECK_NUMBER; index ++)
	{
		g_check_info[index].cur_offset = g_check_info[index].current_adc - g_check_info[index].cur_last_value;
		g_check_info[index].cur_offset_sum += g_check_info[index].cur_offset;	
		g_check_info[index].cur_last_value = g_check_info[index].current_adc;
		//get current offset sum.
		if((g_check_info[index].cur_offset_sum > 0) && (g_check_info[index].cur_offset_sum >= g_check_info[index].stall_cmp))
			g_check_info[index].overload_time_flag = 1;		//过载标志
		else
			g_check_info[index].overload_time_flag = 0;
		
		if(g_check_info[index].current_adc >= g_check_info[index].stall_sum)  //过流
			g_check_info[index].overcurrent_time_flag = 1;
		else
			g_check_info[index].overcurrent_time_flag = 0;
		//过压和欠压标志
		if((g_check_info[index].max_voltage == 0) || (g_check_info[index].min_voltage == 0))return;
		
		if(g_check_info[index].voltage_adc >= g_check_info[index].max_voltage)
			g_check_info[index].overvoltage_time_flag = 1;
		else
			g_check_info[index].overvoltage_time_flag = 0;
		
		if(g_check_info[index].voltage_adc <= g_check_info[index].min_voltage)
			g_check_info[index].undervoltage_time_flag = 1;
		else
			g_check_info[index].undervoltage_time_flag = 0;
	}
}
/*
start time.
*/
void Check_ErrorTimer(void)
{
	uint8_t index = 0;
	for(index = 0; index < CHECK_NUMBER; index ++)
	{	//过载计时
		if(g_check_info[index].overload_time_flag)
		{
			g_check_info[index].load_error_time ++;
			if(g_check_info[index].load_error_time >= g_check_info[index].stall_time)
			{
				g_check_info[index].load_error_time = 0;
				g_check_info[index].overload_flag = 1;		//过载标志
			}
		}
		else
		{
			g_check_info[index].load_error_time = 0;
		}
		//当检测到过流后，或者过载后，电机处于保护阶段，当过一段时间后，电机就恢复
		if(g_check_info[index].overload_flag)
		{
			g_check_info[index].free_load_error_time ++;
			if(g_check_info[index].free_load_error_time >= g_check_info[index].free_time)
			{
				g_check_info[index].free_load_error_time = 0;
				g_check_info[index].overload_flag = 0;
			}
		}
		//-------------------------------------------------//
		if(g_check_info[index].overcurrent_time_flag)
		{
			g_check_info[index].current_error_time ++;
			if(g_check_info[index].current_error_time >= g_check_info[index].stall_time)
			{
				g_check_info[index].current_error_time = 0;
				g_check_info[index].overcurrent_flag = 1;
			}
		}
		else
		{
			g_check_info[index].current_error_time = 0;
		}
		//当检测到过流后，或者过载后，电机处于保护阶段，当过一段时间后，电机就恢复
		if(g_check_info[index].overcurrent_flag)
		{
			g_check_info[index].free_cur_error_time ++;
			if(g_check_info[index].free_cur_error_time >= g_check_info[index].free_time)
			{
				g_check_info[index].free_cur_error_time = 0;
				g_check_info[index].overcurrent_flag = 0;
			}
		}
		//过压标志
		if(g_check_info[index].overvoltage_time_flag)
		{
			g_check_info[index].overvoltage_error_time ++;
			if(g_check_info[index].overvoltage_error_time >= ERROR_TIME)
			{
				g_check_info[index].overvoltage_error_time = 0;
				g_check_info[index].overvoltage_flag = 1;
			}
		}
		else
		{
			g_check_info[index].overvoltage_error_time = 0;
			g_check_info[index].overvoltage_flag = 0;
		}
		//欠压标志
		if(g_check_info[index].undervoltage_time_flag)
		{
			g_check_info[index].undervoltage_error_time ++;
			if(g_check_info[index].undervoltage_error_time >= ERROR_TIME)
			{
				g_check_info[index].undervoltage_error_time = 0;
				g_check_info[index].undervoltage_flag = 1;
			}
		}
		else
		{
			g_check_info[index].undervoltage_error_time = 0;
			g_check_info[index].undervoltage_flag = 0;
		}	
	}
}

//
/*
以5s为一个单位
*/
/*
end of file.
*/
