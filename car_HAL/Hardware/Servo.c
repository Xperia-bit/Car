#include "Servo.h"
#include "main.h"
#include "tim.h"

void Servo_Init(void)
{
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);
}

void Set_Angle(float angle)
{
	float pwm_set = angle / 180.0f * 2000.0f + 500.0f;//将角度线性变换
	__HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_3,pwm_set);
}

