
#include "REG52.h"
#ifndef _dht11_H
#define _dht11_H


#define  u8 unsigned char
#define uint  unsigned int
//�ܽŶ���
sbit DHT11_DQ=P0^7;

//��������
u8 DHT11_Init(void);
void DHT11_Rst(void);
u8 DHT11_Check(void);
u8 DHT11_Read_Byte(void);
u8 DHT11_Read_Data(u8 *temp,u8 *humi);

#endif
