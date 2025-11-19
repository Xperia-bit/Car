#ifndef __TRACK_H
#define __TRACK_H

#include "main.h"

#define OUT1_Pin GPIO_PIN_3
#define OUT1_GPIO_Port GPIOA
#define OUT2_Pin GPIO_PIN_4
#define OUT2_GPIO_Port GPIOA
#define OUT3_Pin GPIO_PIN_5
#define OUT3_GPIO_Port GPIOA
#define OUT4_Pin GPIO_PIN_6
#define OUT4_GPIO_Port GPIOA
#define OUT5_Pin GPIO_PIN_7
#define OUT5_GPIO_Port GPIOA

extern uint8_t SensorState[5];

uint8_t Track_Read(void);


#endif
