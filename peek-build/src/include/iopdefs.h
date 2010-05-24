

#ifndef IOPDEFS_H
#define IOPDEFS_H

#include "exeapi.h"
#include "exedefs.h"
#include "iopapi.h"
#include "sysdefs.h"

/*------------------------------------------------------------------------
* Declare global constants used in IOP unit
*------------------------------------------------------------------------*/
	/* signals 1 to 10 are used by other tasks and defined in iopapi.h */
#define IOP_UART0_RX_ETS_MSG_SIGNAL   EXE_SIGNAL_11
#define IOP_UART0_ETS_ERROR_SIGNAL    EXE_SIGNAL_12
#define IOP_UART1_RX_MSG_SIGNAL       EXE_SIGNAL_13
#define IOP_GPINT1_SIGNAL             EXE_SIGNAL_14
#define IOP_UART1_TX_DONE_SIGNAL      EXE_SIGNAL_15
#define IOP_UART1_SLEEP_RESUME_SIGNAL EXE_SIGNAL_16
#define IOP_GPS_RX_MSG_SIGNAL         EXE_SIGNAL_17
#define IOP_GPS_TIMER_EXPIRED_SIGNAL  EXE_SIGNAL_18
#define IOP_GPS_TX_DONE_SIGNAL        EXE_SIGNAL_19

	/* msg size constants are in iopapi.h */
/*
#define IOP_MSG_ID_LEN      
#define IOP_CHECKSUM_LEN    
#define IOP_MAX_MSG_BUFF_LEN
#define IOP_MAX_MSG_LEN     
#define IOP_MAX_PACKET_LEN  
*/

	/* msg framing chars are in iopapi.h for use by bootldr */
/*
#define IOP_SER_ESC_CHAR   
#define IOP_SER_SYNC_CHAR 
#define IOP_SER_MOD_CHAR  
*/

/* Define UART0 specific constants */
#define IOP_UART0_TX_ETS_BUFFER_SIZE   IOP_MAX_PACKET_LEN * 10
#define IOP_UART0_RX_ETS_BUFFER_SIZE   IOP_MAX_PACKET_LEN * 3

/* Define UART1 specific constants */
#define	IOP_UART1_RX_BUFFER_NUM	 2
#define IOP_UART1_TX_BUFFER_SIZE 1600
#define IOP_UART1_RX_BUFFER_SIZE 664
#define IOP_UART1_HI_WATER_MARK  (IOP_UART1_RX_BUFFER_SIZE*8/10)
#define IOP_UART1_LOW_WATER_MARK (IOP_UART1_RX_BUFFER_SIZE*2/10)

/*------------------------------------------------------------------------
 * various data structures and variables
 *------------------------------------------------------------------------*/
typedef struct 
{
    uint16         EtsMsgId; 
    ExeTaskIdT     TaskId;  
    ExeMailboxIdT  MailboxId;  
    uint32         CpMsgId; 
    bool           RspFlg; 
    bool           NoGenericAck;
} IopRoutingTableT;

extern const IopRoutingTableT IopRoutingTable[];
extern uint8 EtsUartNum;
extern uint8 DefaultDataUartNum;
extern bool  PswPktRelayModelInitByMMI;

/* ISRs procedure pointers */
typedef void (*pLISR_PROC_PTR)(uint32 IsrStatus, uint8 UartNum);
typedef void (*pHISR_PROC_PTR) (uint8 UartNum);

/* Uart control data type */
typedef union {
	struct {
		/* Define ETS RX serial buffer read, write and end pointers used by task 
		and ISR in conjunction with RX circular buffer */
		uint8  *IopUartRxEtsRdP;
		uint8  *IopUartRxEtsWrP;
		uint8  *IopUartRxEtsEndP;
		uint32  IopUartRxEtsCount;

		/* Define ETS TX serial buffer pointer and count used by task and ISR */
		uint8  *IopUartTxEtsRdP;
		uint8  *IopUartTxEtsWrP;
		uint8  *IopUartTxEtsEndP;
		uint32  IopUartTxEtsCount;
		
		/* used to compute checksum and keep track of bytes transferred */
		uint8	TxChecksum;
		uint16	TxCount;

	}ets;

	struct {
		/* Define DATA RX serial buffer read, write and end pointers used by task 
		and ISR in conjunction with RX circular buffer */
		uint8  *IopDataRxRdP;
		uint8  *IopDataRxWrP;
		uint16  IopDataRxCount;
		bool    IopDataDTRAssert;

		/* Define DATA TX serial buffer pointer and count used by task and ISR */
		uint8  *IopDataTxRdP;
		int16   IopDataTxCount;
		uint8   DataUartCTSState;

		bool    IopDataUartTxFIFOEmpty;
		bool    IopDataTxPending;
		bool    IopDataUartSleep;
		/* Flag indicating data received by DATA UART may be sent to a task */
		bool	IopTxDataAck;
		bool	IopRxDataAck;
	}data;

	struct {
		/* Define GPS RX serial buffer read, write and end pointers used by task 
		and ISR in conjunction with RX circular buffer */
		uint8  *IopUartRxGpsRdP;
		uint8  *IopUartRxGpsWrP;
		uint8  *IopUartRxGpsEndP;
		uint32  IopUartRxGpsCount;

		/* Define GPS TX serial buffer pointer and count used by task and ISR */
		uint8  *IopUartTxGpsRdP;
		uint8  *IopUartTxGpsWrP;
		uint8  *IopUartTxGpsEndP;
		uint32  IopUartTxGpsCount;
	}gps;

}UartControlT;

