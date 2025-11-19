#include "Ultrasound.h"
#include "tim.h"
#include "oled.h"

float Voice_Speed = 350.0f;

//超声波初始化
void Ultrasound_Init(void)
{
	HAL_GPIO_WritePin(GPIO_TRIG_PORT,GPIO_TRIG,GPIO_PIN_RESET);
}

//获取超声波测量距离
float Ultrasound_Get_Distance(void)
{
	uint16_t counter_sum = 0;
	float distance = 0.0f;
	for(int i=0;i<5;i++)
	{
		HAL_GPIO_WritePin(GPIO_TRIG_PORT,GPIO_TRIG,GPIO_PIN_SET);//trig拉高,发送起始信号
		delay_us(20);//延时20US
		HAL_GPIO_WritePin(GPIO_TRIG_PORT,GPIO_TRIG,GPIO_PIN_RESET);//trig拉低
		
		while(HAL_GPIO_ReadPin(GPIO_ECHO_PORT,GPIO_ECHO) == GPIO_PIN_RESET);//等待echo从低电平变为高电平
		
		//开启定时器,清空定时器从零开始计数
		__HAL_TIM_SET_COUNTER(&htim2,0);
		HAL_TIM_Base_Start(&htim2);
		
		while(HAL_GPIO_ReadPin(GPIO_ECHO_PORT,GPIO_ECHO) == GPIO_PIN_SET);//echo变为低电平

		HAL_TIM_Base_Stop(&htim2);  // 关闭定时器
		counter_sum += __HAL_TIM_GetCounter(&htim2);
	}
	distance = counter_sum*Voice_Speed*0.00001f;	//单位:cm
	counter_sum = 0;
	return distance;
}

void delay_us(uint32_t us)
{
	__HAL_TIM_SET_COUNTER(&htim2, 0);
	HAL_TIM_Base_Start(&htim2);
	while (__HAL_TIM_GET_COUNTER(&htim2) < us)
	{
	}
	HAL_TIM_Base_Stop(&htim2);  // 关闭定时器
}	


