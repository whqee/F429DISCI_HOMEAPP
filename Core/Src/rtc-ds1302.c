#include "rtc.h"
#include "spi.h"
#include "gpio.h"
 

 /***** modify here ******/
#define spi hspi5
/******         ********/


#define	RTC_CMD_READ	0x81		/* Read command */
#define	RTC_CMD_WRITE	0x80		/* Write command */

#define	RTC_CMD_WRITE_ENABLE	0x00		/* Write enable */
#define	RTC_CMD_WRITE_DISABLE	0x80		/* Write disable */

#define RTC_ADDR_RAM0	0x20		/* Address of RAM0 */
#define RTC_ADDR_TCR	0x08		/* Address of trickle charge register */
#define RTC_CLCK_BURST	0x1F		/* Address of clock burst */
#define	RTC_CLCK_LEN	0x08		/* Size of clock burst */
#define	RTC_ADDR_CTRL	0x07		/* Address of control register */
#define	RTC_ADDR_YEAR	0x06		/* Address of year register */
#define	RTC_ADDR_DAY	0x05		/* Address of day of week register */
#define	RTC_ADDR_MON	0x04		/* Address of month register */
#define	RTC_ADDR_DATE	0x03		/* Address of day of month register */
#define	RTC_ADDR_HOUR	0x02		/* Address of hour register */
#define	RTC_ADDR_MIN	0x01		/* Address of minute register */
#define	RTC_ADDR_SEC	0x00		/* Address of second register */

static int inline __ds1302_write(unsigned char *buf, int count)
{
    int status;

    HAL_GPIO_WritePin(DS1302_CS_GPIO_Port, DS1302_CS_Pin, GPIO_PIN_SET);

    status = HAL_SPI_Transmit(&spi, buf, count, 100);
    
    HAL_GPIO_WritePin(DS1302_CS_GPIO_Port, DS1302_CS_Pin, GPIO_PIN_RESET);

	if (status)
		return status;
    
    return 0;
}
static int inline __ds1302_write_then_read(unsigned char addr,unsigned char *buf, int count)
{
    int status;

    HAL_GPIO_WritePin(DS1302_CS_GPIO_Port, DS1302_CS_Pin, GPIO_PIN_SET);

    HAL_SPI_Transmit(&spi, &addr, sizeof(addr), 10);
    status = HAL_SPI_Receive(&spi, buf, count,10);

    HAL_GPIO_WritePin(DS1302_CS_GPIO_Port, DS1302_CS_Pin, GPIO_PIN_RESET);

	if (status)
		return status;
    
    return 0;
}


static int ds1302_rtc_set_time(struct rtc_time *time)
{
	unsigned char	buf[1 + RTC_CLCK_LEN];
	unsigned char	*bp;
	int		status;

	/* Enable writing */
	bp = buf;
	*bp++ = RTC_ADDR_CTRL << 1 | RTC_CMD_WRITE;
	*bp++ = RTC_CMD_WRITE_ENABLE;

    status = __ds1302_write(buf, 2);
	if (status)
		return status;

	/* Write registers starting at the first time/date address. */
	bp = buf;
	*bp++ = RTC_CLCK_BURST << 1 | RTC_CMD_WRITE;

	*bp++ = bin2bcd(time->sec);
	*bp++ = bin2bcd(time->min);
	*bp++ = bin2bcd(time->hour);
	*bp++ = bin2bcd(time->date);
	*bp++ = bin2bcd(time->mon + 1);
	*bp++ = time->day + 1;
	*bp++ = bin2bcd(time->year % 100);
	*bp++ = RTC_CMD_WRITE_DISABLE;

    HAL_GPIO_WritePin(DS1302_CS_GPIO_Port, DS1302_CS_Pin, GPIO_PIN_SET);
	status = HAL_SPI_Transmit(&spi, buf, sizeof(buf), 1000);
    HAL_GPIO_WritePin(DS1302_CS_GPIO_Port, DS1302_CS_Pin, GPIO_PIN_RESET);

	return 0;
}

static int ds1302_rtc_get_time(struct rtc_time *time)
{
	unsigned char	addr = RTC_CLCK_BURST << 1 | RTC_CMD_READ;
	unsigned char	buf[RTC_CLCK_LEN - 1];
	int		status;

	/* Use write-then-read to get all the date/time registers
	 * since dma from stack is nonportable
	 */
    
    status = __ds1302_write_then_read(addr, buf, sizeof(buf));
	if (status)
		return status;

	/* Decode the registers */
	time->sec = bcd2bin(buf[RTC_ADDR_SEC]);
	time->min = bcd2bin(buf[RTC_ADDR_MIN]);
	time->hour = bcd2bin(buf[RTC_ADDR_HOUR]);
	time->day = buf[RTC_ADDR_DAY] - 1;
	time->date = bcd2bin(buf[RTC_ADDR_DATE]);
	time->mon = bcd2bin(buf[RTC_ADDR_MON]) - 1;
	time->year = bcd2bin(buf[RTC_ADDR_YEAR]) + 2000;

	return 0;
}

static const struct rtc_class_ops ds1302_ops = {
    .read_time = ds1302_rtc_get_time,
    .set_time = ds1302_rtc_set_time,
};
EXPORT(&ds1302_ops);

