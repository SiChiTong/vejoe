#ifndef __MOTOR_H
#define __MOTOR_H
#include <sys.h>	 

#define PI			3.1415926
#define PWMA   	TIM1->CCR1 //PA8
#define AIN2   	PCout(9)
#define AIN1   	PCout(8)

#define BIN1  	PAout(15)
#define BIN2    PCout(12)
#define PWMB   	TIM1->CCR4  //PA11

void MiniBalance_PWM_Init(u16 arr,u16 psc);
void MiniBalance_Motor_Init(void);
#endif
