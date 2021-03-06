#ifndef __OLED_H
#define __OLED_H


int OLED_Write_Cmd(unsigned char cmd);
int OLED_Write_Data(unsigned char data);
int OLED_Init();
void OLED_SetPos(unsigned char x, unsigned char y);
void OLED_Fill(unsigned char fill_Data);
void OLED_CLS();
void OLED_ON();
void OLED_OFF();
void OLED_ShowStr(unsigned char x, 
			unsigned char y, unsigned char ch[], unsigned char TextSize);
void OLED_ShowCN(unsigned char x, unsigned char y, 
													unsigned char N);
void OLED_DrawBMP(unsigned char x0,unsigned char y0,
				unsigned char x1,unsigned char y1,unsigned char BMP[]);

#endif // !__OLED_H