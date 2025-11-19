#include "TB6612.h"
#include "main.h"
#include "tim.h"

void TB6612_Init(void)
{
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_4);
}

/**
 * @description: 根据带符号的pwm值，设置转速和方向
 * @param {int} pwma   A电机，带符号
 * @param {int} pwmb   B电机，带符号
 * @return {*}
 */
void TB6612_SetPWM(int left_pwm,int right_pwm)
{
    /* 根据带符号的pwm值：1、处理方向；2、设置pwm占空比 */


    /* 1.处理pwma */
    /* 1.1 根据符号处理方向 */
    if(left_pwm > 0)
    {
        /* 正转 */
        HAL_GPIO_WritePin(AIN1_GPIO_Port,AIN1_Pin,GPIO_PIN_SET);
		HAL_GPIO_WritePin(AIN2_GPIO_Port,AIN2_Pin,GPIO_PIN_RESET);
    }
    else if(left_pwm < 0)
    {
        /* 反转 */
        HAL_GPIO_WritePin(AIN1_GPIO_Port,AIN1_Pin,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(AIN2_GPIO_Port,AIN2_Pin,GPIO_PIN_SET);
        /* 取正值值 */
        left_pwm = -left_pwm;
    }
    else 
    {
        /* 刹车 */
        HAL_GPIO_WritePin(AIN1_GPIO_Port,AIN1_Pin,GPIO_PIN_SET);
		HAL_GPIO_WritePin(AIN2_GPIO_Port,AIN2_Pin,GPIO_PIN_SET);
    }
    /* 设置对应定时器通道的ccr值 */
	TIM1 ->CCR1 = left_pwm;
    //__HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_4,left_pwm);
	
	//TIM1->CCR4 = left_pwm;


    /* 2.处理pwmb */
    /* 1.1 根据符号处理方向 */
    if(right_pwm > 0)
    {
        /* 正转 */
        HAL_GPIO_WritePin(BIN1_GPIO_Port,BIN1_Pin,GPIO_PIN_SET);
		HAL_GPIO_WritePin(BIN2_GPIO_Port,BIN2_Pin,GPIO_PIN_RESET);
    }
    else if(right_pwm < 0)
    {
        /* 反转 */
        HAL_GPIO_WritePin(BIN1_GPIO_Port,BIN1_Pin,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(BIN2_GPIO_Port,BIN2_Pin,GPIO_PIN_SET);
        /* 取正值值 */
        right_pwm = -right_pwm;
    }
    else 
    {
        /* 刹车 */
        HAL_GPIO_WritePin(BIN1_GPIO_Port,BIN1_Pin,GPIO_PIN_SET);
		HAL_GPIO_WritePin(BIN2_GPIO_Port,BIN2_Pin,GPIO_PIN_SET);
    }
    /* 设置对应定时器通道的ccr值 */
    TIM1 ->CCR4 = right_pwm;
	//__HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_1,right_pwm);
	
	//TIM1->CCR1 = right_pwm;
}

