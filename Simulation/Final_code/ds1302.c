#include "reg52.h"
#include "intrins.h"
#include "ds1302.h"
/*******************************************************************************
* 函 数 名       : unsigned char ReadByte(void)
* 函数功能		 : 读取一字节数
* 输    入       : 无
* 输    出       : 读取的数值
*******************************************************************************/
unsigned char ReadByte(void)
{
	unsigned char dat=0;
	unsigned char i;
	SDA = 1;
	for(i=0;i<8;i++)			// 读取一字节，8比特
	{
		SCK = 0;				// 下降沿读取
		_nop_();_nop_();
		if(SDA)
			dat |= 1<<i;
		_nop_();_nop_();
		SDA=1;
		SCK = 1;
		_nop_();_nop_();
	}
	SCK = 0;

	return dat;					// 返回读取值
}
/*******************************************************************************
* 函 数 名       : void WriteByte(unsigned char dat)
* 函数功能		 : 写入一字节数
* 输    入       : dat 待写入数据
* 输    出       : 无
*******************************************************************************/
void WriteByte(unsigned char dat)
{
	unsigned char i;
	for(i=0;i<8;i++)			// 写入一字节，8比特
	{
		SCK = 0;
		_nop_();_nop_();
		if(dat&0X01)			
			SDA = 1;
		else
			SDA = 0;
		_nop_();_nop_();
		SCK = 1; 				// 上升沿有效
		_nop_();_nop_();
		dat>>=1;				// LSB先写入
	}
	SCK = 0;

}
/*******************************************************************************
* 函 数 名       : void WriteCommand(unsigned char Add,unsigned char Dat)
* 函数功能		 : 写入时间
* 输    入       : Add 待写入数据地址，Dat 待写入数据
* 输    出       : 无
*******************************************************************************/
void WriteCommand(unsigned char Add,unsigned char Dat)
{
	CE = 1;				// 使能
	WriteByte(Add);		// 写入地址
	WriteByte(Dat);		// 写入数据
	CE = 0;
/*
	unsigned char i;
	CE=1;					//启动DS1302总线	
	//写入目标地址：addr
	addr = addr & 0xFE;   //最低位置零，寄存器0位为0时写，为1时读
	for (i = 0; i < 8; i ++) {
		if (addr & 0x01) {
			SDA=1;
			}
		else {
			SDA=0;
			}
		SCK=1;      //产生时钟
		SCK=0;
		addr = addr >> 1;
		}	
	//写入数据：d
	for (i = 0; i < 8; i ++) {
		if (d & 0x01) {
			SDA=1;
			}
		else {
			SDA=0;
			}
		SCK=1;    //产生时钟
		SCK=0;
		d = d >> 1;
		}
	CE=0;		//停止DS1302总线
*/
}
/*******************************************************************************
* 函 数 名       : unsigned char ReadCommand(unsigned char Add)
* 函数功能		 : 读取时间
* 输    入       : Add 待读取数据地址
* 输    出       : 读取数据
*******************************************************************************/
unsigned char ReadCommand(unsigned char Add)
{
	unsigned char dat = 0;
	CE = 1;					// 使能
	WriteByte(Add);			// 写入待读取数据地址
	dat = ReadByte();		// 读取数据	
	CE = 0;
	return dat;
/*******************************************************************************
	unsigned char i,temp;	
	CE=1;					//启动DS1302总线
	//写入目标地址：addr
	addr = addr | 0x01;    //最低位置高，寄存器0位为0时写，为1时读
	for (i = 0; i < 8; i ++) {
		if (addr & 0x01) {
			SDA=1;
			}
		else {
			SDA=0;
			}
		SCK=1;
		SCK=0;
		addr = addr >> 1;
		}	
	//输出数据：temp
	for (i = 0; i < 8; i ++) {
		temp = temp >> 1;
		if (SDA) {
			temp |= 0x80;
			}
		else {
			temp &= 0x7F;
			}
		SCK=1;
		SCK=0;
		}	
	CE=0;					//停止DS1302总线
	return temp;
*******************************************************************************/
}
/*******************************************************************************
* 函 数 名       : void DS1302_Init(void)
* 函数功能		 : DS1302初始化数据
* 输    入       : 无
* 输    出       : 无
*******************************************************************************/
void DS1302_Init(void)
{
	CE = 0;
	SCK = 0;
	WriteCommand(0x8E,0);				// 去写保护
	WriteCommand(SECO_WRITE,0X00);		// 工作在正常模式
	
}