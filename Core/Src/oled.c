#include "oled.h"
#include "codetable.h"
#include "i2c.h"
#include "main.h"

#define i2c hi2c3


#define DEV_ADDR 0x3C
#define I2C_READ 1
#define I2C_WRITE 0




int OLED_Write_Cmd(unsigned char cmd)
{
    return HAL_I2C_Mem_Write(&i2c, 0x78, 0x00, I2C_MEMADD_SIZE_8BIT, &cmd, 1, 1000);
}

int OLED_Write_Data(unsigned char data)
{
    return HAL_I2C_Mem_Write(&i2c, 0x78, 0x40, I2C_MEMADD_SIZE_8BIT, &data, 1, 1000);
}

int OLED_Init()
{
	HAL_Delay(200); //这里的延时很重要
	
	if(OLED_Write_Cmd(0xAE)) //display off
		goto err;
	if(OLED_Write_Cmd(0x20))	//Set Memory Addressing Mode	
		goto err;
	if(OLED_Write_Cmd(0x10))	//00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
		goto err;
	if(OLED_Write_Cmd(0xb0))	//Set Page Start Address for Page Addressing Mode,0-7
		goto err;
	if(OLED_Write_Cmd(0xc8))	//Set COM Output Scan Direction
		goto err;
	if(OLED_Write_Cmd(0x00)) //---set low column address
		goto err;
	if(OLED_Write_Cmd(0x10)) //---set high column address
		goto err;
	if(OLED_Write_Cmd(0x40)) //--set start line address
		goto err;
	if(OLED_Write_Cmd(0x81)) //--set contrast control register
		goto err;
	if(OLED_Write_Cmd(0xff)) //亮度调节 0x00~0xff
		goto err;
	if(OLED_Write_Cmd(0xa1)) //--set segment re-map 0 to 127
		goto err;
	if(OLED_Write_Cmd(0xa6)) //--set normal display
		goto err;
	if(OLED_Write_Cmd(0xa8)) //--set multiplex ratio(1 to 64)
		goto err;
	if(OLED_Write_Cmd(0x3F)) //
		goto err;
	if(OLED_Write_Cmd(0xa4)) //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
		goto err;
	if(OLED_Write_Cmd(0xd3)) //-set display offset
		goto err;
	if(OLED_Write_Cmd(0x00)) //-not offset
		goto err;
	if(OLED_Write_Cmd(0xd5)) //--set display clock divide ratio/oscillator frequency
		goto err;
	if(OLED_Write_Cmd(0xf0)) //--set divide ratio
		goto err;
	if(OLED_Write_Cmd(0xd9)) //--set pre-charge period
		goto err;
	if(OLED_Write_Cmd(0x22)) //
		goto err;
	if(OLED_Write_Cmd(0xda)) //--set com pins hardware configuration
		goto err;
	if(OLED_Write_Cmd(0x12))
		goto err;
	if(OLED_Write_Cmd(0xdb)) //--set vcomh
		goto err;
	if(OLED_Write_Cmd(0x20)) //0x20,0.77xVcc
		goto err;
	if(OLED_Write_Cmd(0x8d)) //--set DC-DC enable
		goto err;
	if(OLED_Write_Cmd(0x14)) //
		goto err;
	if(OLED_Write_Cmd(0xaf)) //--turn on oled panel
		goto err;

	return 0;
err:
	return -1;
}

void OLED_SetPos(unsigned char x, unsigned char y) //设置起始点坐标
{ 
	OLED_Write_Cmd( 0xb0+y);
	OLED_Write_Cmd( ((x&0xf0)>>4)|0x10);
	OLED_Write_Cmd( (x&0x0f)|0x01);
}

void OLED_Fill(unsigned char fill_Data)//全屏填充
{
	unsigned char m,n;
	for(m=0;m<8;m++)
	{
		OLED_Write_Cmd(0xb0+m);		//page0-page1
		OLED_Write_Cmd(0x00);		//low column start address
		OLED_Write_Cmd(0x10);		//high column start address
		for(n=0;n<128;n++)
			{
				OLED_Write_Data(fill_Data);
			}
	}
}

void OLED_CLS()//清屏
{
	OLED_Fill( 0x00);
}

