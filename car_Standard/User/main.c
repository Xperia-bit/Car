#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "oled.h"
#include "Ultrasound.h"
#include "Servo.h"
#include "Serial.h"
#include "Track.h"
#include "string.h"
#include "TB6612.h"

uint8_t mode;		// 0-蓝牙 1-自动避障 2-循迹
char data;
uint8_t rx_flag;
float distance;
uint8_t sensorPattern;
char message[16];
uint8_t mode2_state;
uint8_t area,last_area;

void OLED_Show(void);

int main(void)
{
	OLED_Init();			//显示器初始化
	TB6612_Init();			//电机初始化
	Serial_Init();			//串口初始化
	Servo_Init();			//舵机初始化
	Ultrasound_Init();		//超声波初始化
	Sensor_Init();			//红外传感器
	
	Set_Angle(90);			//舵机复位
	
	mode = 2;
	
	while (1)
	{	
		
	  //OLED显示部分
	  OLED_Show();
	  
	  //蓝牙模式
	  if(mode == 0)
	  {
		  if(rx_flag)
		  {
			rx_flag = 0;
			if(data == '0')			TB6612_SetPWM(20,20);		//前进
			else if(data == '1')	TB6612_SetPWM(-20,-20);		//后退
			else if(data == '2')	TB6612_SetPWM(20,50);		//左转
			else if(data == '3')	TB6612_SetPWM(50,20);		//右转
			else if(data == '4')	TB6612_SetPWM(-20,20);		//原地左转
			else if(data == '5')	TB6612_SetPWM(20,-20);		//原地右转
			else if(data == '6')	TB6612_SetPWM(0,0);		//刹车
		  }
	  }
	  
	  //自动避障模式
	  else if(mode == 1)
	  {
		  switch(mode2_state)
		  {
			  case 0:
				  distance = Ultrasound_Get_Distance();//获取距离
				  if(distance >= 5.0f)	TB6612_SetPWM(15,15);//前方无障碍物,直走
				  else	//前方有障碍物
				  {
					  TB6612_SetPWM(0,0);//停车
					  Set_Angle(180);//先向左观察
					  Delay_ms(400);
					  distance = Ultrasound_Get_Distance();
					  if(distance >= 15.0f)//左边无障碍物,原地左转90°调整方向
					  {
						Set_Angle(90);
						Delay_ms(400);
						TB6612_SetPWM(-40,40);
						Delay_ms(500);
						TB6612_SetPWM(0,0);
					  }
					  else//左方有障碍物
					  {
						  TB6612_SetPWM(0,0);//停车
						  Set_Angle(0);//向右观察
						  Delay_ms(400);
						  distance = Ultrasound_Get_Distance();
						  if(distance >= 15.0f) //右边无障碍物,原地右转90°调整方向
						  {
							Set_Angle(90);
							Delay_ms(400);
							TB6612_SetPWM(40,-40);
							Delay_ms(500);
							TB6612_SetPWM(0,0);
						  }							 
						  else//右方有障碍物 ---> 左中右都有障碍物
						  {
							  Set_Angle(90);//舵机复位
							  Delay_ms(400);
							  mode2_state = 1;//进入原地等待状态
						  }
					  }
				  }
				  break;
			  case 1:
				  //原地停车,等待前方障碍物移走
				  TB6612_SetPWM(0,0);
				  distance = Ultrasound_Get_Distance();
				  if(distance >= 5.0f)
				  {
					mode2_state = 0;
				  }
				  break;
			  default:
				  break;
		  }
		  Delay_ms(100);
	  }
	  //循迹模式
	  else if(mode == 2)
	  {
		sensorPattern = Track_Read();
		switch (sensorPattern) 
		{
			//----- 基础模式 -----
			case 0x1B: // 0b11011 直线 
				TB6612_SetPWM(15,15);
				break;

			//----- 小幅度修正 -----
			case 0x13: // 0b10011 左小转 
				TB6612_SetPWM(0,30);
				area = 1;
				break;
			case 0x19: // 0b11001 右小转 
				TB6612_SetPWM(30,0);
			    area = 2;
				break;
			
			//----- 中幅度修正 -----
			case 0x17: // 0b10111 左中转 
				TB6612_SetPWM(0,40);//40
				area = 3;
				break;
			case 0x1D: // 0b11101 右中转 
				TB6612_SetPWM(40,0);//40
			    area = 4;
				break;

			//----- 大幅度修正 -----
			case 0x07: // 0b00111 左大转 
				TB6612_SetPWM(-15,40);
				area = 5;
				break;
			case 0x1C: // 0b11100 右大转 
				TB6612_SetPWM(40,-15);
			    area = 6;
				break;
			
			//----- 极端角度修正 -----
			case 0x0F: // 0b01111 左急转 
				TB6612_SetPWM(-25,55);
				area = 7;
				break;
			case 0x1E: // 0b11110 右急转
				TB6612_SetPWM(55,-25);
			    area = 8;
				break;

			//----- 全白处理 -----
			case 0x1F: // 0b11111 全白
				if(last_area == 2)
				{
					TB6612_SetPWM(25,0);
				}
				else if(last_area == 1)
				{
					TB6612_SetPWM(0,25);
				}
				else
				{
					TB6612_SetPWM(0,0);
				}
				break;
			
			
			//----- 连续偏转 -----
			//左连续偏转
			case 0x03: // 0b00011 
			case 0x05: // 0b00101
				TB6612_SetPWM(-30,40);
				area = 9;
				break;
			//右连续偏转
			case 0x18: // 0b11000 
			case 0x1A: // 0b11010
				TB6612_SetPWM(40,-30);		
				area = 10;
				break;
			
			//----- 默认处理 -----
			default:
				TB6612_SetPWM(0,0);
				break;
		}
		last_area = area;
		Delay_ms(20);
	  }
	}
}
	
void USART1_IRQHandler(void)
{
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
		data=USART_ReceiveData(USART1);
		//模式切换指令
		if(data - '0' >= 7)
		{
			if(data == '7')						mode = 0;
			else if(data == '8')				mode = 1;
			else if(data == '9')				mode = 2;
			OLED_Clear();//每次切换模式要清屏显示
		}
		else	//行进方向指令
		{
			//确保在蓝牙模式下才能无线控制小车
			if(mode == 0)	rx_flag = 1;	//接收标志位置1,在while中处理
		}
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
}

void OLED_Show(void)
{
	OLED_ShowString(0,0,(uint8_t *)"mode:",16,1);
	if(mode == 0)
	{
		OLED_ShowString(40,0,(uint8_t *)"Bluetooth",16,1);
		if(data - '0' >= 0 && data - '0' <= 9)
		{
			sprintf(message,"data:%c   ",data);
		}
		else
		{
			sprintf(message,"data:Null");
		}
		OLED_ShowString(0,16,(uint8_t *)message,16,1);
	}
	else if(mode == 1)
	{
		OLED_ShowString(40,0,(uint8_t *)"Auto avoid",16,1);
		sprintf(message,"Dis:%.2fcm",distance);
		OLED_ShowString(0,16,(uint8_t *)message,16,1);
	}
	else if(mode == 2)
	{
		OLED_ShowString(40,0,(uint8_t *)"Trace",16,1);
		sprintf(message,"%d-%d-%d-%d-%d",SensorState[0],SensorState[1],SensorState[2],SensorState[3],SensorState[4]);
		OLED_ShowString(0,16,(uint8_t *)message,16,1);
	}
	OLED_Refresh();
}


