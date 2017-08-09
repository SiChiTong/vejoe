/****************************************************
file     : tools.h  for stm32
author   : kernel_007@sina.cn
copyright: 2013.9.05~2015.9.05
*****************************************************/
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

//-----------------------fsm manager module--by houzuping-------------------------
#if defined FSM_MANAGER_MODULE

	#define 	CONDITION_TURE		1  	//条件满足，正确
	#define 	CONDITION_FLASE 	0	//状态条件不满足.

	#define 	INIT_FSM			0 	//所有状态机的最原始状态
//	#define 	MAX_JUMP_FSM_NUM    5   //从当前状态跳转到其他状态，最大跳转数量
	
	//-----------//
	struct _FSM_Module
	{
		struct _FSM_Manager *Fsm_Info;
		UINT8 SystemFsm;			//系统状态机
		UINT8 FsmNum;				//状态机的个数
	};
	
	struct _FSM_Manager
	{
		BOOL  ConcomitanceFlag;		//是否有伴随事件状态标志
		BOOL  OneOffEventFlag;		//本状态一次性事件标志
		UINT8 CurFsm;				//状态机的状态管理变量
		UINT8 NextFsm;				//下一个状态
		HANDLER03 FsmCondition;		//状态机条件判断函数
		HANDLER01 FsmFunction;		//当前状态的循环执行函数
		HANDLER01 FsmOneOffEvent;	//当前状态的一次性函数
		HANDLER01 FsmConcomitance;	//状态机伴随函数
	};

	//----------------------------函数声明区----------------------------------//
	//配置所有事件模块个数，因为每一个事件都有一个状态机模块
	void ConfigEventModuleNum(UINT8 num);
	//配置某一事件状态机的个数
	BOOL ConfigEventFsmNum(UINT8 num, UINT8 module);
	//注册事件状态机
	void RegisterEventFSMList(struct _FSM_Manager *EventFSM, UINT8 FsmId, UINT8 module);
	//状态机运行函数
	void AnalyseModuleFSM(UINT8 module);
	
#endif
//--------------------------------------------------------------------------------
//--------------------------key pad scan by houzuping---------------------------//
#if defined USE_KEY_SCAN

	#define 	KEY_INPUT		0
	#define 	KEY_OUTPUT		1

	struct Key_Scanx
	{
		HANDLER02 RowxCtlDir;
		HANDLER02 RowxCtlValue;
		HANDLER03 RowxRead;
	};
	
	struct Key_Scany
	{
		HANDLER02 RowyCtlDir;
		HANDLER02 RowyCtlValue;
		HANDLER03 RowyRead;
	};	

	struct Key_Pad
	{
		UINT8 RegisterFlag;			//模块注册标记
		UINT8 KeyPadId;				//pad id .
		UINT8 KeyRowx;				//x方向的IO数量
		UINT8 KeyRowy;				//y方向的IO数量
		UINT8 *KeyValue;				//按键值.
		struct Key_Scanx *Key_Info_x;
		struct Key_Scany *Key_Info_y;
	};
	//-------------------------------------read------------------------------------
	void ConfigKeyPad(UINT8 num, UINT8 timer);
	void SetKeyPadSize(UINT8 index, UINT8 row_x, UINT8 row_y);
	void RegisterRowxCtlValue(UINT8 index, UINT16 key_rowx, ...);
	void RegisterRowyCtlValue(UINT8 index, UINT16 key_rowy, ...);
	void RegisterRowxDir(UINT8 index, UINT16 key_rowx, ...);
	void RegisterRowyDir(UINT8 index, UINT16 key_rowy, ...);
	void RegisterRowxRead(UINT8 index, UINT16 key_rowx, ...);
	void RegisterRowyRead(UINT8 index, UINT16 key_rowy, ...);
	void ScanKeyPad(void);
	UINT8 GetKeyPadValue(UINT8 index, UINT8 row_xn, UINT8 row_yn);
	
	
#endif
//---------------------------------------------------------------------------------
#if defined USE_KEY_EVENT

	#define 	KEY_HOLD	0x00
	#define		KEY_UP		0x01
	#define 	KEY_DOWN	0x02  
		
	#define		HOLD_MAX_TIME		1500
	#define		UPS_MIN_TIME		5    //100MS
	#define		UPS_MAX_TIME		3000
	
	#define   	KEY_MAX_WAIT_DELAY  10000
//----------event层-------------//
	struct key_event
	{
		UINT8 key_id;
		UINT8 delay_id;                 //延时模块id
		UINT8 key_value;
		UINT8 key_up;
		UINT8 key_down;
		UINT8 key_pulse;
		UINT8 key_double_hit;
		UINT8 key_hold_event;
	};
