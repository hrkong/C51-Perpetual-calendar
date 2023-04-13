#ifndef _key_H
#define _key_H

#include "public.h"

//��������������ƽ�
sbit KEY1=P2^0;
sbit KEY2=P2^1;
sbit KEY3=P2^2;
sbit KEY4=P2^3;	  //����LCD12864ҲҪ�õ�P3.3�����Բ�������LCD��RST��


//ʹ�ú궨������������µļ�ֵ
#define KEY1_PRESS	1				//��һ����������ģʽ����
#define KEY2_PRESS	2				//�ڶ�����������ѡ�������ĸ�ʱ��
#define KEY3_PRESS	3				//����������������
#define KEY4_PRESS	4				//���ĸ���������
#define KEY_UNPRESS	0


u8 key_scan(u8 mode);

#endif