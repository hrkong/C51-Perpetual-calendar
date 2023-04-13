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
* 函 数 名       : void Delay10ms()		//@11.0592MHz
* 函数功能		 : 软件延时函数
* 输    入       : 无
* 输    出       : 无
*******************************************************************************/
void delay(unsigned char ms) //延时模块// 

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
void delay1()//一个for循环大概需要8个多机器周期一个机器周期为1us晶振为12MHz也就是说本函数延时8us多此延时函数必须德稍微精确一点 

{ 

  unsigned char i; 

  for(i=0;i<1;i++); 

} 
void delay2()//27个机器周期，12M晶振，函数延时为27us 

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
* 函 数 名       : void Start(void)
* 函数功能		 : 起始信号
* 输    入       : 无
* 输    出       : 无
*******************************************************************************/
void start()//开始信号 

{ 

  DAT=1; 

  delay1(); 

  DAT=0; 

  delay(25);// 主机把总线拉低必须大于18ms保证DHT11能检测到起始信号 

  DAT=1;    //发送开始信号结束后拉高电平延时20-40us 

  delay2();//以下三个延时函数差不多为24us符合要求 
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
* 函 数 名       : unsigned char ReadByte(void)
* 函数功能		 : 读取一个字节数据
* 输    入       : 无
* 输    出       : 无
*******************************************************************************/

/*unsigned char ReadOneByte(void)
{
	unsigned char i,j=0,dat=0;
	for(i=0;i<8;i++)
	{
		while(!DAT);		// 等待高电平
		while(DAT)			// 高电平持续时间计算
		{
			i++;			// 一个循环为5个指令周期，约5us
			
		}
		if(i>6)			// 0--24~28us；1 >70us				
		{
			dat|=0x01;
		}
		dat<<=1;		
	}
	return dat;
}
*/


unsigned char receive_byte()//接收一个字节// 

{ 

  unsigned char i,temp; 

  for(i=0;i<8;i++)//接收8bit的数据 

  { 

 	while(!DAT);//等待50us的低电平开始信号结束 

 	delay2();//开始信号结束之后延时26us-28us以下三个延时函数  

 	temp=0;//时间为26us-28us表示接收的为数据'0' 

 	if(DAT==1) 

	 temp=1; //如果26us-28us之后还为高电平则表示接收的数据为'1' 

    while(DAT);//等待数据信号高电平'0'为26us-28us'1'为70us 

 	data_byte<<=1;//接收的数据为高位在前右移 

	 data_byte|=temp; 

  } 

  return data_byte; 
} 

/*******************************************************************************
* 函 数 名       : void Start(void)
* 函数功能		 : 起始信号
* 输    入       : 无
* 输    出       : 无
*******************************************************************************/
//void ReadData(unsigned int *temp,unsigned int *humi)
/*
void ReadData(void)
{
	unsigned char i;
	unsigned int t,h,add;
	Start();
	while(DAT);				// 等待响应
	while(!DAT)
	{
			
		i++;
	}
	if(i<15)				// 响应信号应为80us
	{
		
		return;
	}
	while(DAT);				// 等待发送数据
	h = ReadOneByte();			// 读取温度高8位
	h<<=8;	 
	h+= ReadOneByte();			// 读取温度低8位
	t = ReadOneByte();			// 读取湿度高8位
	t<<=8;
	t+= ReadOneByte();			// 读取湿度低8位
	check = ReadOneByte();		// 读取校验码
	add = (t>>8)+(t&0xff)+(h>>8)+(h&0xff);		// 求校验码
	if(check!=(add&0xff))		// 检验校验码是否正确
	{
		return;
	}
 	if (DHT==1)					// DHT11数据转换
	temp = t>>8;
	humi = h>>8;
	LED = 0;

}
*/


void receive()//接收数据 

{ 

  unsigned char T_H,T_L,R_H,R_L,check,num_check,i; 

  start();//开始信号// 

  DAT=1;   //主机设为输入判断从机DHT11响应信号 

  if(!DAT)//判断从机是否有低电平响应信号// 

  {  
	while(!DAT);//判断从机发出 80us 的低电平响应信号是否结束 

	while(DAT);//判断从机发出 80us 的高电平是否结束如结束则主机进入数据接收状态 

	R_H=receive_byte();//湿度高位 

 	R_L=receive_byte();//湿度低位 

	T_H=receive_byte();//温度高位 

	T_L=receive_byte();//温度低位 

	check=receive_byte();//校验位 

	DAT=0; //当最后一bit数据接完毕后从机拉低电平50us// 

	for(i=0;i<7;i++)//差不多50us的延时 

	delay1(); 

	DAT=1;//总线由上拉电阻拉高进入空闲状态 

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

