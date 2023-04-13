#include "reg52.h"
#include "intrins.h"
#include "ds1302.h"
/*******************************************************************************
* �� �� ��       : unsigned char ReadByte(void)
* ��������		 : ��ȡһ�ֽ���
* ��    ��       : ��
* ��    ��       : ��ȡ����ֵ
*******************************************************************************/
unsigned char ReadByte(void)
{
	unsigned char dat=0;
	unsigned char i;
	SDA = 1;
	for(i=0;i<8;i++)			// ��ȡһ�ֽڣ�8����
	{
		SCK = 0;				// �½��ض�ȡ
		_nop_();_nop_();
		if(SDA)
			dat |= 1<<i;
		_nop_();_nop_();
		SDA=1;
		SCK = 1;
		_nop_();_nop_();
	}
	SCK = 0;

	return dat;					// ���ض�ȡֵ
}
/*******************************************************************************
* �� �� ��       : void WriteByte(unsigned char dat)
* ��������		 : д��һ�ֽ���
* ��    ��       : dat ��д������
* ��    ��       : ��
*******************************************************************************/
void WriteByte(unsigned char dat)
{
	unsigned char i;
	for(i=0;i<8;i++)			// д��һ�ֽڣ�8����
	{
		SCK = 0;
		_nop_();_nop_();
		if(dat&0X01)			
			SDA = 1;
		else
			SDA = 0;
		_nop_();_nop_();
		SCK = 1; 				// ��������Ч
		_nop_();_nop_();
		dat>>=1;				// LSB��д��
	}
	SCK = 0;

}
/*******************************************************************************
* �� �� ��       : void WriteCommand(unsigned char Add,unsigned char Dat)
* ��������		 : д��ʱ��
* ��    ��       : Add ��д�����ݵ�ַ��Dat ��д������
* ��    ��       : ��
*******************************************************************************/
void WriteCommand(unsigned char Add,unsigned char Dat)
{
	CE = 1;				// ʹ��
	WriteByte(Add);		// д���ַ
	WriteByte(Dat);		// д������
	CE = 0;
/*
	unsigned char i;
	CE=1;					//����DS1302����	
	//д��Ŀ���ַ��addr
	addr = addr & 0xFE;   //���λ���㣬�Ĵ���0λΪ0ʱд��Ϊ1ʱ��
	for (i = 0; i < 8; i ++) {
		if (addr & 0x01) {
			SDA=1;
			}
		else {
			SDA=0;
			}
		SCK=1;      //����ʱ��
		SCK=0;
		addr = addr >> 1;
		}	
	//д�����ݣ�d
	for (i = 0; i < 8; i ++) {
		if (d & 0x01) {
			SDA=1;
			}
		else {
			SDA=0;
			}
		SCK=1;    //����ʱ��
		SCK=0;
		d = d >> 1;
		}
	CE=0;		//ֹͣDS1302����
*/
}
/*******************************************************************************
* �� �� ��       : unsigned char ReadCommand(unsigned char Add)
* ��������		 : ��ȡʱ��
* ��    ��       : Add ����ȡ���ݵ�ַ
* ��    ��       : ��ȡ����
*******************************************************************************/
unsigned char ReadCommand(unsigned char Add)
{
	unsigned char dat = 0;
	CE = 1;					// ʹ��
	WriteByte(Add);			// д�����ȡ���ݵ�ַ
	dat = ReadByte();		// ��ȡ����	
	CE = 0;
	return dat;
/*******************************************************************************
	unsigned char i,temp;	
	CE=1;					//����DS1302����
	//д��Ŀ���ַ��addr
	addr = addr | 0x01;    //���λ�øߣ��Ĵ���0λΪ0ʱд��Ϊ1ʱ��
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
	//������ݣ�temp
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
	CE=0;					//ֹͣDS1302����
	return temp;
*******************************************************************************/
}
/*******************************************************************************
* �� �� ��       : void DS1302_Init(void)
* ��������		 : DS1302��ʼ������
* ��    ��       : ��
* ��    ��       : ��
*******************************************************************************/
void DS1302_Init(void)
{
	CE = 0;
	SCK = 0;
	WriteCommand(0x8E,0);				// ȥд����
	WriteCommand(SECO_WRITE,0X00);		// ����������ģʽ
	
}