#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "delay_us.h"
#include <stdio.h>
#include "swTimer.h"

/* === 핀 정의 === */
// 전방 센서
#define TRIG_FRONT_PORT GPIOC
#define TRIG_FRONT_PIN  GPIO_PIN_11
#define ECHO_FRONT_TIMER &htim4
#define ECHO_FRONT_CHANNEL TIM_CHANNEL_2   // PB7 (TIM4_CH2)

// 왼쪽 센서
#define TRIG_LEFT_PORT GPIOC
#define TRIG_LEFT_PIN  GPIO_PIN_8
#define ECHO_LEFT_TIMER &htim4
#define ECHO_LEFT_CHANNEL TIM_CHANNEL_3    // PB8 (TIM4_CH3)

// 오른쪽 센서
#define TRIG_RIGHT_PORT GPIOC
#define TRIG_RIGHT_PIN  GPIO_PIN_5
#define ECHO_RIGHT_TIMER &htim4
#define ECHO_RIGHT_CHANNEL TIM_CHANNEL_4   // PB9 (TIM4_CH4)

/* === 모터 핀 정의 (L298N 기준) === */
#define MOTOR_GPIO GPIOA
#define MOTOR_LEFT1  GPIO_PIN_5
#define MOTOR_LEFT2  GPIO_PIN_6
#define MOTOR_RIGHT1 GPIO_PIN_7
#define MOTOR_RIGHT2 GPIO_PIN_8

// PWM 핀 (속도 제어용)
#define PWM_LEFT_TIMER  &htim2
#define PWM_RIGHT_TIMER &htim2
#define PWM_LEFT_CHANNEL  TIM_CHANNEL_1   // ENA → PA0
#define PWM_RIGHT_CHANNEL TIM_CHANNEL_2   // ENB → PA1

/* === 전역 변수 === */
// 전방
uint16_t IC_Front1 = 0, IC_Front2 = 0;
uint8_t captureFlag_Front = 0;
uint32_t echoTime_Front = 0;
uint16_t distance_Front = 0;

// 왼쪽
uint16_t IC_Left1 = 0, IC_Left2 = 0;
uint8_t captureFlag_Left = 0;
uint32_t echoTime_Left = 0;
uint16_t distance_Left = 0;

// 오른쪽
uint16_t IC_Right1 = 0, IC_Right2 = 0;
uint8_t captureFlag_Right = 0;
uint32_t echoTime_Right = 0;
uint16_t distance_Right = 0;

/* === 함수 선언 === */
void SystemClock_Config(void);
void Error_Handler(void);
void HCSR04_Trigger_Front(void);
void HCSR04_Trigger_Left(void);
void HCSR04_Trigger_Right(void);
void Motor_Stop(void);
void Motor_Forward(void);
void Motor_Left(void);
void Motor_Right(void);
void Motor_SetSpeed(uint16_t left, uint16_t right);

/* === printf 리다이렉트 === */
int _write(int file, unsigned char *p, int len)
{
    HAL_UART_Transmit(&huart2, p, len, 100);
    return len;
}

/* === 초음파 트리거 === */
void HCSR04_Trigger_Front(void)
{
    HAL_GPIO_WritePin(TRIG_FRONT_PORT, TRIG_FRONT_PIN, GPIO_PIN_RESET);
    delay_us(2);
    HAL_GPIO_WritePin(TRIG_FRONT_PORT, TRIG_FRONT_PIN, GPIO_PIN_SET);
    delay_us(10);
    HAL_GPIO_WritePin(TRIG_FRONT_PORT, TRIG_FRONT_PIN, GPIO_PIN_RESET);
    __HAL_TIM_ENABLE_IT(ECHO_FRONT_TIMER, TIM_IT_CC2);
}

void HCSR04_Trigger_Left(void)
{
    HAL_GPIO_WritePin(TRIG_LEFT_PORT, TRIG_LEFT_PIN, GPIO_PIN_RESET);
    delay_us(2);
    HAL_GPIO_WritePin(TRIG_LEFT_PORT, TRIG_LEFT_PIN, GPIO_PIN_SET);
    delay_us(10);
    HAL_GPIO_WritePin(TRIG_LEFT_PORT, TRIG_LEFT_PIN, GPIO_PIN_RESET);
    __HAL_TIM_ENABLE_IT(ECHO_LEFT_TIMER, TIM_IT_CC3);
}