//-------------------函数声明区------------------//
	UINT8 ConfigKeyMode(UINT8 num, UINT8 timer);
	void RegisterKey(UINT8 id, CB_READ IOhdl, UINT16 debounceMS);

	void GetKeyEvent(void);
	void GetKeyValue(void);
	UINT8 CheckDoubleHitEvent(UINT8 key_id);
	void ClearDoubleTime(UINT8 key_id);
	void StartDoubleTime(UINT8 key_id);
	UINT8 CheckHoldHitEvent(UINT8 key_id);
	void ClearHoldTime(UINT8 key_id);
	void StartHoldTime(UINT8 key_id);
	//----------------------------------------------//
	struct key_event *ReturnKeyEvent(UINT8 key_id);
	void ClrKeyDownEvent(UINT8 key_id);
	void ClrKeyUpEvent(UINT8 key_id);
	void ClrKeyHoldEvent(UINT8 key_id);
	void ClrKeyDoubleHitEvent(UINT8 key_id);

#endif
//-----------------------Debounce-------------------------------------------------
#if defined USE_DEBOUNCE
	struct Debounce_Info
	{
		BOOL _IOState;
		BOOL _DebouncedIO;

		CB_READ _readIO;
		UINT16 _Debounce_Count;
		UINT16 _Debounce_Tick;

		BOOL _start_flag;
	};
	
	void Init_Debounce(UINT8 number, UINT8 timer);
	void Config_Debounce(UINT8 index, CB_READ IOhdl, UINT16 debounceMS);
	BOOL Get_DebouncedIO(UINT8 index);
	void Clr_Tick(UINT8 index);
	void _Debounce_Tick(void);
#endif
//-----------------------End of Debounce------------------------------------------

//----------------------- Communication-------------------------------------------
#if defined USE_COMMUNICATION
	//定义通讯的4中方式：1。发送并要求答复，2。接受并答复，3.广播（只发送）,4.只接受
	#define CS_REPORT					0
	#define CS_CHECK					1
	#define CS_BROADCAST				2
	#define CS_LISTENONLY				3
	
	//第一种通讯方式的状态机
	#define CREPORT_IDLE				0
	#define CREPORT_SEND				1
	#define CREPORT_WAITREPLY			2
	#define CREPORT_END					3
	
	//第二种通讯方式的状态机
	#define CCHECK_RECEIVE				0
	#define CCHECK_REPLY				1
	#define CCHECK_END					2
	
	//发送  时的状态
	#define CS_INPROGRESS				0
	#define CS_SUCCESS					1
	#define CS_FAILURE					2
	
	//发送模块（子函数）的状态机
	#define SEND_IDLE					0
	#define SEND_RENDER_DATA			1
	#define SEND_TRANSMIT				2
	#define SEND_WAIT_SUCCESS			3
	#define SEND_TIMEOUT				4
	#define SEND_FAILURE				5
	#define SEND_END					6
	
	//等待回复 时的状态
	#define CR_WAIT						0
	#define CR_SUCCESS					1
	#define CR_TIMEOUT					2
	
	//等待回复模块（子函数）的状态机
	#define RECEIVE_IDLE				0
	#define RECEIVE_WAIT				1
	#define RECEIVE_SUCCESS				2
	#define RECEIVE_TIMEOUT				3
	
	#define COMMU_SEND_FAILURE			0x01
	#define COMMU_NO_REPLY				0x02
	#define COMMU_REPLY_FAILURE			0x04 
	
	typedef BOOL (*COMMU_HANDLER)(UINT16*);
	
	struct _Commu_Info
	{
		UINT16* _pParent;
	
		UINT8 _commu_type;
		UINT8 _commu_FSM;
		//------------------//
		UINT8 _receive_error; //接受数据错误
		//-----2011-12-7----//
		UINT8 _trytimes;
		UINT8 _TRYTIMES;
		UINT8 _sendtimes;
		UINT8 _SENDTIMES;
		UINT16	_timeout;
		UINT16 _TIMEOUT;
	
		BOOL _start_send;
		UINT8 _send_FSM;
		UINT8 _send_status;
		BOOL _data_empty_flag;
		COMMU_HANDLER _render_data;
		COMMU_HANDLER _load_data_to_buffer;
		COMMU_HANDLER _transmit;
		COMMU_HANDLER _transmit_success;
		COMMU_HANDLER _cancel_transmit;
		COMMU_HANDLER _send_end;
		COMMU_HANDLER _send_failure;
	
		UINT8 _receive_FSM;
		UINT8 _receive_status;
		COMMU_HANDLER _process_data;
		BOOL _new_data_flag;
	
		UINT8 _errorCode;
	};
	typedef struct _Commu_Info COMMUInfo;
	
	void Communication(COMMUInfo* info);
	void Commu_Reset_Data(COMMUInfo* info);
	void Commu_Timer(COMMUInfo* info);
	
	void _Commu_Report(COMMUInfo* info);
	void _Commu_Check(COMMUInfo* info);
	void _Commu_BroadCast(COMMUInfo* info);
	void _Commu_ListenOnly(COMMUInfo* info);
	
	UINT8 _Commu_Send(COMMUInfo* info);
	UINT8 _Commu_Receive(COMMUInfo* info);
