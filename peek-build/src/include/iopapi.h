

#ifndef IOPAPI_H
#define IOPAPI_H

#include "exeapi.h"
#include "exedefs.h"
#include "iopets.h"
#include "monapi.h"
#include "monerrs.h"
#include "pdeapi.h"
#include "sysdefs.h"
#include "sysapi.h"

/*------------------------------------------------------------------------
* Define constants used in IOP API
*------------------------------------------------------------------------*/

	/* number of available UARTs */
#define IOP_MAX_UARTS_NUMBER 3

	/* ETS display size for MMI */	
#define IOP_MAX_DISPLAY_COLS      16
#define IOP_MAX_DISPLAY_ROWS      2

	/* msg size constants */
#define IOP_MSG_ID_LEN         2
#define IOP_CHECKSUM_LEN       1
#define IOP_MAX_MSG_BUFF_LEN   EXE_MAX_IOP_MSG_SIZE
#define IOP_MAX_MSG_LEN        (IOP_MAX_MSG_BUFF_LEN + IOP_MSG_ID_LEN)
#define IOP_MAX_PACKET_LEN     (IOP_MAX_MSG_LEN + IOP_CHECKSUM_LEN)

	/* msg framing chars */
#define IOP_SER_ESC_CHAR       0xFD 
#define IOP_SER_SYNC_CHAR      0xFE
#define IOP_SER_MOD_CHAR       0xFF

/* define macros for Uart0/1/2 registers. These macros are used for registers arrays,
   initialized in ioptask.c */
#define HWD_UART_RHR(x)  HWD_UART_BASE + x*0x100 + 0x00  /* Tx holding data */
#define HWD_UART_THR(x)  HWD_UART_BASE + x*0x100 + 0x00  /* Tx holding data */
#define HWD_UART_IER(x)  HWD_UART_BASE + x*0x100 + 0x04  /* Interrupt enable */
#define HWD_UART_ISR(x)  HWD_UART_BASE + x*0x100 + 0x08  /* Interrupt iden. register */
#define HWD_UART_FCR(x)  HWD_UART_BASE + x*0x100 + 0x08  /* Fifo control */
#define HWD_UART_LCR(x)  HWD_UART_BASE + x*0x100 + 0x0C  /* Line control */
#define HWD_UART_MCR(x)  HWD_UART_BASE + x*0x100 + 0x10  /* Modem control */
#define HWD_UART_LSR(x)  HWD_UART_BASE + x*0x100 + 0x14  /* Line status */
#define HWD_UART_MSR(x)  HWD_UART_BASE + x*0x100 + 0x18  /* Modem status */
#define HWD_UART_SPR(x)  HWD_UART_BASE + x*0x100 + 0x1C  /* Scratchpad */
#define HWD_UART_DLL(x)  HWD_UART_BASE + x*0x100 + 0x00  /* LSB divisor latch */
#define HWD_UART_DLM(x)  HWD_UART_BASE + x*0x100 + 0x04  /* MSB divisor latch */

/*------------------------------------------------------------------------
 *	Mailbox, signal amd message IDs exported by IOP
 *-----------------------------------------------------------------------*/

#define IOP_MAILBOX_ETS           EXE_MAILBOX_1_ID  /* ETS mailbox */
#define IOP_MAILBOX_CMD           EXE_MAILBOX_2_ID  /* Command mailbox */
#define IOP_MAILBOX_DATA          EXE_MAILBOX_3_ID  /* Data mailbox */
#define IOP_MAILBOX_MSC           EXE_MAILBOX_4_ID

	/* 
	 *	signal 11 to 20 are used by IOP and defined in iopdefs.h
	 */
#define IOP_ETS_QUEUE_FULL_SIGNAL     EXE_SIGNAL_1	/* used by EXE */

#define IOP_USB_RX_ETS_MSG_SIGNAL	EXE_SIGNAL_2	/* data received */
#define IOP_USB_TX_ETS_MSG_SIGNAL	EXE_SIGNAL_3	/* data sent */
#define	IOP_USB_RX_DATA_SIGNAL		EXE_SIGNAL_4	/* data received */
#define	IOP_USB_TX_DATA_SIGNAL		EXE_SIGNAL_5	/* data sent */
#define	IOP_USB_DEV_CONFIG_SIGNAL	EXE_SIGNAL_6	/* usb reset, setconf or setintf */

#define   IOP_USB_RX_BULKONLY_SIGNAL    EXE_SIGNAL_8
#define   IOP_USB_TX_BULKONLY_SIGNAL    EXE_SIGNAL_9
#define   IOP_USB_ALL_BULKONLY_SIGNAL          (IOP_USB_RX_BULKONLY_SIGNAL | IOP_USB_TX_BULKONLY_SIGNAL)

	/* grouping of USB data signals */
