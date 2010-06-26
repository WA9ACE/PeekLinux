#ifndef __FMS_H
#define __FMS_H

#include "fm_types.h"
#include "fm_utils.h"
#include "fms_api.h"
#include "fm_drivers.h"
#include "fms_transport.h"

void FMS_Init(void);
void FMS_Deinit(void);

void Fm_Process(void);
void Fm_Timer_Process(void);
void Fm_Interrupts_Process(void);
void Fm_Commands_Process(void);
void Fm_Events_Process(void);
void fm_radio_init(void);


typedef TIFM_U8 FmOpType;

#define READ_EVENT		0
#define WRITE_EVENT		1

/* RDS definitions */
#define RDS_NUM_TUPLES				64
#define RDS_BLOCK_SIZE				3
#define RDS_BLOCKS_IN_GROUP			4
#define RDS_GROUP_SIZE				((RDS_BLOCK_SIZE) * (RDS_BLOCKS_IN_GROUP))
#define RDS_PS_NAME_SIZE			8
#define RDS_PS_NAME_LAST_INDEX		((RDS_PS_NAME_SIZE) / 2)
#define RDS_RADIO_TEXT_SIZE			64
#define RDS_RT_LAST_INDEX			((RDS_RADIO_TEXT_SIZE) / 4)

#define RDS_BLOCK_A		0
#define RDS_BLOCK_B		1
#define RDS_BLOCK_C		2
#define RDS_BLOCK_Ctag	3
#define RDS_BLOCK_D		4
#define RDS_BLOCK_E		5

#define RDS_BLOCK_INDEX_A		0
#define RDS_BLOCK_INDEX_B		1
#define RDS_BLOCK_INDEX_C		2
#define RDS_BLOCK_INDEX_D		3
#define RDS_BLOCK_INDEX_UNKNOWN	0xF0

#define RDS_NEXT_PS_INDEX_RESET				9	/* Bigger than the max index */
#define RDS_NEXT_RT_INDEX_RESET				17	/* Bigger than the max index */
#define RDS_PREV_RT_AB_FLAG_RESET			2	/* AB flag can be 0 or 1 */

#define RDS_BLOCK_A_PI_INDEX				0
#define RDS_BLOCK_A_STATUS_INDEX			2
#define RDS_BLOCK_B_GROUP_TYPE_INDEX		3
#define RDS_BLOCK_B_STATUS_INDEX			5
#define RDS_BLOCK_C_AF1						6
#define RDS_BLOCK_C_AF2						7
#define RDS_BLOCK_C_STATUS_INDEX			8
#define RDS_BLOCK_D_PS1_INDEX				9
#define RDS_BLOCK_D_PS2_INDEX				10
#define RDS_BLOCK_D_STATUS_INDEX			11
#define RDS_BLOCK_C_RT1_INDEX				6
#define RDS_BLOCK_C_RT2_INDEX				7
#define RDS_BLOCK_D_RT1_INDEX				9
#define RDS_BLOCK_D_RT2_INDEX				10

#define RDS_STATUS_ERROR_MASK				0x18
#define RDS_BLOCK_B_GROUP_TYPE_MASK			0xF0
#define RDS_BLOCK_B_AB_BIT_MASK				0x0800
#define RDS_BLOCK_B_PS_INDEX_MASK			0x0003
#define RDS_BLOCK_B_RT_INDEX_MASK			0x000F
#define RDS_BLOCK_B_AB_FLAG_MASK			0x0010

#define RDS_RT_END_CHARACTER				0x0D	/* indicates carriage return	*/

#define RDS_GROUP_TYPE_0					0
#define RDS_GROUP_TYPE_2					2

#define RDS_MIN_AF			1
#define RDS_MAX_AF			204
#define RDS_MAX_AF_JAPAN	140
#define RDS_1_AF_FOLLOWS	225
#define RDS_25_AF_FOLLOWS	249
#define RDS_MAX_AF_LIST		25

