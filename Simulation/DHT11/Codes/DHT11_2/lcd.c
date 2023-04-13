#include"lcd.h"
 
void Lcd1602_Write_Cmd(unsigned char cmd)     //写命令
{
    //Read_Busy();
    LCD1602_RS = 0;
    LCD1602_RW = 0;
    LCD1602_DB = cmd;
		LCD_Delay10ms(1);
    LCD1602_EN = 1;
		LCD_Delay10ms(1);
    LCD1602_EN = 0;    
}
 
void Lcd1602_Write_Data(unsigned char dat)   //写数据
{
      //Read_Busy();
      LCD1602_RS = 1;
      LCD1602_RW = 0;
      LCD1602_DB = dat;
			LCD_Delay10ms(1);
      LCD1602_EN = 1;
			LCD_Delay10ms(1);
      LCD1602_EN = 0;
}
//指定位置开始显示数据！
void LcdSetCursor(unsigned char x,unsigned char y)  //坐标显示
{
    unsigned char addr;
    if(y == 0)
        addr = 0x00 + x;//第一行开始，x表示一行的第x个
    else
        addr = 0x40 + x;//第二行开始，x表示一行的第x个
    
    Lcd1602_Write_Cmd(addr|0x80);
}
 
void LcdShowStr(unsigned char x,unsigned char y,unsigned char *str)     //显示字符串
{
    LcdSetCursor(x,y);      //当前字符的坐标
    while(*str != '\0')
    {
        Lcd1602_Write_Data(*str++);
    }
}
 
void InitLcd1602()              //1602初始化
{
    Lcd1602_Write_Cmd(0x38);    //打开，5*8,8位数据
    Lcd1602_Write_Cmd(0x0c);
    Lcd1602_Write_Cmd(0x06);
    Lcd1602_Write_Cmd(0x01);    //清屏   
}
 
void LCD_Delay10ms(unsigned int c)   //误差 0us
{
    unsigned char a,b;
    for(;c>0;c--)
        for(b=38;b>0;b--)
            for(a=130;a>0;a--);
}