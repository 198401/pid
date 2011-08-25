#ifndef __HT1622_H__
#define __HT1622_H__

extern void display_char( unsigned char * temp);
extern void display_digital(unsigned char * disbuf,unsigned char dotp,unsigned char shine);
extern void clearLCD(void);
extern void initLCD_1622(void);
extern void floattochar(const float fdata,unsigned char * disbuf,unsigned char dotp);

#endif
