
#include <reg52.h>
#include "oled.h"

#include <intrins.h>
#define uchar unsigned char
#define uint  unsigned int

typedef unsigned char BYTE;
typedef unsigned int WORD;



uchar dis_time_buf[16]={0};
uchar dis_time_buf2[16]={0};
uint i;

uint flag=5;//���ó�ʼʱ��  ��ʼ�ڡ��֡���
uint flag2=1;//��������ʱ��
uchar data_byte; 
uchar RH,RL,TH,TL; 
sbit io=P0^7;  //dht11data�˽ӵ�Ƭ����P0^7��// 
sbit k1=P2^1;	 //����P31����k1   ����ѡ������
sbit k2=P2^2;	 //����P32����k2    �Ӽ�
sbit k3=P2^3;	 //����P33����k3    ����
sbit k4=P2^4;	 //����P34����k4    �������ü�
sbit LED=P2^7;       //����2.7�Ǹ��ƻ��߷���������������
sbit LED2=P2^6;       //����2.6�Ǹ��ƻ��߷�������������
sbit BEEP=P1^7;		 //������
sbit DOUT=P2^0;	 //���嵥Ƭ��P2�ڵĵ�1λ ����P2.0��Ϊ���������������
 


//DS1302���Ŷ���
sbit RST=P1^3;
sbit IO=P1^2;
sbit SCK=P1^1;
//DS1302��ַ����
#define ds1302_sec_add			0x80		//�����ݵ�ַ
#define ds1302_min_add			0x82		//�����ݵ�ַ
#define ds1302_hr_add			0x84		//ʱ���ݵ�ַ
#define ds1302_date_add			0x86		//�����ݵ�ַ
#define ds1302_month_add		0x88		//�����ݵ�ַ
#define ds1302_day_add			0x8a		//�������ݵ�ַ
#define ds1302_year_add			0x8c		//�����ݵ�ַ
#define ds1302_control_add		0x8e		//�������ݵ�ַ
#define ds1302_charger_add		0x90 					 
#define ds1302_clkburst_add		0xbe
//��ʼʱ�䶨��
uchar time_buf[8] = {0x20,0x22,0x5,0x011,0x18,0x34,0x00,0x03};//��ʼʱ��2022��5��10��20��29��30�� ����1
uchar time_buf2[8] = {0x20,0x22,0x5,0x05,0x09,0x30,0x05,0x05};//��ʼ����2022��4��25��20��29��30�� ����1

void receive();
//����:��ʱ1����
//��ڲ���:x
//���ڲ���:��
//˵��:������Ϊ12Mʱ��j<112��������Ϊ11.0592Mʱ��j<122
void Delay_xms(uint x)
{
  uint i,j;
  for(i=0;i<x;i++)
    for(j=0;j<112;j++);
}
//----------------------------��ʱ�ӳ���---------------------------------
//���ܣ�12us��ʱ
//STC89C52Ϊ1T��Ƭ��,��1��ʱ��/��������,�ٶ�ΪAT89C52��12��
void Delay_xus(uint t)	  		 		
{ 
  for(;t>0;t--)
   {
	 _nop_();
   }
}
void delay1()//һ��forѭ�������Ҫ8����������ڪ�һ����������Ϊ1us������Ϊ12MHz����Ҳ����˵��������ʱ8us�ડ����ʱ�����������΢��ȷһ�㪢 

{ 

  uchar i; 

  for(i=0;i<1;i++); 

} 
 void delay(uchar ms) //��ʱģ��// 

{ 

  uchar i; 

  while(ms--)                 

        

 for(i=0;i<100;i++); 

} 

//--------------------------BCDת��----------------------
uchar BCD_Decimal(uchar bcd)//BCD��תʮ���ƺ���������BCD������ʮ����
{
 	uchar Decimal;
 	Decimal=bcd>>4;
 	return(Decimal=Decimal*10+(bcd&=0x0F));
}



