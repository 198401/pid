
/********************************************************
 Author        : 

 Date          : 

 File          : lcd

 Hardware      : ADuC702X

 Description   : LCD_HT1622
*************************************************************/

#include <aduc7024.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef unsigned char           BYTE;       /* Prefix: by	*/
typedef unsigned int	        WORD;       /* Prefix: w	*/

/********************LCD_HT1622端口定义***********************/
//di ---p1.3
#define setDI_1622   	GP1DAT |= 0x08080000
#define clrDI_1622   	GP1DAT &= ~0x00080000 
//wr-sk--p1.2
#define setSK_1622  	GP1DAT |= 0x04040000  
#define clrSK_1622   	GP1DAT &= ~0x00040000 
//cs---p4.2
#define setCS_1622  	GP4DAT |= 0x04040000  
#define clrCS_1622   	GP4DAT &= ~0x00040000 

/********************LCD_HT1622模式定义***********************/
#define LCD_1622_BIAS		0X52 //LCD 1/3 偏置选择 4背级
#define LCD_1622_SYSEN 		0X02 //打开系统振荡器
#define LCD_1622_LCDON 		0X06 //打开LCD偏置发生器
#define LCD_1622_SYSDIS 	0X00 //关闭系统振荡器和LCD偏置发生器
#define LCD_1622_LCDOFF		0X04 //关闭LCD偏置发生器
#define LCD_1622_CLRWDT 	0X1c //清零 WDT
#define LCD_1622_TIMERDIS	0X08 //禁止时基输出

//const unsigned char LED7Addr[]={
//	0x0C4,		// 0 LSB
//	0x0CC,		// 0 MSB
//	0x004,		// 1 LSB
//	0x0BC,		// 1 MSB
//	0x014,		// 2 LSB
//	0x00C,		// 2 MSB
//	0x024,		// 3 LSB
//	0x01C,		// 3 MSB
//	0x034,		// 4 LSB
//	0x02C,		// 4 MSB
//	0x044,		// 5 LSB
//	0x03C,		// 5 MSB
//};

const unsigned char LED7Code[]={
	0x0F0,		// 0 LSB
	0x050,		// 0 MSB
	0x060,		// 1 LSB
	0x000,		// 1 MSB
	0x0B0,		// 2 LSB
	0x060,		// 2 MSB
	0x0F0,		// 3 LSB
	0x020,		// 3 MSB
	0x060,		// 4 LSB
	0x030,		// 4 MSB
	0x0D0,		// 5 LSB
	0x030,		// 5 MSB
	0x0D0,		// 6 LSB
	0x070,		// 6 MSB
	0x070,		// 7 LSB
	0x000,		// 7 MSB
	0x0F0,		// 8 LSB
	0x070,		// 8 MSB
	0x0F0,		// 9 LSB
	0x030,		// 9 MSB
//	0x070,		// A LSB
//	0x070,		// A MSB
//	0x0C0,		// B LSB
//	0x070,		// B MSB
//	0x090,		// C LSB
//	0x050,		// C MSB
//	0x0E0,		// D LSB
//	0x060,		// D MSB
//	0x090,		// E LSB
//	0x070,		// E MSB
//	0x010,		// F LSB
//	0x070,		// F MSB
	0x000,
	0x000
};