typedef struct {
    TIFM_U8 last_block_index;
	TIFM_U8 rdsGroup[RDS_GROUP_SIZE];  /* Contains a whole group */
	TIFM_U8 nextPsIndex;
	char psName[RDS_PS_NAME_SIZE+1];
	TIFM_U8 prevABFlag;
	TIFM_BOOL changed;
	TIFM_U8 nextRtIndex;
	char radioText[RDS_RADIO_TEXT_SIZE+1];
} RdsParams;

#define NO_STATION_FREQ		0xFFFFFFFF
#define NO_PI_CODE			0
typedef struct {
    TIFM_U32 freq;
	TIFM_U16 piCode;
	TIFM_U8 psName[RDS_PS_NAME_SIZE+1];
	TIFM_U32 afList[RDS_MAX_AF_LIST];
	TIFM_U8 afListSize;
	TIFM_U8 afListCurMaxSize;  /* The number of af we are expecting to receive */
} TunedStationParams;

typedef struct _EventUp2Fms {                
    TIFM_U8		cmdCmpltEvent;		/* Indicate whether an event was received or not */
	TIFM_U16		read_param;
} EventUp2Fms;

typedef struct _FmOperation {                
	TIFM_ListNode   node;
    FmOpType    opType;
} FmOperation;

typedef struct {
    FmOperation op;
} FmGenOp;

typedef struct {
    FmOperation op;
    TIFM_U8         mode; 
} FmMoStSetOp;

typedef struct {
    FmOperation op;
    TIFM_U8         mode; 
} FmBandSetOp;

typedef struct {
    FmOperation op;
    TIFM_U8         mode; 
} FmMuteOp;

typedef struct {
    FmOperation op;
    TIFM_U16         gain; 
} FmVolumeSetOp;

typedef struct {
    FmOperation op;
    TIFM_U8         mode; 
} FmRdsSetOp;

typedef struct {
    FmOperation op;
    TIFM_U32         freq; 
} FmTuneOp;

typedef struct {
    FmOperation op;
    TIFM_U8      dir; 
	TIFM_U8		curStage;	/* Stage of seek/stop seek operation */
	TIFM_U8		seekStageBeforeStop;
	TIFM_U8		status;
} FmSeekOp;

typedef struct {
    FmOperation op;
    TIFM_U8      mode; 
} FmSetAFOp;

typedef struct {
    FmOperation op;
    TIFM_U8      mode; 
} FmSetStereoBlendOp;

typedef struct {
    FmOperation op;
    TIFM_U8      mode; 
} FmSetDeemphasisModeOp;

typedef struct {
    FmOperation op;
    TIFM_S8     rssi_level; 
} FmSetRssiSearchLevel;

typedef struct {
    FmOperation op;
    TIFM_U8      pause_level; 
} FmSetPauseLevel;

typedef struct {
    FmOperation op;
    TIFM_U8      pause_duration; 
} FmSetPauseDuration;

typedef struct {
    FmOperation op;
    TIFM_U8      mode; 
} FmSetRdsRbdsMode;

typedef struct {
	TIFM_U8  rdsData[RDS_THRESHOLD_MAX*RDS_BLOCK_SIZE];
	TIFM_U16 len;		/* Actual read length */
} FmReadRdsOp;

typedef struct {
	TIFM_U8  data[RDS_THRESHOLD_MAX*(RDS_BLOCK_SIZE-1)];
	TIFM_U16 len;		/* Combined length of correctly received groups */
} FmRawRdsEvt;

typedef struct {
    FmOperation op;
	FmReadRdsOp	readRdsOp;
} FmGenIntOp;

typedef struct {
    FmOperation op;
} FmTimeoutOp;

typedef struct{
	union{
		TIFM_U8	mode;
		TIFM_U16 gain;
		TIFM_U32 freq;
		TIFM_S8 rssi_level;
		TIFM_U8 pause_level;
		TIFM_U8 pause_duration;
	}param;
}FmCurOpParam;

#define GEN_INT_MAL_STAGE						0
#define GEN_INT_AFTER_MAL_STAGE					1
#define GEN_INT_AFTER_STEREO_CHANGE_STAGE		2
#define GEN_INT_AFTER_RDS_STAGE					3
#define GEN_INT_AFTER_LOW_RSSI_STAGE			4
#define GEN_INT_AFTER_FINISH_STAGE				5