#endif
//-----------------------End of Communication------------------------------------

//----------------------- CANApp ------------------------------------------------
#if defined USE_CANAPP
	//通讯timout的缺省值：timeout时间为500ms，重发次数为3，重试次数为3
	#define CANAPP_DEFAULT_TIMEOUT			100
	#define CANAPP_DEFAULT_SENDTIMES		3
	#define CANAPP_DEFAULT_TRYTIMES			3
	
	struct _CANAPP_Info
	{
		struct _Commu_Info _commu_info;
		struct _CANMsgInfo _myMsg;
		union _CANData* _data;
		UINT16 _len;
		UINT16 _slen;
		UINT16 _rlen;
		UINT16 _index;
	};
	typedef struct _CANAPP_Info CANAPP_INFO;
	
	void CANAPP_Init(CAN_TypeDef* can_x, UINT32 fcy, UINT32 baud, UINT16 num, ...);
	void CANAPP_Config(UINT8 num, UINT16 timer);
	void CANAPP_Set_Data(UINT8 ID, UINT8 packageNum, UINT32 receiver, UINT32 sender, UINT32 rmt, UINT32 smt, UINT8 commuType);
	void CANAPP_Set_Para(UINT8 ID, COMMU_HANDLER render, COMMU_HANDLER process);
	void CANAPP_Communication(void);
	void CANAPP_Start(UINT8 ID);
	CANAPP_INFO* CANAPP_GetInfo(UINT8 ID);
	
	void _CANAPP_Timer(void);
	BOOL _CANAPP_load_data_to_buffer(UINT16* pName);
	BOOL _CANAPP_transmit(UINT16* pName);
	BOOL _CANAPP_transmit_success(UINT16* pName);
	BOOL _CANAPP_cancel_transmit(UINT16* pName);
	BOOL _CANAPP_send_end(UINT16* pName);
	BOOL _CANAPP_send_failure(UINT16* pName);
	void _CANAPP_Get_NewMsg(void);
    
#endif
//-----------------------End of CANApp -------------------------------------------

//----------------------- STEPPER MOTOR CONTROL-----------------------------------
#if defined USE_UNIPOLAR_STEP_DRIVER
	#define UNIPOLAR_STEPPER	0
	#define PUSHPULL_STEPPER	1
	
	#define PWM_DRIVER			0
	#define OC_DRIVER			1
	
	#define APOS				0
	#define BPOS				1
	#define ANEG				2
	#define BNEG				3
	
	#define STEP_CLOCKWISE				0			//顺时针（正对电机轴）
	#define STEP_COUNTERCLOCKWISE		1			//逆时针
	
	struct _Driver_Info
	{
		UINT8 _mode;			//UNIPOLAR / PUSH_PULL
		UINT8 _style;			//USE_PWM / USE_OC
		UINT16 _resolution;		//马达的细分步数
		UINT16 _PWM_Base;		//PWM整周期对应的计数值
		UINT16* _table;			//占空比的表格
			
		UINT8 _ch[4];			//各通道的控制选择（PWM1L,PWM1H.... / OC1,OC2.....）
		UINT16* _PDC[4];		//改变PWM占空比的指针,(PWM方式改变PDC,OC方式改变OCRS)
	
		UINT16* _OVDC;			//PWM方式下，OVDC的寄存器指针
		UINT16 _phase[4];		//PWM方式下，改变马达象限时的OVDC的值
		UINT16 _OVDC_Mask;
	
		BOOL _enable;			//电机是否使能
		BOOL _dir;				//电机转动方向
		UINT8 _step;			//当前步数（位置）
		UINT8 _quadrant;		//当前象限
		UINT8 _currentLevel;
	};
	
	void Set_DirverNum(UINT8 num);
	void Config_PWM(UINT8 index, UINT8 style, UINT32 fcy, UINT32 frequency, UINT8 timer);
	void Config_Driver(UINT8 index, UINT8 mode, UINT16 resolution);
	void Config_Channel(UINT8 index, UINT8 chApos, UINT8 chBpos, UINT8 chAneg, UINT8 chBneg);
	
	void Driver_Direction(UINT8 index, BOOL dir);
	void Driver_Hold(UINT8 index, UINT8 para);
	void Driver_Free(UINT8 index);
	void Driver_Step(UINT8 index);
	
	void Set_CurrentLevel(UINT8 index, UINT8 currentLevel);
	UINT8 Get_Current_Level(UINT8 index, UINT16 speed, UINT16 current);
	void Set_Driver_Position(UINT8 index, UINT16 angle);
	struct _Driver_Info Get_Driver_Info(UINT8 index);
	
	void _change_quadrant(UINT8 index);
	void _change_PDC(UINT8 index);
	void _modify_current(UINT16* pdc1, UINT16* pdc2, UINT8 level);
	void _set_quadrant_PDC(UINT8 index, UINT16 Tapos, UINT16 Tbpos, UINT16 Taneg, UINT16 Tbneg);
