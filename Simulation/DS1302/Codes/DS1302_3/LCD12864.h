#ifndef  __LCD12864_H_
#define  __LCD12864_H_

#ifndef  uchar
#define  uchar   unsigned char
#endif

#ifndef  uint
#define  uint	 unsigned int
#endif


void lcd_init();
void write_cmd(uchar cmd);
void lcd12864_printf(uchar x,uchar y,uchar n,uchar *buf);

#endif