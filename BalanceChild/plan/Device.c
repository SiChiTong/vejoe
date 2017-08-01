/******************************************************************************
* @file    	device.c
* @author  	Houzuping.
* @version 	V0.0.1
* @date    	2016-09-6
******************************************************************************/

#include "Device.h"

/*
system tick module.
*/
#ifdef USE_SYSTICK

struct sysTick_Info _sysTick_info;
/*
config system clock
*/
void Config_SysTick(UINT32 fcy)
{
	RCC_ClocksTypeDef Tick_RCC_Clocks;
	//clock enable.
	if(_sysTick_info._isInit) return;
	_sysTick_info._frequency = fcy;
                          
	RCC_GetClocksFreq(&Tick_RCC_Clocks);
	SysTick_Config(Tick_RCC_Clocks.HCLK_Frequency / fcy); //
  
	_sysTick_info._isInit = TRUE;
}
/*

*/
void SysTick_Register(CB_VOID function)
{
	UINT8 i;
	if(function == 0) return;
	if(_sysTick_info._callback_Num == CALLBACK_NUM) return;

	for (i = 0; i < _sysTick_info._callback_Num; i++)
	{
		if(_sysTick_info._callback[i] == function) return;	
	}

	_sysTick_info._callback[_sysTick_info._callback_Num] = function;
	_sysTick_info._callback_Num++;
}

/*

*/
struct sysTick_Info *Get_SysTickInfo(void)
{	
  return &_sysTick_info;
}
/*
interrupt function.all callback function use timer not > 1ms
*/
void SysTick_Handler(void)
{
    UINT8 i = _sysTick_info._callback_Num;
    while(i > 0)
    {
        i--;
        _sysTick_info._callback[i]();
    }
}

#endif

//----------------------------timer module------------------------------------//
#ifdef USE_TIMER
struct timer_Info _timer_info[TIMER_NUM];
/*
timer:TIM2~TIM11
fcy : system coclk
pri : timer interrupt 优先级
fry : timer interrupt cycle.
*/
void Config_TIMER(UINT8 timer, UINT8 pri, UINT32 fry)
{
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure; 
	TIM_TimeBaseInitTypeDef  timer_struct;
	RCC_ClocksTypeDef RCC_Clocks;
	
	UINT32 CCR1_Val = 0;
	
	if(timer < TIMER_NUM)
	{
		if(_timer_info[timer]._isInit == 1)  //说明已经初始化
		return;
		else
		_timer_info[timer]._isInit = 1;  
	}
	else
	return;
  /* Configure one bit for preemption priority */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
	if(timer <= TIMER_4)
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn + timer;  // interrupt channel.
	#ifdef STM32F10X_XL
	else if(timer == TIMER_5)
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
	else if(timer == TIMER_7)
	NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
	#endif
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);  
	//get system clock.
	RCC_GetClocksFreq(&RCC_Clocks); 
	//timer config.
	_timer_info[timer]._frequency = fry ;
	timer_struct.TIM_Period = 1000000 / _timer_info[timer]._frequency  - 1; //
	timer_struct.TIM_Prescaler = (RCC_Clocks.HCLK_Frequency / 1000000) - 1;   //
	timer_struct.TIM_ClockDivision = 0x0;  	                                //
	timer_struct.TIM_CounterMode = TIM_CounterMode_Up; 	                    //
	//oc  
	// CCR1_Val = fry >> 1;                                   //中断周期 / 2.
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Inactive;                     //
	TIM_OCInitStructure.TIM_Pulse = CCR1_Val;  
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;                 //
  
	switch(timer)
	{
	  case TIMER_2: /* TIM clock enable */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
		TIM_TimeBaseInit(TIM2, &timer_struct);
		
		TIM_OC1Init(TIM2, &TIM_OCInitStructure);
		TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Disable);	//
		TIM_ARRPreloadConfig(TIM2, ENABLE);
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC1| TIM_IT_Update); 
		TIM_ITConfig(TIM2, TIM_IT_CC1 | TIM_IT_Update, ENABLE);
		TIM_Cmd(TIM2, ENABLE); 
		break;
		
	  case TIMER_3:
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
		TIM_TimeBaseInit(TIM3, &timer_struct);
		
		TIM_OC1Init(TIM3, &TIM_OCInitStructure);
		TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Disable);	//
		TIM_ARRPreloadConfig(TIM3, ENABLE);
		TIM_ClearITPendingBit(TIM3, TIM_IT_CC1| TIM_IT_Update); 
		TIM_ITConfig(TIM3, TIM_IT_CC1 | TIM_IT_Update, ENABLE);
		TIM_Cmd(TIM3, ENABLE); 
		break;
		
	  case TIMER_4:
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
		TIM_TimeBaseInit(TIM4, &timer_struct);
		
		TIM_OC1Init(TIM4, &TIM_OCInitStructure);
		TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Disable);	//
		TIM_ARRPreloadConfig(TIM4, ENABLE);
		TIM_ClearITPendingBit(TIM4, TIM_IT_CC1| TIM_IT_Update); 
		TIM_ITConfig(TIM4, TIM_IT_CC1 | TIM_IT_Update, ENABLE);
		TIM_Cmd(TIM4, ENABLE); 
		break;
	#ifdef STM32F10X_XL		
	case TIMER_5:
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
		TIM_TimeBaseInit(TIM5, &timer_struct);
		
		TIM_OC1Init(TIM5, &TIM_OCInitStructure);
		TIM_OC1PreloadConfig(TIM5, TIM_OCPreload_Disable);	//
		TIM_ARRPreloadConfig(TIM5, ENABLE);
		TIM_ClearITPendingBit(TIM5, TIM_IT_CC1| TIM_IT_Update); 
		TIM_ITConfig(TIM5, TIM_IT_CC1 | TIM_IT_Update, ENABLE);
		TIM_Cmd(TIM5, ENABLE); 
		break;	
		
	case TIMER_7:
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
		TIM_TimeBaseInit(TIM7, &timer_struct);
		
		TIM_OC1Init(TIM7, &TIM_OCInitStructure);
		TIM_OC1PreloadConfig(TIM7, TIM_OCPreload_Disable);	//
		TIM_ARRPreloadConfig(TIM7, ENABLE);
		TIM_ClearITPendingBit(TIM7, TIM_IT_CC1| TIM_IT_Update); 
		TIM_ITConfig(TIM7, TIM_IT_CC1 | TIM_IT_Update, ENABLE);
		TIM_Cmd(TIM7, ENABLE); 
		break;
	#endif
	}
}
/*

*/
void Timer_SetPrn(uint8_t timer, uint16_t pr)
{
	TIM_SetAutoreload(TIM4, pr);
}
/*
get timer info.
*/
struct timer_Info *Get_TimerInfo(UINT8 timer)
{
    return &_timer_info[timer];
}
/*

*/
void Timer_Register(UINT8 timer, CB_VOID function)
{
	UINT8 i;
	if(function == 0) return;
	if(_timer_info[timer]._callback_Num == CALLBACK_NUM) return;

	for (i = 0; i < _timer_info[timer]._callback_Num; i++)
	{
		if(_timer_info[timer]._callback[i] == function) return;	
	}

	_timer_info[timer]._callback[_timer_info[timer]._callback_Num] = function;
	_timer_info[timer]._callback_Num++;
}
/*
timer2 interrupt function.
*/
void TIM2_IRQHandler(void)
{
	UINT8 i = 0;
	if (TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);
	}  
	else if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update); 
		i = _timer_info[TIMER_2]._callback_Num;
		while(i > 0)
		{
			i--;
			_timer_info[TIMER_2]._callback[i]();
		}      
	}
}
/*
timer3 interrupt function.
*/
/*
void TIM3_IRQHandler(void)
{
	UINT8 i = 0;
	if (TIM_GetITStatus(TIM3, TIM_IT_CC1) != RESET)
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);
	}  
	else if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update); 
		i = _timer_info[TIMER_3]._callback_Num;
		while(i > 0)
		{
			i--;
			_timer_info[TIMER_3]._callback[i]();
		}      
	}
}*/
/*
timer5 interrupt function.
*/
 void TIM4_IRQHandler(void)
 {
 	UINT8 i = 0;
 	if (TIM_GetITStatus(TIM4, TIM_IT_CC1) != RESET)
 	{
 		TIM_ClearITPendingBit(TIM4, TIM_IT_CC1);
 	}  
 	else if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
 	{
 		TIM_ClearITPendingBit(TIM4, TIM_IT_Update); 
 		i = _timer_info[TIMER_4]._callback_Num;
 		while(i > 0)
 		{
 			i--;
 			_timer_info[TIMER_4]._callback[i]();
 		}      
 	}
 }
