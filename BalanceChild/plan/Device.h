/******************************************************************************
* @file    	device.h STM32F1XX serils mcu device lib.
* @author  	Houzuping.
* @version 	V0.0.1
* @date    	2016-09-6
******************************************************************************/

#ifndef DEVICE_H
#define DEVICE_H

	#include "Config.h"
	//config device source.
	#include <stm32f10x.h>
	#include "stm32f10x_can.h"
	#include "stm32f10x_rcc.h"  
	#include "misc.h"
	#include "stm32f10x_gpio.h"
	#include "stm32f10x_adc.h"
	#include "stm32f10x_tim.h"
	#include "stm32f10x_flash.h"
	#include "stm32f10x_usart.h"
	#include "stm32f10x_dma.h"
	#include "stm32f10x_spi.h"
	#include "stm32f10x_exti.h"
	#include "stm32f10x_i2c.h"
	#include "stm32f10x_pwr.h"
	#include "stm32f10x_bkp.h"
/*
system clock module.
*/
#ifdef USE_SYSTICK
	//系统定时器最大常规任务数
	#define CALLBACK_NUM			15
	#define SYS_TIMER				0xff
	struct sysTick_Info
	{
		UINT32 _frequency;
		UINT8 _callback_Num;
		CB_VOID _callback[CALLBACK_NUM];
		BOOL _isInit;
	};
	
	void Config_SysTick(UINT32 frequency);   //配置系统时钟中断
	void SysTick_Register(CB_VOID function); //回调函数注册
	struct sysTick_Info *Get_SysTickInfo(void);
	//interrupt.
	void SysTick_Handler(void);              //中断函数

#endif  
/*
timer module.    
*/ 
#ifdef USE_TIMER
    
    #define  TIMER_CALLBACK_NUM   15
    
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
      BOOL    _isInit;
      UINT8   _callback_Num;
      UINT32  _frequency;
      CB_VOID _callback[TIMER_CALLBACK_NUM];
    };
    void Config_TIMER(UINT8 timer, UINT8 pri, UINT32 fry);
    void Timer_Register(UINT8 timer, CB_VOID function);
	void Timer_SetPrn(uint8_t timer, uint16_t pr);
    struct timer_Info *Get_TimerInfo(UINT8 timer);
    //interrupt.
	void TIM1_UP_IRQHandler(void);
    void TIM2_IRQHandler(void);              //中断函数    
    void TIM3_IRQHandler(void);              //中断函数    
   // void TIM4_IRQHandler(void);
	void TIM5_IRQHandler(void);
	void TIM7_IRQHandler(void);
	
#endif

/*
uart module.
*/  
#ifdef USE_UART

    //uart baudrate.
    #define     BAUD_9600       9600        //波特率为9600
    #define     BAUD_115200     115200      //为115200.

    #define     UART_RT_BUF_SIZE  250
    //UART ID  uart1 ~uart3
    enum
    {
      UART_1 = 0,
      UART_2,
      UART_3,
      UART_NUM
    };
    struct uart_Info
    {
        BOOL   _isInit;
        UINT8  _rx_rd_index;
		UINT8  _rx_wr_index;							//
        UINT8  _tx_wr_index;
		UINT8  _tx_rd_index;
		UINT8 *_rx_wr_pointer;						//接收缓冲区指针
        UINT8 *_rx_rd_pointer;	
        UINT8 *_tx_wr_pointer;          	//发送缓冲区指针
		UINT8 *_tx_rd_pointer;						//向缓冲区中取数据
		UINT8  _rx_buf[UART_RT_BUF_SIZE];	//接收队列buf大小
        UINT8  _tx_buf[UART_RT_BUF_SIZE]; //发送buf大小
        UINT32 _baudrate;
        HANDLER01 _callback; 
    };
    //函数声明区
    void Config_UART(UINT8 uart, UINT32 baudrate, UINT8 pri);	
    void Enable_UartRX(USART_TypeDef* uart_id);
    void Disable_UartRX(USART_TypeDef* uart_id);
    void Enable_UartTX(USART_TypeDef* uart_id);
    void Register_UartReceCallback(UINT8 uart, HANDLER01 _callback);
    void Disable_UartTX(USART_TypeDef* uart_id);
    UINT8 UARTS_SendData(UINT8 uart, UINT16 uart_data);
    UINT8 UARTS_ReadData(UINT8 uart, UINT16 *uart_data);
    struct uart_Info *Get_UartInfo(UINT8 uart);
    //interrupt;
    void USART1_IRQHandler(void);
    void USART2_IRQHandler(void);    
    void USART3_IRQHandler(void); 
    void USART6_IRQHandler(void);
#endif
/***************************************************************************/
#ifdef USE_PWM
    /*function extern.*/
    void Set_PWM1Pulse(UINT16 pulse);
	void Set_PWM2Pulse(UINT16 pulse);
    void Init_ComplementaryPWM(UINT32 pwm_fcy);
	UINT16 Get_PWMResolution(void);
	
#endif

/***************************************************************************/    
#ifdef USE_QEI

	//QEI module. QEI模块使用TIMER4  
	struct Qei_info
	{
		UINT8   dir;				//编码器脉冲方向
		UINT16 cur_position;	//当前位置
		UINT16 last_position;	//上一次位置
		UINT32 cycle;			//电机转动的圈数
		UINT32 resolution; 		//encode的分辨率
		UINT32 postion_reg;		//位置寄存器
		float  angle;			//角度
	};	
	
	//函数声明区
	void Init_QEIModule(UINT16 resolution, UINT16 init_postion);
	void Get_CurPositionAngle(float *angle);
	UINT16 Get_QEICounter(void);
	UINT8 Get_QEIDirect(void);
	void TIM4_IRQHandler(void);              //中断函数 
	void EXTI9_5_IRQHandler(void);
	
