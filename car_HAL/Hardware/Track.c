#include "Track.h"

uint8_t SensorState[5] = {0};

uint8_t Track_Read(void)
{
	uint8_t sensorPattern = 0;
	//1-白线，0-黑线		
	SensorState[0] = (HAL_GPIO_ReadPin(OUT1_GPIO_Port,OUT1_Pin));
	SensorState[1] = (HAL_GPIO_ReadPin(OUT2_GPIO_Port,OUT2_Pin));
	SensorState[2] = (HAL_GPIO_ReadPin(OUT3_GPIO_Port,OUT3_Pin));
	SensorState[3] = (HAL_GPIO_ReadPin(OUT4_GPIO_Port,OUT4_Pin));
	SensorState[4] = (HAL_GPIO_ReadPin(OUT5_GPIO_Port,OUT5_Pin));
	
	for(uint8_t i=0;i<5;i++)
	{
		sensorPattern |= (SensorState[i] << (4-i));		// 最高位对应最左侧传感器,最右边的传感器没用到
	}
	
	return sensorPattern;
}