/*
timer5 interrupt function.
*/
void TIM5_IRQHandler(void)
{
	UINT8 i = 0;
	if (TIM_GetITStatus(TIM5, TIM_IT_CC1) != RESET)
	{
		TIM_ClearITPendingBit(TIM5, TIM_IT_CC1);
	}  
	else if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM5, TIM_IT_Update); 
		i = _timer_info[TIMER_5]._callback_Num;
		while(i > 0)
		{
			i--;
			_timer_info[TIMER_5]._callback[i]();
		}      
	}
}
/*
timer7 interrupt function.
*/
void TIM7_IRQHandler(void)
{
	UINT8 i = 0;
	if (TIM_GetITStatus(TIM7, TIM_IT_CC1) != RESET)
	{
		TIM_ClearITPendingBit(TIM7, TIM_IT_CC1);
	}  
	else if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM7, TIM_IT_Update); 
		i = _timer_info[TIMER_7]._callback_Num;
		while(i > 0)
		{
			i--;
			_timer_info[TIMER_7]._callback[i]();
		}      
	}
}
#endif

//----------------------------uart module----------------------------//
#ifdef USE_UART


struct uart_Info _uart_info[UART_NUM];
/*
inti uart1.
*/
void Config_UART(UINT8 uart, UINT32 baudrate, UINT8 pri)
{
    UINT8 index = 0;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;    
    
    if(uart < UART_NUM)
    {
    if(_uart_info[uart]._isInit == 1)  //说明已经初始化
    return;
    else
    _uart_info[uart]._isInit = 1;  
    }
    else
    return;
    //init uart buf.
    _uart_info[uart]._baudrate = baudrate;
    _uart_info[uart]._rx_wr_pointer = _uart_info[uart]._rx_buf;
    _uart_info[uart]._rx_rd_pointer = _uart_info[uart]._rx_buf;
    _uart_info[uart]._tx_wr_pointer = _uart_info[uart]._tx_buf;
    _uart_info[uart]._tx_rd_pointer = _uart_info[uart]._tx_buf;    
    for(index = 0; index < UART_RT_BUF_SIZE; index ++)
    {
        _uart_info[uart]._rx_buf[index] = 0; 
        _uart_info[uart]._tx_buf[index] = 0; 
    }   
    //-------------------------------------------------------------//
    USART_InitStructure.USART_BaudRate = baudrate;      //115200
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
      /* Enable the USARTx Interrupt */
	if(uart <= UART_3)
    NVIC_InitStructure.NVIC_IRQChannel = (USART1_IRQn + uart);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
	
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;    
  	/* NVIC configuration */
    NVIC_Init(&NVIC_InitStructure);
    //io
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed= GPIO_Speed_2MHz;

    switch(uart)
    {
        case UART_1:
            /* Enable UART clock */
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
            /* Connect PXx to USARTx_Tx */  
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
            GPIO_Init(GPIOA, &GPIO_InitStructure);
		
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
            GPIO_Init(GPIOA, &GPIO_InitStructure); 	
            /* USART configuration */
            USART_Init(USART1, &USART_InitStructure);
            USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
            /* Enable USART */
            USART_Cmd(USART1, ENABLE);          
            break;
          
        case UART_2:
            /* Enable UART clock */
            RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
            /* Connect PXx to USARTx_Tx */ 
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; 
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
            GPIO_Init(GPIOA, &GPIO_InitStructure); 
		
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; 
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
            GPIO_Init(GPIOA, &GPIO_InitStructure); 
            /* USART configuration */
            USART_Init(USART2, &USART_InitStructure);
            USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
            /* Enable USART */
            USART_Cmd(USART2, ENABLE);            
            break;
          
        case UART_3:
            /* Enable UART clock */
            RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
            /* Connect PXx to USARTx_Tx */ 
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
            GPIO_Init(GPIOB, &GPIO_InitStructure);  

			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; 
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
            GPIO_Init(GPIOB, &GPIO_InitStructure); 		
            /* USART configuration */
            USART_Init(USART3, &USART_InitStructure);
            USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
            /* Enable USART */
            USART_Cmd(USART3, ENABLE);            
            break;
    }
}
/*
register callback function.
*/
void Register_UartReceCallback(UINT8 uart, HANDLER01 _callback)
{
  _uart_info[uart]._callback = _callback;
}
/*
write data to buf
*/
UINT8 Wr_UART_RXBuf(UINT8 uart, UINT16 data)
{
  if(uart > UART_NUM)return 0;
	if((_uart_info[uart]._rx_rd_pointer <= _uart_info[uart]._rx_wr_pointer) ||
       ((_uart_info[uart]._rx_rd_pointer - _uart_info[uart]._rx_wr_pointer) > 1))
	{
    *_uart_info[uart]._rx_wr_pointer = data;   //写数据到发送缓冲区
		_uart_info[uart]._rx_wr_pointer ++; 
		_uart_info[uart]._rx_wr_index ++;
		if(_uart_info[uart]._rx_wr_index >= UART_RT_BUF_SIZE)
		{	
      _uart_info[uart]._rx_wr_index = 0;
			_uart_info[uart]._rx_wr_pointer = _uart_info[uart]._rx_buf;
		}
	}
	else//说明只有一个buf空余	
	return 0;/*认为缓冲区已近满了*/
	return 1;	
}
/*

*/
UINT8 Rd_UART_RXBuf(UINT8 uart, UINT16 *data)
{
	UINT16 _back_data = 0;
	if(_uart_info[uart]._rx_rd_pointer != _uart_info[uart]._rx_wr_pointer)
	{
		_back_data = *_uart_info[uart]._rx_rd_pointer;
		_uart_info[uart]._rx_rd_pointer ++; 
		_uart_info[uart]._rx_rd_index ++;
		if(_uart_info[uart]._rx_rd_index >= UART_RT_BUF_SIZE)
		{
			_uart_info[uart]._rx_rd_index = 0; 
			_uart_info[uart]._rx_rd_pointer = _uart_info[uart]._rx_buf;
		}
		*data = _back_data;
		return 1;
	}
	return 0;    
}

