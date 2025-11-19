#include "stm32f10x.h"//头文件
//超声波测距定时器TIM2 初始化
void Timer_Init(void)
{	//开启定时器时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	//使用内部时钟
	TIM_InternalClockConfig(TIM4);
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;//不分频
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;//向上计数
	TIM_TimeBaseInitStructure.TIM_Period = 65535;//ARR
	TIM_TimeBaseInitStructure.TIM_Prescaler=72-1;//预分频值
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;//CNT到达ARR中断一次
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);
	TIM_SetCounter(TIM4,0);//CNT值清0
}

//开启TIM2定时器
void Timer_ON(void)
{
	TIM_Cmd(TIM4,ENABLE);
}

//关闭TIM2定时器
void Timer_OFF(void)
{
	TIM_Cmd(TIM4,DISABLE);
}
