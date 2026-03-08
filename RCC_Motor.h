RCC_Motor.h


#ifndef INC_RCC_MOTOR_H_
#define INC_RCC_MOTOR_H_


#include "main.h"


//바퀴방향,핀번호
#define LEFT_BACK_PIN       GPIO_PIN
#define LEFT_FORWARD_PIN    GPIO_PIN
#define RIGHT_BACK_PIN      GPIO_PIN
#define RIGHT_FORWARD_PIN   GPIO_PIN

#define MOTOR_GPIO GPIOA



// 함수 선언
void Motor_Init(void);           //PWM설정

void Motor_Forward(void);            //직진해!
void Motor_Backward(void);           //후진해!
void Motor_Right(void);              //오른쪽!
void Motor_Left(void);               //왼쪽!
void Motor_Stop(void);               //멈춰!


void Motor_SetSpeed(uint8_t speed);


#endif /* INC_RCC_MOTOR_H_ */