/*
write data to buf
*/
UINT8 Wr_UART_TXBuf(UINT8 uart, UINT16 data)
{
  if(uart > UART_NUM)return 0;
	if((_uart_info[uart]._tx_rd_pointer <= _uart_info[uart]._tx_wr_pointer) ||
       ((_uart_info[uart]._tx_rd_pointer - _uart_info[uart]._tx_wr_pointer) > 1))
	{
	  *_uart_info[uart]._tx_wr_pointer = data;   //写数据到发送缓冲区
		_uart_info[uart]._tx_wr_pointer ++; 
		_uart_info[uart]._tx_wr_index ++;
		if(_uart_info[uart]._tx_wr_index >= UART_RT_BUF_SIZE)
		{	
      _uart_info[uart]._tx_wr_index = 0;
			_uart_info[uart]._tx_wr_pointer = _uart_info[uart]._tx_buf;
		}
	}
	else//说明只有一个buf空余	
	return 0;/*认为缓冲区已近满了*/
	return 1;	
}
/*

*/
UINT8 Rd_UART_TXBuf(UINT8 uart, UINT16 *data)
{
	UINT16 _back_data = 0;
	if(_uart_info[uart]._tx_rd_pointer != _uart_info[uart]._tx_wr_pointer)
	{
		_back_data = *_uart_info[uart]._tx_rd_pointer;
		_uart_info[uart]._tx_rd_pointer ++; 
		_uart_info[uart]._tx_rd_index ++;
		if(_uart_info[uart]._tx_rd_index >= UART_RT_BUF_SIZE)
		{
			_uart_info[uart]._tx_rd_index = 0; 
			_uart_info[uart]._tx_rd_pointer = _uart_info[uart]._tx_buf;
		}
		*data = _back_data;
		return 1;
	}
	return 0;    
}
/*

*/
void Enable_UartRX(USART_TypeDef* uart_id)
{
  USART_ITConfig(uart_id, USART_IT_RXNE, ENABLE);
}
/*

*/
void Disable_UartRX(USART_TypeDef* uart_id)
{
  USART_ITConfig(uart_id, USART_IT_RXNE, DISABLE);
}
/*
enable uart TX.
*/
void Enable_UartTX(USART_TypeDef* uart_id)
{
  USART_ITConfig(uart_id, USART_IT_TXE, ENABLE);
}

/*
disable uart RX.
*/
void Disable_UartTX(USART_TypeDef* uart_id)
{
  USART_ITConfig(uart_id, USART_IT_TXE, DISABLE);
}

/*
uart send data.
*/
UINT8 UARTS_SendData(UINT8 uart, UINT16 uart_data)
{
  UINT16 temp = 0;
  temp = Wr_UART_TXBuf(uart, uart_data);
  if(temp)
  {
    switch(uart)
    {
        case UART_1: Enable_UartTX(USART1); break;              
        case UART_2: Enable_UartTX(USART2); break;  
        case UART_3: Enable_UartTX(USART3); break;   
        default: return 0;
    }
  }
  else      //缓冲区满
  return 0;
  return 1; //放入缓冲区成功
}