#endif
//-----------------------End of STEPPER MOTOR CONTROL-----------------------------

//----------------------- 步进电机任务管理模块------------------------------------
#if defined USE_STEPPER_TASK_MANAGER
	#define ONE_WAY_MOTION				0
	#define BACK_FORTH_MOTION			1
	#define AUTO_BACK_FORTH_MOTION		2
	
	#define MOVING_NONE					0
	#define MOVING_FORWARD				1
	#define MOVING_BACKWARD				2

	#define DRIVER_IDLE					0
	#define DRIVER_WAIT					1
	#define DRIVER_STARTPOINT			2
	#define DRIVER_MOVINGFORWARD		3
	#define DRIVER_MIDDLEPOINT			4
	#define DRIVER_MOVINGBACKWARD		5
	#define DRIVER_ENDPOINT				6
	#define DRIVER_ERROR				7
	
	#define SPEED_UP					0
	#define SPEED_UNIFORM				1
	#define SPEED_DOWN					2
	
	struct _Stepper_Task_Manager
	{
		UINT16 _isActive;						//任务是否被激活的标志

		UINT16 _driverID;						//对应的driver编号
		UINT16 _driverResolution;				//driver的分辨率
		UINT16 _driverMode;						//driver的模式
		UINT16 _taskType;						//任务类型：1.单程（无中间点）， 2.受控的往返（有中间点）， 3.自动往返
		UINT16 _taskDirection;					//任务起始方向
		UINT16 _taskTime;						//任务的时间
		UINT16 _taskAngle;						//任务需要转动的角度
		UINT16 _taskSteps;						//任务对应的电机步数
		
		UINT16 _isRest_KeepHold;				//静止时是否保持HOLD
		UINT16 _Rest_Current;					//静止时HOLD的电流
		UINT16 _StartPoint_HoldTime;			//起点HOLD的时间（可为0）
		UINT16 _StartPoint_Current;				//起点HOLD时的电流
		UINT16 _Moving_Current;					//运行时的电流
		UINT16 _isMiddlePoint_KeepHold;			//到达中间点时，是否保持HOLD
		UINT16 _MiddlePoint_HoldTime;			//中间点HOLD的时间
		UINT16 _MiddlePoint_Current;			//中间点HOLD的电流
		UINT16 _EndPoint_HoldTime;				//结束点HOLD的时间
		UINT16 _EndPoint_Current;				//结束点HOLD的电流
		
		UINT16 _FSM;							//状态机
		UINT16 _subFSM;							//子状态机
		UINT16 _movingFlag;						//开始移动标志
		UINT16 _currentStep;					//当前已移动的步数
		UINT16 _timer;							//于此任务关联的timer
		UINT16 _fcy;							//fcy
		UINT16 _msPrn;							//1毫秒对应的Prn
		UINT16 _tick;							//定时计数器
		
		UINT16 _speedupSteps;					//加速阶段的步数
		UINT16* _speedup_PrnTable;				//加速阶段的prn表
		UINT16* _speedup_CurrentLevelTable;		//加速阶段的电流数值
		UINT16 _speeddownSteps;					//减速阶段的步数
		UINT16* _speeddown_PrnTable;			//减速阶段的prn表
		UINT16* _speeddown_CurrentLevelTable;	//减速阶段的电流数值
		UINT16 _maxSpeed;						//最高速阶段（匀速阶段）
		UINT16 _maxSpeed_Prn;					//最高速对应的prn
		
		UINT16 _Rest_CurrentLevel;				//静止时电流对应的level
		UINT16 _StartPoint_CurrentLevel;		//起点时电流对应的level
		UINT16 _Moving_CurrentLevel;			//移动时电流对应的level
		UINT16 _MiddlePoint_CurrentLevel;		//中间点时电流对应的level
		UINT16 _EndPoint_CurrentLevel;			//结束点时电流对应的level
	};
	
	void Set_ExternSpeed(UINT8 index, UINT16 speed1, UINT16 time1, UINT16 speed2, UINT16 time2);
	
	void Set_TaskNum(UINT8 num);
	void Config_Task(UINT8 index, UINT8 driverID, UINT32 fcy, UINT8 timer, UINT8 pri);
	void Set_Task(UINT8 index, UINT8 type, UINT8 dir, UINT16 angle, UINT16 time);
	void Set_Speed(UINT8 index, UINT16 speed1, UINT16 time1, UINT16 speed2, UINT16 time2);
	void Set_Rest_Para(UINT8 index, BOOL isKeepHold, UINT16 restCurrent);
	void Set_StartPoint_Para(UINT8 index, UINT16 holdTime, UINT16 holdCurrent);
	void Set_MiddlePoint_Para(UINT8 index, UINT16 holdTime, UINT16 holdCurrent);
	void Set_EndPoint_Para(UINT8 index, UINT16 holdTime, UINT16 holdCurrent);
	void Set_MovingCurrent(UINT8 index, UINT16 current);
	void Activate_Task(UINT8 index, UINT8 state);
	void Start_Task(UINT8 index, UINT8 state);
	struct _Stepper_Task_Manager Get_Task_Info(UINT8 index);
	
	void _stepper_manager_timer1(void);
	void _stepper_manager_timer2(void);
	void _stepper_manager_timer3(void);
	void _stepper_manager(UINT8 index);
	BOOL _moving(UINT8 index);
	//添加Houzuping
	void UpdataStepMotorSpeedup(UINT8 index, UINT16* speedup_PrnTable, UINT8 *speedup_steps);
	void UpdataStepMotorSpeeddown(UINT8 index, UINT16* speeddown_PrnTable, UINT8 *speeddown_steps);
	void UpdataStepMotorSpeedArge(UINT8 index, UINT16 max_speed, UINT16 max_prn, UINT16 steps);
	void UpdataTaskArgv(UINT8 index, UINT8 MotorDirect, UINT16 speedup_steps, UINT16 speeddown_steps);
	UINT8 CopySpeedupTable(UINT8 index, UINT16 *uptable);
	UINT8 CopySpeeddownTable(UINT8 index, UINT16 *downtable);
	
	//
