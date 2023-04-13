/*******************˵��:**************************
��ʵʱʱ������ͨ��LCD1602��ʾ
--------------------------------------------------
����ս��Ƭ��������51/AVR��Сϵͳ�弰DS1302
ʵʱʱ��ģ���д
�Ա����̵�ַ: http://zsmcu.taobao.com/
QQ:284083167
�ֻ�:15016732495
**************************************************/
#include <reg52.h>
#include <intrins.h>
#define uchar unsigned char
#define uint  unsigned int
uchar dis_time_buf[16]={0};
//LCD1602���Ŷ���
//����8λ���з�ʽ,DB0~DB7������LCDDATA0~LCDDATA7
sbit RS=P2^0;
sbit RW=P2^1;
sbit CS=P2^2;
#define LCDDATA P0
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
uchar time_buf[8] = {0x20,0x10,0x06,0x01,0x23,0x59,0x55,0x02};//��ʼʱ��2010��6��1��23��59��55�� ���ڶ�
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
//���ܣ�12us��ʱ
//STC89C52Ϊ1T��Ƭ��,��1��ʱ��/��������,�ٶ�ΪAT89C52��12��
void Delay_xus(uint t)	  		 		
{ 
  for(;t>0;t--)
   {
	 _nop_();
   }
}
//����LCDдʱ��
void LCD_en_write(void)       
{
   CS=1;    
    Delay_xus(20);
   CS=0;   
	Delay_xus(20);
}
  
//дָ���
void Write_Instruction(uchar command)
{
  RS=0;
  RW=0;
  CS=1;
  LCDDATA=command;
  LCD_en_write();//д��ָ������
}

//д���ݺ���
void Write_Data(uchar Wdata)
{
  RS=1;
  RW=0;
  CS=1;
  LCDDATA=Wdata;
  LCD_en_write();//д������
}

//�ַ���ʾ��ʼ��ַ����
void LCD_SET_XY(uchar X,uchar Y)
{
  uchar address;
  if(Y==0)
    address=0x80+X;//Y=0,��ʾ�ڵ�һ����ʾ����ַ����Ϊ0x80
  else 
    address=0xc0+X;//Y��0ʱ����ʱ�ڵڶ�����ʾ����ַ����Ϊ0xC0
  Write_Instruction(address);//дָ�������ʾ��ʼ��ַ
}
//�ڵ�X��Y�п�ʼ��ʾWdata����Ӧ�ĵ����ַ�
void LCD_write_char(uchar X,uchar Y,uchar Wdata)
{
  LCD_SET_XY(X,Y);//д��ַ
  Write_Data(Wdata);//д�뵱ǰ�ַ�����ʾ
}
//��������
void LCD_clear(void)
{
  Write_Instruction(0x01);
  Delay_xms(5);
}
//��ʾ����ʼ������
void LCD_init(void) 
{	
	Write_Instruction(0x38);				//8bit interface,2line,5*7dots
	Delay_xms(5);
	Write_Instruction(0x38);	
	Delay_xms(5);
	Write_Instruction(0x38);	

	Write_Instruction(0x08);	//����ʾ�����Թ�꣬��겻��˸
	Write_Instruction(0x01);	//����
	Delay_xms(5);
	
	Write_Instruction(0x04);	//дһ�ַ���������ʾ���ƶ�
	//Write_Instruction(0x05);	//дһ�ַ�����������
	//Write_Instruction(0x06);	//дһ�ַ���������ʾ���ƶ�
	//Write_Instruction(0x07);	//дһ�ַ�����������
	Delay_xms(5);
	
	//Write_Instruction(0x0B);	//�ر���ʾ������ʾ�ַ���ֻ�б�������
	Write_Instruction(0x0C);	//����ʾ����ꡢ��˸���ر�
	//Write_Instruction(0x0D);	//����ʾ������ʾ��꣬�������˸
	//Write_Instruction(0x0E);	//����ʾ����ʾ��꣬����겻��˸
	//Write_Instruction(0x0F);	//����ʾ����ꡢ��˸����ʾ
}
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
	//ds1302_write_byte(ds1302_charger_add,0xa9);	    //������ 
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
void Display(void)
{
   
   LCD_write_char(3,0,dis_time_buf[0]+'0');
   
   LCD_write_char(4,0,dis_time_buf[1]+'0');
   
   LCD_write_char(5,0,dis_time_buf[2]+'0');
   
   LCD_write_char(6,0,dis_time_buf[3]+'0');

   LCD_write_char(7,0,'/');
   
   
   LCD_write_char(8,0,dis_time_buf[4]+'0');
   
   LCD_write_char(9,0,dis_time_buf[5]+'0');

   LCD_write_char(10,0,'/');

   
   LCD_write_char(11,0,dis_time_buf[6]+'0');
   
   LCD_write_char(12,0,dis_time_buf[7]+'0');
   
   
   LCD_write_char(15,0,dis_time_buf[14]+'0');
   //��2����ʾ  
   
   LCD_write_char(3,1,dis_time_buf[8]+'0');
  
   LCD_write_char(4,1,dis_time_buf[9]+'0');

   LCD_write_char(5,1,':');

  
   LCD_write_char(6,1,dis_time_buf[10]+'0');
   
   LCD_write_char(7,1,dis_time_buf[11]+'0');

   LCD_write_char(8,1,':');

   
   LCD_write_char(9,1,dis_time_buf[12]+'0');
   
   LCD_write_char(10,1,dis_time_buf[13]+'0');
}
//��ʱ���жϺ���
void Timer2() interrupt 5	  //��ʱ��2��5���ж�
{
 static uchar t;
 TF2=0;
 t++;
 if(t==4)               //���200ms(50ms*4)��ȡһ��ʱ��
  {
   t=0;
   ds1302_read_time();  //��ȡʱ�� 
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
   
  }
}
//��ʱ��2��ʼ��
void Init_timer2(void)
{
 RCAP2H=0x3c;//��T2��ʼֵ0x3cb0�����20��Ϊ1��,ÿ�����ʱ��Ϊ50ms
 RCAP2L=0xb0;
 TR2=1;	     //������ʱ��2
 ET2=1;		 //�򿪶�ʱ��2�ж�
 EA=1;		 //�����ж�
}


//������
void main(void)
{ 
 Delay_xms(50);//�ȴ�ϵͳ�ȶ�
 LCD_init();   //LCD��ʼ��
 LCD_clear();  //����   
 ds1302_init();  //DS1302��ʼ��
 Delay_xms(10);
 ds1302_write_time(); //д���ʼֵ
 Init_timer2(); //��ʱ��2��ʼ�� 
 while(1)
  {  
   Display();
  }
}
