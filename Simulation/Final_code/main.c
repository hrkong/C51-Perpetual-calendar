#include "reg52.h"
#include "LCD12864.h"
#include "ds1302.h"
#include "dhtXX.h"
#include "key.h"
#include "beep.h"
#include "time_set.h"

sbit MQ2 = P1^6;
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
	uint  hour1;
	uint  min1;
}SYSTEM = {0,22,6,27,1,18,30,0,150,740,0,0};
unsigned char temp_H,temp_L,humi_H,humi_L;
unsigned char alarm_switch_str[]=" Alarm: setting ";
unsigned char time_switch_str[]="  Time: setting ";
unsigned char setting[]="  Keep Setting  ";
_calendar g_calendar;

uchar wk[7][2] = {{"日"},{"一"},{"二"},{"三"},{"四"},{"五"},{"六"}};

uchar dis1[16] = {"20  /  /    节气"};			// 显示年月日星期
uchar dis2[16] = {"    :  :    周  "}; 			// 显示时分秒
uchar dis3[16] = {"温度:  . C  五月"};			// 显示温度
uchar dis4[16] = {"湿度:  . %  廿九"};			// 显示湿度
uchar dis5[16] = {"闹钟:  :        "};			// 显示闹钟时间
uchar dis6[16] = {"时间:  :  PM    "};			// PM
uchar dis7[16] = {"时间:  :  AM    "};			// AM
uchar dis8[16] = {"    :  :    周  "}; 			// 显示时分秒
								 
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
	unsigned char year,month,date,week,hour,min,sec;

	year = ((g_calendar.year/10)<<4)+(g_calendar.year%10);
	month = ((g_calendar.month/10)<<4)+(g_calendar.month%10);
	date = ((g_calendar.day/10)<<4)+(g_calendar.day%10);
	week = g_calendar.week;
	hour = ((g_calendar.hour/10)<<4)+(g_calendar.hour%10);
	min = ((g_calendar.min/10)<<4)+(g_calendar.min%10);
	sec=((g_calendar.sec/10)<<4)+(g_calendar.sec%10);
	WriteCommand(YEAR_WRITE,year);
	WriteCommand(MONT_WRITE,month);
	WriteCommand(DATE_WRITE,date);
	WriteCommand(WEEK_WRITE,week);
	WriteCommand(HOUR_WRITE,hour);
	WriteCommand(MINU_WRITE,min);
	WriteCommand(SECO_WRITE,sec);
}
void SetTime1(void)
{
	unsigned char year,month,date,week,hour,min;

	year = ((g_calendar.year/10)<<4)+(g_calendar.year%10);
	month = ((g_calendar.month/10)<<4)+(g_calendar.month%10);
	date = ((g_calendar.day/10)<<4)+(g_calendar.day%10);
	week = g_calendar.week;
	hour = ((g_calendar.hour/10)<<4)+(g_calendar.hour%10);
	min = ((g_calendar.min/10)<<4)+(g_calendar.min%10);
	
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
/*void KEY_Init(void)
{
	IT0 = 1;
	IT1 = 1;
	EX0 = 1;
	EX1 = 1;
	EA = 1;
}
  */

void calendar_save_set_time(void)
{
	SYSTEM.year=g_calendar.year;
	SYSTEM.month=g_calendar.month;
	SYSTEM.date=g_calendar.day;
	SYSTEM.week=g_calendar.week;
	SYSTEM.hour=g_calendar.hour;
	SYSTEM.min=g_calendar.min;
	SYSTEM.sec=g_calendar.sec;
	SetTime();
	DS1302_Init();		
}

void calendar_set_time(void)//时钟设置模式
{
	if(g_calendar.mode==1)//时钟设置
	{
		if(g_calendar.add==1)
		{
			g_calendar.add=0;	
			switch(g_calendar.time_choice)
			{
				case 0: g_calendar.sec++;
						//if((g_calendar.sec&0x0f)>9)g_calendar.sec+=6;
						if(g_calendar.sec>=0x60)g_calendar.sec=g_calendar.sec%60;
						break;
				case 1: g_calendar.min++;
						//if((g_calendar.min&0x0f)>9)g_calendar.min+=6;
						if(g_calendar.min>=0x60)g_calendar.min=g_calendar.min%60;
						break;
				case 2: g_calendar.hour++;
						//if((g_calendar.hour&0x0f)>9)g_calendar.hour+=6;
						if(g_calendar.hour>=0x24)g_calendar.hour=g_calendar.hour%24;
						break;
				case 3: g_calendar.week++;
						//if((g_calendar.week&0x0f)>9)g_calendar.week+=6;
						if(g_calendar.week>=0x08)g_calendar.week=g_calendar.week%7;
						break;
				case 4: g_calendar.day++;
						//if((g_calendar.day&0x0f)>9)g_calendar.day+=6;
						if(g_calendar.month==1||g_calendar.month==3||g_calendar.month==5||g_calendar.month==7||
						g_calendar.month==8||g_calendar.month==10||g_calendar.month==12)
						{
							if(g_calendar.day>=0x32)g_calendar.day=g_calendar.day%31;	
						}
						else if(g_calendar.month==2)
						{
							g_calendar.day=g_calendar.day%28;
						}
						else
						{	
							g_calendar.day=g_calendar.day%30 ;
						}
						//if(g_calendar.day>=0x32)g_calendar.day=1;
						break;
				case 5: g_calendar.month++;
						if((g_calendar.month&0x0f)>9)g_calendar.month+=6;
						if(g_calendar.month>=0x13)g_calendar.month=g_calendar.month%12;
						break;
				case 6: g_calendar.year++;
					//	if((g_calendar.year&0x0f)>9)g_calendar.year+=6;
						if(g_calendar.year>=0x99)g_calendar.year=0;
						break;		
			}	
		}																
	}				
}

void calendar_set_alarm(void)//闹钟设置模式
{
	if(g_calendar.mode==2)//闹钟设置
	{
		if(g_calendar.add==1)
		{
			g_calendar.add=0;	
			switch(g_calendar.time_choice)
			{
				case 0: g_calendar.alarm_time[0]++;
						//if((g_calendar.alarm_time[0]&0x0f)>9)g_calendar.alarm_time[0]+=6;
						if(g_calendar.alarm_time[0]>=0x60)g_calendar.alarm_time[0]=g_calendar.alarm_time[0]%60;
						break;
				case 1: g_calendar.alarm_time[1]++;
					//	if((g_calendar.alarm_time[1]&0x0f)>9)g_calendar.alarm_time[1]+=6;
						if(g_calendar.alarm_time[1]>=0x24)g_calendar.alarm_time[1]=g_calendar.alarm_time[1]%24;
						break; 
				case 2: g_calendar.alarm=!g_calendar.alarm;
						break;
				case 3: g_calendar.time_choice=0;
						break;
			}	
		}																	
	}		
}

void delay111(uint a)
{
	uchar i;
	while(a--)for(i=0;i<120;i++);
}

void main(void)
{
	unsigned char key_temp=0;

	
	lcd_init();
	DS1302_Init();
	BEEP=0;
	MQ2=1;
	
	//KEY_Init();
	DAT = 1;
	
	
	g_calendar.mode=0;
	g_calendar.time_choice=0;
	g_calendar.add=0;
	g_calendar.alarm_time[1]=0;
	g_calendar.alarm_time[0]=0;

	g_calendar.year=SYSTEM.year;	 //初始化日历时间
	g_calendar.month=SYSTEM.month;
	g_calendar.day=SYSTEM.date;
	g_calendar.week=SYSTEM.week;
	g_calendar.hour=SYSTEM.hour;
	g_calendar.min=SYSTEM.min;
	g_calendar.sec=SYSTEM.sec;
	SetTime();
	
	while(1)
	{
		key_temp=key_scan(0);
		if(key_temp==KEY2_PRESS)
		{
			lcd_init();
			beep_alarm(100,10);
			g_calendar.mode++;
		
			g_calendar.setok=0;
			g_calendar.time_choice=0;
			
			if(g_calendar.mode==1)
			{
				lcd_init();
				lcd12864_printf(1,0,16,time_switch_str);
				delay111(2000);	
			}
			if(g_calendar.mode==2)
			{
				calendar_save_set_time();
				lcd_init();
				lcd12864_printf(1,0,16,alarm_switch_str);
				delay111(2000);	
			}			
		}
		else if(key_temp==KEY3_PRESS)//进入设置模式时，对应位选择设置
		{
			g_calendar.time_choice++;
			if(g_calendar.time_choice==7)
				g_calendar.time_choice=0;
			beep_alarm(100,10);			
		}
		else if(key_temp==KEY4_PRESS)//进入设置模式时，进行数据加操作
		{
			g_calendar.add=1;
			beep_alarm(100,10);
		}
		if(g_calendar.mode==3)//设置完成，恢复正常显示模式
		{
			//beep_alarm(100,10);
			lcd_init();
			lcd12864_printf(1,0,16,setting);			
			delay111(2000);
			g_calendar.mode=0;
			g_calendar.setok=1;
			g_calendar.time_choice=0;			
			calendar_save_set_time();
		}

		if(g_calendar.mode==1)//模拟光标闪烁
		{
			if(g_calendar.time_choice==0)
				lcd12864_printf(2,4,2," ");
			else if(g_calendar.time_choice==1)
			{
				dis8[2] = SYSTEM.hour/10+'0';
				dis8[3] = SYSTEM.hour%10+'0';
				dis8[5] = ' ';
				dis8[6] = ' ';
				dis8[8] = SYSTEM.sec/10+'0';
				dis8[9] = SYSTEM.sec%10+'0';
				lcd12864_printf(2,0,16,dis8);
				lcd12864_printf(2,7,2,wk[SYSTEM.week]);
			}
			else if(g_calendar.time_choice==2)
			{
			    lcd12864_printf(2,1,2," ");
			}
			else if(g_calendar.time_choice==3)
			{
				
				lcd12864_printf(2,7,2," ");
			}
			else if(g_calendar.time_choice==4)
			{
				
				lcd12864_printf(1,4,2," ");
			}
			else if(g_calendar.time_choice==5)
			{
				dis1[2] = SYSTEM.year/10+'0';
				dis1[3] = SYSTEM.year%10+'0';
				dis1[5] = ' ';
				dis1[6] = ' ';
				dis1[8] = SYSTEM.date/10+'0';
				dis1[9] = SYSTEM.date%10+'0';
				lcd12864_printf(1,0,16,dis1);
			}
			else if(g_calendar.time_choice==6)
			{
				
				lcd12864_printf(1,1,2," ");
			}	
			SetTime1();
		}
		if(g_calendar.mode==2)
		{
			if(g_calendar.time_choice==0)
			{
				lcd12864_printf(1,4,2," ");
			}
			else if(g_calendar.time_choice==1)
			{
				dis5[5]=' ';
				dis5[6]=' ';
				dis5[8]=g_calendar.alarm_time[0]/10+'0';
				dis5[9]=g_calendar.alarm_time[0]%10+'0';
				lcd12864_printf(1,0,16,dis5);
			}	
		}

		calendar_set_time();
		calendar_set_alarm();
			

		
		//SetTime();
		GetTime();

		receive();			//获取温湿度
		temp_H=TH; 	
	    temp_L=TL; 	
	    humi_H=RH; 	
	    humi_L=RL;

		dis1[2] = SYSTEM.year/10+'0';
		dis1[3] = SYSTEM.year%10+'0';
		dis1[5] = SYSTEM.month/10+'0';
		dis1[6] = SYSTEM.month%10+'0';
		dis1[8] = SYSTEM.date/10+'0';
		dis1[9] = SYSTEM.date%10+'0';

		
		dis2[2] = SYSTEM.hour/10+'0';
		dis2[3] = SYSTEM.hour%10+'0';
		dis2[5] = SYSTEM.min/10+'0';
		dis2[6] = SYSTEM.min%10+'0';
		dis2[8] = SYSTEM.sec/10+'0';
		dis2[9] = SYSTEM.sec%10+'0';
		
	
		dis3[5] = temp_H/10+'0';
		dis3[6] = temp_H%10+'0';
		dis3[8] = temp_L/10+'0';
		
		
		dis4[5] = humi_H/10+'0';
		dis4[6] = humi_H%10+'0';
		dis4[8] = humi_L/10+'0';

		dis5[5]=g_calendar.alarm_time[1]/10+'0';
		dis5[6]=g_calendar.alarm_time[1]%10+'0';
		dis5[8]=g_calendar.alarm_time[0]/10+'0';
		dis5[9]=g_calendar.alarm_time[0]%10+'0';

		

		

		if(g_calendar.mode==2)
		{
			lcd12864_printf(1,0,16,dis5);
			if(SYSTEM.hour>=12)
			{
				SYSTEM.hour1=SYSTEM.hour-12;
				SYSTEM.min1=SYSTEM.min;
				dis6[5]=SYSTEM.hour1/10+'0';
				dis6[6]=SYSTEM.hour1%10+'0';
				dis6[8]=SYSTEM.min1/10+'0';
				dis6[9]=SYSTEM.min1%10+'0';
				lcd12864_printf(2,0,16,dis6);	
			}
			else
			{
				SYSTEM.hour1=SYSTEM.hour;
				SYSTEM.min1=SYSTEM.min;
				dis7[5]=SYSTEM.hour1/10+'0';
				dis7[6]=SYSTEM.hour1%10+'0';
				dis7[8]=SYSTEM.min1/10+'0';
				dis7[9]=SYSTEM.min1%10+'0';
				lcd12864_printf(2,0,16,dis7);	
			}
		}		
		else
		{	
			//calendar_save_set_time();
			lcd12864_printf(1,0,16,dis1);
			lcd12864_printf(2,0,16,dis2);
			lcd12864_printf(2,7,2,wk[SYSTEM.week]);  //第一个数字表示第几行，第二个数字表示从哪里开始，第三个长度
			lcd12864_printf(3,0,16,dis3);
			lcd12864_printf(4,0,16,dis4);		
		}
		

		if(g_calendar.hour==g_calendar.alarm_time[1]&&g_calendar.min==g_calendar.alarm_time[0])
		{
				beep_alarm(300000,10);
		}
		
		if(MQ2==1)
		{
			delay_ms(5);
			if(MQ2==1)
			{
				 //beep_alarm(100,10);
			}
				//beep_alarm(6000000,10);			
		}
	}
}

