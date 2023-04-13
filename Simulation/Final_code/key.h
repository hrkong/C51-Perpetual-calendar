#ifndef _key_H
#define _key_H

#include "public.h"

//定义独立按键控制脚
sbit KEY1=P2^0;
sbit KEY2=P2^1;
sbit KEY3=P2^2;
sbit KEY4=P2^3;	  //由于LCD12864也要用到P3.3，所以不能连接LCD的RST口


//使用宏定义独立按键按下的键值
#define KEY1_PRESS	1				//第一个按键用于模式设置
#define KEY2_PRESS	2				//第二个按键用于选择设置哪个时间
#define KEY3_PRESS	3				//第三个按键用来加
#define KEY4_PRESS	4				//第四个按键结束
#define KEY_UNPRESS	0


u8 key_scan(u8 mode);

#endif