void HCSR04_Trigger_Right(void)
{
    HAL_GPIO_WritePin(TRIG_RIGHT_PORT, TRIG_RIGHT_PIN, GPIO_PIN_RESET);
    delay_us(2);
    HAL_GPIO_WritePin(TRIG_RIGHT_PORT, TRIG_RIGHT_PIN, GPIO_PIN_SET);
    delay_us(10);
    HAL_GPIO_WritePin(TRIG_RIGHT_PORT, TRIG_RIGHT_PIN, GPIO_PIN_RESET);
    __HAL_TIM_ENABLE_IT(ECHO_RIGHT_TIMER, TIM_IT_CC4);
}

/* === 모터 제어 === */
void Motor_SetSpeed(uint16_t left, uint16_t right)
{
    __HAL_TIM_SET_COMPARE(PWM_LEFT_TIMER, PWM_LEFT_CHANNEL, left);
    __HAL_TIM_SET_COMPARE(PWM_RIGHT_TIMER, PWM_RIGHT_CHANNEL, right);
}

void Motor_Forward(void)
{
    HAL_GPIO_WritePin(MOTOR_GPIO, MOTOR_LEFT1,  GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR_GPIO, MOTOR_LEFT2,  GPIO_PIN_SET);
    HAL_GPIO_WritePin(MOTOR_GPIO, MOTOR_RIGHT1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR_GPIO, MOTOR_RIGHT2, GPIO_PIN_SET);
}

void Motor_Left(void)
{
    HAL_GPIO_WritePin(MOTOR_GPIO, MOTOR_LEFT1,  GPIO_PIN_SET);
    HAL_GPIO_WritePin(MOTOR_GPIO, MOTOR_LEFT2,  GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR_GPIO, MOTOR_RIGHT1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR_GPIO, MOTOR_RIGHT2, GPIO_PIN_SET);
}

void Motor_Right(void)
{
    HAL_GPIO_WritePin(MOTOR_GPIO, MOTOR_LEFT1,  GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR_GPIO, MOTOR_LEFT2,  GPIO_PIN_SET);
    HAL_GPIO_WritePin(MOTOR_GPIO, MOTOR_RIGHT1, GPIO_PIN_SET);
    HAL_GPIO_WritePin(MOTOR_GPIO, MOTOR_RIGHT2, GPIO_PIN_RESET);
}

void Motor_Stop(void)
{
    HAL_GPIO_WritePin(MOTOR_GPIO, MOTOR_LEFT1,  GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR_GPIO, MOTOR_LEFT2,  GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR_GPIO, MOTOR_RIGHT1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR_GPIO, MOTOR_RIGHT2, GPIO_PIN_RESET);
    Motor_SetSpeed(0, 0);
}

