#include "bluetooth.h"
#include "usart.h"
#include "motor.h"

extern uint8_t rxByte;

void Bluetooth_Init(void)
{
    HAL_UART_Receive_IT(&huart6, &rxByte, 1);
}

static void Bluetooth_Process(uint8_t data)
{
    switch (data)
    {
        case 'w': Motor_Forward(); break;
        case 's': Motor_Backward(); break;
        case 'a': Motor_Left(); break;
        case 'd': Motor_Right(); break;
        case 'x': Motor_Stop(); break;
        default: break;
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART6)
    {
        Bluetooth_Process(rxByte);
        HAL_UART_Receive_IT(&huart6, &rxByte, 1);
    }
}
