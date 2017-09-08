#include "PCB.h"

void IO_Init(void)
{
	SET_GPIO_OUTPUT01(GPIOB, GPIO_Pin_8);
	SET_GPIO_OUTPUT01(GPIOB, GPIO_Pin_9);
	SET_GPIO_OUTPUT01(GPIOA, GPIO_Pin_1);
	
	Set_Led1(OFF);
	Set_Led2(OFF);
	Set_VoiceCtl(OFF);
	
	SET_GPIO_INPUT01(GPIOC, GPIO_Pin_13);
	SET_GPIO_INPUT01(GPIOC, GPIO_Pin_14);
	SET_GPIO_INPUT01(GPIOC, GPIO_Pin_15);
	SET_GPIO_INPUT01(GPIOA, GPIO_Pin_0);
}

void Set_Led1(UINT8 state)
{
	if(state) GPIO_SetBits(GPIOB, GPIO_Pin_9);
	else			GPIO_ResetBits(GPIOB, GPIO_Pin_9);
}

void Set_Led2(UINT8 state)
{
	if(state) GPIO_SetBits(GPIOB, GPIO_Pin_8);
	else			GPIO_ResetBits(GPIOB, GPIO_Pin_8);
}

//·äÃùÆ÷¿ØÖÆ¶Ë¿Ú PA1¿Ú
void Set_VoiceCtl(UINT8 state)
{
	if(state) GPIO_ResetBits(GPIOA, GPIO_Pin_1);
	else			GPIO_SetBits(GPIOA, GPIO_Pin_1);
}

BOOL Get_SW1(void) {return !GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13);}
BOOL Get_SW2(void) {return !GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_14);}
BOOL Get_SW3(void) {return !GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_15);}
BOOL Get_SW4(void) {return !GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0);}

UINT16 Get_Voltage_Sample(void)
{
	return Get_ADC_Value(0);
}

UINT16 Get_Current_Sample(void)
{
	return Get_ADC_Value(1);
}

INT32 Get_Encoder(void)
{
	return Read_Encoder_TIM4();
}