#define	IOP_USB_ALL_ETS_SIGNAL		(IOP_USB_RX_ETS_MSG_SIGNAL | IOP_USB_TX_ETS_MSG_SIGNAL)
#define	IOP_USB_ALL_DATA_SIGNAL		(IOP_USB_RX_DATA_SIGNAL | IOP_USB_TX_DATA_SIGNAL)
#define	IOP_USB_ALL_SIGNAL			(IOP_USB_ALL_ETS_SIGNAL | IOP_USB_ALL_DATA_SIGNAL)

/*------------------------------------------------------------------------
 *	Message IDs for _ETS mailbox
 *-----------------------------------------------------------------------*/

	/* enum for ETS message IDs is in iopets.h */

	/* IOP_CP_DISPLAY_ETS */
typedef enum
{
   IOP_ROW_1 = 0,
   IOP_ROW_2 = 1
} IopRowIdT;

typedef PACKED struct 
{
   uint8          Row;
   uint8          Column;
   uint8          Text[IOP_MAX_DISPLAY_COLS + 1];
} IopDispTextMsgT;

	/* IOP_DSPM_PEEK_ETS */
	/* IOP_DSPV_PEEK_ETS */
typedef PACKED struct 
{
   uint16       StartAddr;
   uint16       NumWords;
   uint16       Data[1];
} IopDspPeekRspMsgT;

	/* IOP_GENERIC_ACK_ETS */
typedef PACKED struct 
{
   uint16        MsgId;
} IopGenericAckT;

/*------------------------------------------------------------------------
 *	Message IDs for _CMD mailbox
 *-----------------------------------------------------------------------*/

typedef enum
{
   IOP_SETBAUD_MSG   = 0,
   IOP_SIM_CP_MSG_MSG,
   IOP_SETBAUD_DATA_MSG,
   IOP_CNG_UART_MSG,
   IOP_ENABLE_GENERIC_ACK_MSG,
   IOP_UNAV_START_GPS_INIT_MSG,
   IOP_UNAV_GPS_ENTER_WAKE_UP_REQ_MSG,
   IOP_UNAV_GPS_ENTER_SLEEP_REQ_MSG,
   IOP_SETDEV_MSG,
   IOP_SET_DATA_TO_ETS_MSG,
   IOP_AIW_C109_IND_MSG,
   IOP_USB_C108_IND_MSG,
   IOP_AIW_MODEM_ALIVE_MSG,
   IOP_SET_DATA_THR,
   IOP_BT_TX_ACK_AUX_MSG,
   IOP_USB_CHANGE_DEV_TO_UDISK,
   IOP_MSC_TX_MSG
} IopMsgIdT;

	/* IOP_SETBAUD_MSG */
typedef enum
{
   IOP_19K_BAUD	 = 0,
   IOP_38K_BAUD, 
   IOP_57K_BAUD,
   IOP_115K_BAUD,
   IOP_153K_BAUD,
   IOP_230K_BAUD,
   IOP_9600_BAUD,
   IOP_600_BAUD,
   IOP_300_BAUD
} IopBaudT;

typedef enum
{
   IOP_ACK      = 0,
   IOP_SET_BAUD
} IopBaudCmdT;

typedef PACKED struct 
{
   ExeRspMsgT   RspInfo;    
   IopBaudCmdT  Cmd;
   IopBaudT     BaudRate;
   uint8        Delay;
} IopSetBaudMsgT;

	/* IOP_SIM_CP_MSG_MSG */
typedef PACKED struct 
{
   ExeTaskIdT      TaskId;
   ExeMailboxIdT   MboxId;
   uint16          MsgId;
   uint8           MsgBuf[IOP_MAX_MSG_BUFF_LEN];
} IopCpSimMsgT;

	/* IOP_SETBAUD_DATA_MSG */
typedef PACKED struct 
{
   ExeRspMsgT   RspInfo;    
   uint8        UartNum;
   IopBaudT     BaudRate;
} IopSetDataBaudMsgT;

	/* IOP_CNG_UART_MSG */
typedef enum { 
	NO_UART,
	ETS_UART,
	DATA_UART,
	GPS_UART
}UartTypeT;

typedef PACKED struct 
{
   ExeRspMsgT   RspInfo;    
   UartTypeT	NewUarts[IOP_MAX_UARTS_NUMBER];
} IopUartSelectMsgT;

	/* IOP_ENABLE_GENERIC_ACK_MSG */
typedef PACKED struct
{
    ExeRspMsgT   RspInfo;    
    uint16       Enable;
} IopEnableGenericAckMsgT;

	/* IOP_SETDEV_MSG */
