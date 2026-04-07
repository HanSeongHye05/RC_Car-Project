#include "ultrasonic.h"
#include "tim.h"
#include "gpio.h"
#include "delay_us.h"

#define TRIG_PORT GPIOC
#define TRIG_PIN GPIO_PIN_11

#define ECHO_TIMER &htim4
#define ECHO_CHANNEL TIM_CHANNEL_2

static uint16_t IC_Val1 = 0, IC_Val2 = 0;
static uint8_t captureFlag = 0;
static uint32_t echoTime = 0;
static uint16_t distance = 0;

void HCSR04_Trigger(void)
{
    HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_RESET);
    delay_us(2);
    HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_SET);
    delay_us(10);
    HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_RESET);

    __HAL_TIM_ENABLE_IT(ECHO_TIMER, TIM_IT_CC2);
}

uint16_t Ultrasonic_GetDistance(void)
{
    return distance;
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM4 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
    {
        if (captureFlag == 0)
        {
            IC_Val1 = HAL_TIM_ReadCapturedValue(htim, ECHO_CHANNEL);
            captureFlag = 1;
            __HAL_TIM_SET_CAPTUREPOLARITY(htim, ECHO_CHANNEL, TIM_INPUTCHANNELPOLARITY_FALLING);
        }
        else
        {
            IC_Val2 = HAL_TIM_ReadCapturedValue(htim, ECHO_CHANNEL);
            __HAL_TIM_SET_CAPTUREPOLARITY(htim, ECHO_CHANNEL, TIM_INPUTCHANNELPOLARITY_RISING);

            echoTime = (IC_Val2 > IC_Val1) ? (IC_Val2 - IC_Val1)
                                          : (0xFFFF - IC_Val1 + IC_Val2);

            distance = echoTime / 58;

            captureFlag = 0;
            __HAL_TIM_DISABLE_IT(htim, TIM_IT_CC2);
        }
    }
}
