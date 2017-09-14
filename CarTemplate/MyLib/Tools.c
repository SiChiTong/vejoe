#include "Tools.h"
#include <math.h>
#include <stdarg.h>

//-----------------------LED------------------------------------------------------
#if defined USE_LED
struct _LED_Info *_led_info;
UINT8 _LEDNumber = 0;
UINT32 _LED_MScount;					//定时1毫秒对应的计数值
UINT16 _LED_DimCount;
UINT16 _LED_DimUnit;

void Config_LED(UINT8 number, UINT8 timer)
{
	UINT8 i;
	_LEDNumber = number;
	_led_info = My_malloc(sizeof(struct _LED_Info) * number);
	for(i = 0; i < number; i++)
	{
		_led_info[i]._LEDState = LED_OFF;
		_led_info[i]._LEDhdl = 0;
		_led_info[i]._Control = 0;
		_led_info[i]._BlinkCount = 1000;
		_led_info[i]._DimStep = 0;
		_led_info[i]._Tick = 0;
	}	
	if(timer == SYS_TIMER)
	{
		SysTick_Register(_LED_Tick);
		_LED_MScount = Get_SysTickInfo()->_frequency / 1000;
	}
	else
	{
		Timer_Register(timer, _LED_Tick);
		_LED_MScount = Get_TimerInfo(timer)->_frequency / 1000;
	}
	_LED_DimCount = _LED_MScount * 20;
	_LED_DimUnit = 0;
}

void Register_LED(UINT8 index, CB_SET setLED, UINT8 state, UINT16 blinkPeriod)
{
	_led_info[index]._LEDhdl = setLED;
	_led_info[index]._BlinkCount = blinkPeriod * _LED_MScount;
	Set_LEDState(index, state);
}

void Set_LEDState(UINT8 index, UINT8 state)
{
	_led_info[index]._LEDState = state;

	switch(_led_info[index]._LEDState)
	{
		case LED_OFF:
			_led_info[index]._Control = OFF;
			break;

		case LED_ON:
			_led_info[index]._Control = ON;
			break;

		case LED_BLINK:
		case LED_DIM:
		case LED_CONTROL:
			break;
	}
}

void Set_Control(UINT8 index, BOOL state)
{
	_led_info[index]._Control = state;
}


void Set_LEDFcy(UINT8 index, UINT16 blinkPeriod)
{
	_led_info[index]._BlinkCount = blinkPeriod * _LED_MScount;
}


void LED_DisplayBit(UINT16 value)
{
	UINT8 i;
	for(i = 0; i < _LEDNumber; i++)
	{
		Set_LEDState(i, (value >> i) & 0x0001);
	}
}

void LED_DisplayNumber(UINT16 value)
{
	UINT8 i;
	for(i = 0; i < _LEDNumber; i++)
	{
		Set_LEDState(i, (value >> (i << 1)) & 0x0003);
	}
}

void _LED_Tick(void)
{
	UINT8 i;
	for(i = 0; i < _LEDNumber; i++)
	{
		_led_info[i]._Tick++;

		switch(_led_info[i]._LEDState)
		{
			case LED_OFF:
			case LED_ON:
			case LED_CONTROL:
				break;

			case LED_BLINK:
				if(_led_info[i]._Tick >= _led_info[i]._BlinkCount)
				{
					_led_info[i]._Tick = 0;
				}
				if(_led_info[i]._Tick < (_led_info[i]._BlinkCount >> 1))
				{
					_led_info[i]._Control = ON;
				}
				else _led_info[i]._Control = OFF;
				break;

			case LED_DIM:
				if(_led_info[i]._Tick >= _LED_DimCount)
				{
					_led_info[i]._Tick = 0;
					if(_led_info[i]._DimStep < 50) _LED_DimUnit += _LED_MScount;
					else _LED_DimUnit -= _LED_MScount;
					_led_info[i]._DimStep++;
					if(_led_info[i]._DimStep >= 100) _led_info[i]._DimStep = 0;
				}
				if(_led_info[i]._Tick < (_LED_DimUnit >> 2) + (_LED_DimUnit >> 3)) _led_info[i]._Control = ON;
				else _led_info[i]._Control = OFF;
				break;
		}
		//-----------NEW----------//
		if(_led_info[i]._LEDhdl != 0)
		_led_info[i]._LEDhdl(_led_info[i]._Control);
	}
}

#endif

//-----------------------End of LED-----------------------------------------------

//-----------------------------------beeper -----------------------------------//
#if defined USE_BEEPER

struct _BEEPER_Info *_beeper_info;
UINT8 _beeper_number = 0;

void BeeperTimeCtl(void);
void CreatBeeperAudio(void);
/*
config common.
*/
void Config_Beeper(UINT8 num, UINT8 timer)
{
	_beeper_number = num;
	_beeper_info = (struct _BEEPER_Info *)My_malloc(sizeof(struct _BEEPER_Info) * num);
	My_memset((UINT8 *)_beeper_info, sizeof(struct _BEEPER_Info)* num, 0x00);
	/*普通的定时器，用于设定声音的长短*/
	Timer_Register(timer, BeeperTimeCtl);
}

/*
用于设定声音的频率.
*/
void Config_Beeper_AudioFcy(UINT32 fcy, UINT8 timer, UINT8 pri)
{
	Timer_Init(fcy, timer, pri, MAX_AUDIO_FREQUENCY);
	Timer_Register(timer, CreatBeeperAudio);
}

/*

*/
void RegisterBeeper(UINT8 index, CB_SET IOhd, UINT16 fcy_hz)
{
	if(index < _beeper_number)
	{
		_beeper_info[index].Iohd = IOhd;
		_beeper_info[index].Frequency = (MAX_AUDIO_FREQUENCY * 1.0) / (fcy_hz * 1.0);
		_beeper_info[index].FcyCount = (UINT16)(_beeper_info[index].Frequency / 1);
	}
}

/*
set beeper.
*/
void SetBeeperTime(UINT8 index, UINT16 time)
{
	UINT8 tmp = 0;
	tmp = _beeper_info[index].StartFlag;  //防止出现异常情况.
	_beeper_info[index].StartFlag = 0;
	_beeper_info[index].Tick = time;
	_beeper_info[index].StartFlag = tmp;
}
/*
start beeper.
*/
void StartBeeper(UINT8 index, UINT8 mode)
{
	if(index < _beeper_number)
	{
		_beeper_info[index].BeeperMode = mode;
		_beeper_info[index].StartFlag = 1;
	}
}

/*
stop beeper.
*/
void StopBeeper(UINT8 index)
{
	if(index < _beeper_number)
	_beeper_info[index].StartFlag = 0;
}

/*
mode style .
*/
void BeeperTimeCtl(void)
{
	UINT8 index = 0;
	for(index = 0; index < _beeper_number; index ++)
	{
		if(_beeper_info[index].StartFlag)
		{
			_beeper_info[index].Tick --;
			if(_beeper_info[index].Tick == 0)
			_beeper_info[index].StartFlag = 0;
		}
	}
}

/*

*/
void CreatBeeperAudio(void)
{
	UINT8 i = 0;
	for(i = 0; i < _beeper_number; i ++)
	{
		if(_beeper_info[i].StartFlag)
		{
			_beeper_info[i].FcyCount --;
			if(_beeper_info[i].FcyCount == 0)
			{
				_beeper_info[i].FcyCount = (UINT16)_beeper_info[i].Frequency;  //重新获取初值
				if(_beeper_info[i].BeeperState == BEEPER_ON)
				{
					_beeper_info[i].BeeperState = BEEPER_OFF;
					_beeper_info[i].Iohd(BEEPER_OFF);
				}
				else
				{
					_beeper_info[i].BeeperState = BEEPER_ON;
					_beeper_info[i].Iohd(BEEPER_ON);
				}
			}
		}
		else
		{
			_beeper_info[i].FcyCount = (UINT16)_beeper_info[i].Frequency;
			_beeper_info[i].Iohd(BEEPER_OFF);
		}
	}
}

#endif

//-------------------------------end of beeper-----------------------------------

//-----------------------USE_H_BDC_DRIVER---------------------------------------------
#ifdef USE_BDC_H_DRIVER
UINT16 _HBDC_Timer = 0;
UINT8 _HBDC_State = 0;		//0:stop, 1:forward, 2:backward
UINT8 _HBDC_Percent = 0;

void HBDC_Config(UINT8 timer, UINT32 pwm_fcy)
{
	_HBDC_Timer = timer;
	if(_HBDC_Timer == 1) //timer1
	{
		Init_T1ComplementaryPWM(pwm_fcy);
		HBDC_Stop();
	}
}

void HBDC_MoveForward(UINT8 percent)
{
	if(percent == 0)
	{
		HBDC_Stop();
		return;
	}
	else if(percent > 100) return;
	
	if(_HBDC_Timer == 1) //timer1
	{
		if(_HBDC_State != 1) //从静止或者反转切换到正转
		{
			Set_T1PWM1L_Percent(100);
			Set_T1PWM2L_Percent(100);
			Set_T1PWM1L_Percent(100 - percent);
			_HBDC_State = 1;
		}
		else
		{
			if(_HBDC_Percent != percent)
			{
				Set_T1PWM1L_Percent(100 - percent);
			}
		}
		_HBDC_Percent = percent;
	}
}

void HBDC_MoveBackward(UINT8 percent)
{
	if(percent == 0)
	{
		HBDC_Stop();
		return;
	}
	else if(percent > 100) return;
	
	if(_HBDC_Timer == 1) //timer1
	{
		if(_HBDC_State != 2) //从静止或者正转切换到反转
		{
			Set_T1PWM2L_Percent(100);
			Set_T1PWM1L_Percent(100);
			Set_T1PWM2L_Percent(100 - percent);
			_HBDC_State = 2;
		}
		else
		{
			if(_HBDC_Percent != percent)
			{
				Set_T1PWM2L_Percent(100 - percent);
			}
		}
		_HBDC_Percent = percent;
	}
}
	
void HBDC_Stop(void)
{
	if(_HBDC_Timer == 1) //timer1
	{
		Set_T1PWM1L_Percent(100);
		Set_T1PWM2L_Percent(100);
		_HBDC_State = 0;
		_HBDC_Percent = 0;
	}
}

#endif 
//-----------------------End of USE_H_BDC_DRIVER----------------------------------------	

//-----------------------USE_CURRENT_MANAGER-------------------------------------------------------
#ifdef USE_CURRENT_MANAGER
	struct  _CUR_Info *_cur_info;
	UINT8 _CURNumber = 0;

	void Config_Current_Manager(UINT8 number, UINT8 timer)
	{
		UINT8 i;
		_CURNumber = number;
		_cur_info = My_malloc(sizeof(struct _CUR_Info) * number);
		for(i = 0; i < number; i++)
		{
			_cur_info[i]._coefficient = 0.0;
			_cur_info[i]._Static_ADC = 0;
			_cur_info[i]._ADC = 0;
			_cur_info[i]._Current = 0;
			_cur_info[i]._Tick = 0;
			_cur_info[i]._CurrentHDL = 0;
			_cur_info[i]._FSM = 0;
			_cur_info[i]._is_Init = FALSE;
			_cur_info[i]._filter._filter_type = 0;
			_cur_info[i]._filter._width = 50;
		}	
		if(timer == SYS_TIMER)
		{
			SysTick_Register(_Current_Manager_Tick);
		}
		else
		{
			//Timer_Register(timer, _LED_Tick);
			//_LED_MScount = Get_TimerInfo(timer)->_frequency / 1000;
		}
	}

	void Register_Current(UINT8 index, CB_READ_U16 readADC, float coefficient)
	{
		_cur_info[index]._coefficient = coefficient;
		_cur_info[index]._CurrentHDL = readADC;
	}
	
	BOOL Start_Current_Manager(void)
	{
		for(UINT8 i = 0; i < _CURNumber; i++)
		{
			if(_cur_info[i]._is_Init == FALSE) return FALSE;
		}
		return TRUE;
	}
	
	UINT16 Get_Current(UINT8 index)
	{
		return _cur_info[index]._Current;
	}
	
	void _Current_Manager_Tick(void)
	{
		for(UINT8 i = 0; i < _CURNumber; i++)
		{
			switch(_cur_info[i]._FSM)
			{
				case 0://初始化状态，前100ms的数据忽略
					_cur_info[i]._Tick++;
					if(_cur_info[i]._Tick >= 100)
					{
						_cur_info[i]._FSM = 1;
						_cur_info[i]._Tick = 0;
					}
					break;
				
				case 1://采集电流为0时的静态AD值，采集16次进行平均
					_cur_info[i]._Static_ADC += _cur_info[i]._CurrentHDL();
					_cur_info[i]._Tick++;
					if(_cur_info[i]._Tick == 16)
					{
						_cur_info[i]._Static_ADC >>= 4;
						_cur_info[i]._Tick = 0;
						_cur_info[i]._FSM = 2;
						_cur_info[i]._is_Init = TRUE;
					}
					break;
				
				case 2://正常采集
					_cur_info[i]._ADC = _cur_info[i]._CurrentHDL();
					if(_cur_info[i]._ADC <= _cur_info[i]._Static_ADC)
					{
						_cur_info[i]._ADC = 0;
					}
					else
					{
						_cur_info[i]._ADC = _cur_info[i]._ADC - _cur_info[i]._Static_ADC;
						_cur_info[i]._ADC = (UINT16)(_cur_info[i]._ADC * _cur_info[i]._coefficient);
					}
					_cur_info[i]._Current = Filter16_GetValue(&_cur_info[i]._filter, _cur_info[i]._ADC);
					break;
			}
		}
	}
#endif 
//-----------------------End of USE_CURRENT_MANAGER--------------------------------------------------	

#if defined FSM_MANAGER_MODULE

struct _FSM_Module  *_Fsm_module;//状态机模块

UINT8 _module_num = 0;
/*

*/
void ConfigEventModuleNum(UINT8 num)
{
	_module_num = num;
	_Fsm_module = (struct _FSM_Module*)My_malloc(sizeof(struct _FSM_Module) * num);
	My_memset((UINT8 *)_Fsm_module, sizeof(struct _FSM_Module) * num, 0x00);
}

/*

*/
BOOL ConfigEventFsmNum(UINT8 num, UINT8 module)
{
	if(module < _module_num)
	{
		if(_Fsm_module[module].Fsm_Info == 0)
		{
			_Fsm_module[module].FsmNum = num;     		//每一个事件状态机的个数
			_Fsm_module[module].SystemFsm = INIT_FSM;	//每个状态机的初始状态.
			_Fsm_module[module].Fsm_Info = (struct _FSM_Manager*)My_malloc(sizeof(struct _FSM_Manager) * num);
			My_memset((UINT8 *)_Fsm_module[module].Fsm_Info, sizeof(struct _FSM_Manager) * num, 0x00);
		}
		else
		return 0;
	}
	return 0;
}

/*

*/
void RegisterEventFSMList(struct _FSM_Manager *EventFSM, UINT8 FsmId, UINT8 module)
{
	_Fsm_module[module].Fsm_Info[FsmId].ConcomitanceFlag = EventFSM->ConcomitanceFlag;
	_Fsm_module[module].Fsm_Info[FsmId].OneOffEventFlag  = EventFSM->OneOffEventFlag;
	_Fsm_module[module].Fsm_Info[FsmId].CurFsm			 = EventFSM->CurFsm;
	_Fsm_module[module].Fsm_Info[FsmId].NextFsm 		 = EventFSM->NextFsm;
	_Fsm_module[module].Fsm_Info[FsmId].FsmCondition	 = EventFSM->FsmCondition;
	_Fsm_module[module].Fsm_Info[FsmId].FsmFunction		 = EventFSM->FsmFunction;
	_Fsm_module[module].Fsm_Info[FsmId].FsmOneOffEvent	 = EventFSM->FsmOneOffEvent;
	_Fsm_module[module].Fsm_Info[FsmId].FsmConcomitance  = EventFSM->FsmConcomitance;
}

/*

*/
void AnalyseModuleFSM(UINT8 module)
{
	UINT8 i = 0;
	for(i = 0; i < _Fsm_module[module].FsmNum; i ++)
	{
		if(_Fsm_module[module].SystemFsm == _Fsm_module[module].Fsm_Info[i].CurFsm)
		{
			/*进入状态机时，只执行一次的任务*/
			if(_Fsm_module[module].Fsm_Info[i].OneOffEventFlag)
			{
				if(_Fsm_module[module].Fsm_Info[i].FsmOneOffEvent != 0)
				{
					_Fsm_module[module].Fsm_Info[i].FsmOneOffEvent();
					_Fsm_module[module].Fsm_Info[i].OneOffEventFlag = 0;
				}
			}
			/*action current state task.*/
			if(_Fsm_module[module].Fsm_Info[i].FsmFunction != 0)
			_Fsm_module[module].Fsm_Info[i].FsmFunction();
			/*进入状态条件判断*/
			if(_Fsm_module[module].Fsm_Info[i].FsmCondition())  //若为真,则发生状态跳转
			{
				if(_Fsm_module[module].Fsm_Info[i].ConcomitanceFlag)
				{
					if(_Fsm_module[module].Fsm_Info[i].FsmConcomitance != 0)
					_Fsm_module[module].Fsm_Info[i].FsmConcomitance();
				}
				_Fsm_module[module].Fsm_Info[i].OneOffEventFlag = 1;  //恢复一次功能函数
				_Fsm_module[module].SystemFsm = _Fsm_module[module].Fsm_Info[i].NextFsm;
			}
		}
	}
}


#endif
//-------------------------------end of fsm manager------------------------------

//-------------------------------start key scan add by houzuping-----------------
#if defined USE_KEY_SCAN