/*
uart for back data.
*/
UINT8 UARTS_ReadData(UINT8 uart, UINT16 *uart_data)
{
  UINT8 index = 0;
  index = Rd_UART_RXBuf(uart, uart_data);
  return index;
}
/*

*/
struct uart_Info *Get_UartInfo(UINT8 uart)
{
    return &_uart_info[uart];
}
/*
uart1 interrupt.
*/
void USART1_IRQHandler(void)
{
  UINT8  temp = 0;
  UINT16 uart_rece_buf = 0, uart_send_buf = 0;
  if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  
  {
     USART_ClearITPendingBit(USART1, USART_IT_RXNE);
     uart_rece_buf = USART_ReceiveData(USART1);
     Wr_UART_RXBuf(UART_1, uart_rece_buf);//放入串口接受buf
		 if(_uart_info[UART_1]._callback != 0)
      _uart_info[UART_1]._callback();
  }
  else if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
  {
     temp = Rd_UART_TXBuf(UART_1, &uart_send_buf);//从发送Buf中取数据
     if(temp)
     {
        USART_SendData(USART1, uart_send_buf);  
        while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); 
     }
     else
     Disable_UartTX(USART1);  //disable uart tx.
  }
}
/*
uart3 interrupt.
*/
void USART2_IRQHandler(void)
{
  UINT8  temp = 0;
  UINT16 uart_rece_buf = 0, uart_send_buf = 0;
  if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  
  {
     USART_ClearITPendingBit(USART2, USART_IT_RXNE);
     uart_rece_buf = USART_ReceiveData(USART2);
     Wr_UART_RXBuf(UART_2, uart_rece_buf);          //放入串口接受buf
		 if(_uart_info[UART_2]._callback != 0)
     _uart_info[UART_2]._callback();
  }
  else if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)
  {
     temp = Rd_UART_TXBuf(UART_2, &uart_send_buf);  //从发送Buf中取数据
     if(temp)
     {
        USART_SendData(USART2, uart_send_buf);  
        while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET); 
     }
     else
     Disable_UartTX(USART2);  //disable uart tx.
  }
}
/*
uart3 interrupt.
*/
void USART3_IRQHandler(void)
{
  UINT8  temp = 0;
  UINT16 uart_rece_buf = 0, uart_send_buf = 0;
  if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  
  {
     USART_ClearITPendingBit(USART3, USART_IT_RXNE);
     uart_rece_buf = USART_ReceiveData(USART3);
     Wr_UART_RXBuf(UART_3, uart_rece_buf);//放入串口接受buf
     if(_uart_info[UART_3]._callback != 0)
      _uart_info[UART_3]._callback();
  }
  else if(USART_GetITStatus(USART3, USART_IT_TXE) != RESET)
  {
     temp = Rd_UART_TXBuf(UART_3, &uart_send_buf);//从发送Buf中取数据
     if(temp)
     {
        USART_SendData(USART3, uart_send_buf);  
        while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET); 
     }
     else
     Disable_UartTX(USART3);  //disable uart tx.
  }
}

#endif
//-----------------------------end of module-----------------------------//

//-----------------------------pwm module--------------------------------//
#ifdef USE_PWM

UINT16 g_pwm_resolution = 0;
/*

*/
void Set_PWM1Pulse(UINT16 pulse)
{
	TIM1->CCR1 = pulse;
	//占空比有效
}

/*

*/
void Set_PWM2Pulse(UINT16 pulse)
{
	TIM1->CCR2 = pulse;
}
/*

*/
void Init_ComplementaryPWM(UINT32 pwm_fcy)
{
	//dath time control set.
	TIM_BDTRInitTypeDef  TIM_BDTRInitStructure;
	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	//PA8, PA9
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//PB13, PB14
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	//Enable Timer1 clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	TIM_DeInit(TIM1);
	
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 4000000 / pwm_fcy - 1; //frc
	g_pwm_resolution = TIM_TimeBaseStructure.TIM_Period;
	TIM_TimeBaseStructure.TIM_Prescaler = 18 - 1;  //预分频1Mhz
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x00;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
 
   /* PWM1 Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_OutputState  = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_Pulse = (4000000 / pwm_fcy - 1) >> 1;
	//
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;	 //Low
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OCInitStructure.TIM_OCNIdleState= TIM_OCIdleState_Set;
	//需要不同的PWM通道设置不同的占空比
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_Pulse = (4000000 / pwm_fcy - 1) >> 2;	
	TIM_OC2Init(TIM1, &TIM_OCInitStructure);	
	//死区时间段配置
	TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Disable;//TIM_OSSRState_Enable;
	TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Disable;//TIM_OSSIState_Enable;
	//
	TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_OFF;
	TIM_BDTRInitStructure.TIM_DeadTime = 0xa0;  //死区时间
	TIM_BDTRInitStructure.TIM_Break = TIM_Break_Disable;  //break disable刹车功能disable.
	TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;  //
	TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
	
	TIM_BDTRConfig(TIM1, &TIM_BDTRInitStructure);
	
	//ENABLE
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);	
	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
	
	//timer1  enable.
	TIM_ARRPreloadConfig(TIM1, ENABLE);
	/* TIM11 enable counter */
	TIM_Cmd(TIM1, ENABLE);
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
}

/*

*/
UINT16 Get_PWMResolution(void)
{
	return g_pwm_resolution;
}
/*

*/
#endif
/*
end of module.
*/
#ifdef USE_QEI
	
struct Qei_info g_qei_info;
/*
init qei interface encode user timer id.
*/
void Init_QEIModule(UINT16 resolution, UINT16 init_postion)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	//使用外部中断来判断方向 
	EXTI_InitTypeDef EXTI_InitStructure;
	//
	g_qei_info.resolution = resolution;
	//enable clk.
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	//set io port.
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	//timer4 interrupt
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
	//EXTI 7,
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);	
	//
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource7);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line7;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	//TIMER4  encode input capture module.
	TIM_DeInit(TIM4);
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_Period = 4 * g_qei_info.resolution - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	//encode interface.
	TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM_ICStructInit(&TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_ICFilter = 6;
	TIM_ICInit(TIM4, &TIM_ICInitStructure);
	//clear interface flag.	
	TIM_ClearFlag(TIM4, TIM_FLAG_Update);
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	
	TIM4->CNT = init_postion;
	g_qei_info.postion_reg= init_postion;
	g_qei_info.resolution = 4 * g_qei_info.resolution - 1;
	
	TIM_Cmd(TIM4, ENABLE);
}

/*

*/
void Get_CurPositionAngle(float *angle)
{
	float temp;
	temp = (s32)(TIM_GetCounter(TIM4)) * (s32)(360 / (4 * g_qei_info.resolution));
	*angle = temp;
	g_qei_info.angle = temp;
}

