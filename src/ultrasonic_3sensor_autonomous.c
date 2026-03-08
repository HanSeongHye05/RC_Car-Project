#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "delay_us.h"
#include <stdio.h>

#define TRIG_FRONT_PIN GPIO_PIN_11
#define TRIG_LEFT_PIN  GPIO_PIN_8
#define TRIG_RIGHT_PIN GPIO_PIN_5

uint16_t distance_Front = 0;
uint16_t distance_Left  = 0;
uint16_t distance_Right = 0;

void Motor_Stop(void);
void Motor_Forward(void);

int _write(int file, unsigned char *p, int len)
{
    HAL_UART_Transmit(&huart2, p, len, 100);
    return len;
}

void Motor_Stop(void)
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8, GPIO_PIN_RESET);
}

void Motor_Forward(void)
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6 | GPIO_PIN_8, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5 | GPIO_PIN_7, GPIO_PIN_RESET);
}

int main(void)
{
    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();
    MX_USART2_UART_Init();
    MX_TIM4_Init();
    MX_TIM11_Init();

    HAL_TIM_Base_Start(&htim11);

    Motor_Forward();

    while (1)
    {
        printf("Left: %d cm | Front: %d cm | Right: %d cm\r\n",
               distance_Left, distance_Front, distance_Right);

        if (distance_Front > 0 && distance_Front <= 10)
        {
            printf("Front obstacle detected\r\n");
            Motor_Stop();
        }
        else
        {
            Motor_Forward();
        }

        HAL_Delay(400);
    }
}
