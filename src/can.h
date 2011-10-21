/************************************************************************************************

 Author        : Yu

 Date          : 

 File          : 
                                      
 Hardware      : ADuC702x

 Description   : 
*************************************************************************************************/
#ifndef __CAN_H_
#define __CAN_H_

extern void can_send_message( uint32_t id, uint8_t *data, uint8_t length, uint8_t flags);

#endif 