#endif
//-----------------------End of 步进电机任务管理模块------------------------------

//----------------------- COBS模块------------------------------------------------
#if defined USE_COBS
	void COBS_Encoding8(UINT8* ptr, UINT8 ptr_len, UINT8* dst, UINT8* dst_len);
	void COBS_Decoding8(UINT8* ptr, UINT8 ptr_len, UINT8* dst, UINT8* dst_len);
	
	UINT8 COBS_Encode(UINT8* ptr, UINT8 len);
	UINT8 COBS_Decode(UINT8* ptr, UINT8 len);
	
	UINT8 COBS_Package(UINT8* ptr, UINT8 len);
	UINT8 COBS_UpPackage(UINT8* ptr, UINT8 len);
#endif
//-----------------------End of COBS模块------------------------------------------

//----------------------- UARTApp模块---------------------------------------------
#if defined USE_UARTAPP
	#define UART_COMMON		0
	#define UART_COBS		1
	
	#define UART_HEAD_STAGE			0
	#define UART_DATA_STAGE			1
	
	struct _UARTAPP_Info
	{
		UINT8 _mode;
		UINT8 _ch;
		UINT8* _data;
		UINT8 _len;
		UINT8 _slen;
		UINT8 _rlen;
		UINT8 _index;
		UINT8 _FSM;
		UINT8 _newDataFlag;
		struct _Commu_Info _commu_info;
	};
	
	void UARTAPP_Init(UINT16 num, UINT32 fcy, UINT8 timer);
	void UARTApp_Config(UINT8 index, UINT32 baud, UINT8 pri, UINT8 ch, UINT8 mode, UINT8 bufferLen);
	void UARTApp_Set_Para(UINT8 index, UINT16 timeout, UINT8 sendtimes);
	void UARTAPP_Communicate(void);
	void UARTAPP_Start(UINT8 index);
	struct _UARTAPP_Info* UARTAPP_GetInfo(UINT8 index);

	void _UARTAPP_Timer(void);
	BOOL _UARTAPP_load_data_to_buffer(UINT16* pName);
	BOOL _UARTAPP_transmit(UINT16* pName);
	BOOL _UARTAPP_transmit_success(UINT16* pName);
	void _UARTAPP_Get_NewMsg1(UINT8 data);
	void _UARTAPP_Get_NewMsg2(UINT8 data);	
	void _UARTApp_Get_NewMsg(UINT8 index, UINT8 data);

	UINT8 _calculate_CRC(UINT8* data, UINT8 len);
	UINT8 _add_CRC(UINT8* data, UINT8 len);
	BOOL _check_CRC(UINT8* data, UINT8* len);
