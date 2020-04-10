#ifndef __DHT11_H
#define __DHT11_H


#define HIGH  1
#define LOW   0

//带参宏，可以像内联函数一样使用,输出高电平或低电平
#define DHT11_DATA_OUT(a)	if (a)	\
HAL_GPIO_WritePin(Sensor_DHT11_GPIO_Port, Sensor_DHT11_Pin, GPIO_PIN_SET); \
					else		\
HAL_GPIO_WritePin(Sensor_DHT11_GPIO_Port, Sensor_DHT11_Pin, GPIO_PIN_RESET)

 //读取引脚的电平
#define  DHT11_DATA_IN() \
HAL_GPIO_ReadPin(Sensor_DHT11_GPIO_Port, Sensor_DHT11_Pin)

// int dht11_read_data(unsigned char *data);

typedef unsigned char uint8_t;

typedef struct
{
	uint8_t  humi_int;		//湿度的整数部分
	uint8_t  humi_deci;	 	//湿度的小数部分
	uint8_t  temp_int;	 	//温度的整数部分
	uint8_t  temp_deci;	 	//温度的小数部分
	uint8_t  check_sum;	 	//校验和
		                 
}DHT11_Data_TypeDef;

uint8_t Read_DHT11(DHT11_Data_TypeDef *DHT11_Data);

#endif // !__DHT11_H