//----------------------------ʱ��DS1302ģ���ӳ���---------------------------------
//DS1302��ʼ������
void ds1302_init(void) 
{
	RST=0;			//RST���õ�
	SCK=0;			//SCK���õ�
}
//��DS1302д��һ�ֽ�����
void ds1302_write_byte(uchar addr, uchar d) 
{
	uchar i;
	RST=1;					//����DS1302����	
	//д��Ŀ���ַ��addr
	addr = addr & 0xFE;   //���λ���㣬�Ĵ���0λΪ0ʱд��Ϊ1ʱ��
	for (i = 0; i < 8; i ++) {
		if (addr & 0x01) {
			IO=1;
			}
		else {
			IO=0;
			}
		SCK=1;      //����ʱ��
		SCK=0;
		addr = addr >> 1;
		}	
	//д�����ݣ�d
	for (i = 0; i < 8; i ++) {
		if (d & 0x01) {
			IO=1;
			}
		else {
			IO=0;
			}
		SCK=1;    //����ʱ��
		SCK=0;
		d = d >> 1;
		}
	RST=0;		//ֹͣDS1302����
}

//��DS1302����һ�ֽ�����
uchar ds1302_read_byte(uchar addr) {

	uchar i,temp;	
	RST=1;					//����DS1302����
	//д��Ŀ���ַ��addr
	addr = addr | 0x01;    //���λ�øߣ��Ĵ���0λΪ0ʱд��Ϊ1ʱ��
	for (i = 0; i < 8; i ++) {
		if (addr & 0x01) {
			IO=1;
			}
		else {
			IO=0;
			}
		SCK=1;
		SCK=0;
		addr = addr >> 1;
		}	
	//������ݣ�temp
	for (i = 0; i < 8; i ++) {
		temp = temp >> 1;
		if (IO) {
			temp |= 0x80;
			}
		else {
			temp &= 0x7F;
			}
		SCK=1;
		SCK=0;
		}	
	RST=0;					//ֹͣDS1302����
	return temp;
}
//��DS302д��ʱ������
void ds1302_write_time(void) 
{
	ds1302_write_byte(ds1302_control_add,0x00);			//�ر�д���� 
	ds1302_write_byte(ds1302_sec_add,0x80);				//��ͣʱ�� 
	ds1302_write_byte(ds1302_charger_add,0xa9);	    //������ 
	ds1302_write_byte(ds1302_year_add,time_buf[1]);		//�� 
	ds1302_write_byte(ds1302_month_add,time_buf[2]);	//�� 
	ds1302_write_byte(ds1302_date_add,time_buf[3]);		//�� 
	ds1302_write_byte(ds1302_hr_add,time_buf[4]);		//ʱ 
	ds1302_write_byte(ds1302_min_add,time_buf[5]);		//��
	ds1302_write_byte(ds1302_sec_add,time_buf[6]);		//��
	ds1302_write_byte(ds1302_day_add,time_buf[7]);		//�� 
	ds1302_write_byte(ds1302_control_add,0x80);			//��д����     
}
//��DS302����ʱ������
void ds1302_read_time(void)  
{
	time_buf[1]=ds1302_read_byte(ds1302_year_add);		//�� 
	time_buf[2]=ds1302_read_byte(ds1302_month_add);		//�� 
	time_buf[3]=ds1302_read_byte(ds1302_date_add);		//�� 
	time_buf[4]=ds1302_read_byte(ds1302_hr_add);		//ʱ 
	time_buf[5]=ds1302_read_byte(ds1302_min_add);		//�� 
	time_buf[6]=(ds1302_read_byte(ds1302_sec_add))&0x7f;//�룬������ĵ�7λ�����ⳬ��59
	time_buf[7]=ds1302_read_byte(ds1302_day_add);		//�� 	
}


//----------------------------����  �ж�---------------------------------


//��ʱ��2��ʼ��
void Init_timer2(void)
{
 RCAP2H=0x3c;//��T2��ʼֵ��Ϊ0x3cb0�����20��Ϊ1��,ÿ�����ʱ��Ϊ50ms ��Ҫ����!!!��
 RCAP2L=0xb5;
 TR2=1;	     //������ʱ��2
 ET2=1;		 //�򿪶�ʱ��2�ж�
 EA=1;		 //�����ж�
}