/*

*/
UINT16 Get_QEICounter(void)
{
	g_qei_info.postion_reg = TIM4->CNT;
	return g_qei_info.postion_reg;
}
/*
timer4 interrupt function.
*/
// void TIM4_IRQHandler(void)
// { 
// 	UINT16 temp = TIM_GetCounter(TIM4) & 0xffff;
// 	//每次中断产生一次，则说明电机转动一转
// 	if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
// 	{
// 		TIM_ClearITPendingBit(TIM4, TIM_IT_Update); 
// 		if(temp == g_qei_info.resolution)
// 		{
// 			if(g_qei_info.dir)
// 			{
// 				if(g_qei_info.cycle > 0)
// 				g_qei_info.cycle --;
// 			}
// 		}
// 		else if(temp == 0)
// 		{
// 			if(g_qei_info.dir == 0)
// 			{
// 				g_qei_info.cycle ++;
// 			}
// 		}
// 	}
// }

/*

*/
void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line7) != RESET)
	{
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6))
			g_qei_info.dir = 0;		//
		else
			g_qei_info.dir = 1;    	//为正为负
		
		EXTI_ClearITPendingBit(EXTI_Line7);
	}
}
/*

*/
UINT8 Get_QEIDirect(void)
{
	return g_qei_info.dir;
}
#endif

/*****************************************************************************/
#ifdef USE_ADC

struct adc_info _adc_info[ADC_NUM];

UINT16 Weight_Filter(UINT8 filter_bit, u16 *order_value);
UINT16 Average_Filter(UINT8 filter_bit, u16 *order_value);
void Move_Array(UINT8 array_bit, u16 *array);
/*
config adc.
*/
void Config_ADC(UINT8 adc, UINT16 channel, HANDLER08 _callback)
{
	ADC_InitTypeDef   ADC_InitStructure;
	GPIO_InitTypeDef  ADC_GPIO_InitStructure;
	_adc_info[HardWare_ADC1].callback = _callback;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;       //单通道与多通道采样转换模式
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel= 6;
	//--------------------------select channel---------------------------//
	/* Route the GPIO */  
	ADC_GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	ADC_GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	/* adc 1, 2  pin configuration */
	if(adc >= HardWare_ADC2)return;
	
	ADC_GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6|
	                                  GPIO_Pin_7; 			
	GPIO_Init(GPIOA, &ADC_GPIO_InitStructure);
	//
	ADC_GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_Init(GPIOC, &ADC_GPIO_InitStructure);	
	//	
	ADC_Init(ADC1, &ADC_InitStructure); 	
	//set sample cycle.
	ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 1, ADC_SampleTime_239Cycles5);	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 2, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 3, ADC_SampleTime_239Cycles5);	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 4, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 5, ADC_SampleTime_239Cycles5);	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_14,6, ADC_SampleTime_239Cycles5);	
	//ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 3, ADC_SampleTime_239Cycles5);	
	/* Enable ADC1 */
	ADC_DMAConfig();
	
	ADC_DMACmd(ADC1, ENABLE);
	ADC_Cmd(ADC1, ENABLE);
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	//START
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
	//start adc convert.
//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

/*
DMA config.
*/
u16 ADC_DataBuf[6];
DMA_InitTypeDef DMA_InitStructure;

void ADC_DMAConfig(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	//enable clock.
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr =(u32)( &(ADC1->DR));		//ADC1数据寄存器
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)ADC_DataBuf;			//获取ADC的数组
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;					//片内外设作源头
	DMA_InitStructure.DMA_BufferSize = 6;								//每次DMA16个数据
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	//外设地址不增加
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;				//内存地址增加
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	//半字
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;			//半字
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;						//普通模式循环
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;					//高优先级
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;						//非内存到内存
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	
	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);						//DMA通道1传输完成中断
	
	/* Enable DMA1 channel1 */
	DMA_Cmd(DMA1_Channel1, ENABLE);
}

/*
DMA channel sample interrupt.
*/
void DMA1_Channel1_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC1))
	{
		DMA_ClearITPendingBit(DMA1_IT_GL1);
		for(UINT8 index = 0; index < 6; index ++)
		_adc_info[HardWare_ADC1].adc_value[index] = ADC_DataBuf[index];
	}
}
/*

*/
uint16_t Filter_ADValue[6][16];

void Get_ADCSampleValue(UINT8 channel, UINT16 *value)
{
	if(channel == ADC_Channel_3)
	{
		Move_Array(16, Filter_ADValue[0]);
		Filter_ADValue[0][15] = _adc_info[HardWare_ADC1].adc_value[0];
		*value = Weight_Filter(16, Filter_ADValue[0]);
	}
	else if(channel == ADC_Channel_4)
	{
		Move_Array(16, Filter_ADValue[1]);
		Filter_ADValue[1][15] = _adc_info[HardWare_ADC1].adc_value[1];
		*value = Weight_Filter(16, Filter_ADValue[1]);
	}
	else if(channel == ADC_Channel_5)
	{
		Move_Array(16, Filter_ADValue[2]);
		Filter_ADValue[2][15] = _adc_info[HardWare_ADC1].adc_value[2];
		*value = Weight_Filter(16, Filter_ADValue[2]);
	}
	else if(channel == ADC_Channel_6)
	{
		Move_Array(16, Filter_ADValue[3]);
		Filter_ADValue[3][15] = _adc_info[HardWare_ADC1].adc_value[3];
		*value = Weight_Filter(16, Filter_ADValue[3]);
	}
	else if(channel == ADC_Channel_7)
	{
		Move_Array(16, Filter_ADValue[4]);
		Filter_ADValue[4][15] = _adc_info[HardWare_ADC1].adc_value[4];
		*value = Weight_Filter(16, Filter_ADValue[4]);
	}
	else if(channel == ADC_Channel_14)
	{
		Move_Array(16, Filter_ADValue[5]);
		Filter_ADValue[5][15] = _adc_info[HardWare_ADC1].adc_value[5];
		*value = Weight_Filter(16, Filter_ADValue[5]);
	}
}

/*
adc interrupt.
*/
void ADC1_2_IRQHandler(void)
{	
	UINT16 adc_value = 0;
	//ADC1
	if(ADC_GetITStatus(ADC1, ADC_IT_EOC) != RESET)  
	{ 
		adc_value = ADC_GetConversionValue(ADC1);
		_adc_info[HardWare_ADC1].callback(adc_value);
		ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);  
	}
	//ADC2
	if(ADC_GetITStatus(ADC2, ADC_IT_EOC) != RESET)  
	{ 
		adc_value = ADC_GetConversionValue(ADC2);
		ADC_ClearITPendingBit(ADC2, ADC_IT_EOC); 
		_adc_info[HardWare_ADC2].callback(adc_value);
	}
}

