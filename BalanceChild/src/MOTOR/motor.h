#ifndef __MOTOR_H
#define __MOTOR_H
#include <sys.h>	 

#define PWMA   TIM1->CCR4  
#define AIN2   PAout(15)
#define AIN1   PBout(5)
#define BIN1   PBout(4)
#define BIN2   PAout(12)
#define PWMB   TIM1->CCR1 
void MiniBalance_PWM_Init(u16 arr,u16 psc);
void MiniBalance_Motor_Init(void);
#endif
