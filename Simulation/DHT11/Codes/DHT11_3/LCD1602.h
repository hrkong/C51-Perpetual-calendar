#ifndef _LCD1602_h_
#define _LCD1602_h_

void Delay1ms(void);
void LCD_WriteCommand(unsigned char Command);
void LCD_WriteData(unsigned char Data);
void LCD_Init(void);
void LCD_SetCursor(unsigned char Line,unsigned char Columu);
void LCD_ShowChar(unsigned char Line,unsigned char Columu,unsigned char Char);
void LCD_ShowString(unsigned char Line,unsigned char Columu,unsigned char* String);
void LCD_ShowNum(unsigned char Line,unsigned char Columu,unsigned int Num,unsigned int Length);

#endif
