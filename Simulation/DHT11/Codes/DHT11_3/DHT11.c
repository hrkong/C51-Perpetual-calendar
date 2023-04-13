#include <REGX52.H>
#include <stdio.h>
#include "DHT11.h"
#include "LCD1602.h"

sbit DHT11_data = P3^6;

unsigned int rec_data[4];
unsigned char rec_data_lcd0[6];
unsigned char rec_data_lcd1[6];
unsigned char rec_data_lcd2[6];
unsigned char rec_data_lcd3[6];


void DHT11_delay_us(unsigned char n)
{
		while(--n);
}

void DHT11_delay_ms(unsigned int z)
{
		unsigned int i,j;
		for(i=z;i>0;i++)
			for(j=110;j>0;j--);
}


void DHT11_UART_Init()
{
		SCON = 0x40;
		REN  = 1;
		
		TMOD = 0x20;
		TH1  = 0xF3;
		TL1  = 0xF3;
		
		TI   = 1;
		TR1  = 1;
}

void DHT11_Start()
{
		DHT11_data = 1;
		DHT11_delay_us(30);
		
		DHT11_data = 0;
		DHT11_delay_ms(20);
		
		DHT11_data = 1;
		DHT11_delay_us(30);
}

unsigned char DHT11_Rec_Byte()
{
		unsigned char i = 0;
		unsigned char Data;
		
		for(i=0;i<8;i++)
		{
				while(!DHT11_data);
				DHT11_delay_us(30);
				
				Data<<=1;
				
				if(DHT11_data==1)
				{
						Data+=1;
				}
				
				while(DHT11_data);
		}
		
		return Data;
}

void DHT11_Receive()
{
		unsigned int R_H,R_L,T_H,T_L;
		unsigned char RH,RL,TH,TL,CHECK;
		
		DHT11_Start();
		DHT11_data = 1;
		
		if(DHT11_data==0)
		{
				while(DHT11_data==0);	   //等待从机80us低电平响应信号过去
				while(DHT11_data==1);
				
				R_H = DHT11_Rec_Byte();
				R_L = DHT11_Rec_Byte();
				T_H = DHT11_Rec_Byte();
				T_L = DHT11_Rec_Byte();
				CHECK = DHT11_Rec_Byte();
				
				DHT11_data=0;
				DHT11_delay_us(25);
				DHT11_data=1;
				
				if((R_H+R_L+T_H+T_L) == CHECK)
				{
						RH = R_H;
						RL = R_L;
						TH = T_H;
						TL = T_L;
				}
		}
		
		rec_data[0] = RH;
		rec_data[1] = RL;
		rec_data[2] = TH;
		rec_data[3] = TL;
}


void main()
{
		DHT11_UART_Init();
		LCD_Init();
		LCD_ShowString(1,1,"Humi:");
		LCD_ShowString(2,1,"Temp:");
		
		while(1)
		{
				DHT11_delay_ms(1000);
				DHT11_Receive();
				
				sprintf(rec_data_lcd0,"%d",rec_data[0]);
				sprintf(rec_data_lcd1,"%d",rec_data[1]);
				sprintf(rec_data_lcd2,"%d",rec_data[2]);
				sprintf(rec_data_lcd3,"%d",rec_data[3]);
				
				DHT11_delay_ms(100);
				
				LCD_ShowString(1,6,rec_data_lcd0);
				LCD_ShowString(1,8,".");
				LCD_ShowString(1,9,rec_data_lcd1);
				LCD_ShowString(1,15,"%");
				
				LCD_ShowString(2,6,rec_data_lcd2);
				LCD_ShowString(2,8,".");
				LCD_ShowString(2,9,rec_data_lcd3);
				LCD_ShowString(2,15,"C");
				
				printf("Humi:%d.%d \n",rec_data[0],rec_data[1]); //通过串口
				printf("Temp:%d.%d °C\n",rec_data[2],rec_data[3]);

		}
}

 