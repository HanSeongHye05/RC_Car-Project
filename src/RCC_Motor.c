RCC_Motor.c

#include "RCC_Motor.h"
#include "tim.h"

uint8_t Motor_Setspeed = 100; // 높을수록 유턴하는 속도가 up!

extern TIM_HandleTypeDef htim2;


// === 모터 초기화 ===
void Motor_Init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};


    GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;   // 출력 모드
    GPIO_InitStruct.Pull = GPIO_NOPULL;           // 풀업/풀다운 없음
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(MOTOR_GPIO, &GPIO_InitStruct);

    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);     //왼쪽 모터
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);     //오른쪽 모터



    Motor_Stop();
}

// === 정지 ===
void Motor_Stop(void)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5,RESET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6,RESET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7,RESET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8,RESET);
}

// === 전진 ===
void Motor_Forward(void)
{
   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5,RESET);
   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6,SET);
   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7,RESET);
   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8,SET);

   uint8_t duty = (htim2.Init.Period + 1) * Motor_Setspeed / 100;

   __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, duty);
   __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, duty);

}

// === 후진 ===
void Motor_Backward(void)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5,SET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6,RESET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7,SET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8,RESET);

	uint8_t duty = (htim2.Init.Period + 1) * Motor_Setspeed / 100;

	 __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, duty);
	 __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, duty);
}

// === 왼쪽 ===
void Motor_Left(void)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5,SET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6,RESET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7,RESET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8,SET);

 uint8_t duty = (htim2.Init.Period + 1) * Motor_Setspeed / 100;

 __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, duty);
 __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, duty);

}

// === 오른쪽 ===
void Motor_Right(void)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5,RESET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6,SET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7,SET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8,RESET);


 uint8_t duty = (htim2.Init.Period + 1) * Motor_Setspeed / 100;

 __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, duty);
 __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, duty);

}




void Motor_SetSpeed(uint8_t speed)
{
    if (speed > 100) speed = 100;

    Motor_Setspeed = speed;
}


