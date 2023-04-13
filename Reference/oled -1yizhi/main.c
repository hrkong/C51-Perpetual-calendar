
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

uint flag=5;//设置初始时间  开始在“分”上
uint flag2=1;//设置闹钟时间
uchar data_byte; 
uchar RH,RL,TH,TL; 
sbit io=P0^7;  //dht11data端接单片机的P0^7口// 
sbit k1=P2^1;	 //定义P31口是k1   按键选中设置
sbit k2=P2^2;	 //定义P32口是k2    加键
sbit k3=P2^3;	 //定义P33口是k3    减键
sbit k4=P2^4;	 //定义P34口是k4    闹钟设置键
sbit LED=P2^7;       //定义2.7是个灯或者蜂鸣器，烟雾报警用
sbit LED2=P2^6;       //定义2.6是个灯或者蜂鸣器，闹钟用
sbit BEEP=P1^7;		 //蜂鸣器
sbit DOUT=P2^0;	 //定义单片机P2口的第1位 （即P2.0）为烟雾传感器的输入端
 


//DS1302引脚定义
sbit RST=P1^3;
sbit IO=P1^2;
sbit SCK=P1^1;
//DS1302地址定义
#define ds1302_sec_add			0x80		//秒数据地址
#define ds1302_min_add			0x82		//分数据地址
#define ds1302_hr_add			0x84		//时数据地址
#define ds1302_date_add			0x86		//日数据地址
#define ds1302_month_add		0x88		//月数据地址
#define ds1302_day_add			0x8a		//星期数据地址
#define ds1302_year_add			0x8c		//年数据地址
#define ds1302_control_add		0x8e		//控制数据地址
#define ds1302_charger_add		0x90 					 
#define ds1302_clkburst_add		0xbe
//初始时间定义
uchar time_buf[8] = {0x20,0x22,0x5,0x011,0x18,0x34,0x00,0x03};//初始时间2022年5月10号20点29分30秒 星期1
uchar time_buf2[8] = {0x20,0x22,0x5,0x05,0x09,0x30,0x05,0x05};//初始闹钟2022年4月25号20点29分30秒 星期1

void receive();
//功能:延时1毫秒
//入口参数:x
//出口参数:无
//说明:当晶振为12M时，j<112；当晶振为11.0592M时，j<122
void Delay_xms(uint x)
{
  uint i,j;
  for(i=0;i<x;i++)
    for(j=0;j<112;j++);
}
//----------------------------延时子程序---------------------------------
//功能：12us延时
//STC89C52为1T单片机,即1个时钟/机器周期,速度为AT89C52的12倍
void Delay_xus(uint t)	  		 		
{ 
  for(;t>0;t--)
   {
	 _nop_();
   }
}
void delay1()//一个for循环大概需要8个多机器周期一个机器周期为1us晶振为12MHz也就是说本函数延时8us多此延时函数必须德稍微精确一点 

{ 

  uchar i; 

  for(i=0;i<1;i++); 

} 
 void delay(uchar ms) //延时模块// 

{ 

  uchar i; 

  while(ms--)                 

        

 for(i=0;i<100;i++); 

} 

//--------------------------BCD转换----------------------
uchar BCD_Decimal(uchar bcd)//BCD码转十进制函数，输入BCD，返回十进制
{
 	uchar Decimal;
 	Decimal=bcd>>4;
 	return(Decimal=Decimal*10+(bcd&=0x0F));
}



//----------------------------时钟DS1302模块子程序---------------------------------
//DS1302初始化函数
void ds1302_init(void) 
{
	RST=0;			//RST脚置低
	SCK=0;			//SCK脚置低
}
//向DS1302写入一字节数据
void ds1302_write_byte(uchar addr, uchar d) 
{
	uchar i;
	RST=1;					//启动DS1302总线	
	//写入目标地址：addr
	addr = addr & 0xFE;   //最低位置零，寄存器0位为0时写，为1时读
	for (i = 0; i < 8; i ++) {
		if (addr & 0x01) {
			IO=1;
			}
		else {
			IO=0;
			}
		SCK=1;      //产生时钟
		SCK=0;
		addr = addr >> 1;
		}	
	//写入数据：d
	for (i = 0; i < 8; i ++) {
		if (d & 0x01) {
			IO=1;
			}
		else {
			IO=0;
			}
		SCK=1;    //产生时钟
		SCK=0;
		d = d >> 1;
		}
	RST=0;		//停止DS1302总线
}

