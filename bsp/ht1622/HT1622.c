/********************************************************
 Author        : Yu

 Date          : 

 File          : ht1622

 Hardware      : ADuC702X

 Description   : LCD_HT1622
*************************************************************/
#include <aduc7024.h>
#include <inttypes.h>

/* di ---p1.3*/
#define setDI_1622   	GP1DAT |= 0x08080000
#define clrDI_1622   	GP1DAT &= ~0x00080000 
/* wr-sk--p1.2*/
#define setSK_1622  	GP1DAT |= 0x04040000  
#define clrSK_1622   	GP1DAT &= ~0x00040000 
/* cs---p4.2*/
#define setCS_1622  	GP4DAT |= 0x04040000  
#define clrCS_1622   	GP4DAT &= ~0x00040000 

#define LCD_1622_BIAS		0X52
#define LCD_1622_SYSEN 		0X02 
#define LCD_1622_LCDON 		0X06
#define LCD_1622_SYSDIS 	0X00 
#define LCD_1622_LCDOFF		0X04 
#define LCD_1622_CLRWDT 	0X1c 
#define LCD_1622_TIMERDIS	0X08 

const uint8_t LED7Code[]={
    0x0F0,      // 0 LSB
    0x050,      // 0 MSB
    0x060,      // 1 LSB
    0x000,      // 1 MSB
    0x0B0,      // 2 LSB
    0x060,      // 2 MSB
    0x0F0,      // 3 LSB
    0x020,      // 3 MSB
    0x060,      // 4 LSB
    0x030,      // 4 MSB
    0x0D0,      // 5 LSB
    0x030,      // 5 MSB
    0x0D0,      // 6 LSB
    0x070,      // 6 MSB
    0x070,      // 7 LSB
    0x000,      // 7 MSB
    0x0F0,      // 8 LSB
    0x070,      // 8 MSB
    0x0F0,      // 9 LSB
    0x030,      // 9 MSB
    0x000,
    0x000
};

const uint8_t LED16Char[]={     
    0x010,      // A LSB
    0x010,      // A ...
    0x0D0,      // A ...
    0x0B0,      // A MSB
    0x080,      // B LSB
    0x080,      // B ...
    0x0C0,      // B ...
    0x0B0,      // B MSB
    0x090,      // C LSB
    0x090,      // C ...
    0x000,      // C ...
    0x090,      // C MSB
    0x0D0,      // D LSB
    0x0B0,      // D ...
    0x090,      // D ...
    0x000,      // D MSB
    0x090,      // E LSB
    0x090,      // E ...
    0x040,      // E ...
    0x0B0,      // E MSB
    0x010,      // F LSB
    0x010,      // F ...
    0x040,      // F ...
    0x0B0,      // F MSB
    0x080,      // G LSB
    0x080,      // G ...
    0x0C0,      // G ...
    0x090,      // G MSB
    0x000,      // H LSB
    0x000,      // H ...
    0x0D0,      // H ...
    0x0B0,      // H MSB
    0x0D0,      // I LSB
    0x0B0,      // I ...
    0x000,      // I ...
    0x000,      // I MSB
    0x080,      // J LSB
    0x080,      // J ...
    0x090,      // J ...
    0x000,      // J MSB
    0x000,      // K LSB
    0x040,      // K ...
    0x020,      // K ...
    0x0B0,      // K MSB
    0x080,      // L LSB
    0x080,      // L ...
    0x000,      // L ...
    0x090,      // L MSB
    0x020,      // M LSB
    0x000,      // M ...
    0x0B0,      // M ...
    0x090,      // M MSB
    0x020,      // N LSB
    0x040,      // N ...
    0x090,      // N ...
    0x090,      // N MSB
    0x090,      // O LSB
    0x090,      // O ...
    0x090,      // O ...
    0x090,      // O MSB
    0x010,      // P LSB
    0x010,      // P ...
    0x050,      // P ...
    0x0B0,      // P MSB
    0x090,      // Q LSB
    0x0D0,      // Q ...
    0x090,      // Q ...
    0x090,      // Q MSB
    0x010,      // R LSB
    0x050,      // R ...
    0x050,      // R ...
    0x0B0,      // R MSB
    0x090,      // S LSB
    0x090,      // S ...
    0x0C0,      // S ...
    0x030,      // S MSB
    0x050,      // T LSB
    0x030,      // T ...
    0x000,      // T ...
    0x000,      // T MSB
    0x080,      // U LSB
    0x080,      // U ...
    0x090,      // U ...
    0x090,      // U MSB
    0x000,      // V LSB
    0x000,      // V ...
    0x020,      // V ...
    0x0D0,      // V MSB
    0x000,      // W LSB
    0x040,      // W ...
    0x090,      // W ...
    0x0D0,      // W MSB
    0x020,      // X LSB
    0x040,      // X ...
    0x020,      // X ...
    0x040,      // X MSB
    0x080,      // Y LSB
    0x080,      // Y ...
    0x0D0,      // Y ...
    0x030,      // Y MSB
    0x090,      // Z LSB
    0x090,      // Z ...
    0x020,      // Z ...
    0x040,      // Z MSB
    0x000,
    0x000,
    0x000,
    0x000
};

