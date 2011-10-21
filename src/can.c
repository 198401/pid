/************************************************************************************************

 Author        : Yu

 Date          : 

 File          : 
                                      
 Hardware      : ADuC702x

 Description   : 
*************************************************************************************************/
#include "mcp2515.h"

#define	CAN_RTR		0x80
#define	CAN_EID		0x40

void can_send_message( uint32_t id, uint8_t *data, uint8_t length, uint8_t flags)
{
    uint8_t temp[4];

    if (length > 8)
        length = 0;

    mcp2515_bit_modify( TXB0CTRL, (1<<TXP1)|(1<<TXP0), (1<<TXP1)|(1<<TXP0) );

    if (flags & CAN_EID)
    {
        temp[0] = (uint8_t) (id>>21);
        temp[1] = (((uint8_t) (id>>16)) & 0x03) | (((uint8_t) (id>>13)) & 0xE0) | (1<<EXIDE);
        temp[2] = (uint8_t) (id>>8);
        temp[3] = (uint8_t) id;
    }
    else
    {
        temp[0] = (uint8_t) (id>>3);
        temp[1] = (uint8_t) (id<<5);
        temp[2] = 0;
        temp[3] = 0;
    }
    mcp2515_write_register_p( TXB0SIDH, temp, 4 );


    if (flags & CAN_RTR)
        mcp2515_write_register( TXB0DLC, length | (1<<RTR) );
    else
        mcp2515_write_register( TXB0DLC, length );

    mcp2515_write_register_p( TXB0D0, data, length );

    clrCS;
    spi_putc(SPI_RTS | 0x01);
    setCS;
}

void can_read_message( uint32_t *ID, uint8_t *data, uint8_t *length, uint8_t *flags)
{
    uint8_t Intf_temp;
    uint8_t ID_temp[4] = {0,0,0,0};

    Intf_temp = mcp2515_read_register( CANINTF);        
    Intf_temp = (Intf_temp & ((1<<RX1IF) |(1<<RX0IF)));  

    if (Intf_temp == (1<<RX0IF))
    {
        *flags = 0xA0 ;                     
        //RXB_CTRL_temp = mcp2515_read_register( RXB0CTRL );		

        mcp2515_read_register_p( RXB0SIDH , ID_temp , 4);   

        *length = mcp2515_read_register( RXB0DLC );       

        mcp2515_read_register_p( RXB0D0 , data , *length); 

        mcp2515_bit_modify( CANINTF, (1<<RX0IF), 0 );           
    }
    else if (Intf_temp == (1<<RX1IF))
    {
        *flags = 0xA1 ;                 

        mcp2515_read_register_p( RXB1SIDH , ID_temp , 4);  

        *length = mcp2515_read_register( RXB1DLC );       

        mcp2515_read_register_p( RXB1D0 , data , *length);   

        mcp2515_bit_modify( CANINTF, (1<<RX1IF), 0 );   
    }
    *ID = (((uint32_t) ID_temp[0]<<24)|((uint32_t) ID_temp[1]<<16)|((uint32_t) ID_temp[2]<<8)|ID_temp[3]);

}