//--------------------------------------------------------------
// Prototype      : void OLED_ON(void)
// Calls          : 
// Parameters     : none
// Description    : 将OLED从休眠中唤醒
//--------------------------------------------------------------
void OLED_ON()
{
	OLED_Write_Cmd( 0X8D);  //设置电荷泵
	OLED_Write_Cmd( 0X14);  //开启电荷泵
	OLED_Write_Cmd( 0XAF);  //OLED唤醒
}

//--------------------------------------------------------------
// Prototype      : void OLED_OFF(void)
// Calls          : 
// Parameters     : none
// Description    : 让OLED休眠 -- 休眠模式下,OLED功耗不到10uA
//--------------------------------------------------------------
void OLED_OFF()
{
	OLED_Write_Cmd( 0X8D);  //设置电荷泵
	OLED_Write_Cmd( 0X10);  //关闭电荷泵
	OLED_Write_Cmd( 0XAE);  //OLED休眠
}

//--------------------------------------------------------------
// Prototype      : void OLED_ShowChar(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize)
// Calls          : 
// Parameters     : x,y -- 起始点坐标(x:0~127, y:0~7); ch[] -- 要显示的字符串; TextSize -- 字符大小(1:6*8 ; 2:8*16)
// Description    : 显示codetab.h中的ASCII字符,有6*8和8*16可选择
//--------------------------------------------------------------
void OLED_ShowStr(unsigned char x, 
			unsigned char y, unsigned char ch[], unsigned char TextSize)
{
	unsigned char c = 0,i = 0,j = 0;
	switch(TextSize)
	{
		case 1:
		{
			while(ch[j] != '\0')
			{
				c = ch[j] - 32;
				if(x > 126)
				{
					x = 0;
					y++;
				}
				OLED_SetPos( x,y);
				for(i=0;i<6;i++)
					OLED_Write_Data( F6x8[c][i]);
				x += 6;
				j++;
			}
		}break;
		case 2:
		{
			while(ch[j] != '\0')
			{
				c = ch[j] - 32;
				if(x > 120)
				{
					x = 0;
					y++;
				}
				OLED_SetPos( x,y);
				for(i=0;i<8;i++)
					OLED_Write_Data( F8X16[c*16+i]);
				OLED_SetPos( x,y+1);
				for(i=0;i<8;i++)
					OLED_Write_Data( F8X16[c*16+i+8]);
				x += 8;
				j++;
			}
		}break;
	}
}

//--------------------------------------------------------------
// Prototype      : void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N)
// Calls          : 
// Parameters     : x,y -- 起始点坐标(x:0~127, y:0~7); N:汉字在codetab.h中的索引
// Description    : 显示codetab.h中的汉字,16*16点阵
//--------------------------------------------------------------
void OLED_ShowCN(unsigned char x, unsigned char y, 
													unsigned char N)
{
	unsigned char wm=0;
	unsigned int  adder=32*N;
	OLED_SetPos( x , y);
	for(wm = 0;wm < 16;wm++)
	{
		OLED_Write_Data( F16x16[adder]);
		adder += 1;
	}
	OLED_SetPos( x,y + 1);
	for(wm = 0;wm < 16;wm++)
	{
		OLED_Write_Data( F16x16[adder]);
		adder += 1;
	}
}

//--------------------------------------------------------------
// Prototype      : void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[]);
// Calls          : 
// Parameters     : x0,y0 -- 起始点坐标(x0:0~127, y0:0~7); x1,y1 -- 起点对角线(结束点)的坐标(x1:1~128,y1:1~8)
// Description    : 显示BMP位图
//--------------------------------------------------------------
void OLED_DrawBMP(unsigned char x0,unsigned char y0,
				unsigned char x1,unsigned char y1,unsigned char BMP[])
{
	unsigned int j=0;
	unsigned char x,y;

  if(y1%8==0)
		y = y1/8;
  else
		y = y1/8 + 1;
	for(y=y0;y<y1;y++)
	{
		OLED_SetPos( x0,y);
    for(x=x0;x<x1;x++)
		{
			OLED_Write_Data( BMP[j++]);
		}
	}
}