const unsigned char LED16Char[]={		
	0x010,		// A LSB
	0x010,		// A ...
	0x0D0,		// A ...
	0x0B0,		// A MSB
	0x080,		// B LSB
	0x080,		// B ...
	0x0C0,		// B ...
	0x0B0,		// B MSB
	0x090,		// C LSB
	0x090,		// C ...
	0x000,		// C ...
	0x090,		// C MSB
	0x0D0,		// D LSB
	0x0B0,		// D ...
	0x090,		// D ...
	0x000,		// D MSB
	0x090,		// E LSB
	0x090,		// E ...
	0x040,		// E ...
	0x0B0,		// E MSB
	0x010,		// F LSB
	0x010,		// F ...
	0x040,		// F ...
	0x0B0,		// F MSB
	0x080,		// G LSB
	0x080,		// G ...
	0x0C0,		// G ...
	0x090,		// G MSB
	0x000,		// H LSB
	0x000,		// H ...
	0x0D0,		// H ...
	0x0B0,		// H MSB
	0x0D0,		// I LSB
	0x0B0,		// I ...
	0x000,		// I ...
	0x000,		// I MSB
	0x080,		// J LSB
	0x080,		// J ...
	0x090,		// J ...
	0x000,		// J MSB
	0x000,		// K LSB
	0x040,		// K ...
	0x020,		// K ...
	0x0B0,		// K MSB
	0x080,		// L LSB
	0x080,		// L ...
	0x000,		// L ...
	0x090,		// L MSB
	0x020,		// M LSB
	0x000,		// M ...
	0x0B0,		// M ...
	0x090,		// M MSB
	0x020,		// N LSB
	0x040,		// N ...
	0x090,		// N ...
	0x090,		// N MSB
	0x090,		// O LSB
	0x090,		// O ...
	0x090,		// O ...
	0x090,		// O MSB
	0x010,		// P LSB
	0x010,		// P ...
	0x050,		// P ...
	0x0B0,		// P MSB
	0x090,		// Q LSB
	0x0D0,		// Q ...
	0x090,		// Q ...
	0x090,		// Q MSB
	0x010,		// R LSB
	0x050,		// R ...
	0x050,		// R ...
	0x0B0,		// R MSB
	0x090,		// S LSB
	0x090,		// S ...
	0x0C0,		// S ...
	0x030,		// S MSB
	0x050,		// T LSB
	0x030,		// T ...
	0x000,		// T ...
	0x000,		// T MSB
	0x080,		// U LSB
	0x080,		// U ...
	0x090,		// U ...
	0x090,		// U MSB
	0x000,		// V LSB
	0x000,		// V ...
	0x020,		// V ...
	0x0D0,		// V MSB
	0x000,		// W LSB
	0x040,		// W ...
	0x090,		// W ...
	0x0D0,		// W MSB
	0x020,		// X LSB
	0x040,		// X ...
	0x020,		// X ...
	0x040,		// X MSB
	0x080,		// Y LSB
	0x080,		// Y ...
	0x0D0,		// Y ...
	0x030,		// Y MSB
	0x090,		// Z LSB
	0x090,		// Z ...
	0x020,		// Z ...
	0x040,		// Z MSB
	0x000,
	0x000,
	0x000,
	0x000
};
 
//const unsigned char LED16Num[]={		
//	0x090,		// 0 LSB
//	0x090,		// 0 ...
//	0x090,		// 0 ...
//	0x090,		// 0 MSB
//	0x000,		// 1 LSB
//	0x000,		// 1 ...
//	0x090,		// 1 ...
//	0x000,		// 1 MSB
//	0x090,		// 2 LSB
//	0x090,		// 2 ...
//	0x050,		// 2 ...
//	0x0A0,		// 2 MSB
//	0x090,		// 3 LSB
//	0x090,		// 3 ...
//	0x0D0,		// 3 ...
//	0x020,		// 3 MSB
//	0x000,		// 4 LSB
//	0x000,		// 4 ...
//	0x0D0,		// 4 ...
//	0x030,		// 4 MSB
//	0x090,		// 5 LSB
//	0x090,		// 5 ...
//	0x0C0,		// 5 ...
//	0x030,		// 5 MSB
//	0x090,		// 6 LSB
//	0x090,		// 6 ...
//	0x0C0,		// 6 ...
//	0x0B0,		// 6 MSB
//	0x010,		// 7 LSB
//	0x010,		// 7 ...
//	0x090,		// 7 ...
//	0x000,		// 7 MSB
//	0x090,		// 8 LSB
//	0x090,		// 8 ...
//	0x0D0,		// 8 ...
//	0x0B0,		// 8 MSB
//	0x090,		// 9 LSB
//	0x090,		// 9 ...
//	0x0D0,		// 9 ...
//	0x030,		// 9 MSB
//	0x010,		// A LSB
//	0x010,		// A ...
//	0x0D0,		// A ...
//	0x0B0,		// A MSB
//	0x080,		// B LSB
//	0x080,		// B ...
//	0x0C0,		// B ...
//	0x0B0,		// B MSB
//	0x090,		// C LSB
//	0x090,		// C ...
//	0x000,		// C ...
//	0x090,		// C MSB
//	0x0D0,		// D LSB
//	0x0B0,		// D ...
//	0x090,		// D ...
//	0x000,		// D MSB
//	0x090,		// E LSB
//	0x090,		// E ...
//	0x040,		// E ...
//	0x0B0,		// E MSB
//	0x010,		// F LSB
//	0x010,		// F ...
//	0x040,		// F ...
//	0x0B0,		// F MSB
//	0x000,
//	0x000,
//	0x000,
//	0x000
//};


