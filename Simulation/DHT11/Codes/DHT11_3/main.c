#include "regx52.h"
#include "intrins.h"
#include "LCD1602.h"
#include "DHT11.h"
#include <stdio.h>

extern unsigned int rec_data[4];
extern unsigned char rec_data_lcd0[6];
extern unsigned char rec_data_lcd1[6];
extern unsigned char rec_data_lcd2[6];
extern unsigned char rec_data_lcd3[6];

void Delay500ms()		//@11.0592MHz
{
	unsigned char i, j, k;

	_nop_();
	i = 4;
	j = 129;
	k = 119;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}

void main()
{
		DHT11_UART_Init();
		LCD_Init();
		LCD_ShowString(1,1,"Humi:");
		LCD_ShowString(2,1,"Temp:");
		
		while(1)
		{
				Delay1000ms();;
				DHT11_Receive();
				
				sprintf(rec_data_lcd0,"%d",rec_data[0]);
				sprintf(rec_data_lcd1,"%d",rec_data[1]);
				sprintf(rec_data_lcd2,"%d",rec_data[2]);
				sprintf(rec_data_lcd3,"%d",rec_data[3]);
				
				Delay100ms();
				
				LCD_ShowString(1,6,rec_data_lcd0);
				LCD_ShowString(1,8,".");
				LCD_ShowString(1,9,rec_data_lcd1);
				LCD_ShowString(1,15,"%");
				
				LCD_ShowString(2,6,rec_data_lcd2);
				LCD_ShowString(2,8,".");
				LCD_ShowString(2,9,rec_data_lcd3);
				LCD_ShowString(2,15,"C");
				
//				printf("Humi:%d.%d \n",rec_data[0],rec_data[1]); //Í¨¹ý´®¿Ú
//				printf("Temp:%d.%d ¡ãC\n",rec_data[2],rec_data[3]);

		}
}