UINT8  key_pad_num;
struct Key_Pad *_key_pad;
/*

*/
void ConfigKeyPad(UINT8 num, UINT8 timer)
{
	key_pad_num = num;
	_key_pad = (struct Key_Pad*)My_malloc(sizeof(struct Key_Pad) * num);
	My_memset((UINT8 *)_key_pad, sizeof(struct Key_Pad) * num, 0x00);

	Timer_Register(timer, ScanKeyPad);
}

/*

*/
void SetKeyPadSize(UINT8 index, UINT8 row_x, UINT8 row_y)
{
	if(index < key_pad_num)
	{
		_key_pad[index].KeyPadId = index;
		_key_pad[index].KeyRowx = row_x;
		_key_pad[index].KeyRowy = row_y;
		_key_pad[index].KeyValue = (UINT8 *)My_malloc(sizeof(UINT8) * (row_x * row_y));
		My_memset(_key_pad[index].KeyValue, sizeof(UINT8) * row_x * row_y, 0x00);

		_key_pad[index].Key_Info_x = (struct Key_Scanx*)My_malloc(sizeof(struct Key_Scanx) * row_x);
		My_memset((UINT8 *)_key_pad[index].Key_Info_x, sizeof(struct Key_Scanx) * row_x, 0x00);

		_key_pad[index].Key_Info_y = (struct Key_Scany*)My_malloc(sizeof(struct Key_Scany) * row_y);
		My_memset((UINT8 *)_key_pad[index].Key_Info_y, sizeof(struct Key_Scany) * row_y, 0x00);
	}
}
/*

*/
void RegisterRowxCtlValue(UINT8 index, UINT16 key_rowx, ...)
{
	UINT8 i = 0;
	HANDLER02 io_handler;
	va_list valist;
	va_start(valist, key_rowx);//(UINT16)_key_pad[index].KeyRowx

	for(i = 0; i < _key_pad[index].KeyRowx; i ++)
	{
		io_handler = va_arg(valist, HANDLER02);
		_key_pad[index].Key_Info_x[i].RowxCtlValue = io_handler;
	}

	va_end(valist);
}

/*

*/
void RegisterRowyCtlValue(UINT8 index, UINT16 key_rowy, ...)
{
	UINT8 i = 0;
	HANDLER02 io_handler;
	va_list valist;
	va_start(valist, key_rowy);

	for(i = 0; i < _key_pad[index].KeyRowy; i ++)
	{
		io_handler = va_arg(valist, HANDLER02);
		_key_pad[index].Key_Info_y[i].RowyCtlValue = io_handler;
	}

	va_end(valist);
}

/*

*/
void RegisterRowxDir(UINT8 index, UINT16 key_rowx, ...)
{
	UINT8 i = 0;
	HANDLER02 io_handler;
	va_list valist;
	va_start(valist, key_rowx);

	for(i = 0; i < _key_pad[index].KeyRowx; i ++)
	{
		io_handler = va_arg(valist, HANDLER02);
		_key_pad[index].Key_Info_x[i].RowxCtlDir = io_handler;
	}

	va_end(valist);
}
/*

*/
void RegisterRowyDir(UINT8 index, UINT16 key_rowy, ...)
{
	UINT8 i = 0;
	HANDLER02 io_handler;
	va_list valist;
	va_start(valist, key_rowy);

	for(i = 0; i < _key_pad[index].KeyRowy; i ++)
	{
		io_handler = va_arg(valist, HANDLER02);
		_key_pad[index].Key_Info_y[i].RowyCtlDir = io_handler;
	}

	va_end(valist);
}
/*

*/
void RegisterRowxRead(UINT8 index, UINT16 key_rowx, ...)
{
	UINT8 i = 0;
	HANDLER03 io_handler;
	va_list valist;
	va_start(valist, key_rowx);

	for(i = 0; i < _key_pad[index].KeyRowx; i ++)
	{
		io_handler = va_arg(valist, HANDLER03);
		_key_pad[index].Key_Info_x[i].RowxRead = io_handler;
	}

	va_end(valist);
}
/*

*/
void RegisterRowyRead(UINT8 index, UINT16 key_rowy, ...)
{
	UINT8 i = 0;
	HANDLER03 io_handler;
	va_list valist;
	va_start(valist, key_rowy);

	for(i = 0; i < _key_pad[index].KeyRowy; i ++)
	{
		io_handler = va_arg(valist, HANDLER03);
		_key_pad[index].Key_Info_y[i].RowyRead = io_handler;
	}

	va_end(valist);
}
/*

*/
void ScanKeyPad(void)
{
	UINT8 index = 0, key_rowx = 0, key_rowy = 0, row = 0;
	for(index = 0; index < key_pad_num; index ++)
	{
		if((_key_pad[index].Key_Info_y != 0) || (_key_pad[index].Key_Info_x != 0)) //检查该模块是否注册
		{
			/*设置x方向IO为输入*/
			for(key_rowx = 0; key_rowx < _key_pad[index].KeyRowx; key_rowx ++)
			_key_pad[index].Key_Info_x[key_rowx].RowxCtlDir(KEY_INPUT);
			/*设置y方向IO为输入*/
			for(key_rowy = 0; key_rowy < _key_pad[index].KeyRowy; key_rowy ++)
			_key_pad[index].Key_Info_y[key_rowy].RowyCtlDir(KEY_INPUT);

			for(row = 0; row < _key_pad[index].KeyRowy; row ++) //循环扫描
			{
				//设置一个y方向IO为输出
				_key_pad[index].Key_Info_y[row].RowyCtlDir(KEY_OUTPUT);
				_key_pad[index].Key_Info_y[row].RowyCtlValue(0);
				/*读取x方向IO*/
				for(key_rowx = 0; key_rowx < _key_pad[index].KeyRowx; key_rowx ++)
				_key_pad[index].KeyValue[row * _key_pad[index].KeyRowx + key_rowx] = _key_pad[index].Key_Info_x[key_rowx].RowxRead();

				_key_pad[index].Key_Info_y[row].RowyCtlDir(KEY_INPUT);
			}
		}
	}
}

/*

*/
UINT8 GetKeyPadValue(UINT8 index, UINT8 row_xn, UINT8 row_yn)
{
	return _key_pad[index].KeyValue[row_yn * _key_pad[index].KeyRowx + row_xn];
}

/**/


#endif
//-------------------------------

//-----------------------Debounce-------------------------------------------------
#if defined USE_DEBOUNCE

struct Debounce_Info* _debounce_info;
UINT8 _debounce_IONumber = 0;
UINT8 _debounce_timer;

void Init_Debounce(UINT8 number, UINT8 timer)
{
	_debounce_IONumber = number;
	_debounce_info = (struct Debounce_Info*)My_malloc(sizeof(struct Debounce_Info) * number);
	My_memset((UINT8*)_debounce_info, sizeof(struct Debounce_Info) * number, 0x00);

	_debounce_timer = timer;
	Timer_Register(timer, _Debounce_Tick);
}

void Config_Debounce(UINT8 index, CB_READ IOhdl, UINT16 debounceMS)
{
	_debounce_info[index]._readIO = IOhdl;
	_debounce_info[index]._Debounce_Count = Get_TimerInfo(_debounce_timer)->_frequency / 1000 * debounceMS;
}

void Clr_Tick(UINT8 index)
{
	_debounce_info[index]._Debounce_Tick = 0;
}

BOOL Get_DebouncedIO(UINT8 index)
{
	BOOL tempIO;

	if(_debounce_info[index]._readIO == 0) return FALSE;

 	tempIO = _debounce_info[index]._readIO();

	if(_debounce_info[index]._start_flag == FALSE)
	{
		_debounce_info[index]._IOState = tempIO;
		_debounce_info[index]._DebouncedIO = tempIO;
		_debounce_info[index]._start_flag = TRUE;
	}
	else
	{
		if(_debounce_info[index]._IOState != tempIO)
		{
			_debounce_info[index]._Debounce_Tick = 0;
			_debounce_info[index]._IOState = tempIO;
		}

		if(_debounce_info[index]._Debounce_Tick >= _debounce_info[index]._Debounce_Count)
		{
			_debounce_info[index]._DebouncedIO = _debounce_info[index]._IOState;
		}
	}

	return _debounce_info[index]._DebouncedIO;
}

void _Debounce_Tick(void)
{
	UINT8 i;

	for(i = 0; i < _debounce_IONumber; i++)
	{
		if(_debounce_info[i]._Debounce_Tick < _debounce_info[i]._Debounce_Count)
		{
			_debounce_info[i]._Debounce_Tick++;
		}
	}
}
#endif
//-------------------------------End of Debounce----------------------------
//-------------------------------按键事件驱动产生层------------------------//
//-------------------------------event-------------------------------------//
#if defined USE_KEY_EVENT

UINT8 _key_number = 0;
UINT8 *_key_delay_id;
struct key_event *_key_event;

//function
UINT8 ConfigKeyMode(UINT8 num, UINT8 timer)
{
  	UINT8 index = 0;
	_key_number = num;
    //delay id
	_key_delay_id = (UINT8*)My_malloc(sizeof(UINT8) * num);
	My_memset(_key_delay_id, sizeof(UINT8), 0x00);

    GetDelayIdFunction(_key_number, _key_delay_id);

    for(index = 0; index < _key_number; index ++)
	_key_event[index].delay_id = _key_delay_id[index];
	//debuence
	Init_Debounce(_key_number, timer);
	//event
	_key_event = (struct key_event*)My_malloc(sizeof(struct key_event) * num);
	My_memset((UINT8 *)_key_event, sizeof(struct key_event), 0x00);
	return 0;  //return timer register count.
}
//------------register port scan-------------//
void RegisterKey(UINT8 id, CB_READ IOhdl, UINT16 debounceMS)
{
	if(id < _key_number)
	{
		_key_event[id].key_id = id;
		Config_Debounce(id, IOhdl, debounceMS);  			//debouence module.
		SetCountTime(_key_event[id].delay_id, KEY_MAX_WAIT_DELAY);                   //set delay max time.
	}
}
//envent layer
void GetKeyValue(void)
{
	UINT8 key = 0;
	for(key = 0; key < _key_number; key ++)
	SAVE8(_key_event[key].key_value, Get_DebouncedIO(key));
}

enum
{
	GET_IO_VALUE = 0,
	GET_KEY_EVENT
};
// 如果是第一次扫描，有可能将读不到键值。
void GetKeyEvent(void)
{
	UINT8 i = 0, new_key_event = 0;
	static UINT8 key = 0, FirstScan = 1;
	switch(key)
	{
		case GET_IO_VALUE:
				GetKeyValue();
				for(i = 0; i < _key_number; i ++)
				{
					if(((_key_event[i].key_value & 0x03) == 0x01) || ((_key_event[i].key_value & 0x03) == 0x02) ||
						((_key_event[i].key_value & 0x03) == 0x00))
					new_key_event = 1;
				}
				if(new_key_event)
				{
					if(FirstScan)
					FirstScan = 0;
					else
					key = GET_KEY_EVENT;
				}
				break;

		case GET_KEY_EVENT:
				for(i = 0; i < _key_number; i ++)
				{
					switch(_key_event[i].key_value & 0x03)
					{
						case KEY_HOLD:
							_key_event[i].key_hold_event = CheckHoldHitEvent(i);
							break;

						case KEY_DOWN:
							_key_event[i].key_double_hit = CheckDoubleHitEvent(i);
							ClearDoubleTime(i);
							_key_event[i].key_down = 1;
							StartHoldTime(i);
							break;

						case KEY_UP:
							ClearHoldTime(i);
							_key_event[i].key_up = 1;
							StartDoubleTime(i);  //开始连击事件计时
							break;
					}
				}
				key = GET_IO_VALUE;
				break;
	}
}


/*

*/
UINT8 CheckDoubleHitEvent(UINT8 key_id)
{
	if(ReturnSuccess(_key_event[key_id].delay_id))   //超过最大延时则 判断无效
	{
		ClearDelaySucFlag(_key_event[key_id].delay_id);
		return 0;
	}
	else
	{
		if((ReturnCurCount(_key_event[key_id].delay_id) >= UPS_MIN_TIME) &&
		   (ReturnCurCount(_key_event[key_id].delay_id) <= UPS_MAX_TIME))
		return 1;
	}
	return 0;
}

/*

*/
void ClearDoubleTime(UINT8 key_id)
{
	ClearDelaySucFlag(_key_event[key_id].delay_id);
	StopDelayCount(_key_event[key_id].delay_id);
	SetCountTime(_key_event[key_id].delay_id, 0);
}

/*

*/
void StartDoubleTime(UINT8 key_id)
{
	SetCountTime(_key_event[key_id].delay_id, KEY_MAX_WAIT_DELAY);
	StartDelayCount(_key_event[key_id].delay_id);
}

/*

*/
UINT8 CheckHoldHitEvent(UINT8 key_id)
{
	return(ReturnSuccess(_key_event[key_id].delay_id));
}
/*

*/
void StartHoldTime(UINT8 key_id)
{
	SetCountTime(_key_event[key_id].delay_id, HOLD_MAX_TIME);
	StartDelayCount(_key_event[key_id].delay_id);
}

/*

*/
void ClearHoldTime(UINT8 key_id)
{
	StopDelayCount(_key_event[key_id].delay_id);
	SetCountTime(_key_event[key_id].delay_id, 0);	 //设置值为0，
	ClearDelaySucFlag(_key_event[key_id].delay_id);
}
/*
return key event
*/
struct key_event *ReturnKeyEvent(UINT8 key_id)
{
	return &(_key_event[key_id]);
}

void ClrKeyDownEvent(UINT8 key_id)
{
	_key_event[key_id].key_down = 0;
}
/*
 key_up event.
*/
void ClrKeyUpEvent(UINT8 key_id)
{
	_key_event[key_id].key_up = 0;
}
/*
 key_long_hit event.
*/
void ClrKeyHoldEvent(UINT8 key_id)
{
	_key_event[key_id].key_hold_event = 0;
}
/*
 key_double_hit event
*/
void ClrKeyDoubleHitEvent(UINT8 key_id)
{
	_key_event[key_id].key_double_hit = 0;
}

#endif

//----------------------- Communication-------------------------------------------
#if defined USE_COMMUNICATION

void Communication(COMMUInfo* info)
{
	switch(info->_commu_type)
	{
		case CS_REPORT:
			_Commu_Report(info);
			break;

		case CS_CHECK:
			_Commu_Check(info);
			break;

		case CS_BROADCAST:
			_Commu_BroadCast(info);
			break;

		case CS_LISTENONLY:
			_Commu_ListenOnly(info);
			break;
	}
}

void Commu_Reset_Data(COMMUInfo* info)
{
	info-> _pParent = 0;

	info->_commu_type = 0;
	info->_commu_FSM = 0;
	//----------------//
	info->_receive_error = 0;
	//----2011-12-7---//
	info->_trytimes = 0;
	info->_TRYTIMES = 0;
	info->_sendtimes = 0;
	info->_SENDTIMES = 0;
	info->_timeout = 0;
	info->_TIMEOUT = 0;

	info->_send_FSM = 0;
	info->_start_send = 0;
	info->_send_status = 0;
	info->_data_empty_flag = FALSE;
	info->_render_data = 0;
	info->_load_data_to_buffer = 0;
	info->_transmit = 0;
	info->_transmit_success = 0;
	info->_cancel_transmit = 0;
	info->_send_end = 0;
	info->_send_failure = 0;


	info->_receive_FSM = 0;
	info->_receive_status = 0;
	info->_process_data = 0;
	info->_new_data_flag = FALSE;

	info->_errorCode = 0;
}

void Commu_Timer(COMMUInfo* info)
{
	if(info->_timeout <= info->_TIMEOUT) info->_timeout++;
}

void _Commu_Report(COMMUInfo* info)
{
	UINT8 _status;

	switch(info->_commu_FSM)
	{
		case CREPORT_IDLE:
			if(info->_start_send == TRUE)
			{
				info->_commu_FSM = CREPORT_SEND;
			}
			else break;

		case CREPORT_SEND:
			_status = _Commu_Send(info);
			if(_status == CS_SUCCESS)
			{
				info->_commu_FSM = 	CREPORT_WAITREPLY;
			}
			else if(_status == CS_FAILURE)
			{
				info->_commu_FSM = CREPORT_END;
				info->_errorCode |= COMMU_SEND_FAILURE;
			}
			break;

		case CREPORT_WAITREPLY:
			_status = _Commu_Receive(info);
			if(_status == CR_SUCCESS)
			{
				info->_commu_FSM = CREPORT_END;
			}
			else if(_status == CR_TIMEOUT)
			{
				info->_trytimes++;
				if(info->_trytimes >= info->_TRYTIMES)
				{
					info->_commu_FSM = CREPORT_END;
					info->_errorCode |= COMMU_NO_REPLY;
				}
				else
				{
					info->_commu_FSM = CREPORT_SEND;
					info->_start_send = TRUE;
				}
			}
			break;

		case CREPORT_END:
			info->_commu_FSM = CREPORT_IDLE;
			info->_trytimes = 0;
			break;
	}
}

void _Commu_Check(COMMUInfo* info)
{
	UINT8 _status;

	switch(info->_commu_FSM)
	{
		case CCHECK_RECEIVE:
			if(_Commu_Receive(info) == CR_SUCCESS)
			{
				info->_commu_FSM = CCHECK_REPLY;
				info->_start_send = TRUE;
			}
			else break;

		case CCHECK_REPLY:
			_status = _Commu_Send(info);
			if(_status == CS_SUCCESS)
			{
				info->_commu_FSM = CCHECK_END;
			}
			else if(_status == CS_FAILURE)
			{
				info->_commu_FSM = CCHECK_END;
				info->_errorCode |= COMMU_REPLY_FAILURE;
			}
			break;

		case CCHECK_END:
			info->_commu_FSM = CCHECK_RECEIVE;
			break;
	}
}

