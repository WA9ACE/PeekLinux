
#ifndef BALDEFS_H
#define BALDEFS_H

#include "sysdefs.h"
#include "exeapi.h"
#include "pswnam.h"
#include "balapi.h"
#include "sysKeydef.h"
#include "balsndapi.h"

 
/*-----------------------------------------------------------------
 * Define constants used within BAL 
 *----------------------------------------------------------------*/

/* local BAL signals, grouped for convenience; should start above BAL_SIGNAL_MAX */
/* one second timer functions */
#define  BAL_USERINFO_TIMEOUT_SIGNAL   EXE_SIGNAL_12

/* local val timers, grouped for convenience */
#define  BAL_AUDIO_TIMER_ID            (uint32)1
#define  BAL_MISC_TIMER_ID            (uint32)2 
#define  BAL_USERINFO_TIMER_ID         (uint32)3
#define  BAL_VREC_TIMER_ID             (uint32)4

#define  BAL_LOC_IP_SERVICE_OPTION     33
#define  BAL_LOC_IP_SOCKET_ID          0xFFFE   /* Reserved for Location Service */
#define  BAL_MAX_SPY_LEN               64

/*-----------------------------------------------------------------
 *	valamps.c interface
 *----------------------------------------------------------------*/

#ifdef SYS_OPTION_AMPS
void BalAmpsDtmfToneGenMsg( void *MsgDataP );
void BalAmpsGetRssiAckMsg( void *MsgDataP );
#endif

/* playback mode (once or continuous) */
typedef enum {
   BAL_AUDIO_DTMF_ONCE,
   BAL_AUDIO_DTMF_LOOP
} BalAudioDtmfLoopT;

/*-----------------------------------------------------------------
 *	valpsw.c interface
 *----------------------------------------------------------------*/
#define DISCONNECTING_WAIT			1000	/* msec */

/*-----------------------------------------------------------------
 *	valhlw.c interface
 *----------------------------------------------------------------*/
void BalTcpbConnStatusMsg( void* MsgDataP );
void BalHlwPppConnStatusMsg( void* MsgDataP );
void BalHlwPppCloseStatusMsg( void* MsgDataP );
void BalHlwSocketCreateStatusMsg( void* MsgDataP );
void BalHlwSocketBindStatusMsg( void* MsgDataP );
void BalHlwSocketConnStatusMsg( void* MsgDataP );
void BalHlwSocketCloseStatusMsg( void* MsgDataP );
void BalHlwTcpbDataRecvMsg( void* MsgDataP );
void BalHlwTcpbDataSendRspMsg( void* MsgDataP ); 
void BalTestTcpbSendDataMsg( void* MsgDataP );
void BalTestUdpbSendDataMsg( void* MsgDataP );
void ValUpbRecvDataMsg( void* MsgDataP );
void ValUdpbSentDataRspMsg( void* MsgDataP );

extern PswIs95NamT Nam; 
  
/*-----------------------------------------------------------------
 * val call back utils
 *-----------------------------------------------------------------*/
RegIdT BalRegister( const BalEventHandlerT* EventHandlerP,
                    BalEventFunc CallBack );
void   BalUnRegister( const BalEventHandlerT* EventHandlerP,
                      RegIdT RegId );
void   BalProcessEvent( const BalEventHandlerT* EventHandlerP,
                        uint32 Event,
                        void* EventDataP );

/*-----------------------------------------------------------------
 *	valuserinfo.c interface
 *----------------------------------------------------------------*/
#define BAL_USERINFO_TIMEOUT_VALUE		1000	/* msec */


void   ValUserInfoSignal( uint32 SignalId );
void   ValUserInfoMsg( uint32 MsgId, void* MsgDataP, uint32 MsgSize );


/*-----------------------------------------------------------------
 *	valvrs.c interface (voice recognition)
 *----------------------------------------------------------------*/

void BalVrecInit( void );
void BalVrecStateMachine( void*  MsgP,
                          uint32 MsgSize,
                          uint32 MsgId );
void BalVrecTimeoutSignal( void );
 
/*-----------------------------------------------------------------
 *	valvmemo.c interface (voice memo)
 *----------------------------------------------------------------*/
void BalVmemoInit(void);
void BalVmemoExec( uint32 MsgId, void* MsgDataP, uint32 MsgSize );

/*-----------------------------------------------------------------
 *	valsndtest.c interface (sound test)
 *----------------------------------------------------------------*/
extern void BalSndTestScenarios (uint16 TestNum);

/*-----------------------------------------------------------------
 *	valsndapi.c interface (sound API)
 *----------------------------------------------------------------*/
extern void BalSoundInit( void );
extern void BalSndEdaiMicCtrlDataMsg ( void* MsgDataP );


void BalMiscInit( void );
void BalMiscAudioEdaiMicCtrlDataMsg( void* MsgDataP );
void BalMiscL1DTstGetPhoneStatus( BalL1DTstGetPhoneStatusMsgT* MsgDataP );
void BalMiscReadBatteryVoltage(BalBatteryReadDataMsgT* RxMsgP);
void BalMiscReadTemperature(BalTempInfoDataMsgT *RxMsgP);
void BalMiscReadRxTxPower(BalRxTxPwrInfoMsgT *RxMsgP);
void BalMiscReadPilotPowerMsg(BalPswPilotPwrRptMsgT *RxMsgP);
void BalSetPktSvcState( BalRptCpEventT packetState );
void BalDspmVerRspMsg (void* MsgDataP);
void BalDspvVerRspMsg (void* MsgDataP);
void BalCpVerRspMsg (void* MsgDataP);
void BalDispSendBitmapData( void *MsgDataP); 
void BalDispSendBitmapInfo( void *MsgDataP);
void BmSuspendDataGet(void *MsgDataP);

/*****************************************************************************

  FUNCTION NAME: BalChvDeliverMsg

  DESCRIPTION:
    Deliver CHV response message.  It is used by the BAL mailbox.

  PARAMETERS:
    MsgId:   received message id   
    MsgP:    message
    MsgSize: message size	

  RETURNED VALUES:
    None

*****************************************************************************/
void BalChvDeliverMsg( uint32 MsgId,
                       void*  MsgP,
                       uint32 MsgSize );

/*****************************************************************************

  FUNCTION NAME: BalChvDeliverMsg

  DESCRIPTION:
    Deliver CHV response message.  It is used by the BAL mailbox.

  PARAMETERS:
    MsgId:   received message id   
    MsgP:    message
    MsgSize: message size	

  RETURNED VALUES:
    None

*****************************************************************************/
void BalUimDeliverMsg( uint32 MsgId,
                       void*  MsgP,
                       uint32 MsgSize );



/*-----------------------------------------------------------------
 *	valsndapi.c interface (sound API)
 *----------------------------------------------------------------*/
extern void BalSoundInit( void );
extern void BalSoundResetVoiceDevice (void);
extern void BalSndKeyProcess( SysKeyIdT Key, bool IsPressed );
extern void BalSndEdaiMicCtrlDataMsg ( void* MsgDataP );
extern void BalSndMusicStatusMsg (BalMusicStatusEventMsgT *MusicStatusP);

#endif /* BALDEFS_H */