//��ʱ���жϺ���
void Timer2() interrupt 5	  //��ʱ��2��6���ж�
{
 static uchar t;
 TF2=0;
 t++;
 if(t==4)               //���200ms(50ms*4)��ȡһ��ʱ��
  {
   t=0;
   //ds1302_init();
   //Delay_xms(10);
   ds1302_read_time();  //��ȡʱ�� 
   //ds1302_init();
   Delay_xms(10);
   receive();
   dis_time_buf[0]=(time_buf[0]>>4); //��   
   dis_time_buf[1]=(time_buf[0]&0x0f);
   
   dis_time_buf[2]=(time_buf[1]>>4);   
   dis_time_buf[3]=(time_buf[1]&0x0f);
  

   
   
   dis_time_buf[4]=(time_buf[2]>>4); //��  
   dis_time_buf[5]=(time_buf[2]&0x0f);
   

   dis_time_buf[6]=(time_buf[3]>>4); //��   
   dis_time_buf[7]=(time_buf[3]&0x0f);
   
   dis_time_buf[14]=(time_buf[7]&0x07); //����
   
   //��2����ʾ  
   dis_time_buf[8]=(time_buf[4]>>4); //ʱ   
   dis_time_buf[9]=(time_buf[4]&0x0f);   

  

   dis_time_buf[10]=(time_buf[5]>>4); //��   
   dis_time_buf[11]=(time_buf[5]&0x0f);   

   dis_time_buf[12]=(time_buf[6]>>4); //��   
   dis_time_buf[13]=(time_buf[6]&0x0f);


   //-----------------------------------------------

   dis_time_buf2[0]=(time_buf2[0]>>4); //��   
   dis_time_buf2[1]=(time_buf2[0]&0x0f);
   
   dis_time_buf2[2]=(time_buf2[1]>>4);   
   dis_time_buf2[3]=(time_buf2[1]&0x0f);
  

   
   
   dis_time_buf2[4]=(time_buf2[2]>>4); //��  
   dis_time_buf2[5]=(time_buf2[2]&0x0f);
   

   dis_time_buf2[6]=(time_buf2[3]>>4); //��   
   dis_time_buf2[7]=(time_buf2[3]&0x0f);
   
   dis_time_buf2[14]=(time_buf2[7]&0x07); //����
   
   //��2����ʾ  
   dis_time_buf2[8]=(time_buf2[4]>>4); //ʱ   
   dis_time_buf2[9]=(time_buf2[4]&0x0f);   

  

   dis_time_buf2[10]=(time_buf2[5]>>4); //��   
   dis_time_buf2[11]=(time_buf2[5]&0x0f);   

   dis_time_buf2[12]=(time_buf2[6]>>4); //��   
   dis_time_buf2[13]=(time_buf2[6]&0x0f);
   
  }
}

//----------------------------0.96OLED/LCD��ʾ���ܳ���---------------------------------


void Displaytime(void)
{
   
   OLED_ShowNum(0,0,dis_time_buf[0],1,16);
    OLED_ShowNum(8,0,dis_time_buf[1],1,16);
	 OLED_ShowNum(16,0,dis_time_buf[2],1,16);
	  OLED_ShowNum(24,0,dis_time_buf[3],1,16);
	OLED_ShowChar(32,0,'/');
	   OLED_ShowNum(40,0,dis_time_buf[4],1,16);
	    OLED_ShowNum(48,0,dis_time_buf[5],1,16);
	OLED_ShowChar(56,0,'/');
		 OLED_ShowNum(64,0,dis_time_buf[6],1,16);
		  OLED_ShowNum(72,0,dis_time_buf[7],1,16);
	OLED_ShowCHinese(90,0,5);
		   OLED_ShowNum(106,0,dis_time_buf[14],1,16);
				  //�ڶ���
			
			OLED_ShowCHinese(13,2,3);
			OLED_ShowCHinese(31,2,4);	  
		    OLED_ShowNum(0+47,2,dis_time_buf[8],1,16);
    OLED_ShowNum(8+47,2,dis_time_buf[9],1,16);
	OLED_ShowChar(16+47,2,':');
	 OLED_ShowNum(24+47,2,dis_time_buf[10],1,16);
	  OLED_ShowNum(32+47,2,dis_time_buf[11],1,16);
	OLED_ShowChar(40+47,2,':');
	   OLED_ShowNum(48+47,2,dis_time_buf[12],1,16);
	    OLED_ShowNum(56+47,2,dis_time_buf[13],1,16);
}

