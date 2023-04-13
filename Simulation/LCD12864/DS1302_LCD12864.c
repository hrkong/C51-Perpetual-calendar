/*************************************************************
�������ƣ�LCD12864��ʱ����ʾ��DS1302��
��		д��Τ��ı
��дʱ�䣺2011/1/20
����FCOS��12Mhz
*************************************************************/
#include <intrins.h>
#include <DS1302.h>

#define	uc	unsigned	char
#define	ui	unsigned int
#define	nop()	_nop_()
#define	DB	P0	

sbit RS=P2^7;				
sbit RW=P2^6;			
sbit EN=P2^5;
sbit PSB=P2^4;
sbit RST=P2^2;

uc year,month,day,week,hour,min,sec;//��������ʱ�������

void delay(uc t)
{
 	uc i,j;
	for(i=0;i<t;i++)
		for(j=0;j<121;j++)	nop();
}
//SCK��������д�����ݣ��½��ض�ȡ���ݣ����ݾ����ȵ�λ�����λ
//д����
void DS1302_Write_Byte(uc wdata_temp) 
{
	uc i;
	for (i=8;i>0;i--)				//ѭ��8�� д������
	{ 	
		SDA=wdata_temp&0x01;		//ȡ�����ݵ����λ��д��������
		CLK=0;						//ʱ��������		
		CLK=1; 						//ʱ�������ߣ�����ʱ�ӵ�������		
		wdata_temp>>=1;			//��������һλ								
	}
} 
//������
uc DS1302_Read_Byte(void) 
{
	uc i;
	uc Rdata_temp=0;
	for (i=8;i>0;i--)       //ѭ��8�� ��ȡ����
	{
		if(SDA)	  
		Rdata_temp|=0x80;		//ÿ�δ�����ֽ�
		CLK=1;
		CLK=0;
		Rdata_temp>>=1;		//����һλ	
	} 
	return (ACC);
}
//��ָ����ַд���ݺ���
void DS1302_Write(uc address,uc dat)
{
 	CLK=0;				nop();
	DS1302_RST=0;		nop();nop();
 	DS1302_RST=1;	  					//����
 	DS1302_Write_Byte(address);	//���͵�ַ
 	DS1302_Write_Byte(dat);			//��������
	CLK=0;
 	DS1302_RST=0;  					//�ָ�
}
//��ָ����ַ��һ�ֽ����ݺ���
uc DS1302_Read(uc address)
{
 	uc temp=0x00; 	
 	CLK=0;
	DS1302_RST=0;nop();
 	DS1302_RST=1;nop();
 	DS1302_Write_Byte(address);
	temp=DS1302_Read_Byte();
 	DS1302_RST=0;	_nop_();
	CLK=0;
	return (temp);			//����ֵ
}
//��ȡDS1302��ʱ�亯��,����BCD��ת��Ϊʮ����
void DS1302_Read_RTC(void)
{
	sec=	((DS1302_Read(r_sec)&0x7f)>>4)*10	+(DS1302_Read(r_sec)&0x0f);	
	min=	((DS1302_Read(r_min)&0x7f)>>4)*10	+(DS1302_Read(r_min)&0x0f);		
	hour=	((DS1302_Read(r_hour)&0x3f)>>4)*10	+(DS1302_Read(r_hour)&0x0f);		
	day=	(DS1302_Read(r_day)>>4)*10				+(DS1302_Read(r_day)&0x0f);		
	month=((DS1302_Read(r_month)&0x3f)>>4)*10	+(DS1302_Read(r_month)&0x0f);			
	week=	(DS1302_Read(r_week)&0x07);			
	year=	((DS1302_Read(r_year)&0xff)>>4)*10	+(DS1302_Read(r_year)&0x0f);
}
//�趨DS1302��ʱ�亯��
void DS1302_Set(uc s,m,h,d,mo,y,w)	
{	
	DS1302_Write(w_control,	0x00);
	DS1302_Write(w_sec,		(s/10<<4)|(s%10));		//д��
	DS1302_Write(w_min,		(m/10<<4)|(m%10));		//д��
	DS1302_Write(w_hour,		(h/10<<4)|(h%10));		//дСʱ
	DS1302_Write(w_day,		(d/10<<4)|(d%10));		//д��
	DS1302_Write(w_month,	(mo/10<<4)|(mo%10));		//д��
	DS1302_Write(w_year,		(y/10<<4)|(y%10));		//д�� 
	DS1302_Write(w_week,		(w/10<<4)|(w%10)); 		//д����
	DS1302_Write(w_control,	0x80);	//����DS1302�Ĵ�����ֹд������
}
//12864�ж�æµ״̬
void LCD_busy_check()			
{
	RS=0;
	RW=1;
	delay(1);
	EN=1;
	DB=0xff;
	while((DB&0x80)==0x80);
	EN=0;
}
//12864д����
void LCD_w_com(uc comm)														  
{
	LCD_busy_check();
	RS=0;
	RW=0;
	EN=1;	
	DB=comm;	
	delay(1);		
	EN=0;	
	delay(1);			//��EN������
}
//12864д����
void LCD_w_data(int dat)	
{
	LCD_busy_check();
	RS=1;
	RW=0;
	EN=1;
	DB=dat;	
	delay(1);	
	EN=0;
	delay(1);			//��EN������
}
//д�ַ�
void LCD_w_hz(uc x,uc y,uc code *s)
{
	LCD_busy_check();
	switch(x)
	{
		case 1 :LCD_w_com(0x80|y);	break;
		case 2 :LCD_w_com(0x90|y);	break;
		case 3 :LCD_w_com(0x88|y);	break;
	 	case 4 :LCD_w_com(0x98|y);	break;
	}
	while(*s>0)
  	{
		LCD_w_data(*s);
		s++;
 	  	delay(1);
	}
}
//��ʼ��LCD
void LCD_init()					
{
	delay(45);						//��ʱ45ms
	RST=1;							//��λ����
	delay(1);
	RST=0;
	delay(1);
	RST=1;
	delay(1);
	LCD_w_com(0x30);	delay(5);	//ѡ�����ָ�
	LCD_w_com(0x30);	delay(5);	//ѡ��8bit������
	LCD_w_com(0x0c);	delay(5);	//��ʾ�� ���������
	LCD_w_com(0x01);	delay(5);	//��ʾ����
	LCD_w_com(0x06);	delay(5);	//��ʾ����ƶ�����
}
//������ʾ
void LCD_disp_week()
{
 	switch(week)
	{
	 	case 1:	LCD_w_hz(3,4,"һ");	break;
		case 2:	LCD_w_hz(3,4,"��");	break;
		case 3:	LCD_w_com(0x8c);
					LCD_w_data(0xc8);
					LCD_w_data(0xfd);		break;
		case 4:	LCD_w_hz(3,4,"��");	break;
		case 5:	LCD_w_hz(3,4,"��");	break;
		case 6:	LCD_w_hz(3,4,"��");	break;
		case 7:	LCD_w_hz(3,4,"��");	break;
		default:	break;
	}
}
//ʱ�����ʾ
void LCD_disp_time()
{
	LCD_w_hz(2,0,"20");
	LCD_w_data(year/10+0x30);
	LCD_w_data(year%10+0x30);

	LCD_w_com(0x93);
	LCD_w_data(month/10+0x30);
	LCD_w_data(month%10+0x30);

	LCD_w_com(0x95);
	LCD_w_data(day/10+0x30);
	LCD_w_data(day%10+0x30);

	LCD_disp_week();

	LCD_w_com(0x9a);
	LCD_w_data(hour/10+0x30);
	LCD_w_data(hour%10+0x30);
	LCD_w_data(':');
	LCD_w_data(min/10+0x30);
	LCD_w_data(min%10+0x30);
	LCD_w_data(':');
	LCD_w_data(sec/10+0x30);
	LCD_w_data(sec%10+0x30);
}

void main()
{
	LCD_init();
	DS1302_Set(00,59,13,21,1,11,5);		
	LCD_w_hz(1,2,"�����ǣ�"); 
	LCD_w_hz(2,2,"��");
	LCD_w_hz(2,4,"��");
	LCD_w_hz(2,6,"��"); 
	LCD_w_hz(3,2,"����");
	while(1)
	{
		DS1302_Read_RTC();
		LCD_disp_time();
	}
}