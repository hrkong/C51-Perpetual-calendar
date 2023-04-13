#ifndef	__DS1302_H__
#define	__DS1302_H__


/**********ADDRESS**********/
#define	SECO_READ	0X81
#define	MINU_READ	0X83
#define	HOUR_READ	0X85
#define	DATE_READ	0X87
#define	MONT_READ	0X89
#define	WEEK_READ	0X8B
#define	YEAR_READ	0X8D

#define	SECO_WRITE	0X80
#define	MINU_WRITE	0X82
#define	HOUR_WRITE	0X84
#define	DATE_WRITE	0X86
#define	MONT_WRITE	0X88
#define	WEEK_WRITE	0X8A
#define	YEAR_WRITE	0X8C

sbit CE = P1^2;			 //CE口就是RST口
sbit SCK = P1^1;
sbit SDA = P1^0;

void DS1302_Init(void);
void WriteCommand(unsigned char Add,unsigned char Dat);
unsigned char ReadCommand(unsigned char Add);

#endif