#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "delay_us.h"
#include <stdio.h>

#define TRIG_PORT GPIOC
#define TRIG_PIN  GPIO_PIN_11
#define ECHO_TIMER &htim4
#define ECHO_CHANNEL TIM_CHANNEL_2

uint16_t IC_Value1 = 0;
uint16_t IC_Value2 = 0;
uint8_t captureFlag = 0;
uint32_t echoTime = 0;
uint16_t distance = 0;

void HCSR04_Trigger(void);

int _write(int file, unsigned char *p, int len)
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

    __HAL_TIM_ENABLE_IT(ECHO_TIMER, TIM_IT_CC2);
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM4 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
    {
        if (captureFlag == 0)
        {
            IC_Value1 = HAL_TIM_ReadCapturedValue(htim, ECHO_CHANNEL);
            captureFlag = 1;
            __HAL_TIM_SET_CAPTUREPOLARITY(htim, ECHO_CHANNEL, TIM_INPUTCHANNELPOLARITY_FALLING);
        }
        else
        {
            IC_Value2 = HAL_TIM_ReadCapturedValue(htim, ECHO_CHANNEL);
            __HAL_TIM_SET_CAPTUREPOLARITY(htim, ECHO_CHANNEL, TIM_INPUTCHANNELPOLARITY_RISING);

            echoTime = (IC_Value2 > IC_Value1) ? IC_Value2 - IC_Value1 : (0xFFFF - IC_Value1) + IC_Value2;
            distance = echoTime / 58;

            captureFlag = 0;
            __HAL_TIM_DISABLE_IT(htim, TIM_IT_CC2);
        }
    }
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
    HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_2);

    while (1)
    {
        HCSR04_Trigger();
        HAL_Delay(200);
        printf("Front Distance: %d cm\r\n", distance);
    }
}