void Displaytime2(void)
{
   
   OLED_ShowNum(0,0,dis_time_buf2[0],1,16);
    OLED_ShowNum(8,0,dis_time_buf2[1],1,16);
	 OLED_ShowNum(16,0,dis_time_buf2[2],1,16);
	  OLED_ShowNum(24,0,dis_time_buf2[3],1,16);
	OLED_ShowChar(32,0,'/');
	   OLED_ShowNum(40,0,dis_time_buf2[4],1,16);
	    OLED_ShowNum(48,0,dis_time_buf2[5],1,16);
	OLED_ShowChar(56,0,'/');
		 OLED_ShowNum(64,0,dis_time_buf2[6],1,16);
		  OLED_ShowNum(72,0,dis_time_buf2[7],1,16);
	OLED_ShowCHinese(90,0,5);
		   OLED_ShowNum(106,0,dis_time_buf2[14],1,16);
				  //�ڶ���
			
			OLED_ShowCHinese(3,2,6);	 //��
			OLED_ShowCHinese(21,2,7);	 //�� 
		    OLED_ShowNum(0+47,2,dis_time_buf2[8],1,16);
    OLED_ShowNum(8+47,2,dis_time_buf2[9],1,16);
	OLED_ShowChar(16+47,2,':');
	 OLED_ShowNum(24+47,2,dis_time_buf2[10],1,16);
	  OLED_ShowNum(32+47,2,dis_time_buf2[11],1,16);
	OLED_ShowChar(40+47,2,':');
	   OLED_ShowNum(48+47,2,dis_time_buf2[12],1,16);
	    OLED_ShowNum(56+47,2,dis_time_buf2[13],1,16);
}









void displayRT(void)
{
   OLED_ShowCHinese(0,4,2);
   OLED_ShowCHinese(16,4,1);
   OLED_ShowChar(32,4,':');
   OLED_ShowNum(40,4,RH/10,1,16);
   OLED_ShowNum(48,4,RH%10,1,16);
   OLED_ShowCHinese(0,6,0);
   OLED_ShowCHinese(16,6,1);
   OLED_ShowChar(32,6,':');
   OLED_ShowNum(40,6,TH/10,1,16);
   OLED_ShowNum(48,6,TH%10,1,16);
}
//----------------------------DHT11ģ�鹦���ӳ���---------------------------------
uchar receive_byte()//����һ���ֽ�// 

{ 

  uchar i,temp2; 

  for(i=0;i<8;i++)//����8bit������ 

  { 

 	while(!io);//�ȴ�50us�ĵ͵�ƽ��ʼ�źŽ��� 

 	delay1();//��ʼ�źŽ���֮����ʱ26us-28us������������ʱ������ 

	 delay1(); 

 	delay1(); 

 	temp2=0;//ʱ��Ϊ26us-28us����ʾ���յ�Ϊ����'0' 

 	if(io==1) 

	 temp2=1; //���26us-28us֮�󪣻�Ϊ�ߵ�ƽ�����ʾ���յ�����Ϊ'1' 

    while(io);//�ȴ������źŸߵ�ƽ��'0'Ϊ26us-28us��'1'Ϊ70us�� 

 	data_byte<<=1;//���յ�����Ϊ��λ��ǰ�����ƪ� 

	 data_byte|=temp2; 

  } 

  return data_byte; 

} 
void start()//��ʼ�ź� 

{ 

  io=1; 

  delay1(); 

  io=0; 

  delay(25);// �������������ͱ������18ms����֤DHT11�ܼ�⵽��ʼ�ź� 

  io=1;    //���Ϳ�ʼ�źŽ��������ߵ�ƽ��ʱ20-40us 

  delay1();//����������ʱ�������Ϊ24us������Ҫ�� 

  delay1(); 

  delay1();

  delay1(); 

} 
void receive()//��������// 