void _Commu_BroadCast(COMMUInfo* info)
{
	_Commu_Send(info);
}

void _Commu_ListenOnly(COMMUInfo* info)
{
	_Commu_Receive(info);
}

UINT8 _Commu_Send(COMMUInfo* info)
{
	switch(info->_send_FSM)
	{
		case SEND_IDLE:
			info->_timeout = 0;
			info->_sendtimes = 0;
			info->_send_status = CS_INPROGRESS;
			if(info->_start_send == TRUE)
			{
				info->_send_FSM = SEND_RENDER_DATA;
			}
			else break;

		case SEND_RENDER_DATA:
			if(info->_render_data != 0)
			{
				if(info->_render_data(info->_pParent) != TRUE) break;
			}
			info->_send_FSM = SEND_TRANSMIT;

		case SEND_TRANSMIT:
			if(info->_load_data_to_buffer != 0)
			{
				if(info->_load_data_to_buffer(info->_pParent) != TRUE) break;
			}
			info->_transmit(info->_pParent);
			info->_send_FSM = SEND_WAIT_SUCCESS;

		case SEND_WAIT_SUCCESS:
			if(info->_transmit_success != 0)
			{
				if(info->_transmit_success(info->_pParent) != TRUE) break;
			}

			if(info->_data_empty_flag == FALSE) info->_send_FSM = SEND_TRANSMIT;
			else info->_send_FSM = SEND_END;
			break;

		case SEND_TIMEOUT:
			if(info->_cancel_transmit != 0) info->_cancel_transmit(info->_pParent);
			info->_sendtimes++;
			info->_timeout = 0;
			if(info->_sendtimes > info->_SENDTIMES)
			{
				info->_send_FSM = SEND_FAILURE;
				info->_sendtimes = 0;
			}
			else info->_send_FSM = SEND_RENDER_DATA;
			break;

		case SEND_END:
			if(info->_send_end != 0) info->_send_end(info->_pParent);
			info->_send_FSM = SEND_IDLE;
			info->_start_send = FALSE;
			info->_data_empty_flag = FALSE;
			info->_send_status = CS_SUCCESS;
			info->_timeout = 0;
			break;

		case SEND_FAILURE:
			if(info->_send_failure != 0) info->_send_failure(info->_pParent);
			info->_send_FSM = SEND_IDLE;
			info->_start_send = FALSE;
			info->_data_empty_flag = FALSE;
			info->_send_status = CS_FAILURE;
			info->_timeout = 0;
			break;
	}

	if(info->_send_FSM != SEND_IDLE)
	{
		if(info->_timeout >= info->_TIMEOUT) info->_send_FSM = SEND_TIMEOUT;
	}

	return info->_send_status;
}

UINT8 _Commu_Receive(COMMUInfo* info)
{
	switch(info->_receive_FSM)
	{
		case RECEIVE_IDLE:
			info->_timeout = 0;
			info->_receive_FSM = RECEIVE_WAIT;
			info->_receive_status = CR_WAIT;

		case RECEIVE_WAIT:
			if(info->_new_data_flag == TRUE)
			{
				info->_receive_FSM = RECEIVE_SUCCESS;
			}
			else if((info->_timeout > info->_TIMEOUT))
			{
				info->_receive_FSM = RECEIVE_TIMEOUT;
			}
			break;

		case RECEIVE_SUCCESS:
			if(info->_process_data != 0)
			{
				info->_process_data(info->_pParent);
			}
			info->_new_data_flag = FALSE;
			info->_receive_status = CR_SUCCESS;
			info->_receive_FSM = RECEIVE_IDLE;
			break;

		case RECEIVE_TIMEOUT:
			info->_receive_error = FALSE;
	//		info->_new_data_flag = FALSE;
			info->_receive_status = CR_TIMEOUT;
			info->_receive_FSM = RECEIVE_IDLE;
			break;
	}

	return info->_receive_status;
}

#endif
//-----------------------End of Communication------------------------------------
//----------------------- CANApp ------------------------------------------------
#if defined USE_CANAPP

CANAPP_INFO* _CAN_commu;
UINT8 _CAN_commu_Num;
CANMsgInfo g_Transmit;
CANMsgInfo g_Receive;
CAN_TypeDef* g_can_id = 0;
/*

*/
void CANAPP_Init(CAN_TypeDef* can_x, UINT32 fcy, UINT32 baud, UINT16 num, ...)
{
	UINT8 i;
	UINT32 filter;
	g_can_id = can_x;
	CAN_Reset_CANMsg(&g_Transmit);
	CAN_Reset_CANMsg(&g_Receive);
	CAN_SetData(&g_Transmit, &g_Receive);
	CAN_Register(_CANAPP_Get_NewMsg);
    CAN_Config(can_x, fcy, baud);	
	va_list valist;	
	va_start(valist, num);
	for(i = 0; i < num; i++)
	{
		filter = (UINT32)(va_arg(valist, UINT8));
        _CAN_SetFilter(i, (filter << 8) & 0x0000FF00, 0x0007F800, CAN_FIFO0);
	}
	va_end(valist); 
}
	
/*

*/
void CANAPP_Config(UINT8 num, UINT16 timer)
{
	UINT8 i;
	_CAN_commu_Num = num;
	_CAN_commu = (CANAPP_INFO*)My_malloc(sizeof(CANAPP_INFO) * num);

	for(i = 0; i < num; i++)
	{
		Commu_Reset_Data(&_CAN_commu[i]._commu_info);
		_CAN_commu[i]._commu_info._pParent = (UINT16*)(_CAN_commu + i);
		_CAN_commu[i]._commu_info._TIMEOUT = CANAPP_DEFAULT_TIMEOUT * (Get_TimerInfo(timer)->_frequency / 1000); 
		_CAN_commu[i]._commu_info._SENDTIMES = CANAPP_DEFAULT_SENDTIMES;
		_CAN_commu[i]._commu_info._TRYTIMES = CANAPP_DEFAULT_TRYTIMES;

		_CAN_commu[i]._commu_info._load_data_to_buffer = _CANAPP_load_data_to_buffer;
		_CAN_commu[i]._commu_info._transmit = _CANAPP_transmit;
		_CAN_commu[i]._commu_info._transmit_success = _CANAPP_transmit_success;
		_CAN_commu[i]._commu_info._cancel_transmit = _CANAPP_cancel_transmit;
		_CAN_commu[i]._commu_info._send_end = _CANAPP_send_end;
		_CAN_commu[i]._commu_info._send_failure = _CANAPP_send_failure;
	}
	Timer_Register(timer, _CANAPP_Timer);
}

void CANAPP_Set_Data(UINT8 ID, UINT8 packageNum, UINT32 receiver, UINT32 sender, UINT32 rmt, UINT32 smt, UINT8 commuType)
{
	_CAN_commu[ID]._commu_info._commu_type = commuType;

	CAN_Reset_CANMsg(&_CAN_commu[ID]._myMsg);
	_CAN_commu[ID]._myMsg._receiver = receiver;
	_CAN_commu[ID]._myMsg._sender = sender;
	_CAN_commu[ID]._myMsg._recType = rmt;
	_CAN_commu[ID]._myMsg._transType = smt;

	_CAN_commu[ID]._data = My_malloc(sizeof(union _CANData) * 2 * packageNum);
	_CAN_commu[ID]._len = packageNum;
	_CAN_commu[ID]._slen = 0;
	_CAN_commu[ID]._rlen = 0;
	_CAN_commu[ID]._index = 0;
}

void CANAPP_Set_Para(UINT8 ID, COMMU_HANDLER render, COMMU_HANDLER process)
{
	_CAN_commu[ID]._commu_info._render_data = render;
	_CAN_commu[ID]._commu_info._process_data = process;
}
 
void CANAPP_Communication(void)
{
	UINT8 i = 0;
	for(i = 0; i < _CAN_commu_Num; i++)
	{
		Communication(&_CAN_commu[i]._commu_info);
	}
}

void CANAPP_Start(UINT8 ID)
{
	_CAN_commu[ID]._commu_info._start_send = TRUE;
}

CANAPP_INFO* CANAPP_GetInfo(UINT8 ID)
{
	return _CAN_commu + ID;
}

void _CANAPP_Timer(void)
{
	UINT8 i = 0;
	for(i = 0; i < _CAN_commu_Num; i++)
	{
		Commu_Timer(&_CAN_commu[i]._commu_info);
	}
}
/*

*/
UINT32 l_32 = 0, h_32 = 0;
BOOL _CANAPP_load_data_to_buffer(UINT16* pName)
{
	CANAPP_INFO* pInfo = (CANAPP_INFO*)(((COMMUInfo*)pName)->_pParent);

	if(g_Transmit._isBusy == TRUE) return FALSE;
		
	g_Transmit._receiver = pInfo->_myMsg._receiver;
	g_Transmit._sender = pInfo->_myMsg._sender;
	g_Transmit._transType = pInfo->_myMsg._transType;
	g_Transmit._recType = pInfo->_myMsg._recType;
	
	if(pInfo->_index < pInfo->_slen)
	{
		l_32 = pInfo->_data[pInfo->_index << 1]._DWord;
		h_32 = pInfo->_data[(pInfo->_index << 1) + 1]._DWord;		
		pInfo->_index++;
	}

	if(pInfo->_index < pInfo->_slen)
	{
		g_Transmit._packageHead = 1;
	}
	else
	{
		g_Transmit._packageHead = 0;
		pInfo->_index = 0;
		pInfo->_commu_info._data_empty_flag = TRUE;
	}
	CAN_Set_TransData(g_can_id, l_32,  h_32);	
	g_Transmit._isBusy = TRUE;

	return TRUE;
}

BOOL _CANAPP_transmit(UINT16* pName)
{
	return CAN_TransmitDataPage(g_can_id);
}

BOOL _CANAPP_transmit_success(UINT16* pName)
{
	if(CAN_Transmit_Success(g_can_id, g_Transmit._mailbox))
	{
		g_Transmit._isBusy = FALSE;
		return TRUE;
	}
	else return FALSE;
}

BOOL _CANAPP_cancel_transmit(UINT16* pName)
{
	CAN_Cancel_Transmit(g_can_id, g_Transmit._mailbox);
	return TRUE;
}

BOOL _CANAPP_send_end(UINT16* pName)
{
	return TRUE;
}

BOOL _CANAPP_send_failure(UINT16* pName)
{
	return TRUE;
}

void _CANAPP_Get_NewMsg(void)
{
	UINT8 i;

	for(i = 0; i < _CAN_commu_Num; i++)
	{
		if(g_Receive._recType == _CAN_commu[i]._myMsg._recType)
		{
			//------------------------------------------------//
			if(_CAN_commu[i]._commu_info._commu_type == CS_REPORT)
			{
				if(_CAN_commu[i]._myMsg._sender == g_Receive._receiver)
				{
					if(	_CAN_commu[i]._index < 	_CAN_commu[i]._len)
					{
						_CAN_commu[i]._data[_CAN_commu[i]._index << 1] = g_Receive._data[0];
						_CAN_commu[i]._data[(_CAN_commu[i]._index << 1) + 1] = g_Receive._data[1];
						_CAN_commu[i]._index ++;
					}
					if(g_Receive._packageHead == 0)
					{
						_CAN_commu[i]._rlen = _CAN_commu[i]._index;
						_CAN_commu[i]._commu_info._new_data_flag = TRUE;
						_CAN_commu[i]._index = 0;
					}	
				} 
				else
				{
					_CAN_commu[i]._commu_info._receive_error = 1;
					_CAN_commu[i]._commu_info._new_data_flag = 0;
				}
			}
			//------------------------------------------------//
			else
			{
				if(	_CAN_commu[i]._index < 	_CAN_commu[i]._len)
				{
					_CAN_commu[i]._data[_CAN_commu[i]._index << 1] = g_Receive._data[0];
					_CAN_commu[i]._data[(_CAN_commu[i]._index << 1) + 1] = g_Receive._data[1];
					_CAN_commu[i]._index ++;
				}
				if(g_Receive._packageHead == 0)
				{
					_CAN_commu[i]._rlen = _CAN_commu[i]._index;
					_CAN_commu[i]._commu_info._new_data_flag = TRUE;
					_CAN_commu[i]._index = 0;
				}
			}
			return;	
		}
	}
}

#endif
//-----------------------End of CANApp -------------------------------------------


//----------------------- STEPPER MOTOR CONTROL-----------------------------------
#if defined USE_UNIPOLAR_STEP_DRIVER

const UINT8 _Current_Table1[9][15] =
{
	{4 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0, 0, 0, 0},		// 100mA
	{6 , 6 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0, 0, 0, 0},		// 200mA
	{8 , 7 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0, 0, 0, 0},		// 300mA
	{9 , 9 , 9 , 9 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0, 0, 0, 0},		// 400mA
	{11, 10, 11, 11, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0, 0, 0, 0},		// 500mA
	{12, 12, 12, 12, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0, 0, 0, 0},		// 600mA
	{14, 13, 14, 14, 14, 14, 0 , 0 , 0 , 0 , 0 , 0, 0, 0, 0},		// 700mA
	{15, 14, 15, 16, 16, 16, 16, 0 , 0 , 0 , 0 , 0, 0, 0, 0},		// 800mA
	{18, 16, 17, 18, 19, 19, 19, 19, 23, 28, 34, 0, 0, 0, 0}		// >1A
};

const UINT8 _Current_Table2[9][15] =
{
	{3 , 3 , 5 , 7 , 9 , 10, 11, 12, 0 , 0 , 0 , 0 , 0 , 0 , 0 },		// 100mA
	{5 , 5 , 7 , 9 , 11, 12, 14, 16, 17, 0 , 0 , 0 , 0 , 0 , 0 },		// 150mA
	{6 , 7 , 8 , 11, 12, 14, 17, 19, 21, 22, 0 , 0 , 0 , 0 , 0 },		// 200mA
	{7 , 8 , 10, 12, 14, 16, 19, 21, 23, 26, 20, 0 , 0 , 0 , 0 },		// 250mA
	{8 , 9 , 11, 13, 15, 18, 21, 23, 25, 28, 24, 24, 0 , 0 , 0 },		// 300mA
	{9 , 10, 11, 14, 16, 19, 22, 25, 27, 30, 28, 28, 0 , 0 , 0 },		// 350mA
	{10, 11, 12, 14, 17, 20, 23, 26, 29, 32, 34, 30, 32, 0 , 0 },		// 400mA
	{11, 11, 13, 15, 18, 21, 24, 27, 30, 33, 36, 32, 34, 0 , 0 },		// 450mA
	{12, 12, 14, 16, 19, 22, 25, 28, 31, 34, 38, 34, 37, 39, 39}		// >500mA
};

struct _Driver_Info*  _DInfo;

#define OVDC_BIT(ch)		(0x0001 << (((ch) & 0x0F)))
#define CLR_OVDC			*(_DInfo[index]._OVDC) &= (~_DInfo[index]._OVDC_Mask)
#define SET_OVDC(data)		*(_DInfo[index]._OVDC) |= (data)

UINT16 table_step[10];

void Set_DirverNum(UINT8 num)
{
	_DInfo = (struct _Driver_Info*)My_malloc(sizeof(struct _Driver_Info) * num);
	My_memset((UINT8*)_DInfo, sizeof(struct _Driver_Info) * num, 0x00);
}

void Config_PWM(UINT8 index, UINT8 style, UINT32 fcy, UINT32 frequency, UINT8 timer)
{
	_DInfo[index]._style = style;
	if(style == PWM_DRIVER)
	{
		PWM_Config(fcy, frequency);
	}
}


void Config_Driver(UINT8 index, UINT8 mode, UINT16 resolution)
{
	UINT8 i;

	_DInfo[index]._mode = mode;
	_DInfo[index]._resolution = resolution;
	_DInfo[index]._table = (UINT16*)My_malloc(sizeof(UINT16) * ((resolution >> 2) + 1));

	for(i = 0; i < (resolution >> 2) + 1; i++) //_DInfo[index]._table[i]
	{
		_DInfo[index]._table[i] = _DInfo[index]._PWM_Base * cos(6.28 * i / resolution) * 0.8;
		if(mode == PUSHPULL_STEPPER) _DInfo[index]._table[i] >>= 1;
	}
}

void Config_Channel(UINT8 index, UINT8 chApos, UINT8 chBpos, UINT8 chAneg, UINT8 chBneg)
{
	_DInfo[index]._ch[APOS] = chApos;
	_DInfo[index]._ch[BPOS] = chBpos;
	_DInfo[index]._ch[ANEG] = chAneg;
	_DInfo[index]._ch[BNEG] = chBneg;

 if(_DInfo[index]._style == PWM_DRIVER)
	{
		_DInfo[index]._PDC[APOS] = Get_PDC(chApos);
		_DInfo[index]._PDC[BPOS] = Get_PDC(chBpos);
		_DInfo[index]._PDC[ANEG] = Get_PDC(chAneg);
		_DInfo[index]._PDC[BNEG] = Get_PDC(chBneg);

		_DInfo[index]._OVDC = Get_OVDC();
		_DInfo[index]._OVDC_Mask  = OVDC_BIT(chApos) | OVDC_BIT(chBpos) | OVDC_BIT(chAneg) | OVDC_BIT(chBneg);
		_DInfo[index]._phase[0] = OVDC_BIT(chApos) | OVDC_BIT(chBpos);
		_DInfo[index]._phase[1] = OVDC_BIT(chBpos) | OVDC_BIT(chAneg);
		_DInfo[index]._phase[2] = OVDC_BIT(chAneg) | OVDC_BIT(chBneg);
		_DInfo[index]._phase[3] = OVDC_BIT(chBneg) | OVDC_BIT(chApos);

		Enable_PWM(chApos, PWM_COMPLEMENTARY);
		Enable_PWM(chBpos, PWM_COMPLEMENTARY);
		Enable_PWM(chAneg, PWM_COMPLEMENTARY);
		Enable_PWM(chBneg, PWM_COMPLEMENTARY);

		EnablePwmTimer();
		_DInfo[index]._PWM_Base = PWM_GetBase(PWM0);
	}
}

