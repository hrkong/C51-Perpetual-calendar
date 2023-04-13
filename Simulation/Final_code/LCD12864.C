#include<reg52.h>
#include<intrins.h>
#include "LCD12864.h"


/*�˿ڶ���*/
#define LCD_data P0
sbit LCD_RS=P2^6;
sbit LCD_RW=P2^5;
sbit LCD_EN=P2^7;
sbit LCD_PSB=P3^2;
sbit LCD_RST=P3^3;


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

/*дָ�����ݵ�LCD
RS=L������ʾDB0-DB7Ϊ��ʾָ������
RW=L������ʾDB0-DB7���ݱ�write����E=��H-L��,ָ�����ݱ�д��IR��DR)
E=������
��ʱDB0-DB7=ָ����	*/
void write_cmd(uchar cmd)
{
	LCD_RS=0;			// ������
	LCD_RW=0;			// д
	LCD_EN=0;			// ʹ��
	P0=cmd;
	delay_ms(5);
	LCD_EN=1;				// д�������	
	delay_ms(5);		// д��ȴ�
	LCD_EN=0;			// ʧ��
}
/*д��ʾ���ݵ�LCD*/
/*
	RS=H ������ʾDB0-DB7Ϊ��ʾ����
	RW=L ����RW=L,E='H-L',DB0-DB7���ݱ�д��IR��DR	
	E=������
	DB0-DB7=��ʾ����	
*/
void write_dat(uchar dat)
{
	LCD_RS=1;			// ����
	LCD_RW=0;			// д����
	LCD_EN=1;			// ʹ��
	delay_ms(5);		// д��ȴ�
	P0=dat;				// д������	
	delay_ms(5);		
	LCD_EN=0;			// ʧ��
}

/*x��ʾһ���ַ�������*/
void lcd12864_printf(uchar x, uchar y,uchar n,uchar *buf)
{
	uchar pos,i;
	switch(x)
	{
		case 1 : pos = 0x80+y;break;   	// ��һ��
		case 2 : pos = 0x90+y;break; 	// �ڶ���
		case 3 : pos = 0x88+y;break;	// ������	
		case 4 : pos = 0x98+y;break;	// ������
		default: break;
	}
	write_cmd(pos);						// д��λ����Ϣ
	for(i = 0;i<n;i++)
	{
		write_dat(buf[i]); 				// д����ʾ����
	}
}
/*LCD��ʼ��*/
void lcd_init()
{
	delay_ms(10);
	LCD_PSB=1;//���ڷ�ʽ
	LCD_RST = 0;
	delay_ms(25);
	LCD_RST = 1;
	write_cmd(0x30);//��������ָ��
	delay_ms(5);
	write_cmd(0x0C);//����ʾ�����ر�
	//delay_ms(5);
	write_cmd(0x06);//д�������ݺ������ƣ���ʾ�����ƶ�
	//delay_ms(5);
	write_cmd(0x01);//���LCD��ʾ����
	//delay_ms(5);
}