static void SENDCOMB(BYTE address);
static BYTE LCDWRITE_1622(const unsigned char data,BYTE Addr);
static void SENDCOMA(BYTE command);
static void SendBit_HT1622(BYTE op_data,BYTE num);
static void Delay(int a);
static void Reset_1622(void);
static void Delay50ms(void);

static void SENDCOMB(BYTE address)
{
    Reset_1622();
//    Delay(1);           /*delay 10us*/
    SendBit_HT1622(0xA0,3);   /*ID=101 0*/
    address=address;
    SendBit_HT1622(address,6); /* send address*/
}

static BYTE LCDWRITE_1622(const unsigned char data,BYTE Addr)
{
    SENDCOMB(Addr);//发送数据模式指令101，发送地址
    SendBit_HT1622(data, 4);
    setCS_1622;//CS_1622=1
    return(1);
}

void display_char(unsigned char * temp)   
{
   	LCDWRITE_1622(LED16Char[(*(temp + 0) - 'A')*4],0xC0);
	LCDWRITE_1622(LED16Char[(*(temp + 0) - 'A')*4 + 1],0xB8);
	LCDWRITE_1622(LED16Char[(*(temp + 0) - 'A')*4 + 2],0xB0);
	LCDWRITE_1622(LED16Char[(*(temp + 0) - 'A')*4 + 3],0xC8);
	
	LCDWRITE_1622(LED16Char[(*(temp + 1) - 'A')*4],0x08);
	LCDWRITE_1622(LED16Char[(*(temp + 1) - 'A')*4 + 1],0x10);
	LCDWRITE_1622(LED16Char[(*(temp + 1) - 'A')*4 + 2],0xA8);
	LCDWRITE_1622(LED16Char[(*(temp + 1) - 'A')*4 + 3],0x00); 
	
	LCDWRITE_1622(LED16Char[(*(temp + 2) - 'A')*4],0x20);
	LCDWRITE_1622(LED16Char[(*(temp + 2) - 'A')*4 + 1],0x28);
	LCDWRITE_1622(LED16Char[(*(temp + 2) - 'A')*4 + 2],0xA0);
	LCDWRITE_1622(LED16Char[(*(temp + 2) - 'A')*4 + 3],0x18); 
	
	LCDWRITE_1622(LED16Char[(*(temp + 3) - 'A')*4],0x38);
	LCDWRITE_1622(LED16Char[(*(temp + 3) - 'A')*4 + 1],0x40);
	LCDWRITE_1622(LED16Char[(*(temp + 3) - 'A')*4 + 2],0x98);
	LCDWRITE_1622(LED16Char[(*(temp + 3) - 'A')*4 + 3],0x30); 
	
	LCDWRITE_1622(LED16Char[(*(temp + 4) - 'A')*4],0x88);
	LCDWRITE_1622(LED16Char[(*(temp + 4) - 'A')*4 + 1],0x80);
	LCDWRITE_1622(LED16Char[(*(temp + 4) - 'A')*4 + 2],0x78);
	LCDWRITE_1622(LED16Char[(*(temp + 4) - 'A')*4 + 3],0x90); 
	
	LCDWRITE_1622(LED16Char[(*(temp + 5) - 'A')*4],0x68);
	LCDWRITE_1622(LED16Char[(*(temp + 5) - 'A')*4 + 1],0x60);
	LCDWRITE_1622(LED16Char[(*(temp + 5) - 'A')*4 + 2],0x58);
	LCDWRITE_1622(LED16Char[(*(temp + 5) - 'A')*4 + 3],0x70); 
	
	LCDWRITE_1622(((LED16Char[(*(temp + 6) - 'A')*4]) >> 3)|
				  ((LED16Char[(*(temp + 6) - 'A')*4]) << 3)|
				  ((LED16Char[(*(temp + 6) - 'A')*4]&0x40) >> 1)|
				  ((LED16Char[(*(temp + 6) - 'A')*4]&0x20) << 1)
				  ,0x64);
	LCDWRITE_1622(((LED16Char[(*(temp + 6) - 'A')*4 + 1]) >> 3)|
				  ((LED16Char[(*(temp + 6) - 'A')*4 + 1]) << 3)|
				  ((LED16Char[(*(temp + 6) - 'A')*4 + 1]&0x40) >> 1)|
				  ((LED16Char[(*(temp + 6) - 'A')*4 + 1]&0x20) << 1)
				  ,0x6C);
	LCDWRITE_1622(((LED16Char[(*(temp + 6) - 'A')*4 + 2]&0x80) >> 3)|
				  ((LED16Char[(*(temp + 6) - 'A')*4 + 2]&0x10) << 3)|
				  ((LED16Char[(*(temp + 6) - 'A')*4 + 2]&0x40) >> 1)|
				  ((LED16Char[(*(temp + 6) - 'A')*4 + 2]&0x20) << 1)
				  ,0x74);
	LCDWRITE_1622(((LED16Char[(*(temp + 6) - 'A')*4 + 3]) >> 3)|
				  ((LED16Char[(*(temp + 6) - 'A')*4 + 3]) << 3)|
				  ((LED16Char[(*(temp + 6) - 'A')*4 + 3]&0x40) >> 1)|
				  ((LED16Char[(*(temp + 6) - 'A')*4 + 3]&0x20) << 1)
				  ,0x54);

	LCDWRITE_1622(((LED16Char[(*(temp + 7) - 'A')*4]) >> 3)|
				  ((LED16Char[(*(temp + 7) - 'A')*4]) << 3)|
				  ((LED16Char[(*(temp + 7) - 'A')*4]&0x40) >> 1)|
				  ((LED16Char[(*(temp + 7) - 'A')*4]&0x20) << 1)
				  ,0x8C);
	LCDWRITE_1622(((LED16Char[(*(temp + 7) - 'A')*4 + 1]) >> 3)|
				  ((LED16Char[(*(temp + 7) - 'A')*4 + 1]) << 3)|
				  ((LED16Char[(*(temp + 7) - 'A')*4 + 1]&0x40) >> 1)|
				  ((LED16Char[(*(temp + 7) - 'A')*4 + 1]&0x20) << 1)
				  ,0x94);
	LCDWRITE_1622(((LED16Char[(*(temp + 7) - 'A')*4 + 2]) >> 3)|
				  ((LED16Char[(*(temp + 7) - 'A')*4 + 2]) << 3)|
				  ((LED16Char[(*(temp + 7) - 'A')*4 + 2]&0x40) >> 1)|
				  ((LED16Char[(*(temp + 7) - 'A')*4 + 2]&0x20) << 1)
				  ,0x4C);
	LCDWRITE_1622(((LED16Char[(*(temp + 7) - 'A')*4 + 3]) >> 3)|
				  ((LED16Char[(*(temp + 7) - 'A')*4 + 3]) << 3)|
				  ((LED16Char[(*(temp + 7) - 'A')*4 + 3]&0x40) >> 1)|
				  ((LED16Char[(*(temp + 7) - 'A')*4 + 3]&0x20) << 1)
				  ,0x84);
}