#endif
/***************************************************************************/
#ifdef USE_ADC
    //硬件ADC个数 
		enum
		{
			HardWare_ADC1 = 0,
			HardWare_ADC2,
			ADC_NUM
		};
		//channel
		enum
		{
			ADC_CHANNEL_IN0 = 0x0001,
			ADC_CHANNEL_IN1 = 0x0002,
			ADC_CHANNEL_IN2 = 0x0004,
			ADC_CHANNEL_IN3 = 0x0008,	
			ADC_CHANNEL_IN4 = 0x0010,
			ADC_CHANNEL_IN5 = 0x0020,
			ADC_CHANNEL_IN6 = 0x0040,
			ADC_CHANNEL_IN7 = 0x0080,
			ADC_CHANNEL_IN8 = 0x0100,
			ADC_CHANNEL_IN9 = 0x0200,
			ADC_CHANNEL_IN10 = 0x0400,
			ADC_CHANNEL_IN11 = 0x0800,	
			ADC_CHANNEL_IN12 = 0x1000,
			ADC_CHANNEL_IN13 = 0x2000,
			ADC_CHANNEL_IN14 = 0x4000,
			ADC_CHANNEL_IN15 = 0x8000,
			ADC_CHANNEL_NUM = 16
		};
		//struct 
		struct adc_info
		{
			UINT8 index;
			UINT16 adc_value[6];
			HANDLER08 callback;
		};
    void Config_ADC(UINT8 adc, UINT16 channel, HANDLER08 _callback);
    void ADC1_2_IRQHandler(void); 
    void Enable_ADC(UINT8 adc, BOOL state);
	void Set_SampleChannel(UINT8 adc, uint8_t ADC_Channel);
    void ADC_DMAConfig(void);
	void Get_ADCSampleValue(UINT8 channel, UINT16 *value);		
    
	//---------------------------------///
	UINT16 Weight_Filter(UINT8 filter_bit, u16 *order_value);
	UINT16 Average_Filter(UINT8 filter_bit, u16 *order_value);
	void Move_Array(UINT8 array_bit, u16 *array);	
#endif

/***************************************************************************/
#ifdef USE_SPI
    
	void Config_SPI(void);
	void SPI1_IRQHandler(void);
	void SPI2_IRQHandler(void);
	void SPI1_SendData(uint16_t data);
	void SPI_SetNSS(void);
			
#endif

/********************************** Can bus ********************************/    
#ifdef 	USE_CAN
	
	#define CAN_1_MODULE  	1
	#define CAN_2_MODULE  	2
	
	union _CANData
	{
		UINT32 _DWord;
		UINT16 _Word[2];
		UINT8 _Byte[4];
	};
	
	struct _CANMsgInfo
	{
		UINT8 _sender;
		UINT8 _receiver;
		UINT8 _recType;
		UINT8 _transType;
		UINT8 _packageHead;
		UINT8 _mailbox;
		UINT8 _buffer;
		BOOL  _isBusy;
		union _CANData _data[2];
	};
	typedef struct _CANMsgInfo CANMsgInfo;

	void CAN_Reset_CANMsg(CANMsgInfo* info);
    void CAN_Config(CAN_TypeDef* can_id, UINT32 fcy, UINT32 brd);
	void CAN_SetData(CANMsgInfo* transmit, CANMsgInfo* receive);
	void CAN_InterruptCtl(CAN_TypeDef* CANx);
    
	void CAN_Register(CB_VOID callback);
    void CAN_Set_TransData(CAN_TypeDef* CANx, UINT32 l_32, UINT32 h_32);
	BOOL CAN_TransmitDataPage(CAN_TypeDef* CANx);
	BOOL CAN_Transmit_Success(CAN_TypeDef* CANx, uint8_t TransmitMailbox);
    void CAN_Cancel_Transmit(CAN_TypeDef* CANx, uint8_t Mailbox); 
    void _CAN_GetMsg(CAN_TypeDef* CANx, UINT8 fifo_number);
    
    void _CAN_SetFilter(UINT8 filter_num, UINT32 filter, UINT32 maskSel, UINT8 fifo_num);
    
	//interrupt function.
	void USB_LP_CAN1_RX0_IRQHandler(void);
    void CAN1_RX1_IRQHandler(void);
#endif


#ifdef IIC_USED
	//用I2C 硬件模块
	void I2C_Configuration(uint32_t i2c_clk);
	static void I2C_AcknowledgePolling(I2C_TypeDef *I2CX, uint8_t I2C_Addr, uint8_t *state);
	uint8_t I2C_Read(I2C_TypeDef* I2CX, uint8_t addr, uint8_t *buf, uint8_t longth);
	uint8_t I2C_WriteOneByte(I2C_TypeDef *I2CX, uint8_t addr, uint8_t value);
	uint8_t I2C_Write(I2C_TypeDef* I2CX, uint8_t addr, uint8_t *buf, uint16_t longth);
	//用IO 模拟部分
	struct iic_info
	{
		uint8_t index;
		
		HANDLER02 clk_port;
		HANDLER01 sda_output;
		HANDLER01 sda_input;
		HANDLER02 set_sda;
		HANDLER03 read_sda;
	};
	//函数声明
	
	
#endif

/*

*/
#endif
/*
end of file.
*/

