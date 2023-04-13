#ifndef	__DHTXX_H__
#define	__DHTXX_H__

#define DHT	1

sbit DAT = P1^7;

extern unsigned int temp;
extern unsigned int humi;
extern unsigned char RH,RL,TH,TL;
extern unsigned char check;

void Delay10ms();
void receive();
//void ReadData(unsigned int *temp,unsigned int *humi);
#endif