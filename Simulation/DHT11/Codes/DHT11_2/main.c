/*
使用DHT11模块，检测温湿度，在lcd1602上面显示，因为1602显示不是每次都能出来，所以加了串口打印，会更加直观。
*/
#include <reg51.h>
#include "lcd.h"
#include <intrins.h>
#include <stdio.h>
 
sbit Temp_data=P3^6;	//信号引脚
 
unsigned int rec_dat[4];
unsigned char rec_dat_lcd0[6];
unsigned char rec_dat_lcd1[6];
unsigned char rec_dat_lcd2[6];
unsigned char rec_dat_lcd3[6];
 
void DHT11_delay_us(unsigned char n);
void DHT11_delay_ms(unsigned int z);
void DHT11_start();
unsigned char DHT11_rec_byte();
void DHT11_receive();
void InitUART(void);
 
//主函数
void main()
{	
	InitUART();	//串口中断初始化
	P1=0xf0;
	InitLcd1602();
	LcdShowStr(0,0,"Shidu:");
	LcdShowStr(0,1,"Wendu:");
 
	while(1)
	{
		DHT11_delay_ms(150);
		DHT11_receive();
 
		//这个函数是打印字符函数，应该是起到数值转化的作用。
		sprintf(rec_dat_lcd0,"%d",rec_dat[0]);
		sprintf(rec_dat_lcd1,"%d",rec_dat[1]);
		sprintf(rec_dat_lcd2,"%d",rec_dat[2]);
		sprintf(rec_dat_lcd3,"%d",rec_dat[3]);
		DHT11_delay_ms(100);
		
		//湿度
		LcdShowStr(7,0,rec_dat_lcd0);
		LcdShowStr(9,0,".");
		LcdShowStr(10,0,rec_dat_lcd1);
		LcdShowStr(11,0,"%");
		
		//温度
		LcdShowStr(7,1,rec_dat_lcd2);
		LcdShowStr(9,1,".");
		LcdShowStr(10,1,rec_dat_lcd3);
		LcdShowStr(11,1,"C");
			
		//下面通过串口助手打印温度
		//我也为什么他不用中断服务函数，就可以打印，那好需要这个中断干嘛，不是可以直接打印吗？这是我的疑问。
		printf("Humi:%d.%d \n",rec_dat[0],rec_dat[1]);
		printf("Temp:%d.%d °C\n",rec_dat[2],rec_dat[3]);			
	}
}
 
//DHT11起始信号
void DHT11_start()	
{
	Temp_data=1;
	
	DHT11_delay_us(2);
	
	Temp_data=0;
	
	DHT11_delay_ms(20);
	
	Temp_data=1;
	
	DHT11_delay_us(13);
}
 
//接收一个字节
unsigned char DHT11_rec_byte()
{	
	unsigned char i;
	unsigned char dat=0;  //0000 0000

		
	for(i=0;i<8;i++)  //循环8次
	{
		while(!Temp_data); //等待DHT 50us低电平信号过去
		DHT11_delay_us(30);//延时大概30us 如何还是高电平 则为1 
		

		dat<<=1; //左移一位 因为数据是从高位开始传
		if(Temp_data == 1)
		{
			dat|=0x01;
		}
		else
		{
			dat|=0x00;
		}	 
	
		while(Temp_data); //等待DHT 拉低 准备下一位开始

	}
	return dat;
}
 
//接收温湿度数据
void DHT11_receive()
{
	unsigned int R_H,R_L,T_H,T_L;
	unsigned char RH,RL,TH,TL,revise;
	
	DHT11_start();
	Temp_data=1;
	if(Temp_data==0)
	{
		while(Temp_data==0);   //等待拉高     
        //DHT11_delay_us(40);  //拉高后延时80us
		
        R_H=DHT11_rec_byte();    //接收湿度高八位  
        R_L=DHT11_rec_byte();    //接收湿度低八位  
        T_H=DHT11_rec_byte();    //接收温度高八位  
        T_L=DHT11_rec_byte();    //接收温度低八位
        revise=DHT11_rec_byte(); //接收校正位
 
        //DHT11_delay_us(25);    //结束
 
        if((R_H+R_L+T_H+T_L)==revise)      //校正
        {
            RH=R_H;
            RL=R_L;
            TH=T_H;
            TL=T_L;
	
        } 
        /*数据处理，方便显示*/
        rec_dat[0]=RH;
        rec_dat[1]=RL;
        rec_dat[2]=TH;
        rec_dat[3]=TL;
	}	
}
//延时us   --2*n+5us
void DHT11_delay_us(unsigned char n)
{
    while(--n);
}
 
//延时ms
void DHT11_delay_ms(unsigned int z)
{
   unsigned int i,j;
   for(i=z;i>0;i--)
      for(j=110;j>0;j--);
}
 
//使用定时器1作为串口波特率发生器
void InitUART(void)
{
	TMOD=0x20;					//定时器1的工作方式2
	SCON=0x40;					//串口通信工作方式1
	REN=1;						//允许接收
	TH1=0xFD,TL1=0xFD;		//波特率=9600
	TI=1;                       //这里一定要注意
	TR1=1;	
	EA = 1;                  //开总中断
}	