/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma2d.h"
#include "i2c.h"
#include "ltdc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "fmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "rtc.h"
#include "oled.h"
#include "bh1750.h"
#include "dht11.h"
#include "stm32f429i_discovery_lcd.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#include "print.h"
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */



/* Pictures position */
uint32_t Xpos1 = 0;
uint32_t Xpos2 = 0;
uint32_t Ypos1 = 0;
uint32_t Ypos2 = 160;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

static void PicturesPosition(uint32_t* x1, 
                         uint32_t* y1, 
                         uint32_t* x2, 
                         uint32_t* y2, 
                         uint32_t index);


/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void display_hour(struct rtc_time *time)
{
  char hour[2];
  hour[0] = time->hour / 10 + 48;
  hour[1] = time->hour % 10 + 48;
  OLED_ShowStr(3*16,0,hour,2);	//8*16字符
}
void display_hour_clear()
{
  OLED_ShowStr(3*16,0,"  ",2);
}
void display_min(struct rtc_time *time)
{
  char min[2];
  min[0] = time->min / 10 + 48;
  min[1] = time->min % 10 + 48;
  OLED_ShowStr(4*16+8,0,min,2);	//8*16字符
}
void display_min_clear()
{
  OLED_ShowStr(4*16+8,0,"  ",2);
}
void display_sec(struct rtc_time *time)
{
  char sec[2];
  sec[0] = time->sec / 10 + 48;
  sec[1] = time->sec % 10 + 48;
  OLED_ShowStr(6*16,0,sec,2);	//8*16字符
}
void display_sec_clear()
{
  OLED_ShowStr(6*16,0,"  ",2);
}




int g_signal_work = 1;

void work_thread()
{

  int status;
  struct rtc_time time;

  if (body_sensor && light_sensor) {

    status = rtc->read_time(&time);
    if(status == 0) {
      printf("time: %d:%d:%d\r\n", time.hour,time.min,time.sec);
      if (time.hour > 6 && time.hour < 23) {
        led_on();
        HAL_Delay(2000);
      } else {
        led_off();
        // break;
      }

    } else {
        led_on();
        HAL_Delay(2000);
    }

  } else {
    led_off();
    // break;
  }
  

}

void oled_thread()
{
  // OLED_ON();
  // OLED_Fill(0x00);//全屏灭
  // HAL_Delay(1000);
  for(int i=0;i<3;i++)
  {
    OLED_ShowCN(i*16,0,i+2);//测试显示中文
  }
  OLED_ShowStr(4*16,0,":",2);	//8*16字符
  OLED_ShowStr(5*16+8,0,":",2);	//8*16字符

  struct rtc_time time;
  rtc->read_time(&time);
  printf("s_time: %d:%d:%d\r\n", time.hour,time.min,time.sec);
  display_hour(&time);
  display_min(&time);
  display_sec(&time);
  // display_hour_clear();
  // OLED_ShowStr(0,3,"Hello whqee",1);//测试6*8字符
  // OLED_ShowStr(0,4,"Hello Tech",2);				//测试8*16字符
  // HAL_Delay(1000);
  // OLED_CLS();//清屏
  // OLED_OFF();//测试OLED休眠
  // HAL_Delay(1000);
  // OLED_ON();//测试OLED休眠后唤醒
  // // OLED_DrawBMP( 0,0,128,8,(unsigned char *)BMP1);//测试BMP位图显示
  // HAL_Delay(1000);
}

void idle_thread()
{
  OLED_OFF();
  while (!g_signal_it)
  {
    
  }
}

void schedule_thread()
{
reschedule:
  // while (B1_Press) {
  //   // if (!button_detect()) 
  //     oled_thread();HAL_Delay(2000);
  //   // else {
  //     B1_Press = 0;
  //     OLED_OFF();
  //   // } 
  // }

  oled_thread();
  work_thread();
  // idle_thread();
  if (g_signal_it) {
    __disable_irq();
    g_signal_it = 0;
    __enable_irq();
    goto reschedule;
  }

}

void memset(char *s, char c, int size)
{
  char *p = s;
  for (; size > 0; size--) 
  {
    *p++ = c;
  }
}

