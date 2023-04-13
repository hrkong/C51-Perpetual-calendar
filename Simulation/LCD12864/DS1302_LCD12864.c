/*************************************************************
程序名称：LCD12864的时钟显示（DS1302）
编		写：韦宏谋
编写时间：2011/1/20
调整FCOS：12Mhz
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

uc year,month,day,week,hour,min,sec;//年月日周时分秒变量

void delay(uc t)
{
 	uc i,j;
	for(i=0;i<t;i++)
		for(j=0;j<121;j++)	nop();
}
//SCK的上升沿写入数据，下降沿读取数据，数据均是先低位，后高位
//写函数
void DS1302_Write_Byte(uc wdata_temp) 
{
	uc i;
	for (i=8;i>0;i--)				//循环8次 写入数据
	{ 	
		SDA=wdata_temp&0x01;		//取出数据的最低位，写入数据线
		CLK=0;						//时钟线拉低		
		CLK=1; 						//时钟线拉高，构成时钟的上升沿		
		wdata_temp>>=1;			//数据右移一位								
	}
} 
//读函数
uc DS1302_Read_Byte(void) 
{
	uc i;
	uc Rdata_temp=0;
	for (i=8;i>0;i--)       //循环8次 读取数据
	{
		if(SDA)	  
		Rdata_temp|=0x80;		//每次传输低字节
		CLK=1;
		CLK=0;
		Rdata_temp>>=1;		//右移一位	
	} 
	return (ACC);
}
//向指定地址写数据函数
void DS1302_Write(uc address,uc dat)
{
 	CLK=0;				nop();
	DS1302_RST=0;		nop();nop();
 	DS1302_RST=1;	  					//启动
 	DS1302_Write_Byte(address);	//发送地址
 	DS1302_Write_Byte(dat);			//发送数据
	CLK=0;
 	DS1302_RST=0;  					//恢复
}
//从指定地址读一字节数据函数
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
	return (temp);			//返回值
}
//读取DS1302的时间函数,并将BCD码转换为十进制
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
//设定DS1302的时间函数
void DS1302_Set(uc s,m,h,d,mo,y,w)	
{	
	DS1302_Write(w_control,	0x00);
	DS1302_Write(w_sec,		(s/10<<4)|(s%10));		//写秒
	DS1302_Write(w_min,		(m/10<<4)|(m%10));		//写分
	DS1302_Write(w_hour,		(h/10<<4)|(h%10));		//写小时
	DS1302_Write(w_day,		(d/10<<4)|(d%10));		//写日
	DS1302_Write(w_month,	(mo/10<<4)|(mo%10));		//写月
	DS1302_Write(w_year,		(y/10<<4)|(y%10));		//写年 
	DS1302_Write(w_week,		(w/10<<4)|(w%10)); 		//写星期
	DS1302_Write(w_control,	0x80);	//设置DS1302寄存器禁止写入数据
}
//12864判断忙碌状态
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
//12864写命令
void LCD_w_com(uc comm)														  
{
	LCD_busy_check();
	RS=0;
	RW=0;
	EN=1;	
	DB=comm;	
	delay(1);		
	EN=0;	
	delay(1);			//给EN高脉冲
}
//12864写数据
void LCD_w_data(int dat)	
{
	LCD_busy_check();
	RS=1;
	RW=0;
	EN=1;
	DB=dat;	
	delay(1);	
	EN=0;
	delay(1);			//给EN高脉冲
}
//写字符
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
//初始化LCD
void LCD_init()					
{
	delay(45);						//延时45ms
	RST=1;							//复位操作
	delay(1);
	RST=0;
	delay(1);
	RST=1;
	delay(1);
	LCD_w_com(0x30);	delay(5);	//选择基本指令集
	LCD_w_com(0x30);	delay(5);	//选择8bit数据流
	LCD_w_com(0x0c);	delay(5);	//显示开 及光标设置
	LCD_w_com(0x01);	delay(5);	//显示清屏
	LCD_w_com(0x06);	delay(5);	//显示光标移动设置
}
//星期显示
void LCD_disp_week()
{
 	switch(week)
	{
	 	case 1:	LCD_w_hz(3,4,"一");	break;
		case 2:	LCD_w_hz(3,4,"二");	break;
		case 3:	LCD_w_com(0x8c);
					LCD_w_data(0xc8);
					LCD_w_data(0xfd);		break;
		case 4:	LCD_w_hz(3,4,"四");	break;
		case 5:	LCD_w_hz(3,4,"五");	break;
		case 6:	LCD_w_hz(3,4,"六");	break;
		case 7:	LCD_w_hz(3,4,"日");	break;
		default:	break;
	}
}
//时间的显示
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
	LCD_w_hz(1,2,"今天是："); 
	LCD_w_hz(2,2,"年");
	LCD_w_hz(2,4,"月");
	LCD_w_hz(2,6,"日"); 
	LCD_w_hz(3,2,"星期");
	while(1)
	{
		DS1302_Read_RTC();
		LCD_disp_time();
	}
}