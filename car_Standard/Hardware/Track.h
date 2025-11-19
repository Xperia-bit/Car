#ifndef __Track_H
#define __Track_H

void Sensor_Init(void);
uint8_t Track_Read(void);

extern uint8_t SensorState[5];

#endif