/*Upper events options */
#define UPPER_EVENT_NONE		0
#define UPPER_EVENT_STOP_SEEK	1

/* A function that handles the current state of the operation */
typedef void (*FmOpCurHandler)(void);
/* An array of handlers of the current operation */
typedef FmOpCurHandler* FmOpHandlerArray;


typedef struct {
    TIFM_ListNode	    opList;         /* Operation queue */
	TIFM_U8				band;		    /* Band used japan/us-europe */
	TIFM_BOOL			rfMuteOn;	    /* is RF dependent mute on/off */
	TIFM_U8				muteState;	    /* state of mute (mute/att/unmute) */
	EventUp2Fms		    curEvent;
	TIFM_BOOL			interruptInd;   /* an interrupt was received */
	TIFM_U16			fmFlag;		    /* fm interrupts flag */
	TIFM_U16			fmMask;		    /* fm interrupts mask */
	TIFM_U16			genIntSetBits;	/* The actual interrupts that happened saved for the general int */
	TIFM_U16			opHandlerIntSetBits; /* The actual interrupts that happened saved for the opHandler int */

	TIFM_BOOL			intReadEvt;		/* Indication that interrupt flag was read */
	TIFM_BOOL			waitCmdCmplt;	/* Indicates whether or not we're currently waiting for a cmd complete event */ 
	TIFM_U8				readIntState;	/* states for interrupts handling */ 
	TIFM_U8				upperEventWait;	/* indicates whether the upper event should wait for cmd complete or not */ 
	TIFM_U8				callReason;		/* When calling the operation handler say if it was called because of cmd_complete event, interrupt or upper_event */	

	TIFM_U8				initState;		/* must wait for BT init script to run before fm init script */
	TIFM_U8				upperEvent;		/* Event from the upper api */

    FmOperation*		curOp;          /* Current operation */
    TIFM_U8				fmOpCurStage;
	FmOpCurHandler		fmOpHandler;    /* Handle operation events */
	FmCurOpParam		fmCurOpParam;		

    /* Operations */
    FmGenOp					powerOnOp;   
    FmGenOp					powerOffOp;   
	FmMoStSetOp				moStOp;
	FmBandSetOp				bandSetOp;			
	FmMuteOp				muteOp;
	FmVolumeSetOp			volumeSetOp;
	FmRdsSetOp				rdsSetOp;       
	FmTuneOp				tuneOp;
	FmGenOp					rssiGetOp;
	FmSeekOp				seekOp;
	FmSetAFOp				setAFOp;
	FmSetStereoBlendOp		setStereoBlendOp;
	FmSetDeemphasisModeOp	setDeemphasisModeOp;
	FmSetRssiSearchLevel	setRssiSearchLevelOp;
	FmSetPauseLevel			setPauseLevelOp;
	FmSetPauseDuration		setPauseDurationOp;
	FmSetRdsRbdsMode		setRdsRbdsModeOp;
    FmRawRdsEvt             rawRdsEvt;
	FmGenOp					moStGetOp;
	FmGenIntOp				genIntOp;
	FmGenOp					audioEnableOp;
	FmTimeoutOp				timeoutOp;

	RdsParams				rdsParams;			/* Params of a specific RDS read */
	TunedStationParams		curStationParams;
	TIFM_BOOL				AFOn;				/* Indicates whether AF feature is on/off */
	TIFM_U8					curAfJumpIndex;		/* Holds the index of the current AF jump */
	TIFM_U32				freqBeforeJump;		/* Will hold the frequency before the jump */

	TIFM_U8					commandParams[300];
	BthalOsTimerHandle		fmTimerHandle;
	
	FmDriver		        fmDriver;
} FmGeneral;

extern FmGeneral fm;
#define FMC(s)  (fm.s)

