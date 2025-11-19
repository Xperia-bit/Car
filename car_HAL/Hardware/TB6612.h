#ifndef __TB6612_H
#define __TB6612_H

#define AIN1_GPIO_Port GPIOB
#define AIN1_Pin GPIO_PIN_15
#define AIN2_GPIO_Port GPIOB
#define AIN2_Pin GPIO_PIN_14
#define BIN1_GPIO_Port GPIOB
#define BIN1_Pin GPIO_PIN_13
#define BIN2_GPIO_Port GPIOB
#define BIN2_Pin GPIO_PIN_12

void TB6612_Init(void);
void TB6612_SetPWM(int left_pwm,int right_pwm);

#endif
