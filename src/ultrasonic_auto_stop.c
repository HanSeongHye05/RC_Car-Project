#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "delay_us.h"
#include "RCC_Motor.h"
#include <stdio.h>

#define TRIG_PORT GPIOB
#define TRIG_PIN  GPIO_PIN_9

uint16_t IC_Value1 = 0;
uint16_t IC_Value2 = 0;
uint16_t echoTime = 0;
uint8_t captureFlag = 0;
uint16_t distance = 0;

void SystemClock_Config(void);
void HCSR04_Trigger(void);

int _write(int file, unsigned char* p, int len)
{
    HAL_UART_Transmit(&huart2, p, len, 100);
    return len;
}

void HCSR04_Trigger(void)
{
    HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, 0);
    delay_us(2);
    HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, 1);
    delay_us(10);
    HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, 0);

    __HAL_TIM_ENABLE_IT(&htim10, TIM_IT_CC1);
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
    {
        if (captureFlag == 0)
        {
            IC_Value1 = HAL_TIM_ReadCapturedValue(&htim10, TIM_CHANNEL_1);
            captureFlag = 1;
            __HAL_TIM_SET_CAPTUREPOLARITY(&htim10, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);
        }
        else
        {
            IC_Value2 = HAL_TIM_ReadCapturedValue(&htim10, TIM_CHANNEL_1);
            __HAL_TIM_SET_CAPTUREPOLARITY(&htim10, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);

            echoTime = (IC_Value2 > IC_Value1) ? (IC_Value2 - IC_Value1) : (0xffff - IC_Value1) + IC_Value2;
            distance = echoTime / 58;

            captureFlag = 0;
            __HAL_TIM_DISABLE_IT(&htim10, TIM_IT_CC1);
        }
    }
}

int main(void)
{
    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();
    MX_USART2_UART_Init();
    MX_TIM2_Init();
    MX_TIM10_Init();
    MX_TIM11_Init();

    HAL_TIM_Base_Start(&htim11);
    HAL_TIM_IC_Start_IT(&htim10, TIM_CHANNEL_1);
    Motor_Init();

    while (1)
    {
        HCSR04_Trigger();
        HAL_Delay(200);

        if (distance > 10 || distance == 0)
            Motor_Forward();
        else
            Motor_Stop();

        HAL_Delay(200);
    }
}