void digital(BYTE * data, BYTE dotp)
{
   	LCDWRITE_1622(0,0xC4);
    if(*data == '-')		
		LCDWRITE_1622(0x50,0xCC);
	else
		LCDWRITE_1622(0x10,0xCC);
	switch (dotp)
    {
	case 5:
		LCDWRITE_1622(LED7Code[(*(data + 1) - '0')*2],0x04);
		LCDWRITE_1622(LED7Code[(*(data + 1) - '0')*2 + 1]|0x80,0xBC);
		
		LCDWRITE_1622(LED7Code[(*(data + 2) - '0')*2],0x14);
	  	LCDWRITE_1622(LED7Code[(*(data + 2) - '0')*2 + 1],0x0C);
		
		LCDWRITE_1622(LED7Code[(*(data + 3) - '0')*2],0x24);
		LCDWRITE_1622(LED7Code[(*(data + 3) - '0')*2 + 1],0x1C);
		
		LCDWRITE_1622(LED7Code[(*(data + 4) - '0')*2],0x34);
		LCDWRITE_1622(LED7Code[(*(data + 4) - '0')*2 + 1],0x2C);
		
		LCDWRITE_1622(LED7Code[(*(data + 5) - '0')*2],0x44);
		LCDWRITE_1622(LED7Code[(*(data + 5) - '0')*2 + 1],0x3C);	
        break;
    case 4:
		LCDWRITE_1622(LED7Code[(*(data + 1) - '0')*2],0x04);
		LCDWRITE_1622(LED7Code[(*(data + 1) - '0')*2 + 1],0xBC);
		
		LCDWRITE_1622(LED7Code[(*(data + 2) - '0')*2],0x14);
	  	LCDWRITE_1622(LED7Code[(*(data + 2) - '0')*2 + 1]|0x80,0x0C);
		
		LCDWRITE_1622(LED7Code[(*(data + 3) - '0')*2],0x24);
		LCDWRITE_1622(LED7Code[(*(data + 3) - '0')*2 + 1],0x1C);
		
		LCDWRITE_1622(LED7Code[(*(data + 4) - '0')*2],0x34);
		LCDWRITE_1622(LED7Code[(*(data + 4) - '0')*2 + 1],0x2C);
		
		LCDWRITE_1622(LED7Code[(*(data + 5) - '0')*2],0x44);
		LCDWRITE_1622(LED7Code[(*(data + 5) - '0')*2 + 1],0x3C);	
		break;
    case 3:
		LCDWRITE_1622(LED7Code[(*(data + 1) - '0')*2],0x04);
		LCDWRITE_1622(LED7Code[(*(data + 1) - '0')*2 + 1],0xBC);
		
		LCDWRITE_1622(LED7Code[(*(data + 2) - '0')*2],0x14);
	  	LCDWRITE_1622(LED7Code[(*(data + 2) - '0')*2 + 1],0x0C);
		
		LCDWRITE_1622(LED7Code[(*(data + 3) - '0')*2],0x24);
		LCDWRITE_1622(LED7Code[(*(data + 3) - '0')*2 + 1]|0x80,0x1C);
		
		LCDWRITE_1622(LED7Code[(*(data + 4) - '0')*2],0x34);
		LCDWRITE_1622(LED7Code[(*(data + 4) - '0')*2 + 1],0x2C);
		
		LCDWRITE_1622(LED7Code[(*(data + 5) - '0')*2],0x44);
		LCDWRITE_1622(LED7Code[(*(data + 5) - '0')*2 + 1],0x3C);	
		break;
    case 2:
		LCDWRITE_1622(LED7Code[(*(data + 1) - '0')*2],0x04);
		LCDWRITE_1622(LED7Code[(*(data + 1) - '0')*2 + 1],0xBC);
		
		LCDWRITE_1622(LED7Code[(*(data + 2) - '0')*2],0x14);
	  	LCDWRITE_1622(LED7Code[(*(data + 2) - '0')*2 + 1],0x0C);
		
		LCDWRITE_1622(LED7Code[(*(data + 3) - '0')*2],0x24);
		LCDWRITE_1622(LED7Code[(*(data + 3) - '0')*2 + 1],0x1C);
		
		LCDWRITE_1622(LED7Code[(*(data + 4) - '0')*2],0x34);
		LCDWRITE_1622(LED7Code[(*(data + 4) - '0')*2 + 1]|0x80,0x2C);
		
		LCDWRITE_1622(LED7Code[(*(data + 5) - '0')*2],0x44);
		LCDWRITE_1622(LED7Code[(*(data + 5) - '0')*2 + 1],0x3C);	
		break;
    case 1:
		LCDWRITE_1622(LED7Code[(*(data + 1) - '0')*2],0x04);
		LCDWRITE_1622(LED7Code[(*(data + 1) - '0')*2 + 1],0xBC);
		
		LCDWRITE_1622(LED7Code[(*(data + 2) - '0')*2],0x14);
	  	LCDWRITE_1622(LED7Code[(*(data + 2) - '0')*2 + 1],0x0C);
		
		LCDWRITE_1622(LED7Code[(*(data + 3) - '0')*2],0x24);
		LCDWRITE_1622(LED7Code[(*(data + 3) - '0')*2 + 1],0x1C);
		
		LCDWRITE_1622(LED7Code[(*(data + 4) - '0')*2],0x34);
		LCDWRITE_1622(LED7Code[(*(data + 4) - '0')*2 + 1],0x2C);
		
		LCDWRITE_1622(LED7Code[(*(data + 5) - '0')*2],0x44);
		LCDWRITE_1622(LED7Code[(*(data + 5) - '0')*2 + 1]|0x80,0x3C);	
		break;
    default:
		LCDWRITE_1622(LED7Code[(*(data + 1) - '0')*2],0x04);
		LCDWRITE_1622(LED7Code[(*(data + 1) - '0')*2 + 1],0xBC);
		
		LCDWRITE_1622(LED7Code[(*(data + 2) - '0')*2],0x14);
	  	LCDWRITE_1622(LED7Code[(*(data + 2) - '0')*2 + 1],0x0C);
		
		LCDWRITE_1622(LED7Code[(*(data + 3) - '0')*2],0x24);
		LCDWRITE_1622(LED7Code[(*(data + 3) - '0')*2 + 1],0x1C);
		
		LCDWRITE_1622(LED7Code[(*(data + 4) - '0')*2],0x34);
		LCDWRITE_1622(LED7Code[(*(data + 4) - '0')*2 + 1],0x2C);
		
		LCDWRITE_1622(LED7Code[(*(data + 5) - '0')*2],0x44);
		LCDWRITE_1622(LED7Code[(*(data + 5) - '0')*2 + 1],0x3C);	
    } 
}

