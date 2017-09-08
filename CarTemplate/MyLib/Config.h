#ifndef _CONFIG_H
#define _CONFIG_H

#include "stm32f10x.h"

//-----------------------设置芯片功能模块--------------------------------------//
#define USE_MEM_MANAGER					//使用内存管理功能
#define USE_GPIO								//使用GPIO
#define USE_SYS_TICK       			//使用系统时钟
#define USE_TIMER2_TICK					//使用定时器:(只用timer2,3,4)
//#define USE_TIMER3_TICK				//使用定时器:(只用timer2,3,4)
//#define USE_TIMER4_TICK				//使用定时器:(只用timer2,3,4)
#define USE_TIMER1_CPWM					//使用互补的PWM：用于电机桥式驱动（通常只有timer1,8可用）
//#define USE_TIMER8_CPWM				//使用互补的PWM：用于电机桥式驱动（通常只有timer1,8可用）
//#define USE_TIMER2_ENCODER			//使用Timer2作为encoder的用途
#define USE_TIMER4_ENCODER			//使用Timer4作为encoder的用途
#define USE_ADC				      	  //使用AD,固定使用DMA1， channel 11 中断

//#define USE_PWM						//使用PWM
//#define USE_UART					//使用串口通信定时器
//#define USE_SPI						//使用SPI
//#define USE_FLASH					//使用FLASH读写
//#define USE_I2C				    //使用I2C
//#define USE_EEPROM				//使用EEPROM
//#define USE_CAN						//使用CAN总线
//#define USE_QEI						//QEI模块管理
//------------------------ End of 设置芯片功能模块-----------------------------//

//-----------------------设置Tools模块-------------------------=---------------//
#define USE_LED													//使用LED管理
//#define USE_BEEPER
//#define USE_DEBOUNCE									//使用防抖
//#define USE_KEY_SCAN
#define USE_BDC_H_DRIVER								//有刷直流电机的H型桥驱动
#define USE_CURRENT_MANAGER							//电流管理模块
#define USE_ENCODER_MANAGER							//encoder管理模块
#define USE_FILTER											//滤波模块
#define USE_PID													//PID算法
#define USE_DCMOTOR_CONTROL							//直流电机运动控制模块

//#define USE_KEY_EVENT
//#define USE_UNIPOLAR_STEP_DRIVER			//使用单极性步进电机控制驱动模块
//#define USE_STEPPER_TASK_MANAGER			//使用步进电机任务管理模块
//#define USE_COMMUNICATION				    //使用通讯管理模块
//#define USE_CANAPP							//使用CAN通信管理
//#define USE_COBS							//使用COBS编码
//#define USE_UARTAPP						//使用串口通信管理
//#define USE_QEIAPP							//QEI模块
//#define USE_DELAY							//延时模块
//#define USE_DATA_BUF_QUEUE					//数据缓冲队列模块
//#define USE_KEYPAD						//使用键盘管理
//#define USE_PS2						    //使用PS2协议
//#define USE_ENCRYPTION					//使用加密解密算法
//#define USE_RS485
//#define FSM_MANAGER_MODULE
//#define  USER_CRC_DATA_CHECK
//#define DAC_TLV5620
//#define IIC_DEVICE_MODULE
//#define USE_AT24CXX
//#define USE_DS18B20
//#define  USE_SCHEDULING
//#define  USE_LCD_SSD1302
//------------------------End of 设置Tools模块---------------------------------//

#ifdef USE_GPIO
#include "stm32f10x_rcc.h"  
#include "stm32f10x_gpio.h"
#endif

#ifdef USE_SYSTICK
#include "stm32f10x_rcc.h"  
#endif

#if defined(USE_TIMER2_TICK) || defined(USE_TIMER3_TICK) || defined(USE_TIMER4_TICK) || \
		defined(USE_TIMER1_CPWM) || defined(USE_TIMER8_CPWM) || defined(USE_TIMER_TICK) || \
		defined(USE_TIMER24_ENCODER)
	#define USE_TIMER_TICK
#endif

#ifdef USE_TIMER_TICK
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"
#include "misc.h"
#endif

#ifdef USE_ADC
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_flash.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "misc.h"
#endif

//--------------------数据类型定义---------------------------------------------//
typedef signed char					INT8;
typedef unsigned char				UINT8;
typedef signed short int 		INT16;
typedef unsigned short int 	UINT16;
typedef signed int					INT32;
typedef unsigned int				UINT32;

#define Int8                    INT8
#define Int16                   INT16
#define Int32                   INT32
#define Uint8                   UINT8
#define Uint16                  UINT16
#define Uint32                  UINT32

#ifndef BOOL
#define BOOL	                UINT8
#endif

#define TRUE 		1
#define FALSE 	0

#define ON 		1
#define OFF 	0