#define FmAddOperation(o)		(TIFM_InsertTailList(&FMC(opList), &((o)->op.node)))
#define FmAddOperationFront(o)	(TIFM_InsertHeadList(&FMC(opList), &((o)->op.node)))

/* Operation handlers definitions */
#define NUM_OP_HANDLERS		(FM_LAST_OP+1)

#define NUM_HANDLERS_POWER_ON    					5
#define NUM_HANDLERS_POWER_OFF   					2
#define NUM_HANDLERS_MOST_SET    					2
#define NUM_HANDLERS_BAND_SET    					2
#define NUM_HANDLERS_MUTE	    					2
#define NUM_HANDLERS_VOLUME_SET						2
#define NUM_HANDLERS_RDS_SET						5
#define NUM_HANDLERS_TUNE							8
#define NUM_HANDLERS_RSSI_GET						2
#define NUM_HANDLERS_MAIN_SEEK						1
#define NUM_HANDLERS_SET_AF							2
#define NUM_HANDLERS_SET_STEREO_BLEND				2
#define NUM_HANDLERS_SET_DEEMPHASIS_MODE			2	
#define NUM_HANDLERS_SET_RSSI_SEARCH_LEVEL			2
#define NUM_HANDLERS_SET_PAUSE_LEVEL				2	
#define NUM_HANDLERS_SET_PAUSE_DURATION				2	
#define NUM_HANDLERS_SET_RDS_RBDS_MODE				2
#define NUM_HANDLERS_MOST_GET						2	
#define NUM_HANDLERS_AUDIO_ENABLE					2
#define NUM_HANDLERS_GEN_INT						6
#define NUM_HANDLERS_READ_RDS						2
#define NUM_HANDLERS_AF_JUMP						8
#define NUM_HANDLERS_STEREO_CHANGED					2
#define NUM_HANDLERS_HW_MAL							1
#define NUM_HANDLERS_TIMEOUT						2

#define INCREMENT_STAGE		0xFF

/* Operations wait definitions */
#define DONT_WAIT				0
#define WAIT_FOR_CMD_COMPLETE	1
#define WAIT_FOR_INTERRUPT		2

/* Interrupts handling states */
#define	INT_STATE_NONE								0
#define INT_STATE_WAIT_FOR_CMD_CMPLT				1
#define INT_STATE_READ_INT							2
/* Indicates that a cmd_complete event is waiting to be handled */
#define CMD_CMPLT_EVT_WAIT_MASK					0x10

#define CALL_REASON_NONE				0
#define CALL_REASON_START				1
#define CALL_REASON_CMD_CMPLT_EVT		2
#define CALL_REASON_INTERRUPT			3
#define CALL_REASON_UPPER_EVT			4

/* commands parameteres definitions */
#define WR_CMD_LENGTH			2
#define WR_CMD_LENGTH_LOCATION	1
#define WR_CMD_PARAM_LOCATION	3

#define RD_CMD_LENGTH			2
#define RD_CMD_LENGTH_LOC		1

/* States for events state-machine */
#define FME_NOP					0
#define FME_START				1
#define FME_CMD_COMPLETE_EVT	2
#define FME_INTERRUPT_EVT		3
#define FME_END					4


#define INIT_STATE_FM_OFF					0
#define INIT_STATE_WAIT_CMD_COMPLETE		1
#define INIT_STATE_CMD_COMPLETE_DONE		2
#define INIT_STATE_CMD_COMPLETE_DONE_ERROR	3
#define INIT_STATE_FM_ON					4
#define INIT_STATE_FM_ERROR					5
#define INIT_STATE_FM_OFF_PENDING			6


/* Prototypes */
void fm_recvd_initCmdCmplt(TIFM_BOOL isValid);
void fm_recvd_readFlagCmdCmplt(TIFM_U8 *data);
void fm_recvd_readMaskCmdCmplt(TIFM_U8 *data);
void fm_recvd_readCmdCmplt(TIFM_U8 *data);
void fm_recvd_readRdsCmdCmplt(TIFM_U8 len, TIFM_U8 *data);
void fm_recvd_writeCmdCmplt(void);
//void fm_recvd_intterupt(void);





#endif