#endif
//-----------------------End of UARTApp模块---------------------------------------

//----------------------- ENCRYPTION模块------------------------------------------
#if defined USE_ENCRYPTION
#define R_memset(x, y, z) memset(x, y, z)
#define R_memcpy(x, y, z) memcpy(x, y, z)
#define R_memcmp(x, y, z) memcmp(x, y, z)

typedef unsigned long UINT4;
typedef unsigned char *POINTER;

/* MD5 context. */
typedef struct {
/* state (ABCD) */   
/*四个32bits数，用于存放最终计算得到的消息摘要。当消息长度〉512bits时，也用于存放每个512bits的中间结果*/ 
UINT4 state[4];  

/* number of bits, modulo 2^64 (lsb first) */    
/*存储原始信息的bits数长度,不包括填充的bits，最长为 2^64 bits，因为2^64是一个64位数的最大值*/
UINT4 count[2];

/* input buffer */ 
/*存放输入的信息的缓冲区，512bits*/
unsigned char buffer[64]; 
} MD5_CTX;

void MD5Init(MD5_CTX *);
void MD5Update(MD5_CTX *, unsigned char *, unsigned int);
void MD5Final(unsigned char [16], MD5_CTX *);
void MD5App(unsigned char *source_data, unsigned char *encrypt_data);
void md5data(unsigned char *source, unsigned char *dst);
#endif
//-----------------------End of ENCRYPTION模块-----------------------------------

//-------------------------QEI APP模块-------------------------------------------
#if defined USE_QEIAPP

typedef struct 
{
	UINT16 CurSpeed;    //当前速度，速度和位置信息都是滞后信息
	UINT16 CurAngle;    //当前位置
	float  Time;        //计算间隔时间
	UINT16 MaxCount;    //编码器最大计数
}QEI_Msg;

//-----------------函数声明区------
	void Config_QEITask(float TimerCycle, UINT16 MaxCounter);
	UINT16 QEIReturnSpeed(void);
	UINT16 QEIReturnAngle(void);
	void _QEI_CallBack(void);
#endif
//-------------------------DELAY APP模块-------------------------------------------
#ifdef  USE_DELAY
	
	/*delay module ????id??*/
	#define 	MAX_DELAY_NUM		15
	
	struct delay_info
	{
		UINT8 StartFlag;		//??????
		UINT8 Success;			//????,?????
		UINT16 Time;			//????
		UINT16 TimeCount;		//????
	};

	void ConfigDelayModuleNum(UINT8 num, UINT8 timer);
	UINT8 GetDelayIdFunction(UINT8 num, UINT8 *return_id);
	char SetCountTime(UINT8 id_number, UINT16 time);
	char StartDelayCount(UINT8 id_number);
	char StopDelayCount(UINT8 id_number);
	char ReturnSuccess(UINT8 id_number);
	char ClearDelaySucFlag(UINT8 id_number);
	unsigned int ReturnCurCount(UINT8 id_number);

	//------------------------//
	void Delay(UINT32 time);
	void Delay_Module(UINT16 delay_num);

#endif

