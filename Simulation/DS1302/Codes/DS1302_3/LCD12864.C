#include<reg52.h>
#include<intrins.h>
#include "LCD12864.h"


/*端口定义*/
#define LCD_data P0
sbit LCD_RS=P2^7;
sbit LCD_RW=P2^6;
sbit LCD_EN=P2^5;
sbit LCD_PSB=P2^4;
sbit LCD_RST=P2^3;


void delay_ms(uint x)
{
	int i,j;

	for(j = 0; j < x; j++)
	{
		for(i = 0; i < 110; i++)
		{
			;
		}
	}
}

/*写指令数据到LCD
RS=L——表示DB0-DB7为显示指令数据
RW=L——表示DB0-DB7数据被write（当E=“H-L”,指令数据被写到IR或DR)
E=高脉冲
此时DB0-DB7=指令码	*/
void write_cmd(uchar cmd)
{
	LCD_RS=0;			// 控制字
	LCD_RW=0;			// 写

	P0=cmd;				// 写入控制字
	delay_ms(5);
	LCD_EN=1;			// 使能
	delay_ms(5);		// 写入等待
	LCD_EN=0;			// 失能
}
/*写显示数据到LCD*/
/*
	RS=H ——表示DB0-DB7为显示数据
	RW=L ——RW=L,E='H-L',DB0-DB7数据被写到IR或DR	
	E=高脉冲
	DB0-DB7=显示数据	
*/
void write_dat(uchar dat)
{
	LCD_RS=1;			// 数据
	LCD_RW=0;			// 写操作

	P0=dat;				// 写入数据
	delay_ms(5);
	LCD_EN=1;			// 使能
	delay_ms(5);		// 写入等待	
	LCD_EN=0;			// 失能
}

/*x显示一串字符或文字*/
void lcd12864_printf(uchar x, uchar y,uchar n,uchar *buf)
{
	uchar pos,i;
	switch(x)
	{
		case 1 : pos = 0x80+y;break;   	// 第一行
		case 2 : pos = 0x90+y;break; 	// 第二行
		case 3 : pos = 0x88+y;break;	// 第三行	
		case 4 : pos = 0x98+y;break;	// 第四行
		default: break;
	}
	write_cmd(pos);						// 写入位置信息
	for(i = 0;i<n;i++)
	{
		write_dat(buf[i]); 				// 写入显示数据
	}
}
/*LCD初始化*/
void lcd_init()
{
	delay_ms(10);
	LCD_PSB=1;//并口方式
//	LCD_RST = 0;
//	delay_ms(25);
	LCD_RST = 1;
	write_cmd(0x30);//基本操作指令
	delay_ms(5);
	write_cmd(0x0C);//打开显示，光标关闭
	delay_ms(5);
	write_cmd(0x01);//清除LCD显示类容
	delay_ms(5);
}
