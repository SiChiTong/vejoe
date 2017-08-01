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
	#define LED_OFF					0				//����
	#define LED_ON 					1				//����
	#define LED_BLINK				2				//��˸
	#define LED_DIM					3				//��������
	#define LED_CONTROL				4				//�ⲿ����
	
	struct _LED_Info
	{
		UINT8 _LEDState;			//�Ƶ�״̬
		BOOL _Control;				//�����������ǰ�
		CB_SET _LEDhdl;				//���ƵƵĺ���ָ��
		UINT16 _BlinkCount;			//��Ӧ�ļ���ֵ		
		UINT16 _DimStep;			//��Ӧ�ļ���ֵ
		UINT16 _Tick;				//timer����ֵ
	};	
	
	void Config_LED(UINT8 number, UINT8 timer);
	void Register_LED(UINT8 index, CB_SET setLED, UINT8 state, UINT16 period);
	void Set_LEDState(UINT8 index, UINT8 state);
	void Set_Control(UINT8 index, BOOL state);
	void Set_LEDFcy(UINT8 index, UINT16 blinkPeriod);
	//��LED״̬����ʾ���ֵ�ֵ����2���ƺ�4��������
	void LED_DisplayBit(UINT16 value);
	void LED_DisplayNumber(UINT16 value);
	
	void _LED_Tick(void);
#endif 
//-----------------------End of LED-----------------------------------------------

//-------------------------------BEEPER-by houzuping------------------------------
#if defined USE_BEEPER
	#define MAX_AUDIO_FREQUENCY		10000 //���������Ƶʶ��Χ��20HZ~20000HZ.
	#define MIN_AUDIO_FREQUENCY     20		

	#define BEEPER_TIME		100 //ms ����ʱ�䣬����һ��ģʽ�ĳ���ʱ��.
	#define BEEPER_ON		1
	#define BEEPER_OFF		0

	struct _BEEPER_Info
	{
		UINT8 BeeperState;		//beeper state.
		UINT8 BeeperMode;		//����ģʽ
		UINT8 StartFlag;		//������־
		float Frequency;		//Ƶ��
		UINT16 Tick;			//����ʱ�����
		UINT16 FcyCount;		//���û���Ƶ����ת��Ϊʱ�����
		CB_SET Iohd;			//IO ��������
	};
	
	//---------------------------����������--------------------------//
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

	#define 	CONDITION_TURE		1  	//�������㣬��ȷ
	#define 	CONDITION_FLASE 	0	//״̬����������.

	#define 	INIT_FSM			0 	//����״̬������ԭʼ״̬
//	#define 	MAX_JUMP_FSM_NUM    5   //�ӵ�ǰ״̬��ת������״̬�������ת����
	
	//-----------//
	struct _FSM_Module
	{
		struct _FSM_Manager *Fsm_Info;
		UINT8 SystemFsm;			//ϵͳ״̬��
		UINT8 FsmNum;				//״̬���ĸ���
	};
	
	struct _FSM_Manager
	{
		BOOL  ConcomitanceFlag;		//�Ƿ��а����¼�״̬��־
		BOOL  OneOffEventFlag;		//��״̬һ�����¼���־
		UINT8 CurFsm;				//״̬����״̬�������
		UINT8 NextFsm;				//��һ��״̬
		HANDLER03 FsmCondition;		//״̬�������жϺ���
		HANDLER01 FsmFunction;		//��ǰ״̬��ѭ��ִ�к���
		HANDLER01 FsmOneOffEvent;	//��ǰ״̬��һ���Ժ���
		HANDLER01 FsmConcomitance;	//״̬�����溯��
	};

	//----------------------------����������----------------------------------//
	//���������¼�ģ���������Ϊÿһ���¼�����һ��״̬��ģ��
	void ConfigEventModuleNum(UINT8 num);
	//����ĳһ�¼�״̬���ĸ���
	BOOL ConfigEventFsmNum(UINT8 num, UINT8 module);
	//ע���¼�״̬��
	void RegisterEventFSMList(struct _FSM_Manager *EventFSM, UINT8 FsmId, UINT8 module);
	//״̬�����к���
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
		UINT8 RegisterFlag;			//ģ��ע����
		UINT8 KeyPadId;				//pad id .
		UINT8 KeyRowx;				//x�����IO����
		UINT8 KeyRowy;				//y�����IO����
		UINT8 *KeyValue;				//����ֵ.
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
//----------event��-------------//
	struct key_event
	{
		UINT8 key_id;
		UINT8 delay_id;                 //��ʱģ��id
		UINT8 key_value;
		UINT8 key_up;
		UINT8 key_down;
		UINT8 key_pulse;
		UINT8 key_double_hit;
		UINT8 key_hold_event;
	};
