#include<reg52.h>
#define UINT unsigned int 
#define UCHAR  unsigned char 
 
sbit en=P3^4; //   使能
sbit rs=P3^5; //   LCD的数据和控制端口
sbit rw=P3^6; //   LCD的读写控制
sbit rd=P3^7; //   LCD的串行和并行选择
sbit duan=P2^6; //关闭数码管的显示
sbit wei=P2^7;


sbit sclk=P1^1;//DS1302的控制和数据端口
sbit io=P1^0;
sbit ce=P1^2;//复位端口，我认为和使能口一样。可这样理解

 int c;


UCHAR   line[]="年月日";
UCHAR   line2[]="时分秒";
UCHAR   line3[]="星期";
UCHAR   table[]="一二三四五六日"; 
UCHAR   digit[]="0123456789";

void show_on( UCHAR ADD);
void DS1302_init();
void DS1302_write(UCHAR add,UCHAR dat);
//void LCD_chinese( UCHAR P[10]);
void LCD_rw(UCHAR wr,bit n);

/*******************************************************************
 函数名称：void delay（）；
 
 函数功能： ms 延时
 输入参数： x ，延时时间
 输出函数： 无

********************************************************************/

 void delay( UINT x)   //ms延时
  {
     UINT i,j;
	  for(i=x;i>0;i--)
	    for(j=110;j>0;j--);
  }
/*******************************************************************
 函数名称：void LCD_rw(UCHAR com,bit n)
 
 函数功能： LCD的数据和命令写入
 输入参数： wr 要写入的数据和命令  n:  n=1时，写入数据。n=0时写入命令
 输出函数： 无

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
 函数名称： LCD_init()
 
 函数功能： LCD12864 的初始化
 输入参数： 无
 输出函数： 无

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
 函数名称：1302write_byte( UCHAR dat) 
 
 函数功能： 写入一个字节（ 在sclk的上升沿写入，最低位先写入 ）
 输入参数： dat 要写入的字节
 输出函数： 无

********************************************************************/

void DS1302write_byte( UCHAR dat)
 {
	int i;
	//unsigned char temp;
	  sclk=0;
	
	for( i=0;i<8;i++)
	  {		  
	    	//dat=temp;
	     io=dat&0x01; //比原文少延时
		 sclk=1;
		 delay(2); 	 		   
	   	 sclk=0;	  		 
		 dat>>=1;  

	  }


 }

 /*******************************************************************
 函数名称：void 1302_write(UCHAR add,UCHAR dat ) 
 
 函数功能： 在指定位置写入数据	 (在上升沿将数据写入)
 输入参数： add，地址；dat，数据
 输出函数： 无

********************************************************************/

void DS1302_write(UCHAR add,UCHAR dat)
 {
    ce=0;
	sclk=0;
	ce=1;

	DS1302write_byte( add );// 要写入1302的寄存器的地址
	DS1302write_byte( dat);//	 要写入1302的寄存器的数据

	sclk=1;
	ce=0; //类似于使能端，使用后将其释放
	sclk=0;
	delay(2);
	
 }

/*******************************************************************
 函数名称：UCHAR 1302read_byte( ) 
 
 函数功能： 读取一个字节（ 在sclk的下降沿读出，最低位先读入 ）
 输入参数： 无
 输出函数： DAT ,读出的数据

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
 函数名称：	UCHAR 1302_read( UCHAR add )
 
 函数功能： 读出指定位置的数据 
 输入参数： add，地址；
 输出函数： Temp

********************************************************************/

UCHAR DS1302_read( UCHAR add )
 {
   	 UCHAR Temp;
    ce=0;
	sclk=0;
	ce=1;
	DS1302write_byte( add );	 //这里和原文有区别
	
	Temp=(DS1302read_byte( ) );
	sclk=1 ;
	 	ce=0;
		sclk=0;
	return Temp	;	 //这里有改动，去了个sclk=1；和ce=0换了位置
	
	
 }



/*******************************************************************
 函数名称：	void DS1302_init()
 
 函数功能： 初始化DS1302
 输入参数： 无
 输出函数： 无

********************************************************************/

void DS1302_init()
 {
    DS1302_write( 0x8E,0x00);//这里有改动开了写保护
	DS1302_write( 0x80, ((55/10)<<4|(55%10))); //秒
	delay(1);
	DS1302_write( 0x8a,((6/10)<<4|(6%10)));//星期
	 delay(1);
	
	DS1302_write( 0x82 , ((30/10)<<4|(30%10)));	//分
	delay(1);
	DS1302_write( 0x84 , ((23/10)<<4|(23%10)));	//时
	delay(1);
	DS1302_write( 0x86 , ((23/10)<<4|(23%10)));//日
	delay(1);
	DS1302_write( 0x88, ((4/10)<<4|(4%10))); //月
	delay(1);
	DS1302_write( 0x8c,((22/10)<<4|(22%10)));//年
	delay(1);
   

    DS1302_write( 0x90,0xa6);
	delay(1);
	DS1302_write( 0x8E,0x80);

 }


 /*******************************************
	    对指定地址读取回来的值进行处理

 *******************************************/
  
 
UCHAR read_value(unsigned char add) // 读取指定地址的值，并转化为10进制
 {
     unsigned char BcdValue,DecValue;
	   BcdValue=DS1302_read(  add );
	   DecValue=(((BcdValue&0x70)>>4)*10+(BcdValue&0x0f));

	   return DecValue;

 }

 

UCHAR read_year( )	//年需要另外处理，值为00~99
 {
     unsigned char BcdValue,DecValue;
	   BcdValue=DS1302_read(0x8d);
	   DecValue=(((BcdValue&0xf0)>>4)*10+(BcdValue&0x0f));

	   return DecValue;

 }
 
 void display(unsigned char add,unsigned char x) //将转化为十进制的数进行显示
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
 函数名称：	void init_devices()
 
 函数功能： 初始化所有需要初始化的器件
 输入参数： 无
 输出函数： 无

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
 函数名称：	void init_display()
 
 函数功能：  对需要静态显示的数字和汉字进行初始化
 输入参数： 无
 输出函数： 无

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
 函数名称： void main() 
 
 函数功能： 主函数
 输入参数： 无
 输出函数： 无

********************************************************************/
void main()
 {	   
 	  init_devices();
	  delay(100);
	  init_display( );
	  while(1)
	   {
	       display(0x82,read_year( ));	  //年
		   display(0x84,read_value(0x89)); //月
		   display(0x86,read_value(0x87)); //日
		   display(0x95,read_value(0x81));//秒
		   
		   display(0x93,read_value(0x83));//分
		   display(0x91,read_value(0x85));//时

		   display(0x8c,read_value(0x8b));//星期
		  
		   
		   		 
 }	  
	
 }