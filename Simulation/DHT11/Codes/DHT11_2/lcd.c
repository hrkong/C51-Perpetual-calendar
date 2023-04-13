#include"lcd.h"
 
void Lcd1602_Write_Cmd(unsigned char cmd)     //д����
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
 
void Lcd1602_Write_Data(unsigned char dat)   //д����
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
//ָ��λ�ÿ�ʼ��ʾ���ݣ�
void LcdSetCursor(unsigned char x,unsigned char y)  //������ʾ
{
    unsigned char addr;
    if(y == 0)
        addr = 0x00 + x;//��һ�п�ʼ��x��ʾһ�еĵ�x��
    else
        addr = 0x40 + x;//�ڶ��п�ʼ��x��ʾһ�еĵ�x��
    
    Lcd1602_Write_Cmd(addr|0x80);
}
 
void LcdShowStr(unsigned char x,unsigned char y,unsigned char *str)     //��ʾ�ַ���
{
    LcdSetCursor(x,y);      //��ǰ�ַ�������
    while(*str != '\0')
    {
        Lcd1602_Write_Data(*str++);
    }
}
 
void InitLcd1602()              //1602��ʼ��
{
    Lcd1602_Write_Cmd(0x38);    //�򿪣�5*8,8λ����
    Lcd1602_Write_Cmd(0x0c);
    Lcd1602_Write_Cmd(0x06);
    Lcd1602_Write_Cmd(0x01);    //����   
}
 
void LCD_Delay10ms(unsigned int c)   //��� 0us
{
    unsigned char a,b;
    for(;c>0;c--)
        for(b=38;b>0;b--)
            for(a=130;a>0;a--);
}