typedef enum {
	IOP_DATA_SER_DEV,
	IOP_DATA_USB_DEV
} IopDeviceDataT;	/* data device */

	/* the order of the entry in this enum is important. do not change */
typedef enum {
	IOP_INFO_SER_DEV,
	IOP_INFO_USBSER_DEV,	/* useful for debugging USB */
	IOP_INFO_USB_DEV
} IopDeviceInfoT;	/* ETS device */

typedef PACKED struct {
	IopDeviceInfoT	InfoDev;
	IopDeviceDataT	DataDev;
} IopSetDevMsgT;

	/* IOP_AIW_C109_IND_MSG */
typedef PACKED struct {
	bool	C109On;	/* status of C109, active high */
} IopAiwC109IndMsgT;

   /* IOP_USB_C108_IND_MSG */
typedef PACKED struct {
	bool	C108On;	/* status of C108 as reported by USB windriver, active high */
    bool    CableUnplugged;    /* TRUE if this IND sent because cable unplugged */
} IopUsbC108IndMsgT;

	/* IOP_AIW_MODEM_ALIVE_MSG */
typedef PACKED struct {
	bool	IsAlive;	/* TRUE == modem task is ready to accept AT cmds */
} IopAiwModemAliveMsgT;

/*------------------------------------------------------------------------
 *	Message IDs for _DATA mailbox
 *-----------------------------------------------------------------------*/

typedef enum
{
  IOP_DATA_RX_REQ_MSG = 0,  /* This command is used to send data to IOP */
  IOP_DATA_TX_ACK_MSG       /* This command is used to acknowledge the receipt 
                               of data from IOP */
} IopDataMsgIdT;

	/* IOP_DATA_RX_REQ_MSG */
typedef PACKED struct 
{
   ExeRspMsgT   RspInfo;    
   uint8       *Data;
   uint16       DataLen;
} IopAiwRxMsgT;

/* Define the message type for data to be send to uart1 from HLW */
typedef IopAiwRxMsgT IopHlwRxMsgT;
 
/*------------------------------------------------------------------------
 *	Message IDs for _GPS mailbox
 *-----------------------------------------------------------------------*/

typedef enum
{
  IOP_UNAV_GPS_TX_DATA_MSG = 0   /* Request for IOP to send data to GPS HW */
} IopGpsMsgIdT;

/* Define GPS message definitions */
typedef PACKED struct
{
   uint8  PacketId;
   uint8  PacketLength;
   uint8  Data[1];
#ifdef GPS_SIMULATION_IN_USE
   bool   TestIdle;
#endif /* GPS_SIMULATION_IN_USE */
} IopUnavGpsTxDataMsgT;
   
/*------------------------------------------------------------------------
 *	Exported IOP functions
 *-----------------------------------------------------------------------*/

#ifdef SYS_DEBUG_FAULT_FILE_INFO

#define IopTxCriticalExeFault( ExeFaultData, FaultSize, EtsMsgId) \
        __IopTxCriticalExeFault( ExeFaultData, FaultSize, EtsMsgId, __FilenameBuffer)

extern void __IopTxCriticalExeFault( uint8 *ExeFaultData, uint16 FaultSize,
                                    IopEtsMsgIdT EtsMsgId, char *FilenameBuffer);

#define IopTxCriticalFault(UnitNum, FaultCode1, FaultCode2) \
        __IopTxCriticalFault(UnitNum, FaultCode1, FaultCode2, __FilenameBuffer)

extern void __IopTxCriticalFault(MonFaultUnitT UnitNum, uint32 FaultCode1, uint32 FaultCode2,
                                 char *FilenameBuffer);
#else

extern void IopTxCriticalExeFault( uint8 *ExeFaultData, uint16 FaultSize, IopEtsMsgIdT EtsMsgId);
extern void IopTxCriticalFault(MonFaultUnitT UnitNum, uint32 FaultCode1, uint32 FaultCode2);

#endif /* SYS_DEBUG_FAULT_FILE_INFO */

extern void	IopTxCriticalMsg( uint32 MsgId, void *MsgDataP, uint32 MsgDataSize );
extern void IopTxCriticalMonException(MonExceptionTypeT Exception, uint8 *ExceptionData, uint16 ExceptionDataSize);

extern bool IopPowerKeyStatus(void);
extern void IopSetBaudRate(IopBaudT BaudRate, uint8 UartNum);
extern void IopUpdateDataUartSettings(void);
extern uint8 IopUartNumGet(UartTypeT UartType);
extern void IopGpsUartClockProtect(bool GpsUartClkProtectNeeded);

#endif	/* IOPAPI_H */



