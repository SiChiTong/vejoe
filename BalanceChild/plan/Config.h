/****************************************************
file     : device.h  for breo.
author   : kernel_007@sina.cn
copyright: 2013.9.05~2015.9.05
*****************************************************/
#ifndef _CONFIG_H
#define _CONFIG_H

//-----------------------设置MCU--------------------------------------//
//-----1. 选择芯片系列
//#define USE_MICROCHIP_MCU
//#define USE_RENESES_MCU

//-----2. 选择芯片型号

	
//-----4. 选择是否使用看门狗
//#define USE_WATCH_DOG
//------------------------End OF 设置MCU-----------------------------------//

//-----------------------设置芯片功能模块--------------------------------------//
//#define USE_IO					//使用IO
#define USE_TIMER					//使用定时器
#define USE_PWM						//使用PWM
#define USE_UART					//使用串口通信定时器
#define USE_SPI						//使用SPI
#define USE_FLASH					//使用FLASH读写
#define USE_ADC				        //使用AD
//#define USE_I2C				    //使用I2C
//#define USE_EEPROM				//使用EEPROM
#define USE_CAN						//使用CAN总线
#define USE_SYSTICK                 //使用系统时钟
//#define USE_QEI						//QEI模块管理
//------------------------ End of 设置芯片功能模块-----------------------------//

//-----------------------设置Tools模块-------------------------=---------------//
#define USE_LED								//使用LED管理
//#define USE_BEEPER
#define USE_DEBOUNCE						//使用防抖
//#define USE_KEY_SCAN
//#define USE_KEY_EVENT
//#define USE_UNIPOLAR_STEP_DRIVER			//使用单极性步进电机控制驱动模块
//#define USE_STEPPER_TASK_MANAGER			//使用步进电机任务管理模块
#define USE_COMMUNICATION				    //使用通讯管理模块
#define USE_CANAPP							//使用CAN通信管理
#define USE_COBS							//使用COBS编码
//#define USE_UARTAPP						//使用串口通信管理
//#define USE_QEIAPP							//QEI模块
#define USE_DELAY							//延时模块
#define USE_DATA_BUF_QUEUE					//数据缓冲队列模块
//#define USE_KEYPAD						//使用键盘管理
//#define USE_PS2						    //使用PS2协议
//#define USE_ENCRYPTION					//使用加密解密算法
#define USE_RS485
//#define FSM_MANAGER_MODULE
#define  USER_CRC_DATA_CHECK
//#define DAC_TLV5620
//#define IIC_DEVICE_MODULE
//#define USE_AT24CXX
//#define USE_DS18B20
//#define  USE_SCHEDULING
//#define  USE_LCD_SSD1302
#define	 IIC_USED

//------------------------End of 设置Tools模块---------------------------------//

//--------------------数据类型定义---------------------------------------------//
typedef char					INT8;
typedef int  					INT16;
typedef long 					INT32;
typedef unsigned char			UINT8;
typedef unsigned int  			UINT16;
typedef unsigned long 			UINT32;

#define Int8                    INT8
#define Int16                   INT16
#define Int32                   INT32
#define Uint8                   UINT8
#define Uint16                  UINT16
#define Uint32                  UINT32

#ifndef BOOL
#define BOOL	                UINT8
#endif

//#define TRUE 	1
//#define FALSE 	0

#define ON 		1
#define OFF 	0
#define	NULL	0

typedef void (*CB_VOID)(void);
typedef void (*CB_SET)(BOOL state);
typedef BOOL (*CB_READ)(void);
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

#ifdef GLOBAL_EXT_
#define     GL_TYPE
#else
#define     GL_TYPE extern
#endif

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

//-----------------------内存管理函数---------------------------------------------
struct MY_MEM
{
	UINT8* _memBlock;
	UINT16 _memSize;
	UINT16 _memLeft;
	BOOL _memEmpty;
};

void CreatBuffer(UINT8* src, UINT16 len);
void *My_malloc(UINT16 len);
void My_memset(UINT8* src, UINT16 len, UINT8 value);
void My_free(void* p);
struct MY_MEM Get_Mem_Info(void);
//-----------------------End of 内存管理函数---------------------------------------

/* Exported macro ------------------------------------------------------------*/
#ifdef  USE_FULL_ASSERT
#include "stm32f10x.h"
/**
  * @brief  The assert_param macro is used for function's parameters check.
  * @param  expr: If expr is false, it calls assert_failed function
  *   which reports the name of the source file and the source
  *   line number of the call that failed. 
  *   If expr is true, it returns no value.
  * @retval None
  */
  #define assert_param(expr) ((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))
/* Exported functions ------------------------------------------------------- */
  void assert_failed(uint8_t* file, uint32_t line);
#else
  #define assert_param(expr) ((void)0)
#endif /* USE_FULL_ASSERT */

#endif   // - end of _CONFIG_H