void display_digital(BYTE * data,BYTE dotp,BYTE shine)
{
    if ((shine >0)&&(shine <6))
    {
        digital(data,dotp);
        Delay50ms();      
	   	switch (shine)
	    {
		case 5:
			LCDWRITE_1622(0,0x04);
			LCDWRITE_1622(0,0xBC);
	        break;
	    case 4:
			LCDWRITE_1622(0,0x14);
		  	LCDWRITE_1622(0,0x0C);
			break;
	    case 3:
			LCDWRITE_1622(0,0x24);
			LCDWRITE_1622(0,0x1C);
			break;
	    case 2:
			LCDWRITE_1622(0,0x34);
			LCDWRITE_1622(0,0x2C);
			break;
	    case 1:
			LCDWRITE_1622(0,0x44);
			LCDWRITE_1622(0,0x3C);	
			break;
	    default:
			break;
	    } 
        Delay50ms();  
		digital(data,dotp);
    }
    else   
		digital(data,dotp);
}

void clearLCD(void)
{	 
    unsigned short i;

	SENDCOMB(0);//发送数据模式指令101，发送地址
    for(i = 0; i < 256; i++)
		SendBit_HT1622(0, 8);
    setCS_1622;//CS_1622=1
}

static void SENDCOMA(BYTE command)
{
    Reset_1622();
    SendBit_HT1622(0x80,4);         /*ID=100 0*/
    SendBit_HT1622(command,8);  /* send command*/
}

