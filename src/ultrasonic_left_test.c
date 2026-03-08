#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "delay_us.h"
#include <stdio.h>

#define TRIG_PORT GPIOC
#define TRIG_PIN  GPIO_PIN_8
#define ECHO_TIMER &htim4
#define ECHO_CHANNEL TIM_CHANNEL_3

uint16_t IC_Value1 = 0;
uint16_t IC_Value2 = 0;
uint8_t captureFlag = 0;
uint16_t echoTime = 0;
uint16_t distance = 0;

void HCSR04_Trigger(void);

int _write(int file, unsigned char* p, int len)
{
    HAL_UART_Transmit(&huart2, p, len, 100);
    return len;
}

void HCSR04_Trigger(void)
{
    HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_RESET);
    delay_us(2);
    HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_SET);
    delay_us(10);
    HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_RESET);

    __HAL_TIM_ENABLE_IT(ECHO_TIMER, TIM_IT_CC3);
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM3 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3)
    {
        if (captureFlag == 0)
        {
            IC_Value1 = HAL_TIM_ReadCapturedValue(htim, ECHO_CHANNEL);
            captureFlag = 1;
        }
        else
        {
            IC_Value2 = HAL_TIM_ReadCapturedValue(htim, ECHO_CHANNEL);

            echoTime = (IC_Value2 > IC_Value1) ? IC_Value2 - IC_Value1 : (0xFFFF - IC_Value1) + IC_Value2;
            distance = echoTime / 58;

            captureFlag = 0;
        }
    }
}

int main(void)
{
    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();
    MX_USART2_UART_Init();
    MX_TIM3_Init();
    MX_TIM11_Init();

    HAL_TIM_Base_Start(&htim11);
    HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_3);

    while (1)
    {
        HCSR04_Trigger();
        HAL_Delay(200);
        printf("Left Distance: %d cm\r\n", distance);
    }
}
