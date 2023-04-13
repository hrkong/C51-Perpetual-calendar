/*
ʹ��DHT11ģ�飬�����ʪ�ȣ���lcd1602������ʾ����Ϊ1602��ʾ����ÿ�ζ��ܳ��������Լ��˴��ڴ�ӡ�������ֱ�ۡ�
*/
#include <reg51.h>
#include "lcd.h"
#include <intrins.h>
#include <stdio.h>
 
sbit Temp_data=P3^6;	//�ź�����
 
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
 
//������
void main()
{	
	InitUART();	//�����жϳ�ʼ��
	P1=0xf0;
	InitLcd1602();
	LcdShowStr(0,0,"Shidu:");
	LcdShowStr(0,1,"Wendu:");
 
	while(1)
	{
		DHT11_delay_ms(150);
		DHT11_receive();
 
		//��������Ǵ�ӡ�ַ�������Ӧ��������ֵת�������á�
		sprintf(rec_dat_lcd0,"%d",rec_dat[0]);
		sprintf(rec_dat_lcd1,"%d",rec_dat[1]);
		sprintf(rec_dat_lcd2,"%d",rec_dat[2]);
		sprintf(rec_dat_lcd3,"%d",rec_dat[3]);
		DHT11_delay_ms(100);
		
		//ʪ��
		LcdShowStr(7,0,rec_dat_lcd0);
		LcdShowStr(9,0,".");
		LcdShowStr(10,0,rec_dat_lcd1);
		LcdShowStr(11,0,"%");
		
		//�¶�
		LcdShowStr(7,1,rec_dat_lcd2);
		LcdShowStr(9,1,".");
		LcdShowStr(10,1,rec_dat_lcd3);
		LcdShowStr(11,1,"C");
			
		//����ͨ���������ִ�ӡ�¶�
		//��ҲΪʲô�������жϷ��������Ϳ��Դ�ӡ���Ǻ���Ҫ����жϸ�����ǿ���ֱ�Ӵ�ӡ�������ҵ����ʡ�
		printf("Humi:%d.%d \n",rec_dat[0],rec_dat[1]);
		printf("Temp:%d.%d ��C\n",rec_dat[2],rec_dat[3]);			
	}
}
 
//DHT11��ʼ�ź�
void DHT11_start()	
{
	Temp_data=1;
	
	DHT11_delay_us(2);
	
	Temp_data=0;
	
	DHT11_delay_ms(20);
	
	Temp_data=1;
	
	DHT11_delay_us(13);
}
 
//����һ���ֽ�
unsigned char DHT11_rec_byte()
{	
	unsigned char i;
	unsigned char dat=0;  //0000 0000

		
	for(i=0;i<8;i++)  //ѭ��8��
	{
		while(!Temp_data); //�ȴ�DHT 50us�͵�ƽ�źŹ�ȥ
		DHT11_delay_us(30);//��ʱ���30us ��λ��Ǹߵ�ƽ ��Ϊ1 
		

		dat<<=1; //����һλ ��Ϊ�����ǴӸ�λ��ʼ��
		if(Temp_data == 1)
		{
			dat|=0x01;
		}
		else
		{
			dat|=0x00;
		}	 
	
		while(Temp_data); //�ȴ�DHT ���� ׼����һλ��ʼ

	}
	return dat;
}
 
//������ʪ������
void DHT11_receive()
{
	unsigned int R_H,R_L,T_H,T_L;
	unsigned char RH,RL,TH,TL,revise;
	
	DHT11_start();
	Temp_data=1;
	if(Temp_data==0)
	{
		while(Temp_data==0);   //�ȴ�����     
        //DHT11_delay_us(40);  //���ߺ���ʱ80us
		
        R_H=DHT11_rec_byte();    //����ʪ�ȸ߰�λ  
        R_L=DHT11_rec_byte();    //����ʪ�ȵͰ�λ  
        T_H=DHT11_rec_byte();    //�����¶ȸ߰�λ  
        T_L=DHT11_rec_byte();    //�����¶ȵͰ�λ
        revise=DHT11_rec_byte(); //����У��λ
 
        //DHT11_delay_us(25);    //����
 
        if((R_H+R_L+T_H+T_L)==revise)      //У��
        {
            RH=R_H;
            RL=R_L;
            TH=T_H;
            TL=T_L;
	
        } 
        /*���ݴ���������ʾ*/
        rec_dat[0]=RH;
        rec_dat[1]=RL;
        rec_dat[2]=TH;
        rec_dat[3]=TL;
	}	
}
//��ʱus   --2*n+5us
void DHT11_delay_us(unsigned char n)
{
    while(--n);
}
 
//��ʱms
void DHT11_delay_ms(unsigned int z)
{
   unsigned int i,j;
   for(i=z;i>0;i--)
      for(j=110;j>0;j--);
}
 
//ʹ�ö�ʱ��1��Ϊ���ڲ����ʷ�����
void InitUART(void)
{
	TMOD=0x20;					//��ʱ��1�Ĺ�����ʽ2
	SCON=0x40;					//����ͨ�Ź�����ʽ1
	REN=1;						//�������
	TH1=0xFD,TL1=0xFD;		//������=9600
	TI=1;                       //����һ��Ҫע��
	TR1=1;	
	EA = 1;                  //�����ж�
}	