const uint8_t LED16Num[]={      
    0x090,      // 0 LSB
    0x090,      // 0 ...
    0x090,      // 0 ...
    0x090,      // 0 MSB
    0x000,      // 1 LSB
    0x000,      // 1 ...
    0x090,      // 1 ...
    0x000,      // 1 MSB
    0x090,      // 2 LSB
    0x090,      // 2 ...
    0x050,      // 2 ...
    0x0A0,      // 2 MSB
    0x090,      // 3 LSB
    0x090,      // 3 ...
    0x0D0,      // 3 ...
    0x020,      // 3 MSB
    0x000,      // 4 LSB
    0x000,      // 4 ...
    0x0D0,      // 4 ...
    0x030,      // 4 MSB
    0x090,      // 5 LSB
    0x090,      // 5 ...
    0x0C0,      // 5 ...
    0x030,      // 5 MSB
    0x090,      // 6 LSB
    0x090,      // 6 ...
    0x0C0,      // 6 ...
    0x0B0,      // 6 MSB
    0x010,      // 7 LSB
    0x010,      // 7 ...
    0x090,      // 7 ...
    0x000,      // 7 MSB
    0x090,      // 8 LSB
    0x090,      // 8 ...
    0x0D0,      // 8 ...
    0x0B0,      // 8 MSB
    0x090,      // 9 LSB
    0x090,      // 9 ...
    0x0D0,      // 9 ...
    0x030,      // 9 MSB
};

static void SendBit_HT1622(uint8_t op_data,uint8_t num)
{
    for (uint8_t temp1=0;temp1<num;temp1++)
    {
        if ((op_data&0X80)== 0 )
            clrDI_1622 ;
        else
        {
            setDI_1622;
        }   
        setSK_1622;
        setSK_1622;
        setSK_1622;
        setSK_1622;
        clrSK_1622;     
        op_data=op_data<<1;
    }
}

static void Reset_1622(void)
{ 
    setCS_1622;       
    setSK_1622;       
    setDI_1622;
    clrCS_1622;   
    clrCS_1622;          
    clrSK_1622;
    clrSK_1622;   
}

static void SENDCOMB(uint8_t address)
{
    Reset_1622();
    SendBit_HT1622(0xA0,3);   
    address=address;
    SendBit_HT1622(address,6);
}

static uint8_t LCDWRITE_1622(const uint8_t data,uint8_t Addr)
{
    SENDCOMB(Addr);
    SendBit_HT1622(data, 4);
    setCS_1622;
    return(1);
}

