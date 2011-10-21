#ifndef __SPI_H__
#define __SPI_H__

extern unsigned char SPI_MasterTransmit(unsigned char cData);   
extern void SPI_MasterInit(void);                   
extern void SPI_Disable(void);                          
extern void SPI_Enable(void);                           
#endif
