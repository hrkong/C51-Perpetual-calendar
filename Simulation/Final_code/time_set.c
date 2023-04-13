#include "time_set.h"
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
						if((g_calendar.sec&0x0f)>9)g_calendar.sec+=6;
						if(g_calendar.sec>=0x60)g_calendar.sec=0;
						break;
				case 1: g_calendar.min++;
						if((g_calendar.min&0x0f)>9)g_calendar.min+=6;
						if(g_calendar.min>=0x60)g_calendar.min=0;
						break;
				case 2: g_calendar.hour++;
						if((g_calendar.hour&0x0f)>9)g_calendar.hour+=6;
						if(g_calendar.hour>=0x24)g_calendar.hour=0;
						break;
				case 3: g_calendar.week++;
						if((g_calendar.week&0x0f)>9)g_calendar.week+=6;
						if(g_calendar.week>=0x08)g_calendar.week=1;
						break;
				case 4: g_calendar.day++;
						if((g_calendar.day&0x0f)>9)g_calendar.day+=6;
						if(g_calendar.day>=0x32)g_calendar.day=1;
						break;
				case 5: g_calendar.month++;
						if((g_calendar.month&0x0f)>9)g_calendar.month+=6;
						if(g_calendar.month>=0x13)g_calendar.month=1;
						break;
				case 6: g_calendar.year++;
						if((g_calendar.year&0x0f)>9)g_calendar.year+=6;
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
						if((g_calendar.alarm_time[0]&0x0f)>9)g_calendar.alarm_time[0]+=6;
						if(g_calendar.alarm_time[0]>=0x60)g_calendar.alarm_time[0]=0;
						break;
				case 1: g_calendar.alarm_time[1]++;
						if((g_calendar.alarm_time[1]&0x0f)>9)g_calendar.alarm_time[1]+=6;
						if(g_calendar.alarm_time[1]>=0x24)g_calendar.alarm_time[1]=0;
						break; 
				case 2: g_calendar.alarm=!g_calendar.alarm;
						break;
				case 3: g_calendar.time_choice=0;
						break;
			}	
		}																	
	}		
}