/*------------------------------------------------------------------------
 * IOP I/O device definitions
 *------------------------------------------------------------------------*/

/*	the real procedures for LISR and HISR interrupt for ETS and Data uart types. 
	They are implemented in iopisr.c file */
extern void RealEtsLisrProcedure(uint32 IsrStatus, uint8 UartNum);
extern void RealDataLisrProcedure(uint32 IsrStatus, uint8 UartNum);
extern void DummyLisrProcedure(uint32 IsrStatus, uint8 UartNum);

extern void RealEtsHisrProcedure(uint8 UartNum);
extern void RealDataHisrProcedure(uint8 UartNum);
extern void DummyHisrProcedure(uint8 UartNum);

extern void IopIsrInit(UartTypeT *Uarts);
 
#ifdef SYS_OPTION_USB_ENABLE

/*------------------------------------------------------------------------
 * iopusb.c section
 *------------------------------------------------------------------------*/
#define	EP_RX_DATA	HWD_USB_EP_1	/* incoming data from DATA */
#define	EP_TX_DATA	HWD_USB_EP_1	/* outgoing pipe to DATA */
#define	EP_RX_ETS	HWD_USB_EP_2	/* incoming data from ETS */
#define	EP_TX_ETS	HWD_USB_EP_2	/* outgoing pipe to ETS */

	/*
	 *	IOP message header; same field order as wire protocol
	 */

#define IOP_USB_SYNC	0x98BADCFE		/* SYNCH field, reversed to fit a uint32 format */
#define	IOP_USB_SYNC_SZ	sizeof(uint32)	/* size of SYNCH field */
#define	IOP_USB_PAD		0xFE			/* to pad ETS msgs to 4 bytes */
 
typedef PACKED struct {
	uint32	Synch;		/* synch signature */
	uint16	MsgLen;		/* msg size, including msg id */
	uint16	MsgId;		/* ETS message id */
} IopUsbEtsMsgHeaderT;

extern void IopUsbSignalHandler( ExeSignalT UsbSignal );
extern void IopUsbDataMsgHandler( void );

/*------------------------------------------------------------------------
 * iopusbrw.c section
 *------------------------------------------------------------------------*/

	/* data buffer is allocated in task and used in iopusbrw.c */
extern uint8	IopDataRxBuffer[IOP_UART1_RX_BUFFER_NUM][IOP_UART1_RX_BUFFER_SIZE];

extern void IopUsbDataReadSetup( void );
extern void IopUsbDataReadMore( void );
extern void IopUsbDataReadAckReceived( void );
extern void IopUsbDataReadAbort( void );

extern void IopUsbDataWriteSetup( void );
extern void IopUsbDataWriteReset( IopAiwRxMsgT *MsgP );
extern void IopUsbDataWriteMore( void );
extern void IopUsbDataWriteAbort( void );

extern void IopUsbEtsReadSetup( void );
extern void IopUsbEtsReadMore( void );
extern void IopUsbEtsReadAbort( void );

extern void IopUsbEtsWriteSetup( void );
extern bool IopUsbEtsWriteReset( uint16 OutMsgId, uint8 *OutMsgPtr, uint16 OutMsgSize );
extern bool	IopUsbEtsWriteMore( void );
extern void IopUsbEtsWriteFlush( void );
extern void	IopUsbEtsWriteSynch( uint16 OutMsgId, uint8 *OutMsgPtr, uint16 OutMsgSize );
extern void	IopUsbEtsWriteAbort( void );
extern bool IopUsbEtsWriteIsIdle( void );

extern void IopUsbRWSetup( void );
extern void IopUsbRWConnect( bool Connection );

#endif /* SYS_OPTION_USB_ENABLE */

#endif	/* IOPDEFS_H */


