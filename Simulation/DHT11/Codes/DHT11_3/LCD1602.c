#include "regx52.h"
#include "LCD1602.h"
#include "intrins.h"

sbit LCD_RS = P2^6;
sbit LCD_RW = P2^5;
sbit LCD_E  = P2^7;

#define LCD_DataPort P0

void Delay1ms()		//@11.0592MHz 1ms
{
	unsigned char i, j;

	_nop_();
	i = 2;
	j = 199;
	do
	{
		while (--j);
	} while (--i);
}

void LCD_WriteCommand(unsigned char Command)
{
		LCD_RS = 0;
		LCD_RW = 0;
		LCD_DataPort = Command;
		LCD_E  = 1;
		Delay1ms();
		LCD_E  = 0;
		Delay1ms();
		
}

void LCD_WriteData(unsigned char Data)
{
		LCD_RS = 1;
		LCD_RW = 0;
		LCD_DataPort = Data;
		LCD_E  = 1;
		Delay1ms();
		LCD_E  = 0;
		Delay1ms();
		
}

void LCD_Init()
{
		LCD_WriteCommand(0x38);
		LCD_WriteCommand(0x0c);
		LCD_WriteCommand(0x06);
		LCD_WriteCommand(0x01);
}

void LCD_SetCursor(unsigned char Line,unsigned char Columu)
{
	if(Line == 1)
	{
		LCD_WriteCommand( 0x80 | (Columu-1) );
	}
	else
	{
		LCD_WriteCommand( 0x80 | ( Columu-1 )+0x40 );
	}
}


void LCD_ShowString(unsigned char Line , unsigned char Columu , unsigned char* String)
{
		unsigned char i = 0;
	
		LCD_SetCursor(Line,Columu);
	
		for(i=0;String[i] != '\0';i++)
		{
				LCD_WriteData(String[i]);
		}
}