//从DS1302读出一字节数据
uchar ds1302_read_byte(uchar addr) {

	uchar i,temp;	
	RST=1;					//启动DS1302总线
	//写入目标地址：addr
	addr = addr | 0x01;    //最低位置高，寄存器0位为0时写，为1时读
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
	//输出数据：temp
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
	RST=0;					//停止DS1302总线
	return temp;
}
//向DS302写入时钟数据
void ds1302_write_time(void) 
{
	ds1302_write_byte(ds1302_control_add,0x00);			//关闭写保护 
	ds1302_write_byte(ds1302_sec_add,0x80);				//暂停时钟 
	ds1302_write_byte(ds1302_charger_add,0xa9);	    //涓流充电 
	ds1302_write_byte(ds1302_year_add,time_buf[1]);		//年 
	ds1302_write_byte(ds1302_month_add,time_buf[2]);	//月 
	ds1302_write_byte(ds1302_date_add,time_buf[3]);		//日 
	ds1302_write_byte(ds1302_hr_add,time_buf[4]);		//时 
	ds1302_write_byte(ds1302_min_add,time_buf[5]);		//分
	ds1302_write_byte(ds1302_sec_add,time_buf[6]);		//秒
	ds1302_write_byte(ds1302_day_add,time_buf[7]);		//周 
	ds1302_write_byte(ds1302_control_add,0x80);			//打开写保护     
}
//从DS302读出时钟数据
void ds1302_read_time(void)  
{
	time_buf[1]=ds1302_read_byte(ds1302_year_add);		//年 
	time_buf[2]=ds1302_read_byte(ds1302_month_add);		//月 
	time_buf[3]=ds1302_read_byte(ds1302_date_add);		//日 
	time_buf[4]=ds1302_read_byte(ds1302_hr_add);		//时 
	time_buf[5]=ds1302_read_byte(ds1302_min_add);		//分 
	time_buf[6]=(ds1302_read_byte(ds1302_sec_add))&0x7f;//秒，屏蔽秒的第7位，避免超出59
	time_buf[7]=ds1302_read_byte(ds1302_day_add);		//周 	
}


//----------------------------串口  中断---------------------------------


//定时器2初始化
void Init_timer2(void)
{
 RCAP2H=0x3c;//赋T2初始值赋为0x3cb0，溢出20次为1秒,每次溢出时间为50ms （要调整!!!）
 RCAP2L=0xb5;
 TR2=1;	     //启动定时器2
 ET2=1;		 //打开定时器2中断
 EA=1;		 //打开总中断
}



//定时器中断函数
void Timer2() interrupt 5	  //定时器2是6号中断
{
 static uchar t;
 TF2=0;
 t++;
 if(t==4)               //间隔200ms(50ms*4)读取一次时间
  {
   t=0;
   //ds1302_init();
   //Delay_xms(10);
   ds1302_read_time();  //读取时间 
   //ds1302_init();
   Delay_xms(10);
   receive();
   dis_time_buf[0]=(time_buf[0]>>4); //年   
   dis_time_buf[1]=(time_buf[0]&0x0f);
   
   dis_time_buf[2]=(time_buf[1]>>4);   
   dis_time_buf[3]=(time_buf[1]&0x0f);
  

   
   
   dis_time_buf[4]=(time_buf[2]>>4); //月  
   dis_time_buf[5]=(time_buf[2]&0x0f);
   

   dis_time_buf[6]=(time_buf[3]>>4); //日   
   dis_time_buf[7]=(time_buf[3]&0x0f);
   
   dis_time_buf[14]=(time_buf[7]&0x07); //星期
   
   //第2行显示  
   dis_time_buf[8]=(time_buf[4]>>4); //时   
   dis_time_buf[9]=(time_buf[4]&0x0f);   

  

   dis_time_buf[10]=(time_buf[5]>>4); //分   
   dis_time_buf[11]=(time_buf[5]&0x0f);   

   dis_time_buf[12]=(time_buf[6]>>4); //秒   
   dis_time_buf[13]=(time_buf[6]&0x0f);


   //-----------------------------------------------

   dis_time_buf2[0]=(time_buf2[0]>>4); //年   
   dis_time_buf2[1]=(time_buf2[0]&0x0f);
   
   dis_time_buf2[2]=(time_buf2[1]>>4);   
   dis_time_buf2[3]=(time_buf2[1]&0x0f);
  

   
   
   dis_time_buf2[4]=(time_buf2[2]>>4); //月  
   dis_time_buf2[5]=(time_buf2[2]&0x0f);
   

   dis_time_buf2[6]=(time_buf2[3]>>4); //日   
   dis_time_buf2[7]=(time_buf2[3]&0x0f);
   
   dis_time_buf2[14]=(time_buf2[7]&0x07); //星期
   
   //第2行显示  
   dis_time_buf2[8]=(time_buf2[4]>>4); //时   
   dis_time_buf2[9]=(time_buf2[4]&0x0f);   

  

   dis_time_buf2[10]=(time_buf2[5]>>4); //分   
   dis_time_buf2[11]=(time_buf2[5]&0x0f);   

   dis_time_buf2[12]=(time_buf2[6]>>4); //秒   
   dis_time_buf2[13]=(time_buf2[6]&0x0f);
   
  }
}