/* === 초음파 거리 계산 콜백 === */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    // 전방
    if (htim->Instance == TIM4 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
    {
        if (captureFlag_Front == 0)
        {
            IC_Front1 = HAL_TIM_ReadCapturedValue(htim, ECHO_FRONT_CHANNEL);
            captureFlag_Front = 1;
            __HAL_TIM_SET_CAPTUREPOLARITY(htim, ECHO_FRONT_CHANNEL, TIM_INPUTCHANNELPOLARITY_FALLING);
        }
        else
        {
            IC_Front2 = HAL_TIM_ReadCapturedValue(htim, ECHO_FRONT_CHANNEL);
            __HAL_TIM_SET_CAPTUREPOLARITY(htim, ECHO_FRONT_CHANNEL, TIM_INPUTCHANNELPOLARITY_RISING);
            echoTime_Front = (IC_Front2 > IC_Front1) ? (IC_Front2 - IC_Front1) : (0xFFFF - IC_Front1 + IC_Front2);
            distance_Front = echoTime_Front / 58;
            captureFlag_Front = 0;
            __HAL_TIM_DISABLE_IT(htim, TIM_IT_CC2);
        }
    }

    // 왼쪽
    if (htim->Instance == TIM4 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3)
    {
        if (captureFlag_Left == 0)
        {
            IC_Left1 = HAL_TIM_ReadCapturedValue(htim, ECHO_LEFT_CHANNEL);
            captureFlag_Left = 1;
            __HAL_TIM_SET_CAPTUREPOLARITY(htim, ECHO_LEFT_CHANNEL, TIM_INPUTCHANNELPOLARITY_FALLING);
        }
        else
        {
            IC_Left2 = HAL_TIM_ReadCapturedValue(htim, ECHO_LEFT_CHANNEL);
            __HAL_TIM_SET_CAPTUREPOLARITY(htim, ECHO_LEFT_CHANNEL, TIM_INPUTCHANNELPOLARITY_RISING);
            echoTime_Left = (IC_Left2 > IC_Left1) ? (IC_Left2 - IC_Left1) : (0xFFFF - IC_Left1 + IC_Left2);
            distance_Left = echoTime_Left / 58;
            captureFlag_Left = 0;
            __HAL_TIM_DISABLE_IT(htim, TIM_IT_CC3);
        }
    }

    // 오른쪽
    if (htim->Instance == TIM4 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4)
    {
        if (captureFlag_Right == 0)
        {
            IC_Right1 = HAL_TIM_ReadCapturedValue(htim, ECHO_RIGHT_CHANNEL);
            captureFlag_Right = 1;
            __HAL_TIM_SET_CAPTUREPOLARITY(htim, ECHO_RIGHT_CHANNEL, TIM_INPUTCHANNELPOLARITY_FALLING);
        }
        else
        {
            IC_Right2 = HAL_TIM_ReadCapturedValue(htim, ECHO_RIGHT_CHANNEL);
            __HAL_TIM_SET_CAPTUREPOLARITY(htim, ECHO_RIGHT_CHANNEL, TIM_INPUTCHANNELPOLARITY_RISING);
            echoTime_Right = (IC_Right2 > IC_Right1) ? (IC_Right2 - IC_Right1) : (0xFFFF - IC_Right1 + IC_Right2);
            distance_Right = echoTime_Right / 58;
            captureFlag_Right = 0;
            __HAL_TIM_DISABLE_IT(htim, TIM_IT_CC4);
        }
    }
}

/* === MAIN === */
int main(void)
{
    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();
    MX_USART2_UART_Init();
    MX_TIM2_Init();
    MX_TIM4_Init();
    MX_TIM11_Init();

    HAL_TIM_Base_Start(&htim11);
    HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_2);
    HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_3);
    HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_4);

    HAL_TIM_PWM_Start(PWM_LEFT_TIMER, PWM_LEFT_CHANNEL);
    HAL_TIM_PWM_Start(PWM_RIGHT_TIMER, PWM_RIGHT_CHANNEL);

    printf("\r\n=== AUTO DRIVE: 3 DIRECTION DETECTION ===\r\n");

    Motor_SetSpeed(500, 500);
    Motor_Forward();

    softTimer_Init(swTimerID1, 500);


    while (1)
    {
        // 거리 측정
        HCSR04_Trigger_Front();

        HCSR04_Trigger_Left();

        HCSR04_Trigger_Right();

        if(softTimer_IsTimeOut(swTimerID1) == SET)
        {
        printf("L:%d | F:%d | R:%d cm\r\n", distance_Left, distance_Front, distance_Right);
          softTimer_Reset(swTimerID1);
        }




        // 전방 장애물 감지
        if (distance_Front > 0 && distance_Front <= 20)
        {
            Motor_Stop();
//            printf(" FRONT OBSTACLE DETECTED! Analyzing directions...\r\n");

            // 회피 방향 결정
            if (distance_Left > 25 || distance_Right > 25)
            {
                if (distance_Left > distance_Right)
                {
//                    printf(" Turning LEFT (L:%d > R:%d)\r\n", distance_Left, distance_Right);
                    Motor_Left();
                    Motor_SetSpeed(300, 300);
                    HAL_Delay(200);
                     // 좌회전 시간
                }
                else
                {
//                    printf(" Turning RIGHT (R:%d > L:%d)\r\n", distance_Right, distance_Left);
                    Motor_Right();
                    Motor_SetSpeed(300, 300);
                    HAL_Delay(200);
                     // 우회전 시간
                }
            }
            else
            {
//                printf(" No clear path! Stopping.\r\n");
                Motor_Stop();
            }
        }
        else
        {
            Motor_Forward();
            Motor_SetSpeed(500, 500);
        }


    }
}

/* === System Clock 설정 === */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
      Error_Handler();

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
      Error_Handler();
}

/* === 에러 핸들러 === */
void Error_Handler(void)
{
  __disable_irq();
  while (1) {}
}