void Driver_Direction(UINT8 index, BOOL dir)
{
	_DInfo[index]._dir = dir;
}

void Driver_Hold(UINT8 index, UINT8 para)
{
	if(para == 0)
	{
		_DInfo[index]._step = 0;
		_DInfo[index]._quadrant = 0;
	}
	_change_quadrant(index);
	_change_PDC(index);
	_DInfo[index]._enable = TRUE;		//需要放在末尾
}

void Driver_Free(UINT8 index)
{
	_DInfo[index]._enable = FALSE;		//需要放在第一句
	*(_DInfo[index]._PDC[APOS]) = 0;
	*(_DInfo[index]._PDC[BPOS]) = 0;
	*(_DInfo[index]._PDC[ANEG]) = 0;
	*(_DInfo[index]._PDC[BNEG]) = 0;

	if(_DInfo[index]._style == PWM_DRIVER) CLR_OVDC;
}

void Driver_Step(UINT8 index)
{
	if(_DInfo[index]._enable == TRUE)
	{
		if(_DInfo[index]._dir == STEP_CLOCKWISE)
		{
			_DInfo[index]._step++;
			if(_DInfo[index]._step >= (_DInfo[index]._resolution >> 2))
			{
				_DInfo[index]._step = 0;
				_DInfo[index]._quadrant = ((_DInfo[index]._quadrant + 1) & 0x03);
				_change_quadrant(index);
			}
		}
		else		//STEP_COUNTERCLOCKWISE
		{
			_DInfo[index]._step--;
			if(_DInfo[index]._step == 0xFF)
			{
				_DInfo[index]._step = (_DInfo[index]._resolution >> 2) - 1;
				_DInfo[index]._quadrant = ((_DInfo[index]._quadrant - 1) & 0x03);
				_change_quadrant(index);
			}
		}
		_change_PDC(index);
	}
}

void Set_CurrentLevel(UINT8 index, UINT8 currentLevel)
{
	_DInfo[index]._currentLevel = currentLevel;
}

UINT8 Get_Current_Level(UINT8 index, UINT16 speed, UINT16 current)
{
	UINT8 currentlevel;
	UINT8 speedlevel = speed / (_DInfo[index]._resolution * 25);

	if(speedlevel > 14) speedlevel = 14;

	if(_DInfo[index]._mode == UNIPOLAR_STEPPER)
	{
		if(current <= 150) currentlevel = 0;
		else if(current <= 250) currentlevel = 1;
		else if(current <= 350) currentlevel = 2;
		else if(current <= 450) currentlevel = 3;
		else if(current <= 550) currentlevel = 4;
		else if(current <= 650) currentlevel = 5;
		else if(current <= 750) currentlevel = 6;
		else if(current <= 850) currentlevel = 7;
		else currentlevel = 8;

		return _Current_Table1[currentlevel][speedlevel];
	}
	else//PUSHPULL_STEPPER
	{
		if(current <= 125) currentlevel = 0;
		else if(current <= 175) currentlevel = 1;
		else if(current <= 225) currentlevel = 2;
		else if(current <= 275) currentlevel = 3;
		else if(current <= 320) currentlevel = 4;
		else if(current <= 375) currentlevel = 5;
		else if(current <= 425) currentlevel = 6;
		else if(current <= 475) currentlevel = 7;
		else currentlevel = 8;

		return _Current_Table2[currentlevel][speedlevel];
	}
}

void Set_Driver_Position(UINT8 index, UINT16 angle)
{
	UINT16 steps = (UINT32)angle * _DInfo[index]._resolution * 50 / 360;
	steps = steps % _DInfo[index]._resolution;
	_DInfo[index]._quadrant = steps / (_DInfo[index]._resolution >> 2);
	_DInfo[index]._step = steps % (_DInfo[index]._resolution >> 2);
}

struct _Driver_Info Get_Driver_Info(UINT8 index)
{
	return _DInfo[index];
}

void _change_quadrant(UINT8 index)
{
	if(_DInfo[index]._style == PWM_DRIVER)
	{
		if(_DInfo[index]._mode == PUSHPULL_STEPPER)
		{
			SET_OVDC(_DInfo[index]._phase[0] | _DInfo[index]._phase[2]);
		}
		else
		{
			CLR_OVDC;
			SET_OVDC(_DInfo[index]._phase[_DInfo[index]._quadrant]);
		}
	}
}

void _change_PDC(UINT8 index)
{
	UINT16 pdc1, pdc2, base = 0;

	pdc1 = _DInfo[index]._table[_DInfo[index]._step];
	pdc2 = _DInfo[index]._table[(_DInfo[index]._resolution >> 2) - _DInfo[index]._step];
	_modify_current(&pdc1, &pdc2, _DInfo[index]._currentLevel);

	if(_DInfo[index]._mode == UNIPOLAR_STEPPER)
	{
		base = _DInfo[index]._PWM_Base;
		switch(_DInfo[index]._quadrant)
		{
			case 0:
				_set_quadrant_PDC(index, pdc1, pdc2, 0, 0);
				break;

			case 1:
				_set_quadrant_PDC(index, base - pdc2, pdc1, 0, 0);
				break;

			case 2:
				_set_quadrant_PDC(index, base - pdc1, base - pdc2, 0, 0);
				break;

			case 3:
				_set_quadrant_PDC(index, pdc2, base - pdc1, 0, 0);
				break;
		}
	}
	else		//PUSHPULL_STEPPER
	{
		base = (_DInfo[index]._PWM_Base >> 1);
		switch(_DInfo[index]._quadrant)
		{
			case 0:
				_set_quadrant_PDC(index, pdc1 + base, pdc2 + base, 0, 0);
				break;

			case 1:
				_set_quadrant_PDC(index, base - pdc2, pdc1 + base, 0, 0);
				break;

			case 2:
				_set_quadrant_PDC(index, base - pdc1, base - pdc2, 0, 0);
				break;

			case 3:
				_set_quadrant_PDC(index, pdc2 + base, base - pdc1, 0, 0);
				break;
		}
	}
}

void _modify_current(UINT16* pdc1, UINT16* pdc2, UINT8 level)
{
	UINT16 p1 = 0, p2 = 0, i = 6;

	if(level > 40) level = 40;

	while(i > 0)
	{
		i--;

		if((level & 0x01) == 1)
		{
			p1 += *(pdc1) >> i;
			p2 += *(pdc2) >> i;
		}
		level >>= 1;
	}

	*(pdc1) = p1;
	*(pdc2) = p2;
}

void _set_quadrant_PDC(UINT8 index, UINT16 Tapos, UINT16 Tbpos, UINT16 Taneg, UINT16 Tbneg)
{
	*(_DInfo[index]._PDC[APOS]) = Tapos;
	*(_DInfo[index]._PDC[BPOS]) = Tbpos;
}

#endif
//-----------------------End of STEPPER MOTOR CONTROL-----------------------------

//----------------------- 步进电机任务管理模块------------------------------------
#if defined USE_STEPPER_TASK_MANAGER

struct _Stepper_Task_Manager* _TManager;
UINT8 _taskNum;

void Set_TaskNum(UINT8 num)
{
	_taskNum = num;
	_TManager = (struct _Stepper_Task_Manager*)My_malloc(sizeof(struct _Stepper_Task_Manager) * num);
	My_memset((UINT8*)(_TManager), sizeof(struct _Stepper_Task_Manager), 0x00);
}
void Config_Task(UINT8 index, UINT8 driverID, UINT32 fcy, UINT8 timer, UINT8 pri)
{
	_TManager[index]._driverID = driverID;
	_TManager[index]._driverResolution = Get_Driver_Info(_TManager[index]._driverID)._resolution;
	_TManager[index]._driverMode = Get_Driver_Info(_TManager[index]._driverID)._mode;
	_TManager[index]._fcy = fcy;
	_TManager[index]._timer = timer;
	_TManager[index]._msPrn = fcy / (1000 * 6);

	Timer_Init(fcy, timer, pri, 1000);
	if(driverID == 0) Timer_Register(timer, _stepper_manager_timer1);
	else if(driverID == 1) Timer_Register(timer, _stepper_manager_timer2);
	else if(driverID == 2) Timer_Register(timer, _stepper_manager_timer3);
}

void Set_Task(UINT8 index, UINT8 type, UINT8 dir, UINT16 angle, UINT16 time)
{
	_TManager[index]._taskType = type;
	_TManager[index]._taskDirection = dir;
	_TManager[index]._taskAngle = angle;
	_TManager[index]._taskTime = time;
	_TManager[index]._taskSteps = (UINT32)angle * _TManager[index]._driverResolution * 50 / 360;
}

void Set_Speed(UINT8 index, UINT16 speed1, UINT16 time1, UINT16 speed2, UINT16 time2)
{
	UINT8 i;//double 被替换成了float.
	double v1, t1, l1, a1, v2, t2, l2, a2, v, temp;

	v1 = (double)speed1 * _TManager[index]._driverResolution * 50 / 360;
	v2 = (double)speed2 * _TManager[index]._driverResolution * 50 / 360;
	v = (double)(_TManager[index]._taskAngle) * 1000. / (double)(_TManager[index]._taskTime);
	v *= _TManager[index]._driverResolution * 50 / 360;
	if(v1 > v) v1 = v;
	if(v2 > v) v2 = v;
	t1 = time1 / 1000.;
	t2 = time2 / 1000.;
	v = _TManager[index]._taskSteps - t1 * v1 - t2 * v2;
	v /= _TManager[index]._taskTime / 1000. - t1 - t2;

	_TManager[index]._maxSpeed = (UINT16)v;

	a1 = (v - v1) / t1;
	l1 = v1 * t1 + a1 * t1 * t1 / 2;
	_TManager[index]._speedupSteps = (UINT16)(l1 + 0.5);
	_TManager[index]._speedup_PrnTable = (UINT16*)My_malloc(sizeof(UINT16) * _TManager[index]._speedupSteps);
	_TManager[index]._speedup_CurrentLevelTable = (UINT16*)My_malloc(sizeof(UINT16) * _TManager[index]._speedupSteps);
	for(i = _TManager[index]._speedupSteps; i > 0; i--)
	{
		if(v * v <= 2 * a1 * i) temp = 65534;
		else temp = (1.0 / sqrt(v * v - 2 * a1 * i)) * _TManager[index]._msPrn * 1000;
		if(temp >= 65535) temp = 65534;
		_TManager[index]._speedup_PrnTable[_TManager[index]._speedupSteps - i] = (UINT16)temp;
		temp = _TManager[index]._msPrn * 1000. / temp;
		_TManager[index]._speedup_CurrentLevelTable[_TManager[index]._speedupSteps - i] = (UINT16)temp;
	}

	a2 = (v - v2) / t2;
	l2 = v2 * t2 + a2 * t2 * t2 / 2;
	_TManager[index]._speeddownSteps = (UINT16)(l2 + 0.5);
	_TManager[index]._speeddown_PrnTable = (UINT16*)My_malloc(sizeof(UINT16) * _TManager[index]._speeddownSteps);
	_TManager[index]._speeddown_CurrentLevelTable = (UINT16*)My_malloc(sizeof(UINT16) * _TManager[index]._speeddownSteps);
	for(i = 1; i <= _TManager[index]._speeddownSteps; i++)
	{
		if(v * v <= 2 * a2 * i) temp = 65534;
		else temp = (1.0 / sqrt(v * v - 2 * a2 * i)) * _TManager[index]._msPrn * 1000;
		if(temp >= 65535) temp = 65534;
		_TManager[index]._speeddown_PrnTable[i - 1] = (UINT16)temp;
		temp = _TManager[index]._msPrn * 1000. / temp;
		_TManager[index]._speeddown_CurrentLevelTable[i - 1] = (UINT16)temp;
	}

	_TManager[index]._maxSpeed_Prn = (UINT16)(_TManager[index]._msPrn * 1000. / v);
}

void Set_Rest_Para(UINT8 index, BOOL isKeepHold, UINT16 restCurrent)
{
	_TManager[index]._isRest_KeepHold = isKeepHold;
	_TManager[index]._Rest_Current = restCurrent;
	_TManager[index]._Rest_CurrentLevel = Get_Current_Level(_TManager[index]._driverID, 0, restCurrent);
}
void Set_StartPoint_Para(UINT8 index, UINT16 holdTime, UINT16 holdCurrent)
{
	_TManager[index]._StartPoint_HoldTime = holdTime;
	_TManager[index]._StartPoint_Current = holdCurrent;
	_TManager[index]._StartPoint_CurrentLevel = Get_Current_Level(_TManager[index]._driverID, 0, holdCurrent);
}

void Set_MiddlePoint_Para(UINT8 index, UINT16 holdTime, UINT16 holdCurrent)
{
	if(holdTime > 0) _TManager[index]._isMiddlePoint_KeepHold = TRUE;
	else _TManager[index]._isMiddlePoint_KeepHold = FALSE;
	_TManager[index]._MiddlePoint_HoldTime = holdTime;
	_TManager[index]._MiddlePoint_Current = holdCurrent;
	_TManager[index]._MiddlePoint_CurrentLevel = Get_Current_Level(_TManager[index]._driverID, 0, holdCurrent);
}

void Set_EndPoint_Para(UINT8 index, UINT16 holdTime, UINT16 holdCurrent)
{
	_TManager[index]._EndPoint_HoldTime = holdTime;
	_TManager[index]._EndPoint_Current = holdCurrent;
	_TManager[index]._EndPoint_CurrentLevel = Get_Current_Level(_TManager[index]._driverID, 0, holdCurrent);
}

void Set_MovingCurrent(UINT8 index, UINT16 current)
{
	UINT16 i, temp;

	_TManager[index]._Moving_Current = current;
	_TManager[index]._Moving_CurrentLevel = Get_Current_Level(_TManager[index]._driverID, _TManager[index]._maxSpeed, current);


	for(i = _TManager[index]._speedupSteps; i > 0; i--)
	{
		temp = _TManager[index]._speedup_CurrentLevelTable[_TManager[index]._speedupSteps - i];
		temp = Get_Current_Level(_TManager[index]._driverID, temp, current);
		_TManager[index]._speedup_CurrentLevelTable[_TManager[index]._speedupSteps - i] = temp;
	}

	for(i = 1; i <= _TManager[index]._speeddownSteps; i++)
	{
		temp = _TManager[index]._speeddown_CurrentLevelTable[i - 1];
		temp = Get_Current_Level(_TManager[index]._driverID, temp, current);
		_TManager[index]._speeddown_CurrentLevelTable[i - 1] = temp;
	}
}

void Activate_Task(UINT8 index, UINT8 state)
{
	_TManager[index]._isActive = state;
}

void Start_Task(UINT8 index, UINT8 state)
{
	if(_TManager[index]._movingFlag == MOVING_NONE)
	{
		if(state == MOVING_FORWARD) _TManager[index]._movingFlag = state;
	}
	else if(_TManager[index]._movingFlag == MOVING_FORWARD)
	{
		if(state == MOVING_BACKWARD) _TManager[index]._movingFlag = state;
	}
}

struct _Stepper_Task_Manager Get_Task_Info(UINT8 index)
{
	return _TManager[index];
}

/*
copy speed up table.
*/
UINT8 CopySpeedupTable(UINT8 index, UINT16 *uptable)
{
	UINT8 speed_up = 0, speedup_steps = 0;
	for(speed_up = 0; speed_up < _TManager[index]._speedupSteps; speed_up ++)
	{
		*uptable = _TManager[index]._speedup_PrnTable[speed_up];
		uptable ++;
		speedup_steps ++;
	}
	return speedup_steps;
}
/*
copy speed down table.
*/
UINT8 CopySpeeddownTable(UINT8 index, UINT16 *downtable)
{
	UINT8 speed_down = 0, speeddown_steps = 0;
	for(speed_down = 0; speed_down < _TManager[index]._speeddownSteps; speed_down ++)
	{
		*downtable = _TManager[index]._speeddown_PrnTable[speed_down];
		downtable ++;
		speeddown_steps ++;
	}
	return speeddown_steps;
}


//-------------------满足珠片绣而添加---------------------//

void _stepper_manager_timer1(void)
{
	UINT8 i;
	for(i = 0; i < _taskNum; i++)
	{
		if(_TManager[i]._driverID == 0) _stepper_manager(i);
	}
}

void _stepper_manager_timer2(void)
{
	UINT8 i;
	for(i = 0; i < _taskNum; i++)
	{
		if(_TManager[i]._driverID == 1) _stepper_manager(i);
	}
}

void _stepper_manager_timer3(void)
{
	UINT8 i;
	for(i = 0; i < _taskNum; i++)
	{
		if(_TManager[i]._driverID == 2) _stepper_manager(i);
	}
}