//----------------------------0.96OLED/LCD显示功能程序---------------------------------


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
				  //第二行
			
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
				  //第二行
			
			OLED_ShowCHinese(3,2,6);	 //闹
			OLED_ShowCHinese(21,2,7);	 //钟 
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
//----------------------------DHT11模块功能子程序---------------------------------
uchar receive_byte()//接收一个字节// 

{ 

  uchar i,temp2; 

  for(i=0;i<8;i++)//接收8bit的数据 

  { 

 	while(!io);//等待50us的低电平开始信号结束 

 	delay1();//开始信号结束之后延时26us-28us以下三个延时函数 

	 delay1(); 

 	delay1(); 

 	temp2=0;//时间为26us-28us表示接收的为数据'0' 

 	if(io==1) 

	 temp2=1; //如果26us-28us之后还为高电平则表示接收的数据为'1' 

    while(io);//等待数据信号高电平'0'为26us-28us'1'为70us 

 	data_byte<<=1;//接收的数据为高位在前右移 

	 data_byte|=temp2; 

  } 

  return data_byte; 

} 
void start()//开始信号 

{ 

  io=1; 

  delay1(); 

  io=0; 

  delay(25);// 主机把总线拉低必须大于18ms保证DHT11能检测到起始信号 

  io=1;    //发送开始信号结束后拉高电平延时20-40us 

  delay1();//以下三个延时函数差不多为24us符合要求 

  delay1(); 

  delay1();

  delay1(); 

} 
void receive()//接收数据// 

{ 

  uchar T_H,T_L,R_H,R_L,check,num_check,i; 

  start();//开始信号// 

  io=1;   //主机设为输入判断从机DHT11响应信号 

  if(!io)//判断从机是否有低电平响应信号// 

  {  
	while(!io);//判断从机发出 80us 的低电平响应信号是否结束// 
	//Delay_xus(20);

	while(io);//判断从机发出 80us 的高电平是否结束如结束则主机进入数据接收状态 

	R_H=receive_byte();//湿度高位 

 	R_L=receive_byte();//湿度低位 

	T_H=receive_byte();//温度高位 

	T_L=receive_byte();//温度低位 

	check=receive_byte();//校验位 

	io=0; //当最后一bit数据接完毕后从机拉低电平50us// 

	for(i=0;i<7;i++)//差不多50us的延时 

	delay1(); 

	io=1;//总线由上拉电阻拉高进入空闲状态 

	num_check=R_H+R_L+T_H+T_L; 

	if(num_check==check)//判断读到的四个数据之和是否与校验位相同 

 	{ 

   RH=R_H; 

   RL=R_L; 

   TH=T_H; 

   TL=T_L; 

   check=num_check; 

 	} 

  } 

} 