//----------------------------rs485----------------------------//
#ifdef  USE_RS485
//macros
	#define 	UART_MODE		        0
	#define  	IO_MODE			        1
	//buf size.
	#define 	PACKAGE_SIZE	        16      //
	#define     DATA_SIZE	            8
	///
	#define     RS485_DEFAULT_TIMES 		    10
	#define		RS485_WAIT_BACK_TIME			3000     //3s
	//--------------RS485 DELAY ID--------------//
	#define		RS485_SEND_WAIT_DELAY_ID		0
	#define 	RS485_WAIT_BACK_DELAY_ID		1
	#define 	RS485_WAIT_SYNC_DELAY_ID		2
	#define 	RS485_WAIT_ASYNC_DELAY_ID   3
	//------------------------------------------//
	#define 	SEND_SUCCEED					1

	#define		ENABLE_HOST_MODE			1 	//??????
	#define 	ENABLE_SUB_MODE				0	//??????
	//-
	#define		TRY_SEND_TIMES				3
	//----------------------------------------------//
	#define 	CS_REPORT					0
	#define 	CS_CHECK					1
	#define 	CS_BROADCAST				2
	#define 	CS_LISTENONLY				3
	
	//发送标志
	#define 	CS_INPROGRESS				0
	#define 	CS_SUCCESS					1
	#define 	CS_FAILURE					2

	//--------------rs485 strcut------------//
	struct _RS485_Class             //rs485 ??????
	{
		UINT8 Index;				//????
		UINT8 Package;				//???????????.
		UINT8 StateFlag;			//???????????
		UINT8 Length;				//????
		UINT8 IsBusy;				//???
		UINT8 SenderId;				//???ID
		UINT8 ReceiveId;			//???ID
		UINT8 SendMsgType;			//??????
		UINT8 ReceMsgType;			//??????	
		UINT8 Data[8];		//??
		UINT8 CrcCheck;				//CRC8 校验检查
	};

	//--------------??????------------//
	struct _RS485APP_INFO
	{
		UINT8  COMMU_FSM;						//???????
		UINT8  RECE_FSM;
		UINT8  SEND_FSM;
		UINT8  StartSend;						//???????
		UINT8  TryTimes;						//?????????
		UINT8  CommuType;						//????
		UINT8  WorkBusyFlag;					//?????
		UINT16 _char_time_out;	//字符间超时时间
		UINT16 _frame_time_out;	//帧间超时时间
		UINT16 _error_code;			//错误代码
		struct _RS485_Class _send_message;		//????
		struct _RS485_Class _receive_message;	//?????? 	
		HANDLER04 _transmit_function;			//??????
		HANDLER04 _receive_function;			//??????
	};


	//-----------??????---------//
	struct _RS485APP_CTL			//485????????,????????
	{
		UINT16    UartId;			//
		UINT16    PhyType;			//??IO???????uart??
		UINT16    NodeId;
		HANDLER02 DriCtl;			//??????
	};
	//--------------?????----------------//
	void RS485APP_Init(UINT32 fcy, UINT32 baud, UINT8 pri, UINT8 uart, UINT16 node, UINT16 physic);
	UINT8 RS485ConfigDelayModule(UINT8 delay_id_num);
	void RS485APP_Config(UINT8 num);
	void ConfigRs485DirCtl(HANDLER02 dir_ctl);
	void CreatCRC8_Table(void);
	void RS485APP_Set_pre(UINT8 ID, HANDLER04 _transmit_f, HANDLER04 _receive_f);
	void RS485APP_Set_Argv(UINT8 ID, UINT8 data_long, UINT32 receiver, UINT32 sender, UINT32 rmt, UINT32 smt, UINT8 commuType);
	void RS485APP_Communication(void);
	void RS485APP_Start(UINT8 ID);

	struct _RS485APP_INFO *RS485APP_GetInfo(UINT8 ID);

#endif
//----------------------------end of rs485-----------------------//

//---------------------------signal manage-----------------------//
#ifdef SIG_MANAGE
	#define ONLY_READ		1		//只读信号
	#define READ_WRITE		2		//读写信号

	struct _SigManage
	{
		UINT8  SigType;				//信号类型
		UINT8  SigTriggerFlag;		//信号触发标志
		UINT16 CurSigState;			//当前信号状态
		UINT16 LastSigState;		//上一次信号状态
		void*  (*SigProcess)(void *);//信号处理函数
	};
	
	//-------------函数声明区--------------//

#endif

//------------------------------ps2------------------------------//
#ifdef USE_PS2

	#define PS2_STANDARD_CMD_NUM	17  //17个标准命令.
	#define PS2_CYCLE				80	//ps2 clock 80US.

	struct _ps2_info
	{
		UINT8 SendData;					//发送数据
		UINT8 BackHost; 				//返回主机命令
		UINT8 StartCount;				//开始计时标志.
		UINT8 TimeOverflow;				//时间溢出.
		UINT8 CmdBuf[3];				//命令buf.
		UINT16 WaitTime;				//等待时间
		CB_SET SetDataPortDir;
		CB_SET SetClkPortDir;
		CB_SET SetDataPortValue;
		CB_SET SetClkPortValue;
		CB_READ GetData;
		CB_READ GetClk; 
	};
	//----------------------------------------------------------//
	//函数声明区
	void RegisterPs2DataPort(CB_SET set_port_dir, CB_SET set_data, CB_READ get_data);
	void RegisterPs2ClkPort(CB_SET set_port_dir, CB_SET set_clk, CB_READ get_clk);
	void PS2ReportData(UINT8 data);		