//-------------------����������------------------//
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
	//����ͨѶ��4�з�ʽ��1�����Ͳ�Ҫ��𸴣�2�����ܲ��𸴣�3.�㲥��ֻ���ͣ�,4.ֻ����
	#define CS_REPORT					0
	#define CS_CHECK					1
	#define CS_BROADCAST				2
	#define CS_LISTENONLY				3
	
	//��һ��ͨѶ��ʽ��״̬��
	#define CREPORT_IDLE				0
	#define CREPORT_SEND				1
	#define CREPORT_WAITREPLY			2
	#define CREPORT_END					3
	
	//�ڶ���ͨѶ��ʽ��״̬��
	#define CCHECK_RECEIVE				0
	#define CCHECK_REPLY				1
	#define CCHECK_END					2
	
	//����  ʱ��״̬
	#define CS_INPROGRESS				0
	#define CS_SUCCESS					1
	#define CS_FAILURE					2
	
	//����ģ�飨�Ӻ�������״̬��
	#define SEND_IDLE					0
	#define SEND_RENDER_DATA			1
	#define SEND_TRANSMIT				2
	#define SEND_WAIT_SUCCESS			3
	#define SEND_TIMEOUT				4
	#define SEND_FAILURE				5
	#define SEND_END					6
	
	//�ȴ��ظ� ʱ��״̬
	#define CR_WAIT						0
	#define CR_SUCCESS					1
	#define CR_TIMEOUT					2
	
	//�ȴ��ظ�ģ�飨�Ӻ�������״̬��
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
		UINT8 _receive_error; //�������ݴ���
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
	//ͨѶtimout��ȱʡֵ��timeoutʱ��Ϊ500ms���ط�����Ϊ3�����Դ���Ϊ3
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
	
	#define STEP_CLOCKWISE				0			//˳ʱ�루���Ե���ᣩ
	#define STEP_COUNTERCLOCKWISE		1			//��ʱ��
	
	struct _Driver_Info
	{
		UINT8 _mode;			//UNIPOLAR / PUSH_PULL
		UINT8 _style;			//USE_PWM / USE_OC
		UINT16 _resolution;		//����ϸ�ֲ���
		UINT16 _PWM_Base;		//PWM�����ڶ�Ӧ�ļ���ֵ
		UINT16* _table;			//ռ�ձȵı��
			
		UINT8 _ch[4];			//��ͨ���Ŀ���ѡ��PWM1L,PWM1H.... / OC1,OC2.....��
		UINT16* _PDC[4];		//�ı�PWMռ�ձȵ�ָ��,(PWM��ʽ�ı�PDC,OC��ʽ�ı�OCRS)
	
		UINT16* _OVDC;			//PWM��ʽ�£�OVDC�ļĴ���ָ��
		UINT16 _phase[4];		//PWM��ʽ�£��ı��������ʱ��OVDC��ֵ
		UINT16 _OVDC_Mask;
	
		BOOL _enable;			//����Ƿ�ʹ��
		BOOL _dir;				//���ת������
		UINT8 _step;			//��ǰ������λ�ã�
		UINT8 _quadrant;		//��ǰ����
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

//----------------------- ��������������ģ��------------------------------------
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
		UINT16 _isActive;						//�����Ƿ񱻼���ı�־

		UINT16 _driverID;						//��Ӧ��driver���
		UINT16 _driverResolution;				//driver�ķֱ���
		UINT16 _driverMode;						//driver��ģʽ
		UINT16 _taskType;						//�������ͣ�1.���̣����м�㣩�� 2.�ܿص����������м�㣩�� 3.�Զ�����
		UINT16 _taskDirection;					//������ʼ����
		UINT16 _taskTime;						//�����ʱ��
		UINT16 _taskAngle;						//������Ҫת���ĽǶ�
		UINT16 _taskSteps;						//�����Ӧ�ĵ������
		
		UINT16 _isRest_KeepHold;				//��ֹʱ�Ƿ񱣳�HOLD
		UINT16 _Rest_Current;					//��ֹʱHOLD�ĵ���
		UINT16 _StartPoint_HoldTime;			//���HOLD��ʱ�䣨��Ϊ0��
		UINT16 _StartPoint_Current;				//���HOLDʱ�ĵ���
		UINT16 _Moving_Current;					//����ʱ�ĵ���
		UINT16 _isMiddlePoint_KeepHold;			//�����м��ʱ���Ƿ񱣳�HOLD
		UINT16 _MiddlePoint_HoldTime;			//�м��HOLD��ʱ��
		UINT16 _MiddlePoint_Current;			//�м��HOLD�ĵ���
		UINT16 _EndPoint_HoldTime;				//������HOLD��ʱ��
		UINT16 _EndPoint_Current;				//������HOLD�ĵ���
		
		UINT16 _FSM;							//״̬��
		UINT16 _subFSM;							//��״̬��
		UINT16 _movingFlag;						//��ʼ�ƶ���־
		UINT16 _currentStep;					//��ǰ���ƶ��Ĳ���
		UINT16 _timer;							//�ڴ����������timer
		UINT16 _fcy;							//fcy
		UINT16 _msPrn;							//1�����Ӧ��Prn
		UINT16 _tick;							//��ʱ������
		
		UINT16 _speedupSteps;					//���ٽ׶εĲ���
		UINT16* _speedup_PrnTable;				//���ٽ׶ε�prn��
		UINT16* _speedup_CurrentLevelTable;		//���ٽ׶εĵ�����ֵ
		UINT16 _speeddownSteps;					//���ٽ׶εĲ���
		UINT16* _speeddown_PrnTable;			//���ٽ׶ε�prn��
		UINT16* _speeddown_CurrentLevelTable;	//���ٽ׶εĵ�����ֵ
		UINT16 _maxSpeed;						//����ٽ׶Σ����ٽ׶Σ�
		UINT16 _maxSpeed_Prn;					//����ٶ�Ӧ��prn
		
		UINT16 _Rest_CurrentLevel;				//��ֹʱ������Ӧ��level
		UINT16 _StartPoint_CurrentLevel;		//���ʱ������Ӧ��level
		UINT16 _Moving_CurrentLevel;			//�ƶ�ʱ������Ӧ��level
		UINT16 _MiddlePoint_CurrentLevel;		//�м��ʱ������Ӧ��level
		UINT16 _EndPoint_CurrentLevel;			//������ʱ������Ӧ��level
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
	//���Houzuping
	void UpdataStepMotorSpeedup(UINT8 index, UINT16* speedup_PrnTable, UINT8 *speedup_steps);
	void UpdataStepMotorSpeeddown(UINT8 index, UINT16* speeddown_PrnTable, UINT8 *speeddown_steps);
	void UpdataStepMotorSpeedArge(UINT8 index, UINT16 max_speed, UINT16 max_prn, UINT16 steps);
	void UpdataTaskArgv(UINT8 index, UINT8 MotorDirect, UINT16 speedup_steps, UINT16 speeddown_steps);
	UINT8 CopySpeedupTable(UINT8 index, UINT16 *uptable);
	UINT8 CopySpeeddownTable(UINT8 index, UINT16 *downtable);
	
	//
#endif
//-----------------------End of ��������������ģ��------------------------------

//----------------------- COBSģ��------------------------------------------------
#if defined USE_COBS
	void COBS_Encoding8(UINT8* ptr, UINT8 ptr_len, UINT8* dst, UINT8* dst_len);
	void COBS_Decoding8(UINT8* ptr, UINT8 ptr_len, UINT8* dst, UINT8* dst_len);
	
	UINT8 COBS_Encode(UINT8* ptr, UINT8 len);
	UINT8 COBS_Decode(UINT8* ptr, UINT8 len);
	
	UINT8 COBS_Package(UINT8* ptr, UINT8 len);
	UINT8 COBS_UpPackage(UINT8* ptr, UINT8 len);
#endif
//-----------------------End of COBSģ��------------------------------------------

//----------------------- UARTAppģ��---------------------------------------------
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
//-----------------------End of UARTAppģ��---------------------------------------

//----------------------- ENCRYPTIONģ��------------------------------------------
#if defined USE_ENCRYPTION
#define R_memset(x, y, z) memset(x, y, z)
#define R_memcpy(x, y, z) memcpy(x, y, z)
#define R_memcmp(x, y, z) memcmp(x, y, z)

typedef unsigned long UINT4;
typedef unsigned char *POINTER;

/* MD5 context. */
typedef struct {
/* state (ABCD) */   
/*�ĸ�32bits�������ڴ�����ռ���õ�����ϢժҪ������Ϣ���ȡ�512bitsʱ��Ҳ���ڴ��ÿ��512bits���м���*/ 
UINT4 state[4];  

/* number of bits, modulo 2^64 (lsb first) */    
/*�洢ԭʼ��Ϣ��bits������,����������bits���Ϊ 2^64 bits����Ϊ2^64��һ��64λ�������ֵ*/
UINT4 count[2];

/* input buffer */ 
/*����������Ϣ�Ļ�������512bits*/
unsigned char buffer[64]; 
} MD5_CTX;

void MD5Init(MD5_CTX *);
void MD5Update(MD5_CTX *, unsigned char *, unsigned int);
void MD5Final(unsigned char [16], MD5_CTX *);
void MD5App(unsigned char *source_data, unsigned char *encrypt_data);
void md5data(unsigned char *source, unsigned char *dst);
#endif
//-----------------------End of ENCRYPTIONģ��-----------------------------------

//-------------------------QEI APPģ��-------------------------------------------
#if defined USE_QEIAPP

typedef struct 
{
	UINT16 CurSpeed;    //��ǰ�ٶȣ��ٶȺ�λ����Ϣ�����ͺ���Ϣ
	UINT16 CurAngle;    //��ǰλ��
	float  Time;        //������ʱ��
	UINT16 MaxCount;    //������������
}QEI_Msg;

//-----------------����������------
	void Config_QEITask(float TimerCycle, UINT16 MaxCounter);
	UINT16 QEIReturnSpeed(void);
	UINT16 QEIReturnAngle(void);
	void _QEI_CallBack(void);
#endif
//-------------------------DELAY APPģ��-------------------------------------------
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
	
	//���ͱ�־
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
		UINT8 CrcCheck;				//CRC8 У����
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
		UINT16 _char_time_out;	//�ַ��䳬ʱʱ��
		UINT16 _frame_time_out;	//֡�䳬ʱʱ��
		UINT16 _error_code;			//�������
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
	#define ONLY_READ		1		//ֻ���ź�
	#define READ_WRITE		2		//��д�ź�

	struct _SigManage
	{
		UINT8  SigType;				//�ź�����
		UINT8  SigTriggerFlag;		//�źŴ�����־
		UINT16 CurSigState;			//��ǰ�ź�״̬
		UINT16 LastSigState;		//��һ���ź�״̬
		void*  (*SigProcess)(void *);//�źŴ�����
	};
	
	//-------------����������--------------//

#endif

//------------------------------ps2------------------------------//
#ifdef USE_PS2

	#define PS2_STANDARD_CMD_NUM	17  //17����׼����.
	#define PS2_CYCLE				80	//ps2 clock 80US.

	struct _ps2_info
	{
		UINT8 SendData;					//��������
		UINT8 BackHost; 				//������������
		UINT8 StartCount;				//��ʼ��ʱ��־.
		UINT8 TimeOverflow;				//ʱ�����.
		UINT8 CmdBuf[3];				//����buf.
		UINT16 WaitTime;				//�ȴ�ʱ��
		CB_SET SetDataPortDir;
		CB_SET SetClkPortDir;
		CB_SET SetDataPortValue;
		CB_SET SetClkPortValue;
		CB_READ GetData;
		CB_READ GetClk; 
	};
	//----------------------------------------------------------//
	//����������
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
    UINT8  _isUser;										//��ʹ��
    UINT16 *_write_pointer;								//�򻺳���д����
    UINT16 *_read_pointer;								//�򻺳�����ȡ����
    UINT16 _buf_size[QUEUE_SIZE];					    //����buf��С
  };
  //����������
  void ConfigQueueBufNum(UINT8 buf_num);
  UINT8 InitQueueBuf(UINT8 id);
  UINT8 Get_NewQueueId(UINT8 num, UINT8 *return_id);
  UINT8 WrDataToBufQueue(UINT8 id, UINT16 data);
  UINT8 RdDataFromBufQueue(UINT8 id, UINT16 *data);
  
