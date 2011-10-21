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

static WORD QryCrc8(const BYTE *pBuffer, BYTE Length)  
{
    BYTE i,j,bit0,cbit,cbyte,CRCValue;

    CRCValue=0;
    for (i=0;i<Length;i++)
    {
        cbyte=pBuffer[i];
        for (j=0;j<8;j++)
        {
            cbit=CRCValue&0x01;
            bit0=cbyte&0x01;
            CRCValue>>=1;
            if ((cbit^bit0)==1)
                CRCValue^=0x8c;
            cbyte>>=1;
        }
    }

    return CRCValue;
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
    for (i = 0;i < 10;i++)
        a[i]  = DS18B20_ReadByte();
    if (QryCrc8(a,8)==a[9])
    {
        if (a[1]&0xf0)
        {
            temp= 0-(0xffff-(unsigned short)((a[1]<<8)+a[0])+1)*0.0625;
        }
        else
            temp=((unsigned short)(a[1]<<8)+a[0])*0.0625;
    }
    else
    {
        temp = 9999;
    }
    temp = (a[1]<<8) | a[0];
    temp = temp * 0.0625f;
    return temp;
}



