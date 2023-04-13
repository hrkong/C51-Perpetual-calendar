#include<reg52.h>
#define UINT unsigned int 
#define UCHAR  unsigned char 
 
sbit en=P3^4; //   ʹ��
sbit rs=P3^5; //   LCD�����ݺͿ��ƶ˿�
sbit rw=P3^6; //   LCD�Ķ�д����
sbit rd=P3^7; //   LCD�Ĵ��кͲ���ѡ��
sbit duan=P2^6; //�ر�����ܵ���ʾ
sbit wei=P2^7;


sbit sclk=P1^1;//DS1302�Ŀ��ƺ����ݶ˿�
sbit io=P1^0;
sbit ce=P1^2;//��λ�˿ڣ�����Ϊ��ʹ�ܿ�һ�������������

 int c;


UCHAR   line[]="������";
UCHAR   line2[]="ʱ����";
UCHAR   line3[]="����";
UCHAR   table[]="һ������������"; 
UCHAR   digit[]="0123456789";

void show_on( UCHAR ADD);
void DS1302_init();
void DS1302_write(UCHAR add,UCHAR dat);
//void LCD_chinese( UCHAR P[10]);
void LCD_rw(UCHAR wr,bit n);

/*******************************************************************
 �������ƣ�void delay������
 
 �������ܣ� ms ��ʱ
 ��������� x ����ʱʱ��
 ��������� ��

********************************************************************/

 void delay( UINT x)   //ms��ʱ
  {
     UINT i,j;
	  for(i=x;i>0;i--)
	    for(j=110;j>0;j--);
  }
/*******************************************************************
 �������ƣ�void LCD_rw(UCHAR com,bit n)
 
 �������ܣ� LCD�����ݺ�����д��
 ��������� wr Ҫд������ݺ�����  n:  n=1ʱ��д�����ݡ�n=0ʱд������
 ��������� ��

********************************************************************/

void LCD_rw(UCHAR wr,bit n)
 {	  
 	rw=0;
	P0=wr;
	rs=(0|n);
	en=0;
	delay(5);
	en=1;
    delay(5);
	en=0;
    delay(5);
    
 }
void LCD_chinese( char  *P )
  {		       
	 	LCD_rw( *P,1);
		    P++; 
		 LCD_rw( *P,1);
	
  }
 /*******************************************************************
 �������ƣ� LCD_init()
 
 �������ܣ� LCD12864 �ĳ�ʼ��
 ��������� ��
 ��������� ��

********************************************************************/

void LCD_init()
 {
    rd=1;
    duan=0;
	wei=0;
	delay(15);
	 LCD_rw(0x38,0);
	delay(5);
	 LCD_rw(0x30,0);
   	delay(5);
	 LCD_rw(0x0c,0);
   	delay(5);
	 LCD_rw(0x06,0);
	 delay(5);
	 LCD_rw(0x01,0);
  	delay(5);
 }

 
/*******************************************************************
 �������ƣ�1302write_byte( UCHAR dat) 
 
 �������ܣ� д��һ���ֽڣ� ��sclk��������д�룬���λ��д�� ��
 ��������� dat Ҫд����ֽ�
 ��������� ��

********************************************************************/

void DS1302write_byte( UCHAR dat)
 {
	int i;
	//unsigned char temp;
	  sclk=0;
	
	for( i=0;i<8;i++)
	  {		  
	    	//dat=temp;
	     io=dat&0x01; //��ԭ������ʱ
		 sclk=1;
		 delay(2); 	 		   
	   	 sclk=0;	  		 
		 dat>>=1;  

	  }


 }

 /*******************************************************************
 �������ƣ�void 1302_write(UCHAR add,UCHAR dat ) 
 
 �������ܣ� ��ָ��λ��д������	 (�������ؽ�����д��)
 ��������� add����ַ��dat������
 ��������� ��

********************************************************************/

void DS1302_write(UCHAR add,UCHAR dat)
 {
    ce=0;
	sclk=0;
	ce=1;

	DS1302write_byte( add );// Ҫд��1302�ļĴ����ĵ�ַ
	DS1302write_byte( dat);//	 Ҫд��1302�ļĴ���������

	sclk=1;
	ce=0; //������ʹ�ܶˣ�ʹ�ú����ͷ�
	sclk=0;
	delay(2);
	
 }

/*******************************************************************
 �������ƣ�UCHAR 1302read_byte( ) 
 
 �������ܣ� ��ȡһ���ֽڣ� ��sclk���½��ض��������λ�ȶ��� ��
 ��������� ��
 ��������� DAT ,����������

********************************************************************/


UCHAR DS1302read_byte( )
 {
    int i;
	UCHAR DAT;
	sclk=0;
    for(i=0;i<8;i++)
	  {			 
		 DAT=DAT>>1;
	     if(io==1)
		 DAT|=0x80; 
	     sclk=1;
		delay(2);
	  	sclk=0;
		
	 
	  }
	  return DAT;
 }

/*******************************************************************
 �������ƣ�	UCHAR 1302_read( UCHAR add )
 
 �������ܣ� ����ָ��λ�õ����� 
 ��������� add����ַ��
 ��������� Temp

********************************************************************/

