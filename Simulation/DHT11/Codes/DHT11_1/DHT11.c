 /*-----------------------------------------------
  ��ʾģ�����Ŷ��壺1-VSS 2-VDD 3-V0 4-RS 5-R/W 6-E 7-14 DB0-DB7 15-BLA 16-BLK
------------------------------------------------*/
#include <reg52.h> //����ͷ�ļ���һ���������Ҫ�Ķ���ͷ�ļ��������⹦�ܼĴ����Ķ���
#include <intrins.h>
#include <math.h>
#define uchar unsigned char
#define uint unsigned int	
sbit RS = P1^0;   //����˿� 
sbit RW = P1^1;
sbit EN = P2^5;
sbit DU = P2^0;
sbit WE = P2^1;
 
sbit Data = P3^6;//����������
uchar rec_dat[15];   //������ʾ�Ľ�����������
 
#define RS_CLR RS=0 
#define RS_SET RS=1
 
#define RW_CLR RW=0 
#define RW_SET RW=1 
 
#define EN_CLR EN=0
#define EN_SET EN=1
 
#define DataPort P0
 
 
void cmg88()//������ܣ�������
{
DU=1;  
P0=0X00;
DU=0;
}
/*------------------------------------------------
 uS��ʱ����������������� unsigned char t���޷���ֵ
 unsigned char �Ƕ����޷����ַ���������ֵ�ķ�Χ��
 0~255 ����ʹ�þ���12M����ȷ��ʱ��ʹ�û��,������ʱ
 �������� T=tx2+5 uS 
------------------------------------------------*/
void DelayUs2x(unsigned char t)
{   
 while(--t);
}
/*------------------------------------------------
 mS��ʱ����������������� unsigned char t���޷���ֵ
 unsigned char �Ƕ����޷����ַ���������ֵ�ķ�Χ��
 0~255 ����ʹ�þ���12M����ȷ��ʱ��ʹ�û��
------------------------------------------------*/
void DelayMs(unsigned char t)
{
     
 while(t--)
 {
     //������ʱ1mS
     DelayUs2x(245);
	 DelayUs2x(245);
 }
}
/*------------------------------------------------
              ��æ����
------------------------------------------------*/
 bit LCD_Check_Busy(void) 
 { 
 DataPort= 0xFF; 
 RS_CLR; 
 RW_SET; 
 EN_CLR; 
 _nop_(); 
 EN_SET;
 return (bit)(DataPort & 0x80);
 }
/*------------------------------------------------
              д�������
------------------------------------------------*/
 void LCD_Write_Com(unsigned char com) 
 {  
 while(LCD_Check_Busy()); //æ��ȴ�
 RS_CLR; 
 RW_CLR; 
 EN_SET; 
 DataPort= com; 
 _nop_(); 
 EN_CLR;
 }
/*------------------------------------------------
              д�����ݺ���
------------------------------------------------*/
 void LCD_Write_Data(unsigned char Data) 
 { 
 while(LCD_Check_Busy()); //æ��ȴ�
 RS_SET; 
 RW_CLR; 
 EN_SET; 
 DataPort= Data; 
 _nop_();
 EN_CLR;
 }
 
/*------------------------------------------------
                ��������
------------------------------------------------*/
 void LCD_Clear(void) 
 { 
 LCD_Write_Com(0x01); 
 DelayMs(5);
 }
/*------------------------------------------------
              д���ַ�������
------------------------------------------------*/
 void LCD_Write_String(unsigned char x,unsigned char y,unsigned char *s) 
 {     
 if (y == 0) 
 	{     
	 LCD_Write_Com(0x80 + x);     //��ʾ��һ��
 	}
 else 
 	{      
 	LCD_Write_Com(0xC0 + x);      //��ʾ�ڶ���
 	}        
 while (*s) 
 	{     
 LCD_Write_Data( *s);     
 s ++;     
 	}
 }
/*------------------------------------------------
              д���ַ�����
------------------------------------------------*/
 void LCD_Write_Char(unsigned char x,unsigned char y,unsigned char Data) 
 {     
 if (y == 0) 
 	{     
 	LCD_Write_Com(0x80 + x);     
 	}    
 else 
 	{     
 	LCD_Write_Com(0xC0 + x);     
 	}        
 LCD_Write_Data( Data);  
 }