{ 

  uchar T_H,T_L,R_H,R_L,check,num_check,i; 

  start();//��ʼ�ź�// 

  io=1;   //������Ϊ���몣�жϴӻ���DHT11����Ӧ�ź� 

  if(!io)//�жϴӻ��Ƿ��е͵�ƽ��Ӧ�ź�// 

  {  
	while(!io);//�жϴӻ����� 80us �ĵ͵�ƽ��Ӧ�ź��Ƿ����// 
	//Delay_xus(20);

	while(io);//�жϴӻ����� 80us �ĸߵ�ƽ�Ƿ������������������������ݽ���״̬ 

	R_H=receive_byte();//ʪ�ȸ�λ 

 	R_L=receive_byte();//ʪ�ȵ�λ 

	T_H=receive_byte();//�¶ȸ�λ 

	T_L=receive_byte();//�¶ȵ�λ 

	check=receive_byte();//У��λ 

	io=0; //�����һbit���ݽ���Ϻ󪣴ӻ����͵�ƽ50us// 

	for(i=0;i<7;i++)//���50us����ʱ 

	delay1(); 

	io=1;//�����������������ߪ��������״̬ 

	num_check=R_H+R_L+T_H+T_L; 

	if(num_check==check)//�ж϶������ĸ�����֮���Ƿ���У��λ��ͬ 

 	{ 

   RH=R_H; 

   RL=R_L; 

   TH=T_H; 

   TL=T_L; 

   check=num_check; 

 	} 

  } 

} 



//-------------------------------����----------------------
void keypros1()
{
	if(k1==0)		  //��ⰴ��K1�Ƿ���
	{	
		Delay_xms(10);   //�������� һ���Լ10ms
		if(k1==0)	 //�ٴ��жϰ����Ƿ���
		{
			flag+=1;
			if(flag==8) {flag=1;}
		}
		while(!k1);	 //��ⰴ���Ƿ��ɿ�
	}		
}

void keypros4()
{
	if(k4==0)		  //��ⰴ��K1�Ƿ���
	{	
		Delay_xms(10);   //�������� һ���Լ10ms
		if(k4==0)	 //�ٴ��жϰ����Ƿ���
		{
			flag2=-flag2;
			
		}
		while(!k4);	 //��ⰴ���Ƿ��ɿ�
	}		
}





void keypros2()
{
	

	if(k2==0)		  //��ⰴ��K1�Ƿ���
	{	
		Delay_xms(10);   //�������� һ���Լ10ms
		if(k2==0)	 //�ٴ��жϰ����Ƿ���
		{
		 if(flag2==1)
		 {
		  ds1302_read_time();
		  time_buf[flag]+=0x01;
		  ds1302_init();
		  Delay_xms(10);															 
		  ds1302_write_time();
		  }
		  else{time_buf2[flag]+=0x01;}
		/*
		 ds1302_read_time();
		 ds1302_write_byte(ds1302_control_add,0x00);			//�ر�д���� 
		 ds1302_write_byte(ds1302_sec_add,0x80);				//��ͣʱ�� 
		 ds1302_write_byte(ds1302_charger_add,0xa9);	    //������ 
		 ds1302_write_byte(ds1302_year_add,time_buf[flag]+1);		//��Ӧflagλ��λ
		 ds1302_write_byte(ds1302_control_add,0x80);			//��д���� 
		 */

		}
		while(!k2);	 //��ⰴ���Ƿ��ɿ�
	}		
}
void keypros3()
{
	

	if(k3==0)		  //��ⰴ��K3�Ƿ���
	{	
		Delay_xms(10);   //�������� һ���Լ10ms
		if(k3==0)	 //�ٴ��жϰ����Ƿ���
		{
		if(flag2==1)  
		{
		  ds1302_read_time();
		  time_buf[flag]-=0x01;
		  ds1302_init();
		  Delay_xms(10);															 
		  ds1302_write_time();
		 }
		 else 
		 {	time_buf2[flag]-=0x01;}
		 
		/*
		 ds1302_read_time();
		 ds1302_write_byte(ds1302_control_add,0x00);			//�ر�д���� 
		 ds1302_write_byte(ds1302_sec_add,0x80);				//��ͣʱ�� 
		 ds1302_write_byte(ds1302_charger_add,0xa9);	    //������ 
		 ds1302_write_byte(ds1302_year_add,time_buf[flag]+1);		//��Ӧflagλ��λ
		 ds1302_write_byte(ds1302_control_add,0x80);			//��д���� 
		 */

		}
		while(!k3);	 //��ⰴ���Ƿ��ɿ�
	}		
}