void SENDCOMC(BYTE command)
{
    Reset_1622();
    SendBit_HT1622(0x90,4);         /*ID=100 1*/
    SendBit_HT1622(command,8);  /* send command*/
}

void initLCD_1622(void)
{
    Reset_1622();
//    Delay(10);
    SENDCOMA(LCD_1622_BIAS);    //背光灯
    SENDCOMA(LCD_1622_SYSEN);   //开系统震荡
    SENDCOMA(LCD_1622_LCDON);   //开LCD偏置
    SENDCOMA(LCD_1622_TIMERDIS); //禁止时基输出

}

static void SendBit_HT1622(BYTE op_data,BYTE num)
{
    BYTE temp1;
    for (temp1=0;temp1<num;temp1++)
    {
        if ((op_data&0X80)== 00 )
            clrDI_1622 ;
        else
        {
            setDI_1622;
        }   
        setSK_1622;
        setSK_1622;
        setSK_1622;
        setSK_1622;//SK_1622=1; SK_1622=1; SK_1622=1; SK_1622=1;
        Delay(100);
        clrSK_1622;       //SK_1622=0;
        op_data=op_data<<1;
    }
}

static void Delay(int a)
{
    int m;
    for (m=0;m<a;m++)
    {
        ;
    }
}

static void Reset_1622(void)
{ 
    setCS_1622;//CS_1622=1;        
    setSK_1622;//SK_1622=1;       
    setDI_1622;//DI_1622=1; 
    Delay(200);    
    clrCS_1622;//CS_1622=0;    
    clrCS_1622;//CS_1622=0;          
    clrSK_1622;//SK_1622=0; 
    clrSK_1622;//SK_1622=0;    
}

void floattochar(float fdata,BYTE disbuf[6],BYTE dotp)
{
    long int temp_int;
    float temp_float;
    if (fdata <0)
    {
        temp_float = fdata *(-1.0);
        disbuf[0]='-';             
    }
    else
    {
        disbuf[0]='+';
        temp_float = fdata ;
    }           
    temp_float = temp_float +0.00001;
    if (dotp == 4)
        temp_float = temp_float*10000.0;
    if (dotp == 3)
        temp_float = temp_float*1000.0;
    if (dotp == 2)
        temp_float = temp_float*100.0;
    if (dotp == 1)
        temp_float = temp_float*10.0;
    if (temp_float>99999)
        temp_int = 99999;
    else
        temp_int = temp_float;

    disbuf[1]= temp_int/10000 + '0';
    disbuf[2]= temp_int/1000%10 + '0';
    disbuf[3]= temp_int%1000/100 + '0';
    disbuf[4]= temp_int%100/10 + '0';
    disbuf[5]= temp_int%10 + '0';
}

static void Delay50ms(void)
{
    int m;
    for (m=0;m<100;m++)
    {
        ;
    }
}

