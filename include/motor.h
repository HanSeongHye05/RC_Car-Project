#ifndef INC_MOTOR_H_
#define INC_MOTOR_H_

#include "main.h"

void Motor_Init(void);
void Motor_Forward(void);
void Motor_Backward(void);
void Motor_Left(void);
void Motor_Right(void);
void Motor_Stop(void);
void Motor_SetSpeed(uint8_t speed);

#endif
