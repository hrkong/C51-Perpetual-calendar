#include "reg52.h"
#include "dhtXX.h"
# include <intrins.h>
# include <stdio.h>
unsigned int temp;
unsigned int humi;
unsigned char check;
unsigned char RH,RL,TH,TL;

unsigned char data_byte; 
/*******************************************************************************
* �� �� ��       : void Delay10ms()		//@11.0592MHz
* ��������		 : �����ʱ����
* ��    ��       : ��
* ��    ��       : ��
*******************************************************************************/
void delay(unsigned char ms) //��ʱģ��// 

{ 

  unsigned char i; 

  while(ms--)                 
  for(i=0;i<100;i++); 

}
/*
void Delay10ms()		//@11.0592MHz
{
	unsigned char i, j;

	i = 108;
	j = 145;
	do
	{
		while (--j);
	} while (--i);
}
*/
void delay1()//һ��forѭ�������Ҫ8����������ڪ�һ����������Ϊ1us������Ϊ12MHz����Ҳ����˵��������ʱ8us�ડ����ʱ�����������΢��ȷһ�㪢 

{ 

  unsigned char i; 

  for(i=0;i<1;i++); 

} 
void delay2()//27���������ڣ�12M���񣬺�����ʱΪ27us�� 

{ 
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
} 
/*******************************************************************************
* �� �� ��       : void Start(void)
* ��������		 : ��ʼ�ź�
* ��    ��       : ��
* ��    ��       : ��
*******************************************************************************/
void start()//��ʼ�ź� 

{ 

  DAT=1; 

  delay1(); 

  DAT=0; 

  delay(25);// �������������ͱ������18ms����֤DHT11�ܼ�⵽��ʼ�ź� 

  DAT=1;    //���Ϳ�ʼ�źŽ��������ߵ�ƽ��ʱ20-40us 

  delay2();//����������ʱ�������Ϊ24us������Ҫ�� 
} 


/*void Start(void)
{
	DAT = 0;
	Delay10ms();
	Delay10ms();
	DAT = 1;
}
*/
/*******************************************************************************
* �� �� ��       : unsigned char ReadByte(void)
* ��������		 : ��ȡһ���ֽ�����
* ��    ��       : ��
* ��    ��       : ��
*******************************************************************************/

/*unsigned char ReadOneByte(void)
{
	unsigned char i,j=0,dat=0;
	for(i=0;i<8;i++)
	{
		while(!DAT);		// �ȴ��ߵ�ƽ
		while(DAT)			// �ߵ�ƽ����ʱ�����
		{
			i++;			// һ��ѭ��Ϊ5��ָ�����ڣ�Լ5us
			
		}
		if(i>6)			// 0--24~28us��1 >70us				
		{
			dat|=0x01;
		}
		dat<<=1;		
	}
	return dat;
}
*/


unsigned char receive_byte()//����һ���ֽ�// 

{ 

  unsigned char i,temp; 

  for(i=0;i<8;i++)//����8bit������ 

  { 

 	while(!DAT);//�ȴ�50us�ĵ͵�ƽ��ʼ�źŽ��� 

 	delay2();//��ʼ�źŽ���֮����ʱ26us-28us������������ʱ������  

 	temp=0;//ʱ��Ϊ26us-28us����ʾ���յ�Ϊ����'0' 

 	if(DAT==1) 

	 temp=1; //���26us-28us֮�󪣻�Ϊ�ߵ�ƽ�����ʾ���յ�����Ϊ'1' 

    while(DAT);//�ȴ������źŸߵ�ƽ��'0'Ϊ26us-28us��'1'Ϊ70us�� 

 	data_byte<<=1;//���յ�����Ϊ��λ��ǰ�����ƪ� 

	 data_byte|=temp; 

  } 

  return data_byte; 
} 

/*******************************************************************************
* �� �� ��       : void Start(void)
* ��������		 : ��ʼ�ź�
* ��    ��       : ��
* ��    ��       : ��
*******************************************************************************/
//void ReadData(unsigned int *temp,unsigned int *humi)
/*
void ReadData(void)
{
	unsigned char i;
	unsigned int t,h,add;
	Start();
	while(DAT);				// �ȴ���Ӧ
	while(!DAT)
	{
			
		i++;
	}
	if(i<15)				// ��Ӧ�ź�ӦΪ80us
	{
		
		return;
	}
	while(DAT);				// �ȴ���������
	h = ReadOneByte();			// ��ȡ�¶ȸ�8λ
	h<<=8;	 
	h+= ReadOneByte();			// ��ȡ�¶ȵ�8λ
	t = ReadOneByte();			// ��ȡʪ�ȸ�8λ
	t<<=8;
	t+= ReadOneByte();			// ��ȡʪ�ȵ�8λ
	check = ReadOneByte();		// ��ȡУ����
	add = (t>>8)+(t&0xff)+(h>>8)+(h&0xff);		// ��У����
	if(check!=(add&0xff))		// ����У�����Ƿ���ȷ
	{
		return;
	}
 	if (DHT==1)					// DHT11����ת��
	temp = t>>8;
	humi = h>>8;
	LED = 0;

}
*/


void receive()//�������� 

{ 

  unsigned char T_H,T_L,R_H,R_L,check,num_check,i; 

  start();//��ʼ�ź�// 

  DAT=1;   //������Ϊ���몣�жϴӻ���DHT11����Ӧ�ź� 

  if(!DAT)//�жϴӻ��Ƿ��е͵�ƽ��Ӧ�ź�// 

  {  
	while(!DAT);//�жϴӻ����� 80us �ĵ͵�ƽ��Ӧ�ź��Ƿ���� 

	while(DAT);//�жϴӻ����� 80us �ĸߵ�ƽ�Ƿ������������������������ݽ���״̬ 

	R_H=receive_byte();//ʪ�ȸ�λ 

 	R_L=receive_byte();//ʪ�ȵ�λ 

	T_H=receive_byte();//�¶ȸ�λ 

	T_L=receive_byte();//�¶ȵ�λ 

	check=receive_byte();//У��λ 

	DAT=0; //�����һbit���ݽ���Ϻ󪣴ӻ����͵�ƽ50us// 

	for(i=0;i<7;i++)//���50us����ʱ 

	delay1(); 

	DAT=1;//�����������������ߪ��������״̬ 

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

