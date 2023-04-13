#include <reg51.h>
#include "LCD12864.h"
#include "ds1302.h"
#include "DHT11_final.c"

struct SYS_PARAM{
	uchar set_no;
	uchar year;
	uchar month;
	uchar date;
	uchar week;
	uchar hour;
	uchar min;
	uchar sec;
	uint  temp;
	uint  humi;
}SYSTEM = {0,17,3,3,5,20,57,0,150,740};

sbit KEY0 = P3^2;
sbit KEY1 = P3^3;


uchar wk[7][2] = {{"日"},{"一"},{"二"},{"三"},{"四"},{"五"},{"六"}};

uchar dis1[16] = {"20  /  /        "};			// 显示年月日星期
uchar dis2[16] = {"      :  :      "}; 			// 显示时分秒
uchar dis3[16] = {"温度:  . C      "};			// 显示温度
uchar dis4[16] = {"湿度:  . %      "};			// 显示湿度

/*******************************************************************************
* 函 数 名       : void GetTime(void)
* 函数功能		 : 获取时间数据
* 输    入       : 无
* 输    出       : 无
*******************************************************************************/
void GetTime(void)
{
	unsigned char year=0X17,month=0X02,date=0X26,week=0,hour=0X21,min=0X49,sec=0X30;
	year = ReadCommand(YEAR_READ);
	month = ReadCommand(MONT_READ);
	date = ReadCommand(DATE_READ);
	week = ReadCommand(WEEK_READ);
	hour = ReadCommand(HOUR_READ);
	min = ReadCommand(MINU_READ);
	sec = ReadCommand(SECO_READ);
	SYSTEM.year = (year>>4)*10+(year&0x0f);
	SYSTEM.month = (month>>4)*10+(month&0x0f);
	SYSTEM.date = (date>>4)*10+(date&0x0f);
	SYSTEM.week = week;
	SYSTEM.hour = (hour>>4)*10+(hour&0x0f);
	SYSTEM.min = (min>>4)*10+(min&0x0f);
	SYSTEM.sec = (sec>>4)*10+(sec&0x0f);
}
/*******************************************************************************
* 函 数 名       : void SetTime(void)
* 函数功能		 : 设置时间数据
* 输    入       : 无
* 输    出       : 无
*******************************************************************************/
void SetTime(void)
{
	unsigned char year,month,date,week,hour,min;
	year = ((SYSTEM.year/10)<<4)+(SYSTEM.year%10);
	month = ((SYSTEM.month/10)<<4)+(SYSTEM.month%10);
	date = ((SYSTEM.date/10)<<4)+(SYSTEM.date%10);
	week = SYSTEM.week;
	hour = ((SYSTEM.hour/10)<<4)+(SYSTEM.hour%10);
	min = ((SYSTEM.min/10)<<4)+(SYSTEM.min%10);
	WriteCommand(YEAR_WRITE,year);
	WriteCommand(MONT_WRITE,month);
	WriteCommand(DATE_WRITE,date);
	WriteCommand(WEEK_WRITE,week);
	WriteCommand(HOUR_WRITE,hour);
	WriteCommand(MINU_WRITE,min);
}
/*******************************************************************************
* 函 数 名       : void KEY_Init(void)
* 函数功能		 : 按键初始化
* 输    入       : 无
* 输    出       : 无
*******************************************************************************/
void KEY_Init(void)
{
	IT0 = 1;
	IT1 = 1;
	EX0 = 1;
	EX1 = 1;
	EA = 1;
}

void main(void)
{
	lcd_init();
	DS1302_Init();
	
	KEY_Init();
	io = 1;
	SetTime();
	while(1)
	{
		if(SYSTEM.set_no==0)
			GetTime();

		receive();
		SYSTEM.temp = temp;
		SYSTEM.humi = humi;

		dis1[2] = SYSTEM.year/10+'0';
		dis1[3] = SYSTEM.year%10+'0';
		dis1[5] = SYSTEM.month/10+'0';
		dis1[6] = SYSTEM.month%10+'0';
		dis1[8] = SYSTEM.date/10+'0';
		dis1[9] = SYSTEM.date%10+'0';
		
		dis2[4] = SYSTEM.hour/10+'0';
		dis2[5] = SYSTEM.hour%10+'0';
		dis2[7] = SYSTEM.min/10+'0';
		dis2[8] = SYSTEM.min%10+'0';
		dis2[10] = SYSTEM.sec/10+'0';
		dis2[11] = SYSTEM.sec%10+'0';
		
		dis3[5] = SYSTEM.temp/100+'0';
		dis3[6] = SYSTEM.temp%100/10+'0';
		dis3[8] = SYSTEM.temp%10+'0';

		dis4[5] = SYSTEM.humi/100+'0';
		dis4[6] = SYSTEM.humi%100/10+'0';
		dis4[8] = SYSTEM.humi%10+'0';

		lcd12864_printf(1,0,10,dis1);
		lcd12864_printf(2,0,16,dis2);
		lcd12864_printf(1,6,2,wk[SYSTEM.week]);
		lcd12864_printf(3,0,16,dis3);
		lcd12864_printf(4,0,16,dis4);	
	}
}

void EXT0_Int(void) interrupt 0
{
	Delay10ms();
	if(KEY0==0)
	{
		if(++SYSTEM.set_no>6)
		{
			SetTime();
			SYSTEM.set_no = 0;
		}
	}
}
void EXT1_Int(void) interrupt 2
{
	Delay10ms();
	if(KEY1==0)
	{
		switch (SYSTEM.set_no)
		{
			case 1 : if(++SYSTEM.year>99)SYSTEM.year = 0;break;
			case 2 : if(++SYSTEM.month>12)SYSTEM.month = 1;break;
			case 3 : if(++SYSTEM.date>31)SYSTEM.date = 1;break;
			case 4 : if(++SYSTEM.week>6)SYSTEM.week = 0;break;
			case 5 : if(++SYSTEM.hour>23)SYSTEM.year = 0;break;
			case 6 : if(++SYSTEM.min>59)SYSTEM.year = 0;break;
			default: SYSTEM.set_no = 0;break;
		}
	}
}