/*
权值滤波
*/
UINT16 Weight_Filter(UINT8 filter_bit, u16 *order_value)
{
	UINT8 index = 0;
	UINT16 sum_check = 0;
	for(index = 0; index < filter_bit; index ++)
	{
		if(&(order_value[index]) != 0)
		sum_check += order_value[index] >> (filter_bit - index);
	}
	return sum_check;
}
/*
average filter.
*/
UINT16 Average_Filter(UINT8 filter_bit, u16 *order_value)
{
	UINT8 index = 0;
	UINT32 sum_check = 0;
	for(index = 0; index < filter_bit; index ++)
	{
		if(&(order_value[index]) != 0)
		sum_check += order_value[index];
	}
	sum_check = sum_check / filter_bit;

	return sum_check;	
}
/*

*/
void Move_Array(UINT8 array_bit, u16 *array)
{
	UINT8 index = 0;
    for(index = 0; index < (array_bit - 1); index ++)
    {
		if((&(array[index]) != 0) && (&(array[index + 1]) != 0))
        array[index] = array[index + 1];
    }
}
#endif

/**************************************************************************/
#ifdef USE_SPI

void Config_SPI(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;
	//SPI interface.
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	//---------------------------------------------//
	NVIC_InitStructure.NVIC_IRQChannel = SPI1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	//---------------------------------------------//
	
	/* Enable the SPI periph */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);  

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	/* SPI SCK pin configuration */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	// NSS - GPIO(OUT) (NSS cannot be set by MCU automatically, it can only be set by software using GPIO)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA, GPIO_Pin_4);
	//GPIO_ResetBits(GPIOA, GPIO_Pin_4);	     //Default Off, Active Low      
	/* SPI configuration -------------------------------------------------------*/
	SPI_I2S_DeInit(SPI1);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;            //Sys Clk /2
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;//SPI_FirstBit_MSB
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;

	SPI_Init(SPI1, &SPI_InitStructure);
	SPI_Cmd(SPI1, ENABLE);
}

/*
spi send data.
*/
uint8_t g_spi_cs_flag = 0;
uint16_t g_spi_send_buf = 0;
void SPI1_SendData(uint16_t data)
{
	g_spi_send_buf = (data << 2)&0x0ffc;
	//此处函数用于DAC IC中, 需要移动两位
	GPIO_ResetBits(GPIOA, GPIO_Pin_4);
	SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_TXE, ENABLE);
}
/*
启动一个定时器函数，用于关闭SPI CS.1ms
*/
void SPI_SetNSS(void)
{
	if(g_spi_cs_flag)
	{
		g_spi_cs_flag = 0;
		GPIO_SetBits(GPIOA, GPIO_Pin_4);
	}
}
/*
spi1 interrupt.
*/
void SPI1_IRQHandler(void)
{
	if(SPI_I2S_GetITStatus(SPI1, SPI_I2S_IT_TXE) != RESET)
	{
		SPI_I2S_ClearITPendingBit(SPI1, SPI_I2S_IT_TXE);
		SPI_I2S_SendData(SPI1, g_spi_send_buf);
		g_spi_cs_flag = 1;
		SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_TXE, DISABLE);
	}
}

/*
spi2 interrupt.
*/
void SPI2_IRQHandler(void)
{
  	if(SPI_I2S_GetITStatus(SPI2, SPI_I2S_IT_TXE) != RESET)
	{
		SPI_I2S_SendData(SPI2, g_spi_send_buf);
		SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, DISABLE);
	}
}

/**************************************************************************/
#endif
//-------------------------------------------------------------------------//
//CAN bus control.
#ifdef 	USE_CAN

CANMsgInfo* _transmit;
CANMsgInfo* _receive;
CanTxMsg    TxMessage;
/**
  * @brief  Configures the CAN.
  * @param  None
  * @retval None
  */