UCHAR DS1302_read( UCHAR add )
 {
   	 UCHAR Temp;
    ce=0;
	sclk=0;
	ce=1;
	DS1302write_byte( add );	 //�����ԭ��������
	
	Temp=(DS1302read_byte( ) );
	sclk=1 ;
	 	ce=0;
		sclk=0;
	return Temp	;	 //�����иĶ���ȥ�˸�sclk=1����ce=0����λ��
	
	
 }



/*******************************************************************
 �������ƣ�	void DS1302_init()
 
 �������ܣ� ��ʼ��DS1302
 ��������� ��
 ��������� ��

********************************************************************/

void DS1302_init()
 {
    DS1302_write( 0x8E,0x00);//�����иĶ�����д����
	DS1302_write( 0x80, ((55/10)<<4|(55%10))); //��
	delay(1);
	DS1302_write( 0x8a,((6/10)<<4|(6%10)));//����
	 delay(1);
	
	DS1302_write( 0x82 , ((30/10)<<4|(30%10)));	//��
	delay(1);
	DS1302_write( 0x84 , ((23/10)<<4|(23%10)));	//ʱ
	delay(1);
	DS1302_write( 0x86 , ((23/10)<<4|(23%10)));//��
	delay(1);
	DS1302_write( 0x88, ((4/10)<<4|(4%10))); //��
	delay(1);
	DS1302_write( 0x8c,((22/10)<<4|(22%10)));//��
	delay(1);
   

    DS1302_write( 0x90,0xa6);
	delay(1);
	DS1302_write( 0x8E,0x80);

 }


 /*******************************************
	    ��ָ����ַ��ȡ������ֵ���д���

 *******************************************/
  
 
UCHAR read_value(unsigned char add) // ��ȡָ����ַ��ֵ����ת��Ϊ10����
 {
     unsigned char BcdValue,DecValue;
	   BcdValue=DS1302_read(  add );
	   DecValue=(((BcdValue&0x70)>>4)*10+(BcdValue&0x0f));

	   return DecValue;

 }

 

UCHAR read_year( )	//����Ҫ���⴦��ֵΪ00~99
 {
     unsigned char BcdValue,DecValue;
	   BcdValue=DS1302_read(0x8d);
	   DecValue=(((BcdValue&0xf0)>>4)*10+(BcdValue&0x0f));

	   return DecValue;

 }
 
 void display(unsigned char add,unsigned char x) //��ת��Ϊʮ���Ƶ���������ʾ
   {
      unsigned i,j;
	   i=x/10;
	   j=x%10;
	   LCD_rw( add, 0);
	   LCD_rw( digit[i], 1); 
	   LCD_rw( digit[j], 1); 
	   delay(5);
   }	
/*void display_week()
  {	   //int a;
      unsigned char BcdValue,DecValue;
	   BcdValue=DS1302_read(0x8b);
	   //DecValue=(((BcdValue&0xf0)>>4)*10+(BcdValue&0x0f));
	   LCD_rw( 0x8c, 0);
	   	     
	   //DecValue=DecValue*2-2;
	//	a=(int)(DecValue);
	  // LCD_rw( 0x8c, 0); 
	  // LCD_chinese( &table[4)]);
	   // display(0x8c,DecValue);
  }	*/


/*******************************************************************
 �������ƣ�	void init_devices()
 
 �������ܣ� ��ʼ��������Ҫ��ʼ��������
 ��������� ��
 ��������� ��

********************************************************************/

void init_devices( )
 {
   LCD_init();
   delay(5);
   DS1302_init();
   delay(5);
//   display( );
   delay(5);
 }

/*******************************************************************
 �������ƣ�	void init_display()
 
 �������ܣ�  ����Ҫ��̬��ʾ�����ֺͺ��ֽ��г�ʼ��
 ��������� ��
 ��������� ��

********************************************************************/
void init_display( )
 {
    LCD_rw(0x81,0);
	LCD_rw('2',1);
	LCD_rw('0',1);
	LCD_rw(0x83,0);
	LCD_chinese( &line[0]);
	LCD_rw(0x85,0);
	LCD_chinese( &line[2]);
	LCD_rw(0x87,0);
	LCD_chinese( &line[4]);

	LCD_rw(0x92,0);
	LCD_chinese( &line2[0]);
	LCD_rw(0x94,0);
	LCD_chinese( &line2[2]);
	LCD_rw(0x96,0);
	LCD_chinese( &line2[4]);

    LCD_rw(0x8a,0);
	LCD_chinese( &line3[0]);
	LCD_rw(0x8b,0);
	LCD_chinese( &line3[2]);

 }
 
/*******************************************************************
 �������ƣ� void main() 
 
 �������ܣ� ������
 ��������� ��
 ��������� ��

********************************************************************/
void main()
 {	   
 	  init_devices();
	  delay(100);
	  init_display( );
	  while(1)
	   {
	       display(0x82,read_year( ));	  //��
		   display(0x84,read_value(0x89)); //��
		   display(0x86,read_value(0x87)); //��
		   display(0x95,read_value(0x81));//��
		   
		   display(0x93,read_value(0x83));//��
		   display(0x91,read_value(0x85));//ʱ

		   display(0x8c,read_value(0x8b));//����
		  
		   
		   		 
 }	  
	
 }