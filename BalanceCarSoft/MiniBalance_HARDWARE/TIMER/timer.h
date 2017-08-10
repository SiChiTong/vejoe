#ifndef __TIMER_H
#define __TIMER_H
#include <sys.h>	 

void TIM3_Cap_Init(u16 arr,u16 psc);
void Read_Distane(void);
void TIM3_IRQHandler(void);
void TIM8_Cap_Init(u16 arr, u16 psc);
void TIM8_CC_IRQHandler(void);

    #define  TIMER_CALLBACK_NUM   15
    typedef void (*CB_VOID)(void);
	
    enum
    {
        TIMER_2 = 0,
        TIMER_3,
        TIMER_4,
        TIMER_5, 
        TIMER_7,
//        TIMER_9,
//        TIMER_10,
//        TIMER_11
        TIMER_NUM
    }; 
    //timer struct. info
    struct timer_Info
    {		
      uint8_t    _isInit;
      uint8_t   _callback_Num;
      uint32_t  _frequency;
      CB_VOID _callback[TIMER_CALLBACK_NUM];
    };
    void Config_TIMER(uint8_t timer, uint8_t pri, uint32_t fry);
    void Timer_Register(uint8_t timer, CB_VOID function);
    struct timer_Info *Get_TimerInfo(uint8_t timer);
    //interrupt.
    void TIM2_IRQHandler(void);              //中断函数    
    void TIM3_IRQHandler(void);              //中断函数    
     
	void TIM5_IRQHandler(void);
	void TIM7_IRQHandler(void);	
	/////////////////////////////////////////////////////
	
	#define 	MAX_DELAY_NUM		15
	
	struct delay_info
	{
		uint8_t StartFlag;		//??????
		uint8_t Success;			//????,?????
		uint16_t Time;			//????
		uint16_t TimeCount;		//????
	};

	void ConfigDelayModuleNum(uint8_t num, uint8_t timer);
	uint8_t GetDelayIdFunction(uint8_t num, uint8_t *return_id);
	char SetCountTime(uint8_t id_number, uint16_t time);
	char StartDelayCount(uint8_t id_number);
	char StopDelayCount(uint8_t id_number);
	char ReturnSuccess(uint8_t id_number);
	char ClearDelaySucFlag(uint8_t id_number);
	unsigned int ReturnCurCount(uint8_t id_number);
	
#endif