void CAN_Config(CAN_TypeDef* can_id, UINT32 fcy, UINT32 brd)
{
	UINT32 temp = 0;
	GPIO_InitTypeDef 	GPIO_InitStructure;
	CAN_InitTypeDef  	CAN_InitStructure;
	NVIC_InitTypeDef 	NVIC_InitStructure;
	
	if(can_id != CAN1) return;
	//enable clock.
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
	//enable interrupt, 后续用DMA
	NVIC_InitStructure.NVIC_IRQChannel = USB_HP_CAN1_TX_IRQn;	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;    
	// NVIC configuration
	NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);
	
	// NVIC configuration
	NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX1_IRQn;	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);	
	//-------------------------------------------------//
	//enable gpio clock.
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		            
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//INIT CAN                              
	CAN_DeInit(CAN1);  
	CAN_StructInit(&CAN_InitStructure);
	//CAN cell init   
	CAN_InitStructure.CAN_TTCM = DISABLE;   	//时间出发还需要验证怎么触发
	CAN_InitStructure.CAN_ABOM = DISABLE;  
	CAN_InitStructure.CAN_AWUM = DISABLE;   
	CAN_InitStructure.CAN_NART = DISABLE;   
	CAN_InitStructure.CAN_RFLM = DISABLE;   
	CAN_InitStructure.CAN_TXFP = ENABLE;   
	CAN_InitStructure.CAN_Mode = CAN_Mode_LoopBack;//CAN_Mode_Normal;
	//
	//brd = fcy / ((can_prescaler + 1) * (CAN_SJW + CAN_BS1 + CAN_BS2 + 2));
	temp = (fcy / brd) / (CAN_BS1_6tq + CAN_BS2_8tq + 3) - 1;
	CAN_InitStructure.CAN_SJW  = CAN_SJW_1tq;  	//设置波特率
	CAN_InitStructure.CAN_BS1  = CAN_BS1_6tq; 	//tBS1=6个tCAN时钟周期  
	CAN_InitStructure.CAN_BS2  = CAN_BS2_8tq; 	//tBS2=8个tCAN时钟周期  
	CAN_InitStructure.CAN_Prescaler = temp; 	//ABP总线时钟时钟4分频

	CAN_Init(CAN1, &CAN_InitStructure);
	CAN_InterruptCtl(CAN1);
}
/*
can filter.
*/
void _CAN_SetFilter(UINT8 filter_num, UINT32 filter, UINT32 maskSel, UINT8 fifo_num)
{
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;  
 
    if(filter_num > 14)filter_num = 0;
	CAN_FilterInitStructure.CAN_FilterNumber = filter_num;      					//设置过滤器组0，范围为0~13  
	CAN_FilterInitStructure.CAN_FilterMode   = CAN_FilterMode_IdMask;    			//设置过滤器组0为屏蔽模式  
	CAN_FilterInitStructure.CAN_FilterScale  = CAN_FilterScale_32bit;   			//设置过滤器组0位宽为32位   
	//ext_id<<3对齐，见上图9，再>>16取高16位  
	CAN_FilterInitStructure.CAN_FilterIdHigh = ((filter << 3) >> 16) & 0xffff;  	//设置标识符寄存器高字节。  
	CAN_FilterInitStructure.CAN_FilterIdLow  = (UINT16)(filter << 3) | CAN_ID_EXT;  	//设置标识符寄存器低字节   
	//屏蔽寄存器的设置	
	//maskSel <<= 3;																	//再整体左移3位  
	maskSel |= 0x02; 																//只接收数据帧，不接收远程帧  
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = (maskSel >> 16) & 0xffff; 		//设置屏蔽寄存器高字节  
	CAN_FilterInitStructure.CAN_FilterMaskIdLow  = maskSel & 0xffff;   				//设置屏蔽寄存器低字节  
	if(fifo_num == CAN_FIFO0) 
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO0;  					//此过滤器组关联到接收FIFO0  
	else
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO1;
    
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE; 							//激活此过滤器组  
	CAN_FilterInit(&CAN_FilterInitStructure); 										//设置过滤器 
}
/*
enable interrupt.
*/
void CAN_InterruptCtl(CAN_TypeDef* CANx)
{
	CAN_ClearFlag(CANx, CAN_FLAG_FMP0 | CAN_FLAG_FF0 | CAN_FLAG_FOV0 | CAN_FLAG_FMP1 | CAN_FLAG_FF1 | CAN_FLAG_FOV1);
	CAN_ClearITPendingBit(CANx, CAN_IT_TME | CAN_IT_FMP0 | CAN_IT_FF0 | CAN_IT_FOV0);
    CAN_ITConfig(CANx,  CAN_IT_FMP0, ENABLE);
}
/*

*/
CB_VOID _CAN_callback = 0;
void CAN_Reset_CANMsg(CANMsgInfo* info)
{
	info->_sender = 0;
	info->_receiver = 0;
	info->_recType = 0;
	info->_transType = 0;
	info->_packageHead = 0;

	info->_buffer = 0;
	info->_isBusy = FALSE;

	info->_data[0]._DWord = 0;
	info->_data[1]._DWord = 0;
}
/*

*/
void CAN_SetData(CANMsgInfo* transmit, CANMsgInfo* receive)
{
	_transmit = transmit;
	_receive  = receive;
}
/*

*/
void CAN_Register(CB_VOID callback)
{
	_CAN_callback = callback;
}
/*

*/	
void CAN_Set_TransData(CAN_TypeDef* CANx, UINT32 l_32, UINT32 h_32)
{
	UINT8 index = 0;
	union _CANData msgID;
    msgID._Byte[3] = _transmit->_packageHead;
	msgID._Byte[2] = _transmit->_transType;
	msgID._Byte[1] = _transmit->_receiver;
	msgID._Byte[0] = _transmit->_sender;
	TxMessage.ExtId = msgID._DWord & 0x1FFFFFFF; 	
    TxMessage.IDE = CAN_ID_EXT;   					//设置IDE为扩展CAN ID    
	TxMessage.RTR = CAN_RTR_DATA; 					//数据帧  
	TxMessage.DLC = 8;  							//数据长度 
	 
	//存放8 byte.	
    for(index = 0; index < 8; index ++) 
	{  
		if(index < 4)
	    TxMessage.Data[index] = l_32 >> (8 * index);
        else
        TxMessage.Data[index] = h_32 >> (8 * (index - 4));   
	}
}
/*

*/
BOOL CAN_TransmitDataPage(CAN_TypeDef* CANx)
{
	//记录发报文的邮箱  
	_transmit->_mailbox = CAN_Transmit(CANx, &TxMessage); 
    return 1;
}
/*
transmit success.
*/
BOOL CAN_Transmit_Success(CAN_TypeDef* CANx, uint8_t TransmitMailbox)
{
	UINT8 temp = 0;
	temp = CAN_TransmitStatus(CANx, TransmitMailbox);
	if(temp == CAN_TxStatus_Ok)
	return TRUE;  //CAN_TXMAILBOX_0
	else
	return FALSE;
}

/*
cancel transmit.
*/
void CAN_Cancel_Transmit(CAN_TypeDef* CANx, uint8_t Mailbox)
{
	CAN_CancelTransmit(CANx, Mailbox);		//CAN_TXMAILBOX_0
}

/*
get new msg.
*/
void _CAN_GetMsg(CAN_TypeDef* CANx, UINT8 fifo_number)
{
	UINT8 index = 0;
	union _CANData identifier;
	CanRxMsg  RxMessage;
	if(fifo_number == CAN_FIFO0)
    CAN_Receive(CANx, CAN_FIFO0, &RxMessage);  //共有两个缓冲队列,与关联相关
	else
    CAN_Receive(CANx, CAN_FIFO1, &RxMessage);
    
	identifier._DWord = RxMessage.ExtId;  
	_receive->_recType = identifier._Byte[2];
	_receive->_receiver= identifier._Byte[1];
	_receive->_sender  = identifier._Byte[0];
	_receive->_packageHead = (identifier._Byte[3] & 0x01);

	for(index = 0; index < 8; index ++)
	{
		if(index < 4)
		_receive->_data[0]._Byte[index] = RxMessage.Data[index];
        else
		_receive->_data[1]._Byte[index] = RxMessage.Data[index];	
	}
}

