/********************************************************
 Author        : Yu

 Date          : 

 File          : spi

 Hardware      : ADuC702X

 Description   : spi
*************************************************************/
#include <aduc7024.h>
#include <inttypes.h>

unsigned char SPI_MasterTransmit(unsigned char cData)
{
    SPITX = cData;                                  
    while ((SPISTA & 0x02) != 0x02) ;               /* wait for data received status bit*/
    return SPITX;                                   
}

void SPI_MasterInit(void)   
{
    GP1CON = 0x22220000;                /* configure SPI on SPM*/
    SPIDIV = 0x05;                      /* set SPI clock 40960000/(2x(1+SPIDIV))*/
    SPICON = 0x1043;                    /* enable SPI master in continuous transfer mode*/
}

void SPI_Disable(void)  
{
    SPICON &= ~(1<<0);      
}

void SPI_Enable(void)       
{
    SPICON |= (1<<0);           
}
