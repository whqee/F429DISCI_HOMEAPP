/**
  ******************************************************************************
  * File Name          : gpio.h
  * Description        : This file contains all the functions prototypes for 
  *                      the gpio  
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __gpio_H
#define __gpio_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
#define body_detection_power_on() \
HAL_GPIO_WritePin(Sensor_Power1_GPIO_Port, Sensor_Power1_Pin, GPIO_PIN_SET)

#define body_detection_shutdown() \
HAL_GPIO_WritePin(Sensor_Power1_GPIO_Port, Sensor_Power1_Pin, GPIO_PIN_RESET)

#define body_detect() \
HAL_GPIO_ReadPin(HR_INT_GPIO_Port, HR_INT_Pin)

#define body_detect_it() HR_INT = 1

#define light_detection_power_on() \
HAL_GPIO_WritePin(Sensor_Power2_GPIO_Port, Sensor_Power2_Pin, GPIO_PIN_SET)

#define light_detection_shutdown() \
HAL_GPIO_WritePin(Sensor_Power2_GPIO_Port, Sensor_Power2_Pin, GPIO_PIN_RESET)

#define light_detect() \
HAL_GPIO_ReadPin(LT_INT_GPIO_Port, LT_INT_Pin)

#define light_detect_it() LT_INT = 1

#define button_detect() \
HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin)

#define led_on() \
HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET)

#define led_off() \
HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET)

/* USER CODE END Private defines */

void MX_GPIO_Init(void);

/* USER CODE BEGIN Prototypes */
extern int HR_INT;
extern int body_sensor;
extern int LT_INT;
extern int light_sensor;
extern int g_signal_it;
extern int B1_Press;
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ pinoutConfig_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
