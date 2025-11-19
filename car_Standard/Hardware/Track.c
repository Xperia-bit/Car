#include "stm32f10x.h"                  // Device header

uint8_t SensorState[5] = {0};

void Sensor_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
}

uint8_t Track_Read(void)
{
	uint8_t sensorPattern = 0;
	//1-白线，0-黑线		
	SensorState[0] = (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3));
	SensorState[1] = (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4));
	SensorState[2] = (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5));
	SensorState[3] = (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6));
	SensorState[4] = (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7));
	
	for(uint8_t i=0;i<5;i++)
	{
		sensorPattern |= (SensorState[i] << (4-i));		// 最高位对应最左侧传感器,最右边的传感器没用到
	}
	
	return sensorPattern;
}