/*------------------------------------------------
              ��ʼ������
------------------------------------------------*/
 void LCD_Init(void) 
 {
   LCD_Write_Com(0x38);    /*��ʾģʽ����*/ 
   DelayMs(5); 
   LCD_Write_Com(0x38); 
   DelayMs(5); 
   LCD_Write_Com(0x38); 
   DelayMs(5); 
   LCD_Write_Com(0x38);  
   LCD_Write_Com(0x08);    /*��ʾ�ر�*/ 
   LCD_Write_Com(0x01);    /*��ʾ����*/ 
   LCD_Write_Com(0x06);    /*��ʾ����ƶ�����*/ 
   DelayMs(5); 
   LCD_Write_Com(0x0C);    /*��ʾ�����������*/
   }
   
 
/*------------------------------------------------
              DHT11��ʱ����
------------------------------------------------*/
void DHT11_delay_us(uchar n)
{
    while(--n);
}
 
void DHT11_delay_ms(uint z)
{
   uint i,j;
   for(i=z;i>0;i--)
      for(j=110;j>0;j--);
}
/*------------------------------------------------
              DHT11��ʼ�ź�
------------------------------------------------*/
void DHT11_start()
{
   Data=1;
   DHT11_delay_us(2);
   Data=0;
   DHT11_delay_ms(20);   //��ʱ18ms����
   Data=1;
   DHT11_delay_us(30);
}
/*------------------------------------------------
              ���հ�λ������
------------------------------------------------*/
uchar DHT11_rec_byte()      //����һ���ֽ�
{
   uchar i,dat=0;
  for(i=0;i<8;i++)    //�Ӹߵ������ν���8λ����
   {          
      while(!Data);		//�ȴ�50us�͵�ƽ��ȥ
      DHT11_delay_us(8);     //��ʱ60us�������Ϊ��������Ϊ1������Ϊ0 
      dat<<=1;           //��λʹ��ȷ����8λ���ݣ�����Ϊ0ʱֱ����λ
      if(Data==1)    //����Ϊ1ʱ��ʹdat��1����������1
         dat+=1;
      while(Data);  //�ȴ�����������    
    }  
    return dat;
}
/*------------------------------------------------
              ����40bit����
------------------------------------------------*/
void DHT11_receive()      //����40λ������
{
    uchar R_H,R_L,T_H,T_L,RH,RL,TH,TL,revise; 
    DHT11_start();
 
    if(Data==0)
    {
	
        while(Data==0);   //�ȴ�����     
		
        DHT11_delay_us(40);  //���ߺ���ʱ80us
        R_H=DHT11_rec_byte();    //����ʪ�ȸ߰�λ  
        R_L=DHT11_rec_byte();    //����ʪ�ȵͰ�λ  
        T_H=DHT11_rec_byte();    //�����¶ȸ߰�λ  
        T_L=DHT11_rec_byte();    //�����¶ȵͰ�λ
        revise=DHT11_rec_byte(); //����У��λ
			 
        DHT11_delay_us(25);    //����
 
        if((R_H+R_L+T_H+T_L)==revise)      //У��
        {
            RH=R_H;
            RL=R_L;
            TH=T_H;
            TL=T_L;
        } 
        /*���ݴ���������ʾ*/
        rec_dat[0]='0'+(RH/10);
        rec_dat[1]='0'+(RH%10);
		rec_dat[2] =' ';
		rec_dat[3] = 'R' ;
		rec_dat[4] = 'H';
        rec_dat[5]=' ';
        rec_dat[6]=' ';
        rec_dat[7]=' ';
        rec_dat[8]=' ';
        rec_dat[9]='0'+(TH/10);
        rec_dat[10]='0'+(TH%10);
		rec_dat[11] =' ';
		rec_dat[12] = 'C';
		rec_dat[13] = ' ' ;
        rec_dat[14]=' ';
    }
}
 
 
/*------------------------------------------------
                    ������
------------------------------------------------*/ 
void main(void) 
{ 
	uint i;
	cmg88();//������ܣ�������
 
	LCD_Init(); 
	LCD_Clear();//����
	DHT11_delay_ms(1500);    //DHT11�ϵ��Ҫ�ȴ�1S��Խ�����ȶ�״̬�ڴ��ڼ䲻�ܷ����κ�ָ��
	while (1) 
 	{ 
       	DHT11_receive();
	    
	  
		for(i=0;i<15;i++)
		{
	   		LCD_Write_Char(i,1,rec_dat[i]);
		}        
  		//LCD_Write_Char(0,0,'T');
  		//LCD_Write_Char(0,1,'H');
 	  	//LCD_Write_String(0,1,"HELLO");
 	}
}
 