//-------------------------------键盘----------------------
void keypros1()
{
	if(k1==0)		  //检测按键K1是否按下
	{	
		Delay_xms(10);   //消除抖动 一般大约10ms
		if(k1==0)	 //再次判断按键是否按下
		{
			flag+=1;
			if(flag==8) {flag=1;}
		}
		while(!k1);	 //检测按键是否松开
	}		
}

void keypros4()
{
	if(k4==0)		  //检测按键K1是否按下
	{	
		Delay_xms(10);   //消除抖动 一般大约10ms
		if(k4==0)	 //再次判断按键是否按下
		{
			flag2=-flag2;
			
		}
		while(!k4);	 //检测按键是否松开
	}		
}





void keypros2()
{
	

	if(k2==0)		  //检测按键K1是否按下
	{	
		Delay_xms(10);   //消除抖动 一般大约10ms
		if(k2==0)	 //再次判断按键是否按下
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
		 ds1302_write_byte(ds1302_control_add,0x00);			//关闭写保护 
		 ds1302_write_byte(ds1302_sec_add,0x80);				//暂停时钟 
		 ds1302_write_byte(ds1302_charger_add,0xa9);	    //涓流充电 
		 ds1302_write_byte(ds1302_year_add,time_buf[flag]+1);		//对应flag位单位
		 ds1302_write_byte(ds1302_control_add,0x80);			//打开写保护 
		 */

		}
		while(!k2);	 //检测按键是否松开
	}		
}
void keypros3()
{
	

	if(k3==0)		  //检测按键K3是否按下
	{	
		Delay_xms(10);   //消除抖动 一般大约10ms
		if(k3==0)	 //再次判断按键是否按下
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
		 ds1302_write_byte(ds1302_control_add,0x00);			//关闭写保护 
		 ds1302_write_byte(ds1302_sec_add,0x80);				//暂停时钟 
		 ds1302_write_byte(ds1302_charger_add,0xa9);	    //涓流充电 
		 ds1302_write_byte(ds1302_year_add,time_buf[flag]+1);		//对应flag位单位
		 ds1302_write_byte(ds1302_control_add,0x80);			//打开写保护 
		 */

		}
		while(!k3);	 //检测按键是否松开
	}		
}



//----------------------------主程序---------------------------------
//主函数
void main(void)
{  
	 //u8 temp=0,humi=0;
	 //u8 temp_buf[3],humi_buf[3];
	  BEEP=0;


//-----------------------初始化-----------------------------

 //DHT11_Init();
 Delay_xms(50);//等待系统稳定
 ds1302_init();  //DS1302初始化
 Delay_xms(10);
 ds1302_write_time(); //写入初始值
 Init_timer2(); //定时器2初始化 
 	OLED_Init();
	OLED_Clear;

 while(1)
  {


//-----------------------时间设置-----------------------------
  LED=1;
  LED2=1;
  keypros1();
  keypros2();
  keypros3();
  keypros4();




//-----------------------时间显示-----------------------------
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

//-----------------------闹钟设定-----------------------------
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


//-----------------------------------------------闹钟声音----
	   if(time_buf[4]==time_buf2[4]&&time_buf[5]==time_buf2[5]&&time_buf[6]==time_buf2[6]) 
	   {
	     //声音   到时间
		 BEEP=0;
		 LED2=0;
		 i=80;
		 //Delay_xms(5000);
		  while(i) 
		 	  {
			  	i--;
		 		Displaytime();	  //亮灯的时候 保证时间继续在走
		   	  }
		 BEEP=1;
		 LED2=1;
	   }

//----------------------------------- 温湿度--------------------------
	 
	  // i2++;
	  //if(i2%30==0)
	  //{
		 //DHT11_Read_Data(&temp,&humi);
	   	 //receive();//接收温湿度数据	 移到中断里读取温湿度，不然程序莫名卡死
	  //}
	  //Delay_xms(1);
	  displayRT();
	
 
//-----------------------烟雾------------------------------- 
  
   if(DOUT==0)//当浓度高于设定值时 ，执行条件函数
	 {
        Delay_xms(500);//延时抗干扰
		if(DOUT==0)//确定 浓度高于设定值时 ，执行条件函数
	    {
		 LED=0;	   //点亮P1.0口灯(或其他的烟雾报警)
		}	  
  	 }

//-----------------------------------------------------------


  }
}