void display_char(const uint8_t * temp)   
{
    if (*(temp + 0) >= 'A' && *(temp + 0) <= '[')
    {
        LCDWRITE_1622(LED16Char[(*(temp + 0) - 'A')*4],0xC0);
        LCDWRITE_1622(LED16Char[(*(temp + 0) - 'A')*4 + 1],0xB8);
        LCDWRITE_1622(LED16Char[(*(temp + 0) - 'A')*4 + 2],0xB0);
        LCDWRITE_1622(LED16Char[(*(temp + 0) - 'A')*4 + 3],0xC8);
    }
    else
    {
        LCDWRITE_1622(LED16Num[(*(temp + 0) - '0')*4],0xC0);
        LCDWRITE_1622(LED16Num[(*(temp + 0) - '0')*4 + 1],0xB8);
        LCDWRITE_1622(LED16Num[(*(temp + 0) - '0')*4 + 2],0xB0);
        LCDWRITE_1622(LED16Num[(*(temp + 0) - '0')*4 + 3],0xC8);
    }

    if (*(temp + 1) >= 'A' && *(temp + 1) <= '[')
    {
        LCDWRITE_1622(LED16Char[(*(temp + 1) - 'A')*4],0x08);
        LCDWRITE_1622(LED16Char[(*(temp + 1) - 'A')*4 + 1],0x10);
        LCDWRITE_1622(LED16Char[(*(temp + 1) - 'A')*4 + 2],0xA8);
        LCDWRITE_1622(LED16Char[(*(temp + 1) - 'A')*4 + 3],0x00);
    }
    else
    {
        LCDWRITE_1622(LED16Num[(*(temp + 1) - '0')*4],0x08);
        LCDWRITE_1622(LED16Num[(*(temp + 1) - '0')*4 + 1],0x10);
        LCDWRITE_1622(LED16Num[(*(temp + 1) - '0')*4 + 2],0xA8);
        LCDWRITE_1622(LED16Num[(*(temp + 1) - '0')*4 + 3],0x00);
    } 

    if (*(temp + 2) >= 'A' && *(temp + 2) <= '[')
    {
        LCDWRITE_1622(LED16Char[(*(temp + 2) - 'A')*4],0x20);
        LCDWRITE_1622(LED16Char[(*(temp + 2) - 'A')*4 + 1],0x28);
        LCDWRITE_1622(LED16Char[(*(temp + 2) - 'A')*4 + 2],0xA0);
        LCDWRITE_1622(LED16Char[(*(temp + 2) - 'A')*4 + 3],0x18); 
    }
    else
    {
        LCDWRITE_1622(LED16Num[(*(temp + 2) - '0')*4],0x20);
        LCDWRITE_1622(LED16Num[(*(temp + 2) - '0')*4 + 1],0x28);
        LCDWRITE_1622(LED16Num[(*(temp + 2) - '0')*4 + 2],0xA0);
        LCDWRITE_1622(LED16Num[(*(temp + 2) - '0')*4 + 3],0x18); 
    }

    if (*(temp + 3) >= 'A' && *(temp + 3) <= '[')
    {
        LCDWRITE_1622(LED16Char[(*(temp + 3) - 'A')*4],0x38);
        LCDWRITE_1622(LED16Char[(*(temp + 3) - 'A')*4 + 1],0x40);
        LCDWRITE_1622(LED16Char[(*(temp + 3) - 'A')*4 + 2],0x98);
        LCDWRITE_1622(LED16Char[(*(temp + 3) - 'A')*4 + 3],0x30);
    }
    else
    {
        LCDWRITE_1622(LED16Num[(*(temp + 3) - '0')*4],0x38);
        LCDWRITE_1622(LED16Num[(*(temp + 3) - '0')*4 + 1],0x40);
        LCDWRITE_1622(LED16Num[(*(temp + 3) - '0')*4 + 2],0x98);
        LCDWRITE_1622(LED16Num[(*(temp + 3) - '0')*4 + 3],0x30);
    }

    if (*(temp + 4) >= 'A' && *(temp + 4) <= '[')
    {
        LCDWRITE_1622(LED16Char[(*(temp + 4) - 'A')*4],0x88);
        LCDWRITE_1622(LED16Char[(*(temp + 4) - 'A')*4 + 1],0x80);
        LCDWRITE_1622(LED16Char[(*(temp + 4) - 'A')*4 + 2],0x78);
        LCDWRITE_1622(LED16Char[(*(temp + 4) - 'A')*4 + 3],0x90);
    }
    else
    {
        LCDWRITE_1622(LED16Num[(*(temp + 4) - '0')*4],0x88);
        LCDWRITE_1622(LED16Num[(*(temp + 4) - '0')*4 + 1],0x80);
        LCDWRITE_1622(LED16Num[(*(temp + 4) - '0')*4 + 2],0x78);
        LCDWRITE_1622(LED16Num[(*(temp + 4) - '0')*4 + 3],0x90);
    } 

    if (*(temp + 5) >= 'A' && *(temp + 5) <= '[')
    {
        LCDWRITE_1622(LED16Char[(*(temp + 5) - 'A')*4],0x68);
        LCDWRITE_1622(LED16Char[(*(temp + 5) - 'A')*4 + 1],0x60);
        LCDWRITE_1622(LED16Char[(*(temp + 5) - 'A')*4 + 2],0x58);
        LCDWRITE_1622(LED16Char[(*(temp + 5) - 'A')*4 + 3],0x70);
    }
    else
    {
        LCDWRITE_1622(LED16Num[(*(temp + 5) - '0')*4],0x68);
        LCDWRITE_1622(LED16Num[(*(temp + 5) - '0')*4 + 1],0x60);
        LCDWRITE_1622(LED16Num[(*(temp + 5) - '0')*4 + 2],0x58);
        LCDWRITE_1622(LED16Num[(*(temp + 5) - '0')*4 + 3],0x70);
    } 

    if (*(temp + 6) >= 'A' && *(temp + 6) <= '[')
    {
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
    }
    else
    {
        LCDWRITE_1622(((LED16Num[(*(temp + 6) - '0')*4]) >> 3)|
                      ((LED16Num[(*(temp + 6) - '0')*4]) << 3)|
                      ((LED16Num[(*(temp + 6) - '0')*4]&0x40) >> 1)|
                      ((LED16Num[(*(temp + 6) - '0')*4]&0x20) << 1)
                      ,0x64);
        LCDWRITE_1622(((LED16Num[(*(temp + 6) - '0')*4 + 1]) >> 3)|
                      ((LED16Num[(*(temp + 6) - '0')*4 + 1]) << 3)|
                      ((LED16Num[(*(temp + 6) - '0')*4 + 1]&0x40) >> 1)|
                      ((LED16Num[(*(temp + 6) - '0')*4 + 1]&0x20) << 1)
                      ,0x6C);
        LCDWRITE_1622(((LED16Num[(*(temp + 6) - '0')*4 + 2]&0x80) >> 3)|
                      ((LED16Num[(*(temp + 6) - '0')*4 + 2]&0x10) << 3)|
                      ((LED16Num[(*(temp + 6) - '0')*4 + 2]&0x40) >> 1)|
                      ((LED16Num[(*(temp + 6) - '0')*4 + 2]&0x20) << 1)
                      ,0x74);
        LCDWRITE_1622(((LED16Num[(*(temp + 6) - '0')*4 + 3]) >> 3)|
                      ((LED16Num[(*(temp + 6) - '0')*4 + 3]) << 3)|
                      ((LED16Num[(*(temp + 6) - '0')*4 + 3]&0x40) >> 1)|
                      ((LED16Num[(*(temp + 6) - '0')*4 + 3]&0x20) << 1)
                      ,0x54);
    }

    if (*(temp + 7) >= 'A' && *(temp + 7) <= '[')
    {
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
    else
    {
        LCDWRITE_1622(((LED16Num[(*(temp + 7) - '0')*4]) >> 3)|
                      ((LED16Num[(*(temp + 7) - '0')*4]) << 3)|
                      ((LED16Num[(*(temp + 7) - '0')*4]&0x40) >> 1)|
                      ((LED16Num[(*(temp + 7) - '0')*4]&0x20) << 1)
                      ,0x8C);
        LCDWRITE_1622(((LED16Num[(*(temp + 7) - '0')*4 + 1]) >> 3)|
                      ((LED16Num[(*(temp + 7) - '0')*4 + 1]) << 3)|
                      ((LED16Num[(*(temp + 7) - '0')*4 + 1]&0x40) >> 1)|
                      ((LED16Num[(*(temp + 7) - '0')*4 + 1]&0x20) << 1)
                      ,0x94);
        LCDWRITE_1622(((LED16Num[(*(temp + 7) - '0')*4 + 2]) >> 3)|
                      ((LED16Num[(*(temp + 7) - '0')*4 + 2]) << 3)|
                      ((LED16Num[(*(temp + 7) - '0')*4 + 2]&0x40) >> 1)|
                      ((LED16Num[(*(temp + 7) - '0')*4 + 2]&0x20) << 1)
                      ,0x4C);
        LCDWRITE_1622(((LED16Num[(*(temp + 7) - '0')*4 + 3]) >> 3)|
                      ((LED16Num[(*(temp + 7) - '0')*4 + 3]) << 3)|
                      ((LED16Num[(*(temp + 7) - '0')*4 + 3]&0x40) >> 1)|
                      ((LED16Num[(*(temp + 7) - '0')*4 + 3]&0x20) << 1)
                      ,0x84);
    }
}

void digital(const uint8_t * data, uint8_t dotp)
{
    LCDWRITE_1622(0,0xC4);
    if (*data == '-')
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

static uint8_t byShine;

void display_digital(const uint8_t * data,uint8_t dotp,uint8_t shine)
{
    digital(data,dotp);
    byShine++;
    if (byShine > 2)
    {
        byShine = 0;
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
    }
}

void clearLCD(void)
{    
    SENDCOMB(0);
    for (uint8_t i = 0; i < 208; i++)
        SendBit_HT1622(0, 8);
    setCS_1622;
}

static void SENDCOMA(uint8_t command)
{
    Reset_1622();
    SendBit_HT1622(0x80,4);     
    SendBit_HT1622(command,8); 
}

void SENDCOMC(uint8_t command)
{
    Reset_1622();
    SendBit_HT1622(0x90,4);        
    SendBit_HT1622(command,8); 
}

void initLCD_1622(void)
{
    Reset_1622();
    SENDCOMA(LCD_1622_BIAS);   
    SENDCOMA(LCD_1622_SYSEN);   
    SENDCOMA(LCD_1622_LCDON);   
    SENDCOMA(LCD_1622_TIMERDIS); 

}

void floattochar(float fdata,uint8_t disbuf[6],uint8_t dotp)
{
    uint32_t temp_int;
    float temp_float;
    if (fdata <0)
    {
        temp_float = fdata *(-1.0f);
        disbuf[0]='-';             
    }
    else
    {
        disbuf[0]='+';
        temp_float = fdata ;
    }           
    temp_float = temp_float +0.00001f;
    if (dotp == 4)
        temp_float = temp_float*10000.0f;
    if (dotp == 3)
        temp_float = temp_float*1000.0f;
    if (dotp == 2)
        temp_float = temp_float*100.0f;
    if (dotp == 1)
        temp_float = temp_float*10.0f;

    temp_int = temp_float;

    if (temp_float - temp_int >= 0.5f )
        temp_int += 1;

    if (temp_float > 99999)
        temp_int = 99999;

    disbuf[1]= temp_int/10000 + '0';
    disbuf[2]= temp_int/1000%10 + '0';
    disbuf[3]= temp_int%1000/100 + '0';
    disbuf[4]= temp_int%100/10 + '0';       
    disbuf[5]= temp_int%10 + '0';
    if (dotp == 4)
    {

    }
    else if (dotp == 3)
    {
        if (disbuf[1] == '0')
        {
            disbuf[1] = ':';
        }
    }
    else if (dotp == 2)
    {
        if (disbuf[1] == '0')
        {
            disbuf[1] = ':';
            if (disbuf[2] == '0')
            {
                disbuf[2] = ':';
            }
        }
    }
    else if (dotp == 1)
    {
        if (disbuf[1] == '0')
        {
            disbuf[1] = ':';
            if (disbuf[2] == '0')
            {
                disbuf[2] = ':';
                if (disbuf[3] == '0')
                {
                    disbuf[3] = ':';
                }
            }
        }
    }
    else
    {
        if (disbuf[1] == '0')
        {
            disbuf[1] = ':';
            if (disbuf[2] == '0')
            {
                disbuf[2] = ':';
                if (disbuf[3] == '0')
                {
                    disbuf[3] = ':';
                    if (disbuf[4] == '0')
                    {
                        disbuf[4] = ':';
                    }
                }
            }
        }
    }
}
