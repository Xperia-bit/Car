#include "stm32f10x.h"                  // Device header
#include "Delay.h"

float Voice_Speed = 350.0f;		//当前温度对应的声速

void Ultrasound_Init(void)
{
	//设置一个1us的定时器
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	TIM_InternalClockConfig(TIM2);
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;//不分频
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;//向上计数
	TIM_TimeBaseInitStructure.TIM_Period = 65535;//ARR
	TIM_TimeBaseInitStructure.TIM_Prescaler=72-1;//预分频值
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;//CNT到达ARR中断一次
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
	
	//Echo和Trig引脚配置
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//开GPIO时钟
	GPIO_InitTypeDef GPIO_InitStructure;//定义初始化结构体
	//脉冲触发端口 Trig=PA0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//回波接收端口 Echo=PA1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

//获取超声波测量距离
float Ultrasound_Get_Distance(void)
{
	uint16_t counter_sum = 0;
	float distance = 0.0f;
	for(int i=0;i<5;i++)
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_0);//trig拉高,发送起始信号
		Delay_us(20);
		GPIO_ResetBits(GPIOA, GPIO_Pin_0);//trig拉低
		while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1) == 0);//等待echo从低电平变为高电平
		
		//开启定时器,清空定时器从零开始计数
		TIM_SetCounter(TIM2,0);
		TIM_Cmd(TIM2,ENABLE);
		
		while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1) == 1);//echo变为低电平

		TIM_Cmd(TIM2,DISABLE);  // 关闭定时器
		counter_sum += TIM_GetCounter(TIM2);;
	}
	distance = counter_sum*Voice_Speed*0.00001f;	//单位:cm
	counter_sum = 0.0f;
	return distance;
}