//----------------------------������---------------------------------
//������
void main(void)
{  
	 //u8 temp=0,humi=0;
	 //u8 temp_buf[3],humi_buf[3];
	  BEEP=0;


//-----------------------��ʼ��-----------------------------

 //DHT11_Init();
 Delay_xms(50);//�ȴ�ϵͳ�ȶ�
 ds1302_init();  //DS1302��ʼ��
 Delay_xms(10);
 ds1302_write_time(); //д���ʼֵ
 Init_timer2(); //��ʱ��2��ʼ�� 
 	OLED_Init();
	OLED_Clear;

 while(1)
  {


//-----------------------ʱ������-----------------------------
  LED=1;
  LED2=1;
  keypros1();
  keypros2();
  keypros3();
  keypros4();




//-----------------------ʱ����ʾ-----------------------------
  //Delay_xms(20);
  Displaytime();
  //Delay_xms(20);
	 //s+=0.001 ;
  //Delay_xms(200000);
  
	   OLED_ShowChar(70,4,'f');
	   OLED_ShowChar(78,4,'l');
	   OLED_ShowChar(86,4,'a');
	   OLED_ShowChar(94,4,'g');
       OLED_ShowNum(78,6,flag,1,16);
	   //OLED_ShowNum(80,6,BCD_Decimal(time_buf[6]),2,16);

//-----------------------�����趨-----------------------------
	   if(flag2!=1)
	   {
	    OLED_Clear();
		Delay_xms(1000);
				
		while(1)
		{
		OLED_ShowChar(70,4,'f');
	   OLED_ShowChar(78,4,'l');
	   OLED_ShowChar(86,4,'a');
	   OLED_ShowChar(94,4,'g');
       OLED_ShowNum(78,6,flag,1,16);
	   //OLED_ShowNum(80,6,BCD_Decimal(time_buf[6]),2,16);
	   Displaytime2();
		keypros1();
  		keypros2();
  		keypros3();
		 keypros4();
		 if(flag2==1){break;}
		}
		OLED_Clear();
	   }


//-----------------------------------------------��������----
	   if(time_buf[4]==time_buf2[4]&&time_buf[5]==time_buf2[5]&&time_buf[6]==time_buf2[6]) 
	   {
	     //����   ��ʱ��
		 BEEP=0;
		 LED2=0;
		 i=80;
		 //Delay_xms(5000);
		  while(i) 
		 	  {
			  	i--;
		 		Displaytime();	  //���Ƶ�ʱ�� ��֤ʱ���������
		   	  }
		 BEEP=1;
		 LED2=1;
	   }

//----------------------------------- ��ʪ��--------------------------
	 
	  // i2++;
	  //if(i2%30==0)
	  //{
		 //DHT11_Read_Data(&temp,&humi);
	   	 //receive();//������ʪ������	 �Ƶ��ж����ȡ��ʪ�ȣ���Ȼ����Ī������
	  //}
	  //Delay_xms(1);
	  displayRT();
	
 
//-----------------------����------------------------------- 
  
   if(DOUT==0)//��Ũ�ȸ����趨ֵʱ ��ִ����������
	 {
        Delay_xms(500);//��ʱ������
		if(DOUT==0)//ȷ�� Ũ�ȸ����趨ֵʱ ��ִ����������
	    {
		 LED=0;	   //����P1.0�ڵ�(��������������)
		}	  
  	 }

//-----------------------------------------------------------


  }
}
