/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "TB6612.h"
#include "Track.h"
#include "Ultrasound.h"
#include "Servo.h"
#include "oled.h"
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t mode;		// 0-蓝牙 1-自动避障 2-循迹
char data;
uint8_t rx_flag;
float distance;
uint8_t sensorPattern;
char message[16];
uint8_t mode2_state;
uint8_t area,last_area;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void OLED_Show(void);
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
    HAL_UART_Receive_IT(&huart1,(uint8_t *)&data,1);
	OLED_Init();
	TB6612_Init();
	Servo_Init();
	Set_Angle(90);
	mode = 2;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  
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
					  HAL_Delay(400);
					  distance = Ultrasound_Get_Distance();
					  if(distance >= 15.0f)//左边无障碍物,原地左转90°调整方向
					  {
						Set_Angle(90);
						HAL_Delay(400);
						TB6612_SetPWM(-40,40);
						HAL_Delay(500);
						TB6612_SetPWM(0,0);
					  }
					  else//左方有障碍物
					  {
						  TB6612_SetPWM(0,0);//停车
						  Set_Angle(0);//向右观察
						  HAL_Delay(400);
						  distance = Ultrasound_Get_Distance();
						  if(distance >= 15.0f) //右边无障碍物,原地右转90°调整方向
						  {
							Set_Angle(90);
							HAL_Delay(400);
							TB6612_SetPWM(40,-40);
							HAL_Delay(500);
							TB6612_SetPWM(0,0);
						  }							 
						  else//右方有障碍物 ---> 左中右都有障碍物
						  {
							  Set_Angle(90);//舵机复位
							  HAL_Delay(400);
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
		  HAL_Delay(100);
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
				TB6612_SetPWM(0,40);
				area = 1;
				break;
			case 0x19: // 0b11001 右小转 
				TB6612_SetPWM(40,0);
			    area = 2;
				break;
			
			//----- 小幅度修正 -----
			case 0x17: // 0b10111 左中转 
				TB6612_SetPWM(0,50);//40
				area = 3;
				break;
			case 0x1D: // 0b11101 右中转 
				TB6612_SetPWM(50,0);//40
			    area = 4;
				break;

			//----- 中幅度修正 -----
			case 0x07: // 0b00111 左大转 
				TB6612_SetPWM(-15,50);
				area = 5;
				break;
			case 0x1C: // 0b11100 右大转 
				TB6612_SetPWM(50,-15);
			    area = 6;
				break;
			
			//----- 大幅度修正 -----
			case 0x0F: // 0b01111 左急转 
				TB6612_SetPWM(-25,50);
				area = 7;
				break;
			case 0x1E: // 0b11110 右急转
				TB6612_SetPWM(50,-25);
			    area = 8;
				break;

			//----- 全白处理 -----
			case 0x1F: // 0b11111 全白
				if(last_area == 2)
				{
					TB6612_SetPWM(30,0);
				}
				else if(last_area == 1)
				{
					TB6612_SetPWM(0,30);
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
				TB6612_SetPWM(-30,40);//-15
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
		HAL_Delay(20);
	  }
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == &huart1)
	{
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
		HAL_UART_Receive_IT(&huart1,(uint8_t *)&data,1);
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

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
