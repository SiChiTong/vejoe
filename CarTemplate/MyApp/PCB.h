#ifndef PCB_H
#define PCB_H

#include "Device.h"

void IO_Init(void);

void Set_Led1(UINT8 state);
void Set_Led2(UINT8 state);
void Set_VoiceCtl(UINT8 state);

BOOL Get_SW1(void);
BOOL Get_SW2(void);
BOOL Get_SW3(void);
BOOL Get_SW4(void);

UINT16 Get_Voltage_Sample(void);
UINT16 Get_Current_Sample(void);
INT32 Get_Encoder(void);

#endif
