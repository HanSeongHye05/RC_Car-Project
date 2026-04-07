#include "motor.h"
#include "tim.h"

#define MOTOR_GPIO GPIOA
#define MOTOR_L1 GPIO_PIN_5
#define MOTOR_L2 GPIO_PIN_6
#define MOTOR_R1 GPIO_PIN_7
#define MOTOR_R2 GPIO_PIN_8

extern TIM_HandleTypeDef htim2;

static uint8_t motor_speed = 100;

void Motor_Init(void)
{
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
    Motor_Stop();
}

void Motor_SetSpeed(uint8_t speed)
{
    if (speed > 100) speed = 100;
    motor_speed = speed;
}

static void Motor_ApplyPWM(void)
{
    uint32_t duty = (htim2.Init.Period + 1) * motor_speed / 100;
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, duty);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, duty);
}

void Motor_Forward(void)
{
    HAL_GPIO_WritePin(MOTOR_GPIO, MOTOR_L1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR_GPIO, MOTOR_L2, GPIO_PIN_SET);
    HAL_GPIO_WritePin(MOTOR_GPIO, MOTOR_R1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR_GPIO, MOTOR_R2, GPIO_PIN_SET);
    Motor_ApplyPWM();
}

void Motor_Backward(void)
{
    HAL_GPIO_WritePin(MOTOR_GPIO, MOTOR_L1, GPIO_PIN_SET);
    HAL_GPIO_WritePin(MOTOR_GPIO, MOTOR_L2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR_GPIO, MOTOR_R1, GPIO_PIN_SET);
    HAL_GPIO_WritePin(MOTOR_GPIO, MOTOR_R2, GPIO_PIN_RESET);
    Motor_ApplyPWM();
}

void Motor_Left(void)
{
    HAL_GPIO_WritePin(MOTOR_GPIO, MOTOR_L1, GPIO_PIN_SET);
    HAL_GPIO_WritePin(MOTOR_GPIO, MOTOR_L2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR_GPIO, MOTOR_R1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR_GPIO, MOTOR_R2, GPIO_PIN_SET);
    Motor_ApplyPWM();
}

void Motor_Right(void)
{
    HAL_GPIO_WritePin(MOTOR_GPIO, MOTOR_L1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR_GPIO, MOTOR_L2, GPIO_PIN_SET);
    HAL_GPIO_WritePin(MOTOR_GPIO, MOTOR_R1, GPIO_PIN_SET);
    HAL_GPIO_WritePin(MOTOR_GPIO, MOTOR_R2, GPIO_PIN_RESET);
    Motor_ApplyPWM();
}

void Motor_Stop(void)
{
    HAL_GPIO_WritePin(MOTOR_GPIO,
        MOTOR_L1 | MOTOR_L2 | MOTOR_R1 | MOTOR_R2,
        GPIO_PIN_RESET);
}