void _stepper_manager(UINT8 index)
{
	switch(_TManager[index]._FSM)
	{
		case DRIVER_IDLE:
			if(_TManager[index]._isActive) _TManager[index]._FSM = DRIVER_WAIT;
			else break;

		case DRIVER_WAIT:
			if(!_TManager[index]._isActive)
			{
				_TManager[index]._FSM = DRIVER_IDLE;
				break;
			}

			if(_TManager[index]._isRest_KeepHold == TRUE)
			{
				Set_CurrentLevel(_TManager[index]._driverID, _TManager[index]._Rest_CurrentLevel);
				Driver_Hold(_TManager[index]._driverID, 1);
			}
			else Driver_Free(_TManager[index]._driverID);

			if(_TManager[index]._movingFlag == MOVING_FORWARD)
			{
				_TManager[index]._FSM = DRIVER_STARTPOINT;
				_TManager[index]._tick = 0;
			}
			else break;

		case DRIVER_STARTPOINT:
			if(_TManager[index]._tick < _TManager[index]._StartPoint_HoldTime)
			{
				_TManager[index]._tick++;
				Set_CurrentLevel(_TManager[index]._driverID, _TManager[index]._StartPoint_CurrentLevel);
				Driver_Hold(_TManager[index]._driverID, 1);
				Timer_SetPrn(_TManager[index]._timer, _TManager[index]._msPrn);
				break;
			}
			else
			{
				_TManager[index]._FSM = DRIVER_MOVINGFORWARD;
				_TManager[index]._tick = 0;
				_TManager[index]._subFSM = SPEED_UP;
				Driver_Direction(_TManager[index]._driverID, _TManager[index]._taskDirection);
				Driver_Hold(_TManager[index]._driverID, 1);
			}

		case DRIVER_MOVINGFORWARD:
			if(_moving(index) == FALSE) break;
			else
			{
				if(_TManager[index]._taskType == ONE_WAY_MOTION)
				{
					_TManager[index]._FSM = DRIVER_ENDPOINT;
					break;
				}
				else
				{
					_TManager[index]._FSM = DRIVER_MIDDLEPOINT;
				}
			}

		case DRIVER_MIDDLEPOINT:
			if(_TManager[index]._taskType == AUTO_BACK_FORTH_MOTION)
			{
				if((_TManager[index]._tick < _TManager[index]._MiddlePoint_HoldTime)
						&& (_TManager[index]._isMiddlePoint_KeepHold == TRUE))
				{
					_TManager[index]._tick++;
					Set_CurrentLevel(_TManager[index]._driverID, _TManager[index]._MiddlePoint_CurrentLevel);
					Driver_Hold(_TManager[index]._driverID, 1);
					Timer_SetPrn(_TManager[index]._timer, _TManager[index]._msPrn);
					break;
				}
				_TManager[index]._movingFlag = MOVING_BACKWARD;
			}
			else//BACK_FORTH_MOTION
			{
				if(_TManager[index]._isMiddlePoint_KeepHold == TRUE)
				{
					Set_CurrentLevel(_TManager[index]._driverID, _TManager[index]._MiddlePoint_CurrentLevel);
					Driver_Hold(_TManager[index]._driverID, 1);
				}
				else Driver_Free(_TManager[index]._driverID);
				Timer_SetPrn(_TManager[index]._timer, _TManager[index]._msPrn);
			}

			if(_TManager[index]._movingFlag == MOVING_BACKWARD)
			{
				_TManager[index]._FSM = DRIVER_MOVINGBACKWARD;
				_TManager[index]._tick = 0;
				_TManager[index]._subFSM = SPEED_UP;
				Driver_Direction(_TManager[index]._driverID, 1 - _TManager[index]._taskDirection);
				Driver_Hold(_TManager[index]._driverID, 1);
			}
			else break;

		case DRIVER_MOVINGBACKWARD:
			if(_moving(index) == FALSE) break;
			else _TManager[index]._FSM = DRIVER_ENDPOINT;

		case DRIVER_ENDPOINT:
			if(_TManager[index]._tick < _TManager[index]._EndPoint_HoldTime)
			{
				_TManager[index]._tick++;
				Set_CurrentLevel(_TManager[index]._driverID, _TManager[index]._EndPoint_CurrentLevel);
				Driver_Hold(_TManager[index]._driverID, 1);
				Timer_SetPrn(_TManager[index]._timer, _TManager[index]._msPrn);
			}
			else
			{
				Timer_SetPrn(_TManager[index]._timer, _TManager[index]._msPrn);
				_TManager[index]._movingFlag = MOVING_NONE;
				_TManager[index]._FSM = DRIVER_WAIT;
				_TManager[index]._tick = 0;
				_TManager[index]._subFSM = SPEED_UP;
			}
			break;

		case DRIVER_ERROR:
			_TManager[index]._movingFlag = MOVING_NONE;
			break;

	}
}

BOOL _moving(UINT8 index)
{
	switch(_TManager[index]._subFSM)
	{
		case SPEED_UP:
			Set_CurrentLevel(_TManager[index]._driverID, _TManager[index]._speedup_CurrentLevelTable[_TManager[index]._currentStep]);
			Driver_Step(_TManager[index]._driverID);
			Timer_SetPrn(_TManager[index]._timer, _TManager[index]._speedup_PrnTable[_TManager[index]._currentStep]);
			_TManager[index]._currentStep++;
			if(_TManager[index]._currentStep >= _TManager[index]._speedupSteps)
			{
				_TManager[index]._subFSM = SPEED_UNIFORM;
			}
			break;

		case SPEED_UNIFORM:
			Set_CurrentLevel(_TManager[index]._driverID, _TManager[index]._Moving_CurrentLevel);
			Driver_Step(_TManager[index]._driverID);
			Timer_SetPrn(_TManager[index]._timer, _TManager[index]._maxSpeed_Prn);
			_TManager[index]._currentStep++;
			if(_TManager[index]._currentStep + _TManager[index]._speeddownSteps >= _TManager[index]._taskSteps)
			{
				_TManager[index]._subFSM = SPEED_DOWN;
				_TManager[index]._currentStep = 0;
			}
			break;

		case SPEED_DOWN:
			Set_CurrentLevel(_TManager[index]._driverID, _TManager[index]._speeddown_CurrentLevelTable[_TManager[index]._currentStep]);
			Driver_Step(_TManager[index]._driverID);
			Timer_SetPrn(_TManager[index]._timer, _TManager[index]._speeddown_PrnTable[_TManager[index]._currentStep]);
			_TManager[index]._currentStep++;
			if(_TManager[index]._currentStep >= _TManager[index]._speeddownSteps)
			{
				_TManager[index]._currentStep = 0;
				_TManager[index]._subFSM = SPEED_UP;
				return TRUE;
			}
			break;
	}

	return FALSE;
}
/*

*/
void UpdataStepMotorSpeedup(UINT8 index, UINT16* speedup_PrnTable, UINT8 *speedup_steps)
{
	UINT8 speedup = 0;
	for(speedup = 0; speedup < _TManager[index]._speedupSteps; speedup ++)
	{
		_TManager[index]._speedup_PrnTable[speedup] = speedup_PrnTable[speedup];
	}
	*speedup_steps = _TManager[index]._speedupSteps;
}
/*

*/
void UpdataStepMotorSpeeddown(UINT8 index, UINT16* speeddown_PrnTable, UINT8 *speeddown_steps)
{
	UINT8 speeddown = 0;
	for(speeddown = 0; speeddown < _TManager[index]._speeddownSteps; speeddown ++)
	{
		_TManager[index]._speeddown_PrnTable[speeddown] = speeddown_PrnTable[speeddown];
	}
	*speeddown_steps = _TManager[index]._speeddownSteps;
}
/*

*/
void UpdataStepMotorSpeedArge(UINT8 index, UINT16 max_speed, UINT16 max_prn, UINT16 steps)
{
	_TManager[index]._taskSteps    = steps;
//	_TManager[index]._maxSpeed     = max_speed;
	_TManager[index]._maxSpeed_Prn = max_prn;
}

//---------------------?ú×??é??D???ìí?ó---------------------//
void UpdataTaskArgv(UINT8 index, UINT8 MotorDirect, UINT16 speedup_steps, UINT16 speeddown_steps)
{
	_TManager[index]._taskDirection  = MotorDirect;
	_TManager[index]._speedupSteps   = speedup_steps;
	_TManager[index]._speeddownSteps = speeddown_steps;
}

#endif
//-----------------------End of 步进电机任务管理模块------------------------------

//----------------------- COBS模块------------------------------------------------
#if defined USE_COBS

#define RENDER_BLOCK(x) (*code_ptr = (x), code_ptr = dst, dst++, code = 0x01, len++)

void COBS_Encoding8(UINT8* ptr, UINT8 ptr_len, UINT8* dst, UINT8* dst_len)
{
	UINT8 code = 0x01;
	UINT8* end = ptr + ptr_len;
	UINT8* code_ptr = dst;
	UINT8 len = 0;

	(*end) = 0x00;		//末尾额外加一个0
	dst++;

	while (ptr <= end)
	{
		if(*ptr == 0)
		{
			RENDER_BLOCK(code);
		}
		else
		{
			*dst = *ptr;
			dst++;
			code++;
			len++;
			if(code == 0xFF)
			{
				RENDER_BLOCK(code);
			}
		}
		ptr++;
	}

	*dst_len = len;
}

void COBS_Decoding8(UINT8* ptr, UINT8 ptr_len, UINT8* dst, UINT8* dst_len)
{
	UINT16 i, code;
	UINT8* end = ptr + ptr_len;
	UINT8 len = 0;

	while (ptr < end)
	{
		code = *ptr;
		ptr++;

		for(i = 1; i < code; i++)
		{
			*dst = *ptr;
			dst++;
			ptr++;
			len++;
		}

		if(code < 0xFF)
		{
			*dst = 0;
			dst++;
			len++;
		}
	}

	*dst_len = len - 1;		//去掉末尾的0
}

UINT8 COBS_Encode(UINT8* ptr, UINT8 len)
{
	UINT8 x1, x2, i;
	UINT8* tpt;

	if(len == 0 || len >= 254) return FALSE;
	*(ptr + len) = 0x00;		//末尾额外加一个0
	len++;
	x2 = *ptr;
	tpt = ptr;
	*tpt = 1;

	for(i = 1; i < len; i++)
	{
		ptr++;
		x1 = x2;
		x2 = *ptr;

		if(x1 == 0)
		{
			tpt = ptr;
			*tpt = 1;
		}
		else
		{
			(*tpt)++;
			*ptr = x1;
		}
	}

	return len + 1;
}

UINT8 COBS_Decode(UINT8* ptr, UINT8 len)
{
	UINT8 x, i;

	if(len == 0 || len >= 255) return FALSE;
	x = *ptr;

	for(i = 1; i < len; i++)
	{
		ptr++;
		x--;

		if(x == 0)
		{
			*(ptr - 1) = 0;
			x = *ptr;
		}
		else
		{
			*(ptr - 1) = *ptr;
		}
	}

	return len - 1;
}

UINT8 COBS_Package(UINT8* ptr, UINT8 len)
{
	UINT8 i;

	for(i = len + 3; i > 3; i--)
	{
		*(ptr + i) = *(ptr + i - 4);
	}

	*(ptr + 0) = 'C';
	*(ptr + 1) = 'O';
	*(ptr + 2) = 'B';
	*(ptr + 3) = 'S';
	*(ptr + len + 5) = 0x00;

	COBS_Encode(ptr + 4, len);

	return len + 6;
}

UINT8 COBS_UpPackage(UINT8* ptr, UINT8 len)
{
	UINT8 i;

	for(i = 0; i < len - 5; i++)
	{
		*(ptr + i) = *(ptr + i + 4);
	}

	COBS_Decode(ptr, len - 5);

	return len - 6;
}

#endif
//-----------------------End of COBS模块------------------------------------------

//----------------------- UARTApp模块---------------------------------------------
#if defined USE_UARTMANAGER

#endif

//-----------------------End of UARTApp模块---------------------------------------

//------------------------start delay--------------------------------------------//
#ifdef USE_DELAY

UINT8 d_module_num = 0, _delay_count_id = 0;

struct delay_info *_delay_module;

//函数声明
void DelayLoop(void);
/*

*/
void ConfigDelayModuleNum(UINT8 num, UINT8 timer)
{
	d_module_num = num;
	_delay_module = (struct delay_info*)My_malloc(sizeof(struct delay_info) * num);
	My_memset((UINT8 *)_delay_module, sizeof(struct delay_info) * num, 0x00);
	if(timer == SYS_TIMER)
		SysTick_Register(DelayLoop); 
	else
		Timer_Register(timer, DelayLoop);
}

/*
add delay id.
*/
UINT8 GetDelayIdFunction(UINT8 num, UINT8 *return_id)
{
	UINT8 add_success_flag = 0, index = 0;
	if((_delay_count_id + num) < d_module_num)
	{
		for(index = 0; index < num; index ++)
		{
			if(return_id != 0)
			{
				*return_id = _delay_count_id;
				return_id ++;
				_delay_count_id ++;
			}
			else
			return 0;
		}
		add_success_flag = 1;
	}
	else
	return 0;
	return add_success_flag;
}
/*

*/
char SetCountTime(UINT8 id_number, uint32_t time)
{
	if(id_number < d_module_num)
	{
		_delay_module[id_number].Time = time;
		return 1;
	}
	return 0;
}

/*

*/
char StartDelayCount(UINT8 id_number)
{
	if(id_number < d_module_num)
	{
		_delay_module[id_number].StartFlag = 1;
		return 1;
	}
	return 0;
}

char StopDelayCount(UINT8 id_number)
{
	if(id_number < d_module_num)
	{
		_delay_module[id_number].StartFlag = 0;
		return 1;
	}
	return 0;
}

char ReturnSuccess(UINT8 id_number)
{
	if(id_number < d_module_num)
	return _delay_module[id_number].Success;

	return 0;
}

/*

*/
unsigned int ReturnCurCount(UINT8 id_number)
{
	if(id_number < d_module_num)
	return _delay_module[id_number].TimeCount;

	return 0;
}

/*

*/
char ClearDelaySucFlag(UINT8 id_number)
{
	if(id_number < d_module_num)
	{
		_delay_module[id_number].Success = 0;
		_delay_module[id_number].TimeCount = 0;
		return 1;
	}

	return 0;
}

/*

*/
void DelayLoop(void)
{
	UINT8 index = 0;
	for(index = 0; index < d_module_num; index ++)
	{
		if(_delay_module[index].StartFlag)
		{
			_delay_module[index].TimeCount ++;
			if(_delay_module[index].TimeCount >= _delay_module[index].Time)
			{
				_delay_module[index].TimeCount = 0;
				_delay_module[index].Success = 1;
				_delay_module[index].StartFlag = 0; //????
			}
		}
	}
}

#endif
//-----------------------end delay module----------------------------------------//

//------------------------rs485 -------------------------------------------------//
#ifdef USE_RS485

UINT16 _rs485_number = 0;
UINT8 *_rs485_delay_id = 0;
struct _RS485APP_INFO *_rs485_info;
struct _RS485APP_CTL   _rs485_ctl;
/*uart interrupt receive data buffer.*/
UINT8  _data_pointer = 0;
struct _RS485_Class _receive_msg, _transmit_msg;
/*inter function.*/
UINT8 RS485SendData(UINT8 data);
void RS485_Receive_data(void);
UINT8 TransmitDataPackage(UINT8 pack_size);
void EvaluateTransmitData(struct _RS485_Class *_transmit_data);
UINT8 ReceiveDataPackage(void);
void CopyReceiveData(struct _RS485_Class *_receive_data);
UINT8 RS485GetNewMsg(struct _RS485APP_INFO *info);
UINT8 RS485APP_Report(struct _RS485APP_INFO *info);
UINT8 RS485APP_Check(struct _RS485APP_INFO *info);
BOOL RS485APPSendDataPackage(struct _RS485APP_INFO *pInfo);
/*
rs485 receive data.
*/
void RS485APP_Init(UINT32 fcy, UINT32 baud, UINT8 pri, UINT8 uart, UINT16 node, UINT16 physic)
{
	_rs485_ctl.PhyType = physic;
	_rs485_ctl.UartId = uart;
	if(physic == UART_MODE)
	Config_UART(uart, baud, pri);
//	Config_UART(UART_3, 9600, 1);
	_rs485_ctl.NodeId = node;
	//数据校验初始化
	CRC_Module_Init();
}
/*
rs485 config delay module.return user module number.//
*/
UINT8 RS485ConfigDelayModule(UINT8 delay_id_num)
{
	UINT8 index = 0;
	_rs485_delay_id = My_malloc(sizeof(UINT8) * delay_id_num);
	My_memset(_rs485_delay_id, sizeof(UINT8) * delay_id_num, 0x00);

	if(GetDelayIdFunction(delay_id_num, _rs485_delay_id) == 0)
	return 0;

	for(index = 0; index < delay_id_num; index ++)	//
	SetCountTime(_rs485_delay_id[index], 0);

	return delay_id_num;
}

void RS485APP_Config(UINT8 num)
{
	_rs485_number = num;
	_rs485_info = (struct _RS485APP_INFO*)My_malloc(sizeof(struct _RS485APP_INFO) * num);
	My_memset((UINT8 *)_rs485_info, sizeof(struct _RS485APP_INFO) * num, 0x00);
}

void ConfigRs485DirCtl(HANDLER02 dir_ctl)
{
	_rs485_ctl.DriCtl  = dir_ctl;
}
//-----------------------rs485 ???????------------------------//
enum SEND_FSM
{
	INIT_DATA = 0,
	WAIT_FSM,
	SEND_ERROR
};

UINT8 RS485SendData(UINT8 data)
{
	UINT8 succeed_flag = 0;
	static UINT8 RS485_SEND_FSM = 0;
	switch(RS485_SEND_FSM)
	{
		case INIT_DATA:
				_rs485_ctl.DriCtl(ENABLE_HOST_MODE);
				SetCountTime(_rs485_delay_id[RS485_SEND_WAIT_DELAY_ID], RS485_DEFAULT_TIMES);
				StartDelayCount(_rs485_delay_id[RS485_SEND_WAIT_DELAY_ID]);
				RS485_SEND_FSM = WAIT_FSM;

		case WAIT_FSM:
				if(UARTS_SendData(_rs485_ctl.UartId, data))
				{
					ClearDelaySucFlag(_rs485_delay_id[RS485_SEND_WAIT_DELAY_ID]);
					StopDelayCount(_rs485_delay_id[RS485_SEND_WAIT_DELAY_ID]);

					succeed_flag = SEND_SUCCEED;			//
					RS485_SEND_FSM = INIT_DATA;    		//
				}
				if(ReturnSuccess(_rs485_delay_id[RS485_SEND_WAIT_DELAY_ID]))
				{
					ClearDelaySucFlag(_rs485_delay_id[RS485_SEND_WAIT_DELAY_ID]);
					StopDelayCount(_rs485_delay_id[RS485_SEND_WAIT_DELAY_ID]);
					RS485_SEND_FSM = SEND_ERROR;    	//
				}
				break;

		case SEND_ERROR:
				succeed_flag = FALSE;
				RS485_SEND_FSM = INIT_DATA;
				break;
	}
	return succeed_flag;
}

