#ifndef __COMM_H__
#define __COMM_H__

typedef unsigned char           BYTE; 

typedef struct
{
    BYTE    cbRcvd;
    BYTE    cbSent;
    BYTE    cbOMsg;

    BOOL    bFramed;
    BOOL    bFraming;
    BYTE    cntFrame;
    BYTE    cntFrameMax;

    BOOL    bSending;
    BYTE    cntSendMsg;
} COMM_CTL_BLOCK;

#define MCB_INIT(mcb, Frame_Max_Val) \
	mcb.cbRcvd		= 0;\
	mcb.cbSent		= 0;\
	mcb.cbOMsg		= 0;\
	mcb.bFramed		= false;\
	mcb.bFraming	= false;\
	mcb.cntFrame	= 0xFF;\
	mcb.cntFrameMax	= Frame_Max_Val;\
	mcb.bSending	= false;\
	mcb.cntSendMsg	= 0xFF;

#define MCB_CLEAR(mcb) \
	mcb.cbRcvd		= 0;\
	mcb.cbSent		= 0;\
	mcb.cbOMsg		= 0;\
	mcb.bFramed		= false;\
	mcb.bFraming	= false;\
	mcb.cntFrame	= 0xFF;\
	mcb.bSending	= false;\
	mcb.cntSendMsg	= 0xFF;

#endif