typedef void (*CB_VOID)(void);
typedef void (*CB_SET)(BOOL state);
typedef BOOL (*CB_READ)(void);
typedef UINT16 (*CB_READ_U16)(void);
typedef INT32 (*CB_READ_32)(void);
typedef void (*HANDLER01)(void);
typedef void (*HANDLER02)(BOOL state);
typedef BOOL (*HANDLER03)(void);
typedef BOOL (*HANDLER04)(UINT16* p);
typedef void (*HANDLER05)(UINT8 *, UINT8 *, UINT8);
typedef void (*HANDLER06)(UINT16, UINT16, UINT16, UINT8);
typedef BOOL (*HANDLER07)(BOOL state);
typedef void (*HANDLER08)(UINT16);
typedef void (*HANDLER09)(UINT8*, UINT32);

typedef UINT8(*HANDLER10)(UINT8, UINT16);
typedef UINT8(*HANDLER11)(UINT8, UINT16*);
typedef void (*HANDLER12)(int, UINT8);
typedef void (*UART_READ)(UINT8 data);

#define LSB(a)	((unsigned char)(a))
#define MSB(a)	((unsigned char)(a>>8))
//----------------------End of 数据类型定义-------------------------------------//



//---------------------与位操作有关的函数定义--------------------------
#define L8(data, x)				((UINT8)(data) << (x))
#define L16(data, x)			((UINT16)(data) << (x))
#define L32(data, x)			((UINT32)(data) << (x))
#define R8(data, x)				((UINT8)(data) >> (x))
#define R16(data, x)			((UINT16)(data) >> (x))
#define R32(data, x)			((UINT32)(data) >> (x))

#define MASK8(pos, len)			L8(R8(0xFF, 8 - len), pos + 1 - len)
#define MASK16(pos, len)		L16(R16(0xFFFF, 16 - len), pos + 1 - len)
#define MASK32(pos, len)		L32(R32(0xFFFFFFFF, 32 - len), pos + 1 - len)

#define RMASK8(pos, len)		(~MASK8((pos), (len)))
#define RMASK16(pos, len)		(~MASK16((pos), (len)))
#define RMASK32(pos, len)		(~MASK32((pos), (len)))

#define GETDATA8(data, pos, len)	R8(data & MASK8(pos, len), pos + 1 - len)
#define GETDATA16(data, pos, len)	R16(data & MASK16(pos, len), pos + 1 - len)
#define GETDATA32(data, pos, len)	R32(data & MASK32(pos, len), pos + 1 - len)

#define SETDATA8(data1, pos, len, data2)	data1 = (((UINT8)(data1) & RMASK8(pos, len)) | L8(data2, (pos) + 1 - (len)))
#define SETDATA16(data1, pos, len, data2)	data1 = (((UINT16)(data1) & RMASK16(pos, len)) | L16(data2, (pos) + 1 - (len)))
#define SETDATA32(data1, pos, len, data2)	data1 = (((UINT32)(data1) & RMASK32(pos, len)) | L32(data2, (pos) + 1 - (len)))

#define GET_BIT8(data, pos)					(R8(data, pos) & 0x01)
#define GET_BIT16(data, pos)				(R16(data, pos) & 0x0001)
#define GET_BIT32(data, pos)				(R32(data, pos) & 0x00000001)
#define SET_BIT8(data, pos, value)			(data = SETDATA8(data, pos, 1, value))
#define SET_BIT16(data, pos, value)			(data = SETDATA16(data, pos, 1, value))
#define SET_BIT32(data, pos, value)			(data = SETDATA32(data, pos, 1, value))

#define SAVE8(data, value) 				data = (L8(data, 1) | (UINT8)(value))
#define SAVE16(data, value) 			data = (L16(data, 1) | (UINT16)(value))
#define SAVE32(data, value) 			data = (L32(data, 1) | (UINT32)(value))
#define LOAD8(data) 					((UINT8)(data) & 0x01)
#define LOAD16(data) 					((UINT16)(data) & 0x0001)
#define LOAD32(data) 					((UINT32)(data) & 0x00000001)

#define UP_TRIGGER8(data) 				((data & 0x03) == 0x01)
#define DOWN_TRIGGER8(data)				((data & 0x03) == 0x02)
#define TRIGGER8(data) 					(UP_TRIGGER8(data) || DOWN_TRIGGER8(data))
#define UP_TRIGGER16(data) 				(((data & 0x0003) == 0x0001))
#define DOWN_TRIGGER16(data)			(((data & 0x0003) == 0x0002))
#define TRIGGER16(data) 				(UP_TRIGGER16(data) || DOWN_TRIGGER16(data))
#define UP_TRIGGER32(data) 				((data & 0x00000003) == 0x00000001)
#define DOWN_TRIGGER32(data)			((data & 0x00000003) == 0x00000002)
#define TRIGGER32(data) 				(UP_TRIGGER32(data) || DOWN_TRIGGER32(data))	
//-----------------------End of 位操作-------------------------------------------

#endif   // - end of _CONFIG_H

