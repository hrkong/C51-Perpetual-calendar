#ifndef __DHT11_H
#define __DHT11_H 			   
#include "sys.h"
#include "delay.h"
#include "oled.h"

#define Datout PAout(2)
#define Datin  PAin(2)

void DHT11_start(void);
u8 DHT11_rec_byte(void);
static u8 DHT11_receive(void);
void DHT11_TEST(void);

#endif