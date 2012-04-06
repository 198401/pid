/********************************************************
 Author        : Yu

 Date          : 

 File          : AD5422

 Hardware      : ADuC702X

 Description   : 
*************************************************************/
#include <aduc7024.h>
#include <inttypes.h>

#define SET_CLEAR()     GP1DAT = (GP1DAT | 0x00020000)  /* P1.1->CLEAR    */
#define CLR_CLEAR()		GP1DAT = (GP1DAT & 0xFFFDFFFF)

#define SET_LATCH()     GP1DAT = (GP1DAT | 0x00800000)  /* P1.7->LATCH    */
#define CLR_LATCH()		GP1DAT = (GP1DAT & 0xFF7FFFFF)

#define SET_SCL()       GP1DAT = (GP1DAT | 0x00100000)  /* P1.4->SCLK      */
#define	CLR_SCL()		GP1DAT = (GP1DAT & 0xffefffff)

#define SET_SDO()       GP4DAT = (GP4DAT | 0x00400000)  /* P1.6->SDIN      */
#define CLR_SDO()		GP4DAT = (GP4DAT & 0xffbfffff)

static void delay (int length)
{
    while (length >0)
        length--;
}

void WriteToAD5422(unsigned char count,unsigned char *buf)
{ 
    unsigned    char    ValueToWrite = 0;
    unsigned    char    i = 0;
    unsigned    char    j = 0;


    CLR_LATCH();

    for ( i=count;i>0;i-- )
    {
        ValueToWrite =  *(buf+i-1);
        for (j=0; j<8; j++)
        {
            CLR_SCL();
            if (0x80 == (ValueToWrite & 0x80))
            {
                SET_SDO();      /* Send one to SDIN pin of AD5422*/
            }
            else
            {
                CLR_SDO();      /* Send zero to SDIN pin of AD5422*/
            }

            delay(1);
            SET_SCL();
            delay(1);
            ValueToWrite <<= 1; /* Rotate data*/   
        }  
    }
    CLR_SCL();
    delay(1);
    SET_LATCH();
    delay(20);
}

/*
void ReadFromAD5422(unsigned char count,unsigned char *buf)
{
    unsigned    char    i = 0;
    unsigned    char    j = 0;
    unsigned    int     iTemp = 0;
    unsigned    char    RotateData = 0;

    CLR_LATCH();

    for (j=count; j>0; j--)
    {
        for (i=0; i<8; i++)
        {
            CLR_SCL();
            RotateData <<= 1;       
            delay(1);
            CLR_SDO();             
            iTemp = GP1DAT;         
            SET_SCL();  
            if (0x00000020 == (iTemp & 0x00000020))
            {
                RotateData |= 1;    
            }
            delay(1);

        }
        *(buf+j-1)= RotateData;
    }
    CLR_SCL();
    delay(1);    
    SET_LATCH();
    delay(20);
} 
*/