#endif
	
//------------------------------------------------------------------//
//CRC����У��ģ��	CRCУ���������������
#ifdef  USER_CRC_DATA_CHECK
  #define	 USE_HALF_BYTE_CHECK_TABLE
  //#define  USE_ONE_BYTE_CHECK_TABLE
  //#define  USE_DOUBLE_BYTE_CHECK_TABLE
  
  #define	 USE_CRC8
  //#define  USE_CRC16
  /*
  crc8 ѭ��У����  ���ֽڲ��
  CRC����ʽ���ǹ̶��ģ����û��Լ����壬���Ƕ������Ҫ�޸� CRC8_POLY��ֵ
  CRC����ʽ,CRC8_POLY ��ֵ��Ӧ����ʽ��bit �������λ����д��������߲鿴CRCԭ��
  y(n) = x^8 + x^7 + x^6 + x^4 + x^2 + 1;
  */
  #define 	CRC8_POLY		0xd5
  /*
  Y(n) = x^16 + x^15 + x^2 + 1;  //����ʽ��ͬ���������ʲ�ͬ
  */
  #define		CRC16_POLY  0x8005
  //����������
  void CRC_Module_Init(void);
  UINT8 CRC8Checkout(UINT8 *buf, UINT16 buf_size);
	
#endif

//----------------------���׵�����-------------------------//
#ifdef USE_SCHEDULING
    
    struct sch_info
    {
        UINT8  index;           //
        UINT8  trigger_flag;    //���񴥷���־ 
        UINT8  task_action;     //����ִ�б�־
        UINT16 time_counter;    //����ִ��һ�εļ��ʱ��
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