/*
data package receive function.
*/
UINT8 RS485_Buf[PACKAGE_SIZE];
//需要添加超时等待模式
void RS485_Receive_data(void)
{
	UINT16 _buf_data = 0;
	static UINT8 data_index = 0;
	
	if(UARTS_ReadData(_rs485_ctl.UartId, &_buf_data))  //当返回值为1时，才表明有数据
	{
		RS485_Buf[data_index ++] = _buf_data;
	}

	if(RS485_Buf[0] != 's')			//数据传输格式
	{
		data_index = 0;
		return;
	}
	if(data_index >= PACKAGE_SIZE)		//标准固定位
	{
		if(RS485_Buf[PACKAGE_SIZE - 1] != 'e')
		{
			data_index = 0;
			return;
		}
		data_index = 0;
		_data_pointer = 1;  						//有正确信息标志
	}
}
/*
 send_id + receive_id + id, massage_type
's' + receive_id + send_id + msg_type +8 ??? + crc???(????) + 'e'. 's'??start; 'e'??end.
*/
UINT8 SendBuffer[PACKAGE_SIZE];

UINT8 TransmitDataPackage(UINT8 pack_size)
{
	UINT8 index = 0, CrcCheck = 0;
	SendBuffer[0] = 's';
	SendBuffer[1] = _transmit_msg.ReceiveId;
	SendBuffer[2] = _transmit_msg.SenderId;
	SendBuffer[3] = _transmit_msg.SendMsgType;
	SendBuffer[4] = _transmit_msg.Length;

	if(pack_size <= PACKAGE_SIZE - 8)
	{
		for(index = 5; index < (pack_size + 5); index ++)
		SendBuffer[index] = _transmit_msg.Data[index - 5];
	}
		//data check CRC.
	CrcCheck = CRC8Checkout(&SendBuffer[1], 12);
	SendBuffer[PACKAGE_SIZE - 3] = (CrcCheck >> 8) & 0xff;
	SendBuffer[PACKAGE_SIZE - 2] = CrcCheck & 0xff;
	SendBuffer[PACKAGE_SIZE - 1] = 'e';		//???,???????????

	return 0;
}

/*
transmit.
*/
void EvaluateTransmitData(struct _RS485_Class *_transmit_data)
{
	UINT8 index = 0;
	_transmit_msg.ReceiveId = _transmit_data->ReceiveId;
	_transmit_msg.SenderId  = _transmit_data->SenderId;
	_transmit_msg.SendMsgType = _transmit_data->SendMsgType;
	_transmit_msg.Length = _transmit_data->Length;

	for(index = 0; index < DATA_SIZE; index ++)
	_transmit_msg.Data[index] = _transmit_data->Data[index];
}

/*
解析数据包
*/
UINT8 ReceiveDataPackage(void)
{
	UINT16 crc_check = 0, crc_back = 0;
	UINT8 index = 0;
	/*CRC*/
	crc_back = (RS485_Buf[PACKAGE_SIZE - 3] << 8) + RS485_Buf[PACKAGE_SIZE - 2];

	crc_check = CRC8Checkout(&RS485_Buf[1], 12);
	if(crc_check != crc_back)
	return 0;
	/**/
	_receive_msg.ReceiveId = RS485_Buf[1];
	_receive_msg.SenderId  = RS485_Buf[2];
	_receive_msg.ReceMsgType = RS485_Buf[3];
	_receive_msg.Length   = RS485_Buf[4];
	for(index = 0; index < DATA_SIZE; index ++)
	{
		_receive_msg.Data[index] = RS485_Buf[5 + index];  	//
	}
	return index;		//
}

/*
receive
*/
void CopyReceiveData(struct _RS485_Class *_receive_data)
{
	UINT8 index = 0;
	_receive_data->ReceiveId = _receive_msg.ReceiveId;
	_receive_data->SenderId  = _receive_msg.SenderId;

	for(index = 0; index < DATA_SIZE; index ++)
	_receive_data->Data[index] = _receive_msg.Data[index];
}

/*
Get New Message.
*/
enum RS485_GET_NEW_MSG
{
	RS485_PREPARE = 0,
	RS485_CHECK_DATA,			//
	RS485_RECEIVE_END
};

UINT8 RS485GetNewMsg(struct _RS485APP_INFO *info)
{
	static int WrRdCount = 0, RdRand = 0;
	switch(info->RECE_FSM)
	{
		case RS485_PREPARE:
					info->_receive_message.StateFlag = 0;
					info->_receive_message.IsBusy = 0;
					_rs485_ctl.DriCtl(ENABLE_SUB_MODE);
					if((_data_pointer == 1) && (RdRand == 0))
					{
						if(WrRdCount == 0)
						{
							WrRdCount ++;
							ReceiveDataPackage();
						}
						if((_receive_msg.ReceMsgType == info->_receive_message.ReceMsgType)
						&& (_receive_msg.ReceiveId == _rs485_ctl.NodeId))
						{
							info->RECE_FSM = RS485_CHECK_DATA; //
						}
						else
						break;
					}
					else
					break;

		case RS485_CHECK_DATA:
					info->_receive_message.IsBusy = TRUE;
					_data_pointer = 0;
					CopyReceiveData(&(info->_receive_message));
					if(_receive_msg.Length <= DATA_SIZE)
					{
						info->_receive_message.Length = 0;
						if(info->_receive_function != 0)
						info->_receive_function(0);					//?????
						RdRand = 1;									//????
						info->RECE_FSM =  RS485_RECEIVE_END;
					}
					else
					info->RECE_FSM = RS485_PREPARE;
					break;

		case RS485_RECEIVE_END:
					info->_receive_message.IsBusy = 0;
					info->_receive_message.Index = 0;
					RdRand = 0;	WrRdCount = 0;
					info->_receive_message.StateFlag = TRUE;
					info->RECE_FSM = RS485_PREPARE;
					break;
	}
	return info->_receive_message.StateFlag;
}


/*
send data package.
?????:??ID + ???ID + ????? + ??????? + ???? + CRC???.
?????????????,????????????
*/
enum RS485_SEND_PACKAGE
{
	RS485_SEND_INIT = 0,
	RS485_CHECK_PACKAGE,
	RS485_SEND_PACKAGE
};

BOOL RS485APPSendDataPackage(struct _RS485APP_INFO *pInfo)
{
	UINT8 index = 0;
	switch(pInfo->SEND_FSM)
	{
		case RS485_SEND_INIT:
					pInfo->_send_message.IsBusy = 0;
					pInfo->_send_message.StateFlag = 0;
					if(pInfo->StartSend == TRUE)
					{
						pInfo->StartSend = FALSE;
						pInfo->_send_message.IsBusy = TRUE;
						pInfo->SEND_FSM = RS485_CHECK_PACKAGE;
					}
					else
					break;

		case RS485_CHECK_PACKAGE:
					if(pInfo->_transmit_function != 0)
					pInfo->_send_message.Index = pInfo->_transmit_function(0);
					pInfo->_send_message.Package ++;
					EvaluateTransmitData(&(pInfo->_send_message));

					TransmitDataPackage(DATA_SIZE);
					pInfo->SEND_FSM = RS485_SEND_PACKAGE;

		case RS485_SEND_PACKAGE:
					for(index = 0; index < PACKAGE_SIZE; index ++)
					while(RS485SendData(SendBuffer[index]) == 0);	//???????
					/*?????????????*/
					if(pInfo->_send_message.Index == 0)
					{
						pInfo->_send_message.IsBusy = 0;
						pInfo->SEND_FSM = RS485_SEND_INIT;
						pInfo->_send_message.StateFlag = TRUE;
					}
					else
					pInfo->SEND_FSM = RS485_CHECK_PACKAGE;
					break;
	}
	return pInfo->_send_message.StateFlag;

}

/*

*/
//-----------------------------------------------------//
void RS485APP_Set_Argv(UINT8 ID, UINT8 data_long, UINT32 receiver, UINT32 sender, UINT32 rmt, UINT32 smt, UINT8 commuType)
{
	if(ID < _rs485_number)
	{
		_rs485_info[ID]._send_message.Length = data_long;
		_rs485_info[ID]._send_message.SenderId  = sender;
		_rs485_info[ID]._send_message.ReceiveId = receiver;
		_rs485_info[ID]._send_message.SendMsgType = smt;

		_rs485_info[ID]._receive_message.ReceMsgType = rmt;
		_rs485_info[ID].CommuType = commuType;
	}
}
/*
pointer receive buf and send buf.
*/

void RS485APP_Set_pre(UINT8 ID, HANDLER04 _transmit_f, HANDLER04 _receive_f)
{
	if(ID < _rs485_number)
	{
		_rs485_info[ID]._transmit_function = _transmit_f;
		_rs485_info[ID]._receive_function  = _receive_f;
	}
}
/*
receive data.
*/
void RS485APP_Communication(void)
{
	static UINT8 i = 0;
	RS485_Receive_data();
	switch(_rs485_info[i].CommuType)
	{
		case CS_BROADCAST:
					RS485APPSendDataPackage(&_rs485_info[i]);
					break;

		case CS_LISTENONLY:
					RS485GetNewMsg(&_rs485_info[i]);
					break;

		case CS_CHECK:
					RS485APP_Check(&_rs485_info[i]);
					break;

		case CS_REPORT:
					RS485APP_Report(&_rs485_info[i]);
					break;
		default: break;
	}
	if((_rs485_info[i].WorkBusyFlag == 0) &&
	   (_rs485_info[i]._send_message.IsBusy == 0) && (_rs485_info[i]._receive_message.IsBusy == 0))
	i ++;
	if(i >= _rs485_number)
	i = 0;
}

/*
rs485 report.
*/
enum
{
	RS485_SEND = 0,
	RS485_WAIT,
	RS485_SEND_SUCCESS,
	RS485_SEND_ERROR,
	RS485_SEND_END_WAIT
};

UINT8 RS485APP_Report(struct _RS485APP_INFO *info)
{
	switch(info->COMMU_FSM)
	{
		case RS485_SEND:
					if(RS485APPSendDataPackage(info) == TRUE)
					{
						info->WorkBusyFlag = 1;
						SetCountTime(_rs485_delay_id[RS485_WAIT_BACK_DELAY_ID], RS485_WAIT_BACK_TIME);
 						StartDelayCount(_rs485_delay_id[RS485_WAIT_BACK_DELAY_ID]);
 						info->COMMU_FSM = RS485_WAIT;			//
 						_rs485_ctl.DriCtl(ENABLE_SUB_MODE);
					}
					else
					break;

		case RS485_WAIT:
					if(RS485GetNewMsg(info) == TRUE)
					{
						ClearDelaySucFlag(_rs485_delay_id[RS485_WAIT_BACK_DELAY_ID]);
						StopDelayCount(_rs485_delay_id[RS485_WAIT_BACK_DELAY_ID]);

						_rs485_ctl.DriCtl(ENABLE_HOST_MODE);
						info->COMMU_FSM = RS485_SEND_SUCCESS;
					}
					if(ReturnSuccess(_rs485_delay_id[RS485_WAIT_BACK_DELAY_ID]))
					{
						ClearDelaySucFlag(_rs485_delay_id[RS485_WAIT_BACK_DELAY_ID]);
						StopDelayCount(_rs485_delay_id[RS485_WAIT_BACK_DELAY_ID]);

						_rs485_ctl.DriCtl(ENABLE_HOST_MODE);
						info->COMMU_FSM = RS485_SEND_ERROR;
					}
					break;

		case RS485_SEND_SUCCESS:
					info->TryTimes = 0;
					info->StartSend = 0;
					SetCountTime(_rs485_delay_id[RS485_WAIT_SYNC_DELAY_ID], RS485_DEFAULT_TIMES);   //
 					StartDelayCount(_rs485_delay_id[RS485_WAIT_SYNC_DELAY_ID]);
					info->COMMU_FSM = RS485_SEND_END_WAIT;  //??????????????
					break;

		case RS485_SEND_ERROR:
					info->TryTimes ++;
					if(info->TryTimes >= TRY_SEND_TIMES)
					{
						info->COMMU_FSM = RS485_SEND_SUCCESS;
					}
					else
					{
						info->StartSend = TRUE;
						info->COMMU_FSM = RS485_SEND;
					}
					break;

		case RS485_SEND_END_WAIT:
					if(ReturnSuccess(_rs485_delay_id[RS485_WAIT_SYNC_DELAY_ID]))
					{
						ClearDelaySucFlag(_rs485_delay_id[RS485_WAIT_SYNC_DELAY_ID]);
						StopDelayCount(_rs485_delay_id[RS485_WAIT_SYNC_DELAY_ID]);

						info->WorkBusyFlag = 0;		//????
						info->COMMU_FSM = RS485_SEND;
					}
					break;
	}
	return 1;
}

/*
rs485 check.
*/
enum
{
	RS485_RECEIVE = 0,
	RS485_WAIT_DELAY,
	RS485_CHECK
};

UINT8 RS485APP_Check(struct _RS485APP_INFO *info)
{
	switch(info->COMMU_FSM)
	{
		case RS485_RECEIVE:
					if(RS485GetNewMsg(info) == TRUE)
					{
						info->WorkBusyFlag = 1;					//?????
						info->StartSend = TRUE;
 						info->COMMU_FSM = RS485_WAIT_DELAY;
 						//delay 10ms
 						SetCountTime(_rs485_delay_id[RS485_WAIT_ASYNC_DELAY_ID], RS485_DEFAULT_TIMES + 10);
 						StartDelayCount(_rs485_delay_id[RS485_WAIT_ASYNC_DELAY_ID]);
					}
					break;

		case RS485_WAIT_DELAY:
					if(ReturnSuccess(_rs485_delay_id[RS485_WAIT_ASYNC_DELAY_ID]))
					{
						ClearDelaySucFlag(_rs485_delay_id[RS485_WAIT_ASYNC_DELAY_ID]);
						StopDelayCount(_rs485_delay_id[RS485_WAIT_ASYNC_DELAY_ID]);

						info->COMMU_FSM = RS485_CHECK;
					}
					break;

		case RS485_CHECK:
					if(RS485APPSendDataPackage(info) == TRUE)
					{
 						info->WorkBusyFlag = 0;						//????
						info->COMMU_FSM = RS485_RECEIVE;
 						_rs485_ctl.DriCtl(ENABLE_SUB_MODE);
 					}
					break;
	}
	return 1;
}

/*
start one message.
*/
void RS485APP_Start(UINT8 ID)
{
	_rs485_info[ID].StartSend = TRUE;
}

/*
get anyone data.
*/
struct _RS485APP_INFO *RS485APP_GetInfo(UINT8 ID)
{
	return (_rs485_info + ID);
}

#endif
//----------------------------END OF RS485----------------------------//


#ifdef SIG_MANAGE
//----------------------------signal trigger--------------------------//

#endif

//---------------------------------PS2--------------------------------//
#ifdef  USE_PS2
/*ps2 内部协议规定相关主机命令*/
UINT8 PS2_HostStandardCmd[17] = {0xFF,0xFE,0xF2,0xEE,0xED,0xF4,0xF5,0xF0,
					    	   	0xF7,0xF8,0xF9,0xFA,0xF6,0xFB,0xFC,0xFD,0xF3};
/* 设备回复主机数据 */
UINT8 AckHostData[17] = {0x00,0x00,0x00,0xEE,0xFA,0xFA,0xFA,0xFA,0xFA,0xFA,
						 0xFA,0xFA,0xFA,0xFA,0xFA,0xFA,0xFA};

/*  */
struct _ps2_info Ps2_info;

UINT8 g_Data_From_Host = 0;
//--------------------------------------------------------------------//
//内部函数声明
static void SendBitBlockStyle(UINT8 data);
static BOOL GetBitBlockStyle(void);
static BOOL ParityCheckout(UINT8 *data, UINT8 bit_num);
static BOOL Ps2ReceiveDataBlockStyle(UINT8 *data);
void CheckRequest(void);
void PS2ReportData(UINT8 data);
static void Delay_us(UINT16 time);
static BOOL RspHostData(UINT8 data);
/*

*/
void RegisterPs2DataPort(CB_SET set_port_dir, CB_SET set_data, CB_READ get_data)
{
	Ps2_info.SetDataPortDir = set_port_dir;
	Ps2_info.SetDataPortValue = set_data;
	Ps2_info.GetData = get_data;
}

/*

*/
void RegisterPs2ClkPort(CB_SET set_port_dir, CB_SET set_clk, CB_READ get_clk)
{
	Ps2_info.SetClkPortDir = set_port_dir;
	Ps2_info.SetClkPortValue = set_clk;
	Ps2_info.GetClk = get_clk;
}
/*
send data
*/
static void SendBitBlockStyle(UINT8 data)
{
	Ps2_info.SetDataPortValue(1);
	Ps2_info.SetClkPortValue(1);
	Ps2_info.SetClkPortDir(1);
	Ps2_info.SetDataPortDir(1);

	//设置要输出的数据
	if(LOAD8(data)) Ps2_info.SetDataPortValue(1);
	else Ps2_info.SetDataPortValue(0);
	Delay_us(20);
	Ps2_info.SetClkPortValue(0);
	Delay_us(40);
	Ps2_info.SetClkPortValue(1);
	Delay_us(20);
	//释放数据线和时钟线
	Ps2_info.SetClkPortDir(0);
	Ps2_info.SetDataPortDir(0);
}

