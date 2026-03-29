#include "car_control.h"
#include "RCC_Motor.h"

void Car_Control(uint8_t cmd)
{
    switch(cmd)
    {
        case 'w': Motor_Forward(); break;
        case 's': Motor_Backward(); break;
        case 'a': Motor_Left(); break;
        case 'd': Motor_Right(); break;
        case 'x': Motor_Stop(); break;
        default: break;
    }
}