int strstr(char * haystack, char * needle){
    if (*needle == 0) return 0;
    if (*haystack == 0) return -1;
    char * head = haystack;
    int offset;
    while (*haystack) {
        offset = 0;
        while (1) {
            if (needle[offset] == 0) return haystack - head;
            if (haystack[offset] == 0) return -1;
            if (haystack[offset] != needle[offset]) break;
            offset++;
        }
        haystack++;
    }
    return -1;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  uint32_t index = 0;
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
  // MX_SPI5_Init();
  MX_USART1_UART_Init();
  // MX_LTDC_Init();
  // MX_DMA2D_Init();
  // MX_FMC_Init();
  MX_I2C3_Init();
  MX_UART5_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */
  // HAL_UART_Receive_IT(&huart5, UBuffer, sizeof(UBuffer));
  // HAL_UART_Receive_IT(&huart1, UBuffer, sizeof(UBuffer));
  BSP_LCD_Init();
	BSP_LCD_LayerDefaultInit(LCD_BACKGROUND_LAYER,LCD_FRAME_BUFFER);
	BSP_LCD_LayerDefaultInit(LCD_FOREGROUND_LAYER,LCD_FRAME_BUFFER);
	BSP_LCD_SelectLayer(LCD_FOREGROUND_LAYER);
	BSP_LCD_DisplayOn();
	BSP_LCD_Clear(LCD_COLOR_WHITE);
  BSP_LED_On(LED3);
  BSP_LCD_SetTextColor(LCD_COLOR_DARKBLUE);
  BSP_LCD_DisplayStringAtLine(0,(uint8_t *)"starting...");
  HAL_Delay(500);

  body_detection_power_on();
  body_detect_it();

  // light_detection_power_on();   
  // light_detect_it();
  /* In this proj, I changed it into roomlight control-func */
  #define light_on light_detection_power_on
  #define light_off light_detection_shutdown
  #define light_check() \
  HAL_GPIO_ReadPin(Sensor_Power2_GPIO_Port, Sensor_Power2_Pin)

  struct rtc_time init_time = {
    .hour = 22,
    .min = 58,
    .sec = 30,
  };
  // rtc->set_time(&init_time);

  if (bh1750_init() != HAL_OK && bh1750_init() != HAL_OK)  printf("bh1750 init error.\r\n");
  if (OLED_Init() < 0 && OLED_Init() < 0)  printf("OLED Init error.\r\n");
  // OLED_Fill(0x00);//全屏灭
  // OLED_Fill(0xFF);//全屏灭

  /* debug for usart esp8266 */
  char buff[128];
  char *p = buff;
  // esp8266
  // if(HAL_UART_Transmit(&huart5, "AT+CIPSTART=\"TCP\",\"192.168.10.1\",10000\r\n", 10, 100))
  // if(HAL_UART_Transmit(&huart5, "AT+CWMODE?\r\n", 10, 100))
  //   printf("uart5 send error.\r\n");
  // HAL_Delay(1);
  // if(HAL_UART_Transmit(&huart5, "AT+CWMODE?\r\n", 10, 100))
  //   printf("uart5 send error.\r\n");
  // HAL_UART_Receive(&huart5, buff, sizeof(buff), 1000);
  // printf("%s\r\n", buff);
  // while (p < (buff+1))
  // {
  //   *p = '\0';
  //   p++;
  // }
  // p = buff;

  /*  end debug for esp8266 */

  // debug for dht11
  DHT11_Data_TypeDef DHT11_Data = {};
  


  BSP_LCD_DisplayStringAtLine(0,(uint8_t *)"...           ");
  // if(HAL_UART_Transmit(&huart5, "AT+CWMODE?\r\n", 10, 100))
  //   printf("uart5 send error.\r\n");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    // schedule_thread();
    // oled_thread();
    // int status;
    // struct rtc_time time;

    // if (body_detect() && light_detect()) {

    //   status = rtc->read_time(&time);
    //   if(status == 0) {
    //     printf("time: %d:%d:%d\r\n", time.hour,time.min,time.sec);
    //     if (time.hour > 6 && time.hour < 23) {
    //       led_on();
    //     } else {
    //       led_off();
    //     }

    //   } else {
    //       led_on();
    //   }

    // } else {
    //   led_off();
    // }
    // HAL_Delay(1000);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    char lcd_buf[14];
    memset(buff, 0, sizeof(buff));
    sprintf(buff, "IOT");
    // bh1750
    float lx;
    if (get_illuminance(&lx) != HAL_OK) {
      printf("bh1750 read failed.\r\n");
      BSP_LCD_DisplayStringAtLine(0,"lux:       ");
      sprintf(buff, "%slux:        \r\n", buff);
    }
    else {
      printf("光照度 ");
      print_float(lx);
      memset(lcd_buf, 0, sizeof(lcd_buf));
      sprintf(lcd_buf, "lux: %d.%d%d", (int)lx, (int)((lx - (int)lx)*10), (int)((((lx - (int)lx)*10) - (int)((lx - (int)lx)*10))*10));
      BSP_LCD_DisplayStringAtLine(0,(uint8_t *)lcd_buf);
      sprintf(buff, "%s%s\r\n", buff, lcd_buf);
    }
    /*调用Read_DHT11读取温湿度，若成功则输出该信息*/
    if( Read_DHT11(&DHT11_Data)==SUCCESS)
    {
      printf("\r\n读取DHT11成功!\r\n\r\n湿度为%d.%d ％RH ，温度为 %d.%d℃ \r\n",\
      DHT11_Data.humi_int, DHT11_Data.humi_deci, DHT11_Data.temp_int, DHT11_Data.temp_deci);
      //printf("\r\n 湿度:%d,温度:%d \r\n" ,DHT11_Data.humi_int,DHT11_Data.temp_int);
      
      memset(lcd_buf, 0, sizeof(lcd_buf));
      sprintf(lcd_buf, "Temp: %d.%dC",DHT11_Data.temp_int, DHT11_Data.temp_deci);
      BSP_LCD_DisplayStringAtLine(1,(uint8_t *)lcd_buf);
      sprintf(buff, "%s%s\r\n", buff,lcd_buf);
      memset(lcd_buf, 0, sizeof(lcd_buf));
      sprintf(lcd_buf, "Humi: %d.%d%%",DHT11_Data.humi_int, DHT11_Data.humi_deci);
      BSP_LCD_DisplayStringAtLine(2,(uint8_t *)lcd_buf);
      sprintf(buff, "%s%s\r\n", buff,lcd_buf);
    }
    else
    {
      printf("Read DHT11 ERROR!\r\n");
      // BSP_LCD_DisplayStringAtLine(1,"Temp: null");
      // BSP_LCD_DisplayStringAtLine(2,"Humi: null");
      sprintf(buff, "%sTemp: null\r\nHumi: null\r\n", buff);
    }

    if (light_check()) {
      sprintf(buff, "%slight: ON \r\n", buff);
      BSP_LCD_DisplayStringAtLine(3, "Light: ON ");
    }
    else {
      sprintf(buff, "%slight: OFF\r\n", buff);
      BSP_LCD_DisplayStringAtLine(3, "Light: OFF");
    }

    char net_status[64];
    if(HAL_UART_Transmit(&huart5, "AT\r\n", 5, 100))
      printf("uart5 send error.\r\n");
    HAL_Delay(3);
    HAL_UART_Receive(&huart5, net_status, sizeof(net_status), 1000);
    HAL_Delay(2);
    // if(HAL_UART_Transmit(&huart5, "AT+CWJAP?\r\n", 12, 100))
    //   printf("uart5 send error.\r\n");
    // HAL_Delay(3);
    // memset(net_status, 0, sizeof(net_status));
    // HAL_UART_Receive(&huart5, net_status, sizeof(net_status), 1000);
    // printf("WiFi Check:%s\r\n", net_status);
    // if (strstr(net_status, "+CWJAP:\"") >= 0) {
    //   BSP_LCD_DisplayStringAtLine(4, "WiFi: OK");
    // } 
    // else {
    //   BSP_LCD_DisplayStringAtLine(4, "WiFi :  ");
    // }
    if(HAL_UART_Transmit(&huart5, "AT+CIPSTART=\"TCP\",\"154.201.3.19\",80\r\n", 38, 100))
      printf("uart5 send error.\r\n");
    HAL_Delay(3);
    memset(net_status, 0, sizeof(net_status));
    HAL_UART_Receive(&huart5, net_status, sizeof(net_status), 3000);
    printf("Connect Server:%s\r\n", net_status);
    if (strstr(net_status, "CONNECT") >= 0) {
      BSP_LCD_DisplayStringAtLine(5, "Server: OK");
    } 
    else {
      BSP_LCD_DisplayStringAtLine(5, "Server:   ");
    }
    if(HAL_UART_Transmit(&huart5, "AT+CIPSEND=53\r\n", 16, 100))
      printf("uart5 send error.\r\n");
    HAL_Delay(2);
    if(HAL_UART_Transmit(&huart5, buff, 53, 100))
      printf("uart5 send error.\r\n");
    memset(buff, 0, sizeof(buff));
    HAL_UART_Receive(&huart5, buff, sizeof(buff), 3000);
    printf("%s\r\n", buff);
    HAL_Delay(2);
    if(HAL_UART_Transmit(&huart5, "AT\r\n", 3, 100))
      printf("uart5 send error.\r\n");
    if(HAL_UART_Transmit(&huart5, "AT+CIPCLOSE\r\n", 14, 100))
      printf("uart5 send error.\r\n");
    
    if(strstr(buff, "lon") >= 0) light_on();
    else if(strstr(buff, "loff") >= 0) light_off();

    HAL_Delay(800);

    // if (body_detect()) {
    //   light_on();
    // }
    // else {
    //   light_off();
    // }
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
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Activate the Over-Drive mode 
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
  PeriphClkInitStruct.PLLSAI.PLLSAIN = 50;
  PeriphClkInitStruct.PLLSAI.PLLSAIR = 2;
  PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */


/**
  * @brief  calculate pictures position.
  * @param  x1:    picture1 x position
  * @param  y1:    picture1 y position
  * @param  x2:    picture2 x position
  * @param  y2:    picture2 y position
  * @param  index: 
  * @retval None
  */
static void PicturesPosition(uint32_t* x1, uint32_t* y1, uint32_t* x2, uint32_t* y2, uint32_t index)
{
  /* picture1 position */
  *x1 = 0;
  *y1 = index*4; 
  
  /* picture2 position */
  *x2 = 0;
  *y2 = 160 - index*4;
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
