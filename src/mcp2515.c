/************************************************************************************************

 Author        : Yu

 Date          : 

 File          : main
                                      
 Hardware      : ADuC702x

 Description   : mcp2515
*************************************************************************************************/
#include "mcp2515.h"

void spi_init(void)
{
	GP1CON |= 0x02220000;			
	setCS;
	SPIDIV  = 0x05;			
	SPICON  = 0x1043;		
}

void mcp2515_init(void)
{
	/* SPI Interface init */
	spi_init();
	
	/* MCP2515 per Software Reset */
	clrCS;
	spi_putc( SPI_RESET );
	setCS;
	
	/* CLKOUT Pin */
	mcp2515_bit_modify( CANCTRL, 0xE0, (1<<REQOP2) );
	
	/* 
	 *  Bit Timings
	 *	
     *	Fosc 	   = 16MHz
	 *	BRP        = 7					 
	 *	TQ = 2 * (BRP + 1) / Fosc		 
	 *	
	 *	Sync Seg   = 1TQ
	 *	Prop Seg   = ( PRSEG + 1 ) * TQ  = 1 TQ
	 *	Phase Seg1 = ( PHSEG1 + 1 ) * TQ = 3 TQ
	 *	Phase Seg2 = ( PHSEG2 + 1 ) * TQ = 3 TQ
	 *	
	 *	Bus speed  = 1 / (Total # of TQ) * TQ
	 *			   = 1 / 8 * TQ = 125 kHz
	 */
	
	/* BRP = 7 */
	mcp2515_write_register( CNF1, (1<<BRP0)|(1<<BRP1)|(1<<BRP2) );
	
	/* Prop Seg und Phase */
	mcp2515_write_register( CNF2, (1<<BTLMODE)|(1<<PHSEG11) );
	
	/* Wake-up Filter , Phase Seg2 */
	mcp2515_write_register( CNF3, (1<<PHSEG21) );
	
	/* Rx Buffer Interrupts */
	mcp2515_write_register( CANINTE, /*(1<<RX1IE)|(1<<RX0IE)*/ 0 );
	
	/* Buffer 0  */
	mcp2515_write_register( RXB0CTRL, (1<<RXM1)|(1<<RXM0) );
	
	/* Buffer 1  */
	mcp2515_write_register( RXB1CTRL, (1<<RXM1)|(1<<RXM0) );
	
	uint8_t temp[4] = { 0, 0, 0, 0 };
	
	/* Filter Buffer 0 */
	mcp2515_write_register_p( RXF0SIDH, temp, 4 );
	mcp2515_write_register_p( RXF1SIDH, temp, 4 );
	
	/* Filter Buffer 1 */
	mcp2515_write_register_p( RXF2SIDH, temp, 4 );
	mcp2515_write_register_p( RXF3SIDH, temp, 4 );
	mcp2515_write_register_p( RXF4SIDH, temp, 4 );
	mcp2515_write_register_p( RXF5SIDH, temp, 4 );
	
	mcp2515_write_register_p( RXM0SIDH, temp, 4 );
	
	mcp2515_write_register_p( RXM1SIDH, temp, 4 );
	
	mcp2515_write_register( BFPCTRL, 0 );
	
	mcp2515_write_register( TXRTSCTRL, 0 );
	
	mcp2515_bit_modify( CANCTRL, 0xE0, 0);
}

uint8_t spi_putc( uint8_t data )
{
	SPITX = data;									
	while ((SPISTA & 0x02) != 0x02) 
		; 		
	return SPITX;
}

void mcp2515_write_register( uint8_t adress, uint8_t data )
{
	/* CS low */
	clrCS;
	
	spi_putc(SPI_WRITE);
	
	spi_putc(adress);
	
	spi_putc(data);
	
	/* CS high */
	setCS;
}

uint8_t mcp2515_read_register(uint8_t adress)
{
	uint8_t data;
	
	/* CS low */
	clrCS;
	
	spi_putc(SPI_READ);
	
	spi_putc(adress);
	
	data = spi_putc(0xff);	
	
	/* CS high */
	setCS;
	
	return data;
}

uint8_t mcp2515_read_rx_buffer(uint8_t adress)
{
	uint8_t data;
	
	if (adress & 0xF9)
		return 0;
	
	/* CS low */
	clrCS;
	
	spi_putc(SPI_READ_RX | adress);
	
	data = spi_putc(0xff);	
	
	/* CS high */
	setCS;
	
	return data;
}

void mcp2515_bit_modify(uint8_t adress, uint8_t mask, uint8_t data)
{
	/* CS low */
	clrCS;
	
	spi_putc(SPI_BIT_MODIFY);
	
	spi_putc(adress);
	
	spi_putc(mask);
	
	spi_putc(data);
	
	/* CS high */
	setCS;
}

void mcp2515_write_register_p( uint8_t adress, uint8_t *data, uint8_t length )
{
	uint8_t i;
	
	/* CS low */
	clrCS;
	
	spi_putc(SPI_WRITE);
	
	spi_putc(adress);
	
	for (i=0; i<length ;i++ )
		spi_putc(*data++);
	
	/* CS high */
	setCS;
}

void mcp2515_read_register_p( uint8_t adress, uint8_t *data, uint8_t length )
{
	uint8_t i;
	
	/* CS low */
	clrCS;
	
	spi_putc(SPI_READ);
	
	spi_putc(adress);
	
	for (i=0; i<length ;i++ )
		*data++ = spi_putc(0xff);
	
	/* CS high */
	setCS;
}