/*
//从主机读取一个数据位
*/
static BOOL GetBitBlockStyle(void)
{
	Ps2_info.SetClkPortValue(1);
	Ps2_info.SetClkPortDir(1);
	Delay_us(20);
	Ps2_info.SetClkPortValue(0);
	Delay_us(40);
	Ps2_info.SetClkPortValue(1);
	Delay_us(20);
	Ps2_info.SetClkPortDir(0);
	Ps2_info.SetDataPortDir(0);
	return Ps2_info.GetData();
}

/*
奇偶校验,void *data 指向要校验的数据，bit_num是校验的数据的位数.
*/
static BOOL ParityCheckout(UINT8 *data, UINT8 bit_num)
{
	UINT8 check = 0, i = 0;
	for(i = 0; i < bit_num; i ++)
	{
		if(((*data) >> i) & 0x01)
		check ++;
	}
	return (check & 0x01);
}
/*
ps2 receive data
*/
static BOOL Ps2ReceiveDataBlockStyle(UINT8 *data)
{
	UINT8 check_sum = 0, stop_bit = 0, bit_sum[8], i = 0;
	//data clock line set input.
	Ps2_info.SetClkPortDir(0);
	Ps2_info.SetDataPortDir(0);
	//-------------------------//
    while(!Ps2_info.GetClk());
	//check数据线是否被拉低
	if(Ps2_info.GetData())
	return 0;
	Delay_us(20);
	//读取8为数据
	for(i = 0; i < 8; i ++)
	{
		bit_sum[i] = GetBitBlockStyle();
		if(!Ps2_info.GetClk())
		return 0;
	}
	//读取校验位
	check_sum = GetBitBlockStyle();
	if(!Ps2_info.GetClk())
	return 0;
	//读取停止位
	stop_bit = GetBitBlockStyle();
	if(!Ps2_info.GetClk())
	return 0;
	//等待数据线被释放
	while(!Ps2_info.GetData())
	{
		Ps2_info.SetClkPortDir(1);
		Delay_us(20);
		Ps2_info.SetClkPortValue(0);
		Delay_us(40);
		Ps2_info.SetClkPortValue(1);
		Delay_us(20);
		Ps2_info.SetClkPortDir(0);
	}
	//输出应答位ACK
	Ps2_info.SetDataPortValue(1);
	Ps2_info.SetClkPortValue(1);
	Ps2_info.SetClkPortDir(1);
	Ps2_info.SetDataPortDir(1);
	Delay_us(15);
	Ps2_info.SetDataPortValue(0);
	Delay_us(5);
	Ps2_info.SetClkPortValue(0);
	Delay_us(40);
	Ps2_info.SetClkPortValue(1);
	Delay_us(5);
	Ps2_info.SetDataPortValue(1);
	Ps2_info.SetClkPortDir(0);
	Ps2_info.SetDataPortDir(0);

	if((1 - ParityCheckout((UINT8*)data, 8)) != check_sum)
	return 0;

	for(i = 0; i < 8; i ++)
	{
		*(data) += bit_sum[i] << i;
	}
	Delay_us(45);

	return 1;
}
/*
ps2 report data.
*/
void PS2ReportData(UINT8 data)
{
	UINT8 bit_sum[8], i = 0;

	Ps2_info.SetClkPortDir(0);
	Ps2_info.SetDataPortDir(0);
    //check时钟线
	do
	{
		while(!Ps2_info.GetClk());
		Delay_us(50);
	}
	while(!Ps2_info.GetClk());

	//如果数据线被拉低，则放弃发送
	if(!Ps2_info.GetData())
	{
	    CheckRequest();
	    goto err_no;
	}
    Delay_us(20);

    for(i = 0; i < 8; i++)
	{
		bit_sum[i] = ((data >> i) & 0x01);
	}
	//---------disable interrupt----------//
	//发送开始位，并check时钟线
	SendBitBlockStyle(0);
	if(!Ps2_info.GetClk())
	{
	    CheckRequest();
	    goto err_no;
	}
	//发送数据
    for(i = 0; i < 8; i++)
	{
		SendBitBlockStyle(bit_sum[i]);

		if(!Ps2_info.GetClk())
		{
			CheckRequest();
	   	 	goto err_no;
	 	}
	}

	//发送校验位，并check时钟线
	SendBitBlockStyle(1 - ParityCheckout(&data, 8));
	if(!Ps2_info.GetClk())
	{
	    CheckRequest();
	    goto err_no;
	}
	//发送停止位，并check时钟线
	SendBitBlockStyle(1);
	if(!Ps2_info.GetClk())
	{
	    CheckRequest();
	    goto err_no;
	}
	Delay_us(130);
	if(!Ps2_info.GetClk()) CheckRequest();

err_no:
	Timer_Enable(TIMER1);
	Timer_Enable(TIMER2);
	Timer_Enable(TIMER3);
	return;
}
/*
RSP host data.
*/
static BOOL RspHostData(UINT8 data)
{
	static UINT8 AckValue = 0, flag = 0;
	UINT8 queue = 0;
	for(queue = 0; queue < PS2_STANDARD_CMD_NUM; queue ++)
	{
		if(PS2_HostStandardCmd[queue] == data)
		{
			/*flag */
			flag = 1;
			switch(queue)
			{
				case 0:
					PS2ReportData(0xFA);
					PS2ReportData(0xAA);
					break;

				case 1:
					PS2ReportData(g_Data_From_Host);
					break;

				case 2:
					PS2ReportData(0xFA);
					PS2ReportData(0xAB);
					PS2ReportData(0x83);
					break;

				default: PS2ReportData(AckHostData[queue]); break;
			}
			AckValue = PS2_HostStandardCmd[queue];
		}
	}
	/*除队列中的其他情况*/
	if(!flag)
	{
		if(0xF0 == AckValue)
		{
			AckValue = 0;
			if(!data)
			return 0;
			else
			PS2ReportData(0xFA);
		}
		if((0xED == AckValue)|| (0xF3 == AckValue))
		{
			AckValue = 0;
			PS2ReportData(0xFA);
		}
	}
	flag = 0;
	return AckValue;
}

/*

*/
void CheckRequest(void)
{
	Ps2_info.SetClkPortDir(0);
	Ps2_info.SetDataPortDir(0);
	if(Ps2_info.GetData()) return;

	if(Ps2ReceiveDataBlockStyle(&g_Data_From_Host) == 1)
	{
		RspHostData(g_Data_From_Host);
	}
}

/*

*/
static void Delay_us(UINT16 time)
{
	UINT32 i;//FCY=10MHz，调用一次延时约time微秒
	for(i = 0; i < time ; i+=2)
	{
		asm("nop");
		asm("nop");
		asm("nop");
        asm("nop");
		asm("nop");
        asm("nop");
        asm("nop");
	}
}

#endif
/*
end of file.
*/
//-----------------------------------------------------------------//
/*
data queue tools.
*/
#ifdef USE_DATA_BUF_QUEUE
//value
UINT8 _buf_nums = 0, _user_num = 0;
struct _data_queue *_buf_queue;

void ConfigQueueBufNum(UINT8 buf_num)
{
	_buf_nums = buf_num;
	_buf_queue = My_malloc(sizeof(struct _data_queue) * _buf_nums);
	My_memset((UINT8*)_buf_queue, sizeof(struct _data_queue) * _buf_nums, 0x00);
}
/*
init data queue buf.
*/
UINT8 InitQueueBuf(UINT8 id)
{
	UINT8 index = 0;
	if(id <= _buf_nums)
	{
		if(_buf_queue[id]._isUser == TRUE)   //表明已近初始化
			return 0;
		_buf_queue[id]._read_pointer  = _buf_queue[id]._buf_size;
		_buf_queue[id]._write_pointer = _buf_queue[id]._buf_size;
		for(index = 0; index < QUEUE_SIZE; index ++)
		_buf_queue[id]._buf_size[index] = 0;
	}
	else
		return 0;
	return 1;
}

/*
Get Queue ID
*/
UINT8 Get_NewQueueId(UINT8 num, UINT8 *return_id)
{
	UINT8 index = 0;
	if(num <= (_buf_nums - _user_num))
	{
		for(index = 0; index < num; index ++)
		{
			if(return_id == 0)
				return FALSE;
			else
			{
				*return_id = _user_num;
				_user_num ++;
				return_id ++;
			}
		}
	}
	else
		return FALSE;
	return TRUE;
}
/*
write data to buf
*/
UINT8 WrDataToBufQueue(UINT8 id, UINT16 data)
{
	if(_buf_queue[id]._read_pointer <= _buf_queue[id]._write_pointer)
	{
		*_buf_queue[id]._write_pointer = data;   //写数据到发送缓冲区
		_buf_queue[id]._write_pointer ++;
		_buf_queue[id]._wr_index ++;
		if(_buf_queue[id]._wr_index >= QUEUE_SIZE)
		{
			_buf_queue[id]._wr_index = 0;
			_buf_queue[id]._write_pointer = _buf_queue[id]._buf_size;
		}
	}
	else
	{
		if((_buf_queue[id]._read_pointer - _buf_queue[id]._write_pointer) <= 1)  //说明只有一个buf空余
		{
			/*认为缓冲区已近满了*/
			return 0;
		}
		else
		{
			*_buf_queue[id]._write_pointer = data;   //写数据到发送缓冲区
			_buf_queue[id]._write_pointer ++;
			_buf_queue[id]._wr_index ++;
			if(_buf_queue[id]._wr_index >= QUEUE_SIZE)
			{
				_buf_queue[id]._wr_index = 0;
				_buf_queue[id]._write_pointer = _buf_queue[id]._buf_size;
			}
		}
	}
	return 1;
}

/*
read data from buf.
*/
UINT8 RdDataFromBufQueue(UINT8 id, UINT16 *data)
{
	UINT16 _back_data = 0;
	if(_buf_queue[id]._read_pointer != _buf_queue[id]._write_pointer)
	{
		_back_data = *_buf_queue[id]._read_pointer;
		_buf_queue[id]._read_pointer ++;
		_buf_queue[id]._rd_index ++;
		if(_buf_queue[id]._rd_index >= QUEUE_SIZE)
		{
			_buf_queue[id]._rd_index = 0;
			_buf_queue[id]._read_pointer = _buf_queue[id]._buf_size;
		}
		*data = _back_data;
		return 1;
	}
	return 0;
}

#endif

/*CRC 数据校验模块*/
//暂且只支持CRC8 后续添加CRC16 CRC32.

#ifdef  USER_CRC_DATA_CHECK


#ifdef USE_HALF_BYTE_CHECK_TABLE
//半字查表法。
#ifdef USE_CRC8
UINT8 CRC8_CHECK_TABLE[16];
#endif

#ifdef USE_CRC16
UINT16 CRC16_CHECK_TABLE[16];
#endif

#endif

#ifdef USE_ONE_BYTE_CHECK_TABLE
//单字节查表法
UINT8 CRC8_ONE_CHECK_TABLE[256];
#endif

//模块内部使用函数声明区
static UINT8 CRC8_UINT8_Check(UINT8 data);
static void CreatCRC_HalfTable(void);
static UINT16 CRC16_UINT16_Check(UINT8 data);
/*
CRC module init.
*/
void CRC_Module_Init(void)
{
	#ifdef USE_HALF_BYTE_CHECK_TABLE
	CreatCRC_HalfTable();
	#endif
}

/*
uint8 crc8 check code.
*/
static UINT8 CRC8_UINT8_Check(UINT8 data)
{
	UINT8 index = 0, regi = 0;
	regi = data;
	for(index = 8; index > 0; index--)
	{
		if(regi & 0x80)
		{
			regi = (regi << 1) ^ CRC8_POLY;
		}
		else
		{
			regi <<= 1;
		}
	}
	return regi;
}

/*
uint16 crc16 check code.
*/
static UINT16 CRC16_UINT16_Check(UINT8 data)
{
	UINT8 index = 0, regi = 0;
	regi = ((UINT16)data) << 8;
	for(index = 8; index > 0; index--)
	{
		if(regi & 0x8000)
		{
			regi = (regi << 1) ^ CRC16_POLY;
		}
		else
		{
			regi <<= 1;
		}
	}
	return regi;
}
/*
crc check table.
*/
static void CreatCRC_HalfTable(void)
{
	UINT8 index = 0;
	for(index = 0; index < 16; index ++)
	{
		#ifdef USE_CRC8
		//计算0~15 crc8的校验码
		CRC8_CHECK_TABLE[index] = CRC8_UINT8_Check(index);
		#endif
		//---------------------------------//
		#ifdef USE_CRC16
		CRC16_CHECK_TABLE[index] = CRC16_UINT16_Check(index);
		#endif

	}
}
/*
使用CRC8时 buf的数据量不宜太大，否则会照成错误检测不出来
只适合8位数据使用
*/
UINT8 CRC8Checkout(UINT8 *buf, UINT16 buf_size)
{
	UINT8 index = 0, temp_bits = 0, os_data = 0, crc_add = 0;
	UINT8 poly_index = 0, byte = 0, poly = 0, counts = 0;
	UINT8 *buf_pointer;
	if(buf == 0)
	return 0;
	if(buf_size <= 1)
	{
		return CRC8_CHECK_TABLE[*buf];
	}
	temp_bits = *buf;
	buf_pointer = buf;
	//每个字节需要循环两次才能校验完
	for(index = 0; index < buf_size * 2; index ++)
	{
		poly_index = (temp_bits >> 4) & 0x0f;
		poly = CRC8_CHECK_TABLE[poly_index];
		if(counts % 2)//取下一个字节的低4位
		{
			byte = *(buf_pointer + 1) & 0x0f;
		}
		else					//取下一个字节的高4位
		{
			byte = (*(buf_pointer + 1) >> 4) & 0x0f;
		}
		//-------------------------------------//
		os_data = ((temp_bits << 4) & 0xf0) | byte;
		//
		temp_bits = os_data ^ poly;
		//一个字节需要循环两次才能
		if(counts % 2)
		{
			if(index <= (buf_size - 1)* 2)
			{
				buf_pointer ++;
				if(buf_pointer == 0)
				return 0;
			}
			else
			{
				buf_pointer = &crc_add;
			}
		}
		counts ++;
	}
	return temp_bits;
}
#endif

/*****************************************************************************/
//schdule
#ifdef USE_SCHEDULING

struct sch_info *_sch_info;

UINT8 _sch_num = 0;
void Schedu_Action(void);
/*

*/
void Config_Scheduling(UINT8 task_num, UINT8 timer)
{
    _sch_num = task_num;
    _sch_info = (struct sch_info *)My_malloc(sizeof(struct sch_info));
    My_memset((UINT8*)_sch_info, sizeof(struct sch_info), 0x00);
    //register timer.
    Timer_Register(timer, Schedu_Action);
}

/*

*/
void Register_Task(UINT8 task_id, HANDLER01 task, UINT16 work_time)
{
    if((task_id < _sch_num) &&(task != 0))
    {
        _sch_info[task_id].index = task_id;
        _sch_info[task_id].time_load = work_time;
        _sch_info[task_id].time_counter = work_time;
        _sch_info[task_id].sch_task = task;
    }
    else
    return;
}
/*

*/
void SET_TaskTrigger(UINT8 task_id, UINT8 trigger_flag)
{
    if(task_id < _sch_num)
    {
       if(trigger_flag)
       _sch_info[task_id].trigger_flag = 1;
       else
       {
          _sch_info[task_id].trigger_flag = 0;
          _sch_info[task_id].time_counter = _sch_info[task_id].time_load;
       }
    }
    else
    return;
}
/*

*/
void Schedu_Action(void)
{
    UINT8 index = 0;
    for(index = 0; index < _sch_num; index ++)
    {
        if(_sch_info[index].trigger_flag)
        {
            _sch_info[index].time_counter --;
            if(_sch_info[index].time_counter == 0)
            {
              _sch_info[index].task_action = 1;
              _sch_info[index].time_counter = _sch_info[index].time_load;
            }
        }
    }
}
/*

*/
void Schedu_APP(void)
{
    UINT8 index = 0;
    for(index = 0; index < _sch_num; index ++)
    {
        if(_sch_info[index].task_action)
        {
            if(_sch_info[index].sch_task != 0)
            _sch_info[index].sch_task();
            _sch_info[index].task_action = 0;
        }
    }
}

#endif

//----------------------------------------------------------------------------//
//数据滤波处理
#ifdef FILTER_MODULE



#endif

/*
end of module.
*/
//-----------------------------------------------------------//
//----------------------------------------------------------------------------//
#ifdef  USE_ERROR
#include "primitive.h"
#include "module.h"

static struct errstr_t {
    int available_;
    int last_error_;
    const char **error_array_;
} g_errstr_array [MODULE_COUNT];

#include "errstr.def"

const char *errstr (error_t _error)
{
    static bool initialized = false;
    module_t module_id = MODULE_ID (_error);
    int error_id = MODULE_ERROR (_error);

    if (0 == initialized) {
        errstr_init ();
        initialized = true;
    }

    if (0 == _error) {
        return "SUCCESS";
    }

    if (_error > 0) {
        return "ERROR_ERRSTR_NOT_NEGATIVE";
    }

    if (module_id > MODULE_LAST) {
        return "ERROR_ERRSTR_INVALID_MODULEID";
    }

    if (!g_errstr_array [module_id].available_) {
        return "ERROR_ERRSTR_NOT_AVAILABLE";
    }

    if (error_id > g_errstr_array [module_id].last_error_) {
        return "ERROR_ERRSTR_OUT_OF_LAST";
    }

    if (0 == g_errstr_array [module_id].error_array_ [error_id]) {
        return "ERROR_ERRSTR_NOT_DEFINED";
    }

    return g_errstr_array [module_id].error_array_ [error_id];
}