#endif

//-----------------------------------------------------------------//
#ifdef USE_DATA_BUF_QUEUE
  #define  QUEUE_SIZE				20
  //struct 
  struct _data_queue
  {
    UINT8  _rd_index;									//
    UINT8  _wr_index;
    UINT8  _isUser;										//被使用
    UINT16 *_write_pointer;								//向缓冲区写数据
    UINT16 *_read_pointer;								//向缓冲区中取数据
    UINT16 _buf_size[QUEUE_SIZE];					    //队列buf大小
  };
  //函数声明区
  void ConfigQueueBufNum(UINT8 buf_num);
  UINT8 InitQueueBuf(UINT8 id);
  UINT8 Get_NewQueueId(UINT8 num, UINT8 *return_id);
  UINT8 WrDataToBufQueue(UINT8 id, UINT16 data);
  UINT8 RdDataFromBufQueue(UINT8 id, UINT16 *data);
  
#endif
	
//------------------------------------------------------------------//
//CRC数据校验模块	CRC校验码错误检测率问题
#ifdef  USER_CRC_DATA_CHECK
  #define	 USE_HALF_BYTE_CHECK_TABLE
  //#define  USE_ONE_BYTE_CHECK_TABLE
  //#define  USE_DOUBLE_BYTE_CHECK_TABLE
  
  #define	 USE_CRC8
  //#define  USE_CRC16
  /*
  crc8 循环校验码  半字节查表法
  CRC多项式不是固定的，由用户自己定义，但是定义后需要修改 CRC8_POLY的值
  CRC多项式,CRC8_POLY 的值对应多项式的bit 但是最高位不用写，不理解者查看CRC原理
  y(n) = x^8 + x^7 + x^6 + x^4 + x^2 + 1;
  */
  #define 	CRC8_POLY		0xd5
  /*
  Y(n) = x^16 + x^15 + x^2 + 1;  //多项式不同，错误检测率不同
  */
  #define		CRC16_POLY  0x8005
  //函数声明区
  void CRC_Module_Init(void);
  UINT8 CRC8Checkout(UINT8 *buf, UINT16 buf_size);
	
#endif

//----------------------简易调度器-------------------------//
#ifdef USE_SCHEDULING
    
    struct sch_info
    {
        UINT8  index;           //
        UINT8  trigger_flag;    //任务触发标志 
        UINT8  task_action;     //任务执行标志
        UINT16 time_counter;    //任务执行一次的间隔时间
        UINT16 time_load;       //time load agian
        HANDLER01 sch_task;     //task function.
    };
    
    //----------------------//
    void Config_Scheduling(UINT8 task_num, UINT8 timer);
    void Register_Task(UINT8 task_id, HANDLER01 task, UINT16 work_time);
    void SET_TaskTrigger(UINT8 task_id, UINT8 trigger_flag);
    void Schedu_APP(void);
    
#endif
    

#ifdef USE_ERROR

// since GLIB defines the error_t, to avoid conflicting we have to apply the 
// macro __error_t_defined which is defined in GLIB to determine whether the
// error_t should be defined here or not
#ifndef __error_t_defined
typedef int error_t;
#define __error_t_defined
#endif

#define MODULE_BITS     15
#define ERROR_BITS      16

// if the ERROR_T_SIZE is 4 then the ERROR_MARK should be 0x80000000
// ERROR_MARK is used to make a number as a negative integer
#define ERROR_MARK      (1 << (MODULE_BITS + ERROR_BITS))

#define ERROR_BEGIN(_module_id) ((_module_id) << ERROR_BITS)

#define ERROR_T(_module_error) /*lint -e{648} */(ERROR_MARK | (_module_error))

#define MODULE_ERROR(_error_t) /*lint -e{778} */((_error_t) & ((1 << ERROR_BITS) - 1))
#define MODULE_ID(_error_t) /*lint -e{778,648} */(module_t)(((_error_t) & ~(ERROR_MARK)) >> ERROR_BITS)

#ifdef  __cplusplus
extern "C" {
#endif

const char *errstr (error_t _error);

#ifdef  __cplusplus
}
#endif

#endif //----------------------------//

/*
FFT module 
*/
#ifdef FFT_MODULE

    
    struct fft_info
    {
        
    };

#endif


#endif   // _TOOLS_H
