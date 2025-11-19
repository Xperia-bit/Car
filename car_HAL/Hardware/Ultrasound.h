#ifndef __ULTRASOUND_H
#define __ULTRASOUND_H

#define GPIO_ECHO_PORT GPIOA
#define GPIO_ECHO GPIO_PIN_1

#define GPIO_TRIG_PORT GPIOA
#define GPIO_TRIG GPIO_PIN_0

#include "main.h"

void Ultrasound_Init(void);//超声波初始化
float Ultrasound_Get_Distance(void);//获取超声波测距的距离
void delay_us(uint32_t us);


#endif
