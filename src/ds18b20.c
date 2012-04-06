/************************************************************************************************

 Author        : Yu

 Date          : 

 File          : ds18b20
                                      
 Hardware      : ADuC702x

 Description   : ds18b20
*************************************************************************************************/
#include <ADuC7024.H> 

typedef unsigned char           BYTE;      
typedef unsigned short          WORD;       

#define BIT03					0x00000020
#define BIT19					0x00200000
#define BIT27					0x20000000

#define   GPG_UP       	GP0PAR  &= ~BIT03      	
#define   GPG_DOWN  	GP0PAR  |= BIT03      	
#define   DS18B20_L    	GP0DAT  &= ~BIT19      	
#define   DS18B20_H    	GP0DAT  |= BIT19      
#define   DS18B20_OUT 	GP0DAT  |= BIT27  	
#define   DS18B20_IN   	GP0DAT  &= ~BIT27  	
#define   DS18B20_STU  	GP0DAT&BIT03         	

#define    DS18B20_ReadROM 		0x33    
#define    DS18B20_MatchROM		0x55    
#define    DS18B20_SkipROM   	0xCC    
#define    DS18B20_SearchROM    0xF0   
#define    DS18B20_AlarmROM    	0xEC   

#define    DS18B20_WriteSCR    	0x4E    
#define    DS18B20_ReadSCR    	0xBE   
#define    DS18B20_CopySCR     	0x48   
#define    DS18B20_ConvertTemp 	0x44   
#define    DS18B20_RecallEP    	0xB8   
#define    DS18B20_ReadPower    0xB4   

static BYTE CrcTable[256] =
{
	0,  94, 188,  226,  97,  63,  221,  131,  194,  156,  126,  32,  163,  253,  31,  65,
	157,  195,  33,  127,  252,  162,  64,  30,  95,  1,  227,  189,  62,  96,  130,  220,
	35,  125,  159,  193,  66,  28,  254,  160,  225,  191,  93,  3,  128,  222,  60,  98,
	190,  224,  2,  92,  223,  129,  99,  61,  124,  34,  192,  158,  29,  67,  161,  255,
	70,  24,  250,  164,  39,  121,  155,  197,  132,  218,  56,  102,  229,  187,  89,  7,
	219,  133, 103,  57,  186,  228,  6,  88,  25,  71,  165,  251,  120,  38,  196,  154,
	101,  59, 217,  135,  4,  90,  184,  230,  167,  249,  27,  69,  198,  152,  122,  36,
	248,  166, 68,  26,  153,  199,  37,  123,  58,  100,  134,  216,  91,  5,  231,  185,
	140,  210, 48,  110,  237,  179,  81,  15,  78,  16,  242,  172,  47,  113,  147,  205,
	17,  79,  173,  243,  112,  46,  204,  146,  211,  141,  111,  49,  178,  236,  14,  80,
	175,  241, 19,  77,  206,  144,  114,  44,  109,  51,  209,  143,  12,  82,  176,  238,
	50,  108,  142,  208,  83,  13,  239,  177,  240,  174,  76,  18,  145,  207,  45,  115,
	202,  148, 118,  40,  171,  245,  23,  73,  8,  86,  180,  234,  105,  55,  213, 139,
	87,  9,  235,  181,  54,  104,  138,  212,  149,  203,  41,  119,  244,  170,  72,  22,
	233,  183,  85,  11,  136,  214,  52,  106,  43,  117,  151,  201,  74,  20,  246,  168,
	116,  42,  200,  150,  21,  75,  169,  247,  182,  232,  10,  84,  215,  137,  107,  53
};

static BYTE CRC(BYTE * a,BYTE j)
{
	BYTE i,crc_data=0;
	for(i=0;i < j;i++)
	    crc_data = CrcTable[crc_data^a[i]];
	return (crc_data);
}


static volatile void delay_5us(WORD count)      
{
    WORD i;  
    while (count)
    {
        i =21;
        while (i>0) i--;
        count--;
    }
}

static BYTE Init_DS18B20(void)
{
    BYTE errTime=0;

    DS18B20_OUT;
    DS18B20_H;   
    DS18B20_L;        
    delay_5us(90);      
    DS18B20_H;
    DS18B20_IN;
    delay_5us(16);

    while (DS18B20_STU)
    {
        delay_5us(1);    
        errTime++;
        if (errTime>20)
            return 0;
    }
    errTime=0;
    while (!(DS18B20_STU))
    {
        delay_5us(1);   
        errTime++;
        if (errTime>50)
            return 0;
    }
    return 0xff;
}

static BYTE DS18B20_ReadByte(void)
{
    BYTE i;
    BYTE temp = 0;

    DS18B20_IN;
    for (i=0;i<8;i++)
    {
        temp >>= 1;      
        DS18B20_OUT;
        DS18B20_L;
        delay_5us(1);
        DS18B20_H;
        DS18B20_IN;       
        delay_5us(2);
        if (DS18B20_STU)
            temp |= 0x80;
        delay_5us(10);
        DS18B20_OUT;
        DS18B20_H;
    }
    delay_5us(1);
    return(temp);
}

static void DS18B20_WriteByte(BYTE Data)
{
    BYTE i;
    BYTE temp;
    DS18B20_IN;
    for (i=0;i<8;i++)
    {
        DS18B20_OUT;
        DS18B20_L;
        delay_5us(1);
        temp = Data >> i;
        temp &= 0x01;               
        if (temp)
            DS18B20_H;
        else
            DS18B20_L;
        delay_5us(11);
        DS18B20_IN;
        delay_5us(1);
    }
}


float DS18B20_Temperature(void)
{
    BYTE flag = 0,i = 0;
    BYTE a[10];
    float temp = 0.0f;

    flag = Init_DS18B20();
    if (flag == 0x00)
        return -9999;
    DS18B20_WriteByte(DS18B20_SkipROM);
    DS18B20_WriteByte(DS18B20_ConvertTemp);

    flag = Init_DS18B20();
    if (flag == 0x00)
        return -9999;

    DS18B20_WriteByte(DS18B20_SkipROM);
    DS18B20_WriteByte(DS18B20_ReadSCR);
    
    for(i = 0;i < 9;i++)
        a[i]  = DS18B20_ReadByte();
 
    if(CRC(a,9) == 0)
	{
		if(a[1]&0xf0)
	        temp= 0-(0xffff-(unsigned short)((a[1]<<8)+a[0])+1)*0.0625;
	    else
	        temp=((unsigned short)(a[1]<<8)+a[0])*0.0625;
 	}
	else
		temp = 9999;
    return temp;
}