#endif

//--------------------------------------------------------------------------//

/*
FFT module.
*/
#ifdef FFT_MODULE

//#include<math.h>

#define PI 3.1415926535897932384626433832795028841971               //定义圆周率值
#define FFT_N 128                                                   //定义福利叶变换的点数

struct compx {float real,imag;};                                    //定义一个复数结构
struct compx s[FFT_N];                                              //FFT输入和输出：从S[1]开始存放，根据大小自己定义


/*******************************************************************
函数原型：struct compx EE(struct compx b1,struct compx b2)
函数功能：对两个复数进行乘法运算
输入参数：两个以联合体定义的复数a,b
输出参数：a和b的乘积，以联合体的形式输出
*******************************************************************/
struct compx EE(struct compx a,struct compx b)
{
 struct compx c;
 c.real=a.real*b.real-a.imag*b.imag;
 c.imag=a.real*b.imag+a.imag*b.real;
 return(c);
}

/*****************************************************************
函数原型：void FFT(struct compx *xin,int N)
函数功能：对输入的复数组进行快速傅里叶变换（FFT）
输入参数：*xin复数结构体组的首地址指针，struct型
*****************************************************************/
void FFT(struct compx *xin)
{
  int f,m,nv2,nm1,i,k,l,j=0;
  struct compx u,w,t;

   nv2=FFT_N/2;                  //变址运算，即把自然顺序变成倒位序，采用雷德算法
   nm1=FFT_N-1;
   for(i=0;i<nm1;i++)
   {
    if(i<j)                    //如果i<j,即进行变址
     {
      t=xin[j];
      xin[j]=xin[i];
      xin[i]=t;
     }
    k=nv2;                    //求j的下一个倒位序
    while(k<=j)               //如果k<=j,表示j的最高位为1
     {
      j=j-k;                 //把最高位变成0
      k=k/2;                 //k/2，比较次高位，依次类推，逐个比较，直到某个位为0
     }
   j=j+k;                   //把0改为1
  }

  {
   int le,lei,ip;                            //FFT运算核，使用蝶形运算完成FFT运算
    f=FFT_N;
   for(l=1;(f=f/2)!=1;l++)                  //计算l的值，即计算蝶形级数
           ;
  for(m=1;m<=l;m++)                         // 控制蝶形结级数
   {                                        //m表示第m级蝶形，l为蝶形级总数l=log（2）N
    le=2<<(m-1);                            //le蝶形结距离，即第m级蝶形的蝶形结相距le点
    lei=le/2;                               //同一蝶形结中参加运算的两点的距离
    u.real=1.0;                             //u为蝶形结运算系数，初始值为1
    u.imag=0.0;
    w.real=cos(PI/lei);                     //w为系数商，即当前系数与前一个系数的商
    w.imag=-sin(PI/lei);
    for(j=0;j<=lei-1;j++)                   //控制计算不同种蝶形结，即计算系数不同的蝶形结
     {
      for(i=j;i<=FFT_N-1;i=i+le)            //控制同一蝶形结运算，即计算系数相同蝶形结
       {
        ip=i+lei;                           //i，ip分别表示参加蝶形运算的两个节点
        t=EE(xin[ip],u);                    //蝶形运算，详见公式
        xin[ip].real=xin[i].real-t.real;
        xin[ip].imag=xin[i].imag-t.imag;
        xin[i].real=xin[i].real+t.real;
        xin[i].imag=xin[i].imag+t.imag;
       }
      u=EE(u,w);                           //改变系数，进行下一个蝶形运算
     }
   }
  }

}

/************************************************************
函数原型：void main()
函数功能：测试FFT变换，演示函数使用方法
输入参数：无
输出参数：无
************************************************************/
void main()
{
  int i;
  for(i=0;i<FFT_N;i++)                           //给结构体赋值
  {
     s[i].real=sin(2*3.141592653589793*i/FFT_N); //实部为正弦波FFT_N点采样，赋值为1
     s[i].imag=0;                                //虚部为0
  }

  FFT(s);                                        //进行快速福利叶变换

  for(i=0;i<FFT_N;i++)                           //求变换后结果的模值，存入复数的实部部分
  s[i].real=sqrt(s[i].real*s[i].real+s[i].imag*s[i].imag);

   while(1);
}


#endif


//-----------------------USE_FILTER---------------------------------------------
#ifdef USE_FILTER	

	#define FILTER_MAX_COUNT 16	//最大同时滤波容量
	#define FILTER_MAX_WINDOW_SIZE 32	//滤波最大窗口
	
	u8 weightCurrentIdx=0, averageCurrentIdx=0;
	u8 weightwidowSize[FILTER_MAX_COUNT],averagewidowSize[FILTER_MAX_COUNT];
	int weightDataArray[FILTER_MAX_COUNT][FILTER_MAX_WINDOW_SIZE],averageDataArray[16][FILTER_MAX_WINDOW_SIZE];
	void Filter16_Init(struct _Filter_Data16_EX *filterData, UINT8 type, UINT8 width)
	{
		filterData->_filter_type = type;
		filterData->_width = width;
		filterData->_original_Data = 0;
		filterData->_sum = 0;
		filterData->_filtered_Data = 0;
	}
	
	UINT16 Filter16_GetValue(struct _Filter_Data16_EX *filterData, UINT16 data)
	{
		filterData->_original_Data = data;
		filterData->_sum = filterData->_sum - filterData->_filtered_Data + data;
		filterData->_filtered_Data = filterData->_sum / filterData->_width;
		return filterData->_filtered_Data;
	}
	
	u8 weightFilterInitial(u8 filterWidowSize)
	{
		if(weightCurrentIdx >= FILTER_MAX_COUNT ||
			 filterWidowSize <= 0 || filterWidowSize > FILTER_MAX_WINDOW_SIZE)
		return 0;
		
		weightwidowSize[weightCurrentIdx] = filterWidowSize;
		weightCurrentIdx++;
		return weightCurrentIdx;
	}
	
	u8 averageFilterInitial(u8 filterWidowSize)
	{
		if(averageCurrentIdx >= FILTER_MAX_COUNT ||
			 filterWidowSize <= 0 || filterWidowSize > FILTER_MAX_WINDOW_SIZE)
		return 0;
		
		averagewidowSize[averageCurrentIdx] = filterWidowSize;
		averageCurrentIdx++;
		return averageCurrentIdx;
	}	

	//权值滤波
	//衰减率为1/2,即历史数据的权重衰减一半
	int weightFilter(u8 filterIdx, int newValue)
	{
		if(filterIdx <=0 || filterIdx > FILTER_MAX_COUNT) return -1;
		
		u8 widowSize = weightwidowSize[filterIdx - 1];
		int filterResult = 0;
		int * tempDataArray = weightDataArray[filterIdx];
				
		moveArrayForward(widowSize, tempDataArray);
		tempDataArray[widowSize-1] = newValue;
		
		for(int i = 0; i < widowSize ; i ++)
		{
			if(&(tempDataArray[i]) != 0)
			filterResult += tempDataArray[i] >> (widowSize - i);
		}
		return filterResult;
	}
	
	//均值滤波
	int averageFilter(u8 filterIdx, int newValue)
	{
		if(filterIdx <=0 || filterIdx > FILTER_MAX_COUNT) return -1;
		
		u8 widowSize = averagewidowSize[filterIdx - 1];
		int filterResult = 0;
		int * tempDataArray = averageDataArray[filterIdx];
				
		moveArrayForward(widowSize, tempDataArray);
		tempDataArray[widowSize-1] = newValue;
		
		for(int i = 0; i < widowSize ; i ++)
		{
			if(&(tempDataArray[i]) != 0)
			filterResult += tempDataArray[i];
		}
		filterResult /= widowSize;
		return filterResult;
	}
	
	//测试用例
	void TEST_Filter()
	{
		int filterTest[13] = {500,500,500,500,500,1100,500,500,500,100,500,500,500};
		int averageResult[13],weightResult[13];
		u8 aIdx = averageFilterInitial(3);
		u8 wIdx = weightFilterInitial(3);
		int clearWarnning=0;
		for(int i=0;i<13;i++)
		{
			averageResult[i] = averageFilter(aIdx, filterTest[i]);
			weightResult[i] = weightFilter(wIdx,filterTest[i]);
		}
		clearWarnning = averageResult[0] + weightResult[0];
		clearWarnning = clearWarnning + 1;
	}

#endif 
//-----------------------End of USE_FILTER----------------------------------------		

//-----------------------USE_ENCODER_MANAGER---------------------------------------------
#ifdef USE_ENCODER_MANAGER
	struct  _Encoder_Manager_Info *_em_info;
	UINT8 _EMNumber = 0;
	
	void Config_Encoder_Manager(UINT8 number, UINT8 timer)
	{
		UINT8 i;
		_EMNumber = number;
		_em_info = My_malloc(sizeof(struct _Encoder_Manager_Info) * number);
		for(i = 0; i < number; i++)
		{
			_em_info[i]._sample_frequency = 1000;
			_em_info[i]._position = 0;
			for(UINT8 j  = 0; j < 50; j++) _em_info[i]._buffer[j] = 0;
			_em_info[i]._integral = 0;
			_em_info[i]._speed = 0;
			_em_info[i]._Tick = 0;																//timer计数值
			_em_info[i]._FSM	= 0;														//模块的状态机
			_em_info[i]._is_Init = FALSE;											//是否初始化完成
			_em_info[i]._filter._filter_type = 0;
			_em_info[i]._filter._width = 10;
		}	
		if(timer == SYS_TIMER)
		{
			SysTick_Register(_Encoder_Manager_Tick);
		}
		else
		{
			//Timer_Register(timer, _LED_Tick);
			//_LED_MScount = Get_TimerInfo(timer)->_frequency / 1000;
		}
	}
	
	void Register_Encoder(UINT8 index, CB_READ_32 readEncoder)
	{
		_em_info[index]._EncoderHDL = readEncoder;
	}
	
	BOOL Start_Encoder_Manager(void)
	{
		for(UINT8 i = 0; i < _EMNumber; i++)
		{
			if(_em_info[i]._is_Init == FALSE) return FALSE;
		}
		return TRUE;
	}
	
	INT32 Get_Position(UINT8 index)
	{
		return _em_info[index]._position;
	}
	
	INT32 Get_Speed(UINT8 index)
	{
		return _em_info[index]._speed;
	}
	
	void _Encoder_Manager_Tick(void)
	{
		for(UINT8 i = 0; i < _EMNumber; i++)
		{
			switch(_em_info[i]._FSM)
			{
				case 0://初始化状态，前100ms的数据忽略
					_em_info[i]._Tick++;
					if(_em_info[i]._Tick >= 100)
					{
						_em_info[i]._FSM = 1;
						_em_info[i]._Tick = 0;
						_em_info[i]._is_Init = TRUE;
					}
					break;	
				
				case 1://正常采集
					_em_info[i]._position = _em_info[i]._EncoderHDL();
					_em_info[i]._buffer[_em_info[i]._Tick] = _em_info[i]._position;
					_em_info[i]._Tick++;
					if(_em_info[i]._Tick >= 50) _em_info[i]._Tick = 0;
					_em_info[i]._integral = _em_info[i]._position - _em_info[i]._buffer[_em_info[i]._Tick];							
					_em_info[i]._speed = _em_info[i]._integral;
					break;
			}
		}
	}
#endif 
//-----------------------End of USE_ENCODER_MANAGER----------------------------------------	
	
	
//-----------------------USE_PID---------------------------------------------
#ifdef USE_PID	
	void Config_PID(struct _PID_Info *PID_obj, float kp, float ki, float kd, float ub, float lb)
	{
		PID_obj->_Kp = kp;
		PID_obj->_Ki = ki;
		PID_obj->_Kd = kd;
		PID_obj->_upper_bound = ub;
		PID_obj->_lower_bound = lb;
		
		PID_obj->_error = 0;
		PID_obj->_error_proportion = 0;
		PID_obj->_error_integral = 0;
		PID_obj->_error_differential = 0;
	}
	
	float Get_PID_Output(struct _PID_Info *PID_obj, float error)
	{
		PID_obj->_error = error;
		PID_obj->_error_proportion = PID_obj->_Kp * error;
		PID_obj->_error_integral += (PID_obj->_Ki * error);
		PID_obj->_error_differential = PID_obj->_Kd * (error - PID_obj->_error);
		
		if(PID_obj->_error_integral > PID_obj->_upper_bound)
		{
			PID_obj->_error_integral = PID_obj->_upper_bound;
		}
		else if(PID_obj->_error_integral < PID_obj->_lower_bound)
		{
			PID_obj->_error_integral = PID_obj->_lower_bound;
		}
		
		PID_obj->_control_output = PID_obj->_error_proportion + PID_obj->_error_integral + PID_obj->_error_differential;
		if(PID_obj->_control_output > PID_obj->_upper_bound)
		{
			PID_obj->_control_output = PID_obj->_upper_bound;
		}
		else if(PID_obj->_control_output < PID_obj->_lower_bound)
		{
			PID_obj->_control_output = PID_obj->_lower_bound;
		}
		
		return PID_obj->_control_output;
	}
#endif 
//-----------------------End of USE_PID----------------------------------------	
	
//-----------------------GPIO Config---------------------------------------------
#ifdef GPIO_CONFIGURATION
	u16 getGPIOPortByNumber(u8 gpioNumber)
	{
		u16 tempResult = GPIO_Pin_0;
//		if(gpioNumber <= 15)
//		{
//			tempResult = 1<<gpioNumber;
//		}
			switch(gpioNumber)
			{
				case 1:tempResult=GPIO_Pin_1;break;
				case 2:tempResult=GPIO_Pin_2;break;
				case 3:tempResult=GPIO_Pin_3;break;
				case 4:tempResult=GPIO_Pin_4;break;
				case 5:tempResult=GPIO_Pin_5;break;
				case 6:tempResult=GPIO_Pin_6;break;
				case 7:tempResult=GPIO_Pin_7;break;
				case 8:tempResult=GPIO_Pin_8;break;
				case 9:tempResult=GPIO_Pin_9;break;
				case 10:tempResult=GPIO_Pin_10;break;
				case 11:tempResult=GPIO_Pin_11;break;
				case 12:tempResult=GPIO_Pin_12;break;
				case 13:tempResult=GPIO_Pin_13;break;
				case 14:tempResult=GPIO_Pin_14;break;
				case 15:tempResult=GPIO_Pin_15;break;
				case 255:tempResult=GPIO_Pin_All;break;
				default:break;
			}
			return tempResult;
	}
	
	void setGPIOConfiguration(GPIOChannelType channel,u16 ports,GPIOMode_TypeDef mode, GPIOSpeed_TypeDef speed)
	{		
		u32 rccChannel;
		GPIO_TypeDef * initChannel;
		switch(channel)
		{
			case ChannelA:				rccChannel = RCC_APB2Periph_GPIOA;				initChannel = GPIOA;			break;			
			case ChannelB:				rccChannel = RCC_APB2Periph_GPIOB;				initChannel = GPIOB;			break;			
			case ChannelC:				rccChannel = RCC_APB2Periph_GPIOC;				initChannel = GPIOC;			break;			
			case ChannelD:				rccChannel = RCC_APB2Periph_GPIOD;				initChannel = GPIOD;			break;			
			case ChannelE:				rccChannel = RCC_APB2Periph_GPIOE;				initChannel = GPIOE;			break;			
			case ChannelF:				rccChannel = RCC_APB2Periph_GPIOF;				initChannel = GPIOF;			break;			
			case ChannelG:				rccChannel = RCC_APB2Periph_GPIOG;				initChannel = GPIOG;			break;
			default:return;
		}
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB2PeriphClockCmd(rccChannel, ENABLE); //使能PB端口时钟		
		GPIO_InitStructure.GPIO_Pin = ports;//端口配置
		GPIO_InitStructure.GPIO_Mode = mode;      
		GPIO_InitStructure.GPIO_Speed = speed;     //
		GPIO_Init(initChannel, &GPIO_InitStructure);					      //根据设定参数初始化GPIO 
	}	
	//测试用例
	void TEST_SetGPIO()
	{
		setGPIOConfiguration(ChannelA, getGPIOPortByNumber(1)|getGPIOPortByNumber(2), GPIO_Mode_Out_PP, GPIO_Speed_2MHz);
	}
#endif
//-----------------------End of GPIO Config----------------------------------------	

	

//----------------------- 坐标系变换 ---------------------------------------------
#ifdef EXCHANGE_COORDINATE
	//将霍尔传感器的数值转换为小车的运动距离
	void Hall_2_CarInitial(double ratio,double diameter)
	{
		
	}
	u16 SpeedHall_2_Car(u16 hallValue)
	{
		
	}
#endif
//-----------------------End of  坐标系变换 ----------------------------------------	
	
//----------------------- 数组操作 ---------------------------------------------
#ifdef DATA_ARRAY_HANDLE
	//数组前N个数前移一位
	void moveArrayForward(u8 validLen, int *arrayData)
	{
    for(int i = 0; i < (validLen - 1); i ++)
    {
		if((&(arrayData[i]) != 0) && (&(arrayData[i + 1]) != 0))
        arrayData[i] = arrayData[i + 1];
    }
	}
	
	//测试用例
	void TEST_MoveArray()
	{
		int data[10] = {1,2,3,4,5,6,7,8,9,10};
		moveArrayForward(4,data);
	}
#endif
//-----------------------End of 数组操作 ----------------------------------------	