/*
can interrupt.
*/
void USB_LP_CAN1_RX0_IRQHandler(void)
{
	if(CAN_GetITStatus(CAN1, CAN_IT_FMP0) == SET)
    {
		CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
        _CAN_GetMsg(CAN1, CAN_FIFO0);
        if(_CAN_callback != 0)
        _CAN_callback();
    }
    else if(CAN_GetITStatus(CAN1, CAN_IT_FF0) == SET)
    {
		CAN_ClearITPendingBit(CAN1, CAN_IT_FF0);
        _CAN_GetMsg(CAN1, CAN_FIFO0);
        if(_CAN_callback != 0)
        _CAN_callback();
    }
    else if(CAN_GetITStatus(CAN1, CAN_IT_FOV0) == SET)
    {
		CAN_ClearITPendingBit(CAN1, CAN_IT_FOV0);
        _CAN_GetMsg(CAN1, CAN_FIFO0);
        if(_CAN_callback != 0)
        _CAN_callback();
    }
}
/*

*/
void CAN1_RX1_IRQHandler(void)
{
	if(CAN_GetITStatus(CAN1, CAN_IT_FMP1) == SET)
    {
		CAN_ClearITPendingBit(CAN1, CAN_IT_FMP1);
        _CAN_GetMsg(CAN1, CAN_FIFO1);
        if(_CAN_callback != 0)
        _CAN_callback();
    }
    else if(CAN_GetITStatus(CAN1, CAN_IT_FF1) == SET)
    {
		CAN_ClearITPendingBit(CAN1, CAN_IT_FF1);
        _CAN_GetMsg(CAN1, CAN_FIFO1);
        if(_CAN_callback != 0)
        _CAN_callback();
    }
    else if(CAN_GetITStatus(CAN1, CAN_IT_FOV1) == SET)
    {
		CAN_ClearITPendingBit(CAN1, CAN_IT_FF1);
        _CAN_GetMsg(CAN1, CAN_FIFO1);
        if(_CAN_callback != 0)
        _CAN_callback();
    }	
}

#endif

/*
IIC user.
*/

#ifdef IIC_USED

/*
iic configuration.
*/
void I2C_Configuration(uint32_t i2c_clk)
{
	I2C_InitTypeDef 	I2C_InitStructure;
	GPIO_InitTypeDef 	GPIO_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	//pb6 pb7 pb8.
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	//configuration iic.
	I2C_DeInit(I2C1);
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = 0x30; 	//?address.
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	if(i2c_clk < 10000)
	I2C_InitStructure.I2C_ClockSpeed = 400000; //400k.	
	else
	I2C_InitStructure.I2C_ClockSpeed = i2c_clk; //400k.
	
	//i2c init.
	I2C_Cmd(I2C1, ENABLE);
	I2C_Init(I2C1, &I2C_InitStructure);
	//ACK ENABLE.
	I2C_AcknowledgeConfig(I2C1, ENABLE);
}

/*
staic read iic state.
*/
static void I2C_AcknowledgePolling(I2C_TypeDef *I2CX, uint8_t I2C_Addr, uint8_t *state)
{
	uint16_t temp = 0;
	I2C_GenerateSTART(I2CX, ENABLE);
	temp = I2C_ReadRegister(I2CX, I2C_Register_SR1);
	I2C_Send7bitAddress(I2CX, 0, I2C_Direction_Transmitter);
	
	I2C_ClearFlag(I2CX, I2C_FLAG_AF);
	I2C_GenerateSTOP(I2CX, ENABLE);
	*state = temp;
}

/*
I2C READ
*/
uint8_t I2C_Read(I2C_TypeDef* I2CX, uint8_t addr, uint8_t *buf, uint8_t longth)
{
	if(buf == NULL)return 0;
	//----------------------//
	if(I2C_GetFlagStatus(I2CX, I2C_FLAG_BUSY) == 0)
	return 0;
	I2C_AcknowledgeConfig(I2CX, ENABLE);
	I2C_GenerateSTART(I2CX, ENABLE);
	while(!I2C_CheckEvent(I2CX, I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(I2CX, addr, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2CX, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	I2C_SendData(I2CX, addr);
	while(!I2C_CheckEvent(I2CX, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	I2C_GenerateSTART(I2CX, ENABLE);
	while(!I2C_CheckEvent(I2CX, I2C_EVENT_MASTER_MODE_SELECT));
	
	I2C_Send7bitAddress(I2CX, addr, I2C_Direction_Receiver);
	while(!I2C_CheckEvent(I2CX, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	//
	for(uint8_t index = 0; index < longth; index ++)
	{
		while(!I2C_CheckEvent(I2CX, I2C_EVENT_MASTER_BYTE_RECEIVED));
		*buf = I2C_ReceiveData(I2CX);
	}
	I2C_GenerateSTOP(I2CX, ENABLE);
	return 1;
}
/*
i2c write one byte.
*/
uint8_t I2C_WriteOneByte(I2C_TypeDef *I2CX, uint8_t addr, uint8_t value)
{
	uint8_t temp = 0;
	//i2c start.
	I2C_GenerateSTART(I2CX, ENABLE);
	if(I2C_CheckEvent(I2CX, I2C_EVENT_MASTER_MODE_SELECT) == 0)return 0;
	
	I2C_Send7bitAddress(I2CX, 0x00, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2CX, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	I2C_SendData(I2CX, addr);
	while(!I2C_CheckEvent(I2CX, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	I2C_SendData(I2CX, value);
	while(!I2C_CheckEvent(I2CX, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	I2C_GenerateSTOP(I2CX, ENABLE);
	I2C_AcknowledgePolling(I2CX, 0x00, &temp);
	return temp;
}
/*
write more byte.
*/
uint8_t I2C_Write(I2C_TypeDef* I2CX, uint8_t addr, uint8_t *buf, uint16_t longth)
{
	uint8_t index = 0;
	if((buf == NULL) || (longth == 0))return 0;
	for(index = 0; index < longth; index ++)
	{
		if(I2C_WriteOneByte(I2CX, addr ++, *buf ++) == 0)
		return 0;
	}
	return 1;
}

//---------------------------IIC IO----------------------------------//

/*
config I2C port.
*/
void Config_IOI2C(HANDLER02 Clk_Port, HANDLER02 Sda_Port)
{
	
}
#endif


/*
end of module.
*/
//-----------------------------------POWER MANAGE-----------------------------//
/*

*/


