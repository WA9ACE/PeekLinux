

#include "fms.h"
#include "fm_os.h"
#include "fm_types.h"
#include "fms_transport.h"
#include "btl_config.h"
#include "me.h"
#include "hci.h"
#include "ti_chip_mngr.h"
#include "btl_config.h"


#if 0



#define HCIPP_I2C_FM_READ									(0x0133 | HCC_GROUP_SHIFT(HCC_GRP_VENDOR_SPECIFIC))
#define HCIPP_I2C_FM_READ_HW_REG							(0x0134 | HCC_GROUP_SHIFT(HCC_GRP_VENDOR_SPECIFIC))
#define HCIPP_I2C_FM_WRITE									(0x0135 | HCC_GROUP_SHIFT(HCC_GRP_VENDOR_SPECIFIC))
#define HCIPP_FM_POWER_MODE									(0x0137 | HCC_GROUP_SHIFT(HCC_GRP_VENDOR_SPECIFIC))
#define HCIPP_FM_SET_AUDIO_PATH								(0x0139 | HCC_GROUP_SHIFT(HCC_GRP_VENDOR_SPECIFIC))
#define HCIPP_FM_CHANGE_I2C_ADDR							(0x013A | HCC_GROUP_SHIFT(HCC_GRP_VENDOR_SPECIFIC))

static void fm_transport_initCallback(const BtEvent *Event);
static void fm_transport_readFlagCallback(const BtEvent *Event);
static void fm_transport_readMaskCallback(const BtEvent *Event);
static void fm_transport_readCallback(const BtEvent *Event);
static void fm_transport_readRdsCallback(const BtEvent *Event);
static FmStatus fm_transport_sendFmCommand(BtCallBack callback, TIFM_U16 hci_opcode, TIFM_U8 len);
static TIFM_BOOL fm_transport_isGeneralParamsValid(const BtEvent *Event, TIFM_U8 *len, TIFM_U8 *status, TIFM_U8* *data);
static FmStatus	TransportConvertErrorToFmError(TiChipMngrStatus Status);
static void fm_transportStatusCallback(TiChipMngrFmNotificationType transNotification);

static TiTransportFmNotificationCb		transportFmNotiCB;

static void fm_transport_initCallback(const BtEvent *Event)
{	
 	TIFM_U8 len, status;
	TIFM_U8 *data;
	TIFM_BOOL isValid;
		
	if(fm_transport_isGeneralParamsValid(Event, &len, &status, &data))
	{
		isValid = TIFM_TRUE;
    }
	else
	{
		isValid = TIFM_FALSE;
	}

	fm_recvd_initCmdCmplt(isValid);
}

static void fm_transport_readFlagCallback(const BtEvent *Event)
{	
	TIFM_U8 len, status;
	TIFM_U8 *data;
	
	if(fm_transport_isGeneralParamsValid(Event, &len, &status, &data))
	{
		TIFM_Assert(len == 6);

		fm_recvd_readFlagCmdCmplt(data);

		TIFM_Report(("MEIF: FM read flag returned command complete event."));

    }
	else
	{
		TIFM_Report(("MEIF: Error in FM read flag.\n"));
	}
}

static void fm_transport_readMaskCallback(const BtEvent *Event)
{	
	TIFM_U8 len, status;
	TIFM_U8 *data;
	
	if(fm_transport_isGeneralParamsValid(Event, &len, &status, &data))
	{
		TIFM_Assert(len == 6);

		fm_recvd_readMaskCmdCmplt(data);

		TIFM_Report(("MEIF: FM read mask returned command complete event."));

    }
	else
	{
		TIFM_Report(("MEIF: Error in FM read mask.\n"));
	}
}

static void fm_transport_readCallback(const BtEvent *Event)
{
	TIFM_U8 len, status;
	TIFM_U8 *data;
	
	if(fm_transport_isGeneralParamsValid(Event, &len, &status, &data))
	{
		TIFM_Assert(len == 6);

		fm_recvd_readCmdCmplt(data);

		TIFM_Report(("MEIF: FM read Command returned command complete event."));
   }
	else
	{
		TIFM_Report(("MEIF: Error in FM read Command.\n"));
	}
}

static void fm_transport_readRdsCallback(const BtEvent *Event)
{	
	TIFM_U8 len, status;
	TIFM_U8 *data;
	
	if(fm_transport_isGeneralParamsValid(Event, &len, &status, &data))
	{

		fm_recvd_readRdsCmdCmplt(len, data);
		
		TIFM_Report(("MEIF: HCI FM read RDS Command returned command complete event."));
    }
	else
	{
		TIFM_Report(("MEIF: Error in FM read RDS Command.\n"));
	}
}

static void fm_transport_writeCommandCallback(const BtEvent *Event)
{	
	TIFM_U8 len, status;
	TIFM_U8 *data;
	
	if(fm_transport_isGeneralParamsValid(Event, &len, &status, &data))
	{		
		TIFM_Assert(len == 4);

		fm_recvd_writeCmdCmplt();

		TIFM_Report(("MEIF: HCI write FM Command returned command complete event."));
		
    }
	else
	{
		TIFM_Report(("MEIF: Error in HCI Write to FM Command.\n"));
	}
}


FmStatus fm_transport_sendWriteCommand(void)
{		
	return fm_transport_sendFmCommand(fm_transport_writeCommandCallback, HCIPP_I2C_FM_WRITE, 5);
}

FmStatus fm_transport_sendPowerModeCommand(void)
{	
	return fm_transport_sendFmCommand(fm_transport_writeCommandCallback, HCIPP_FM_POWER_MODE, 1);
}

FmStatus fm_transport_sendInitCmd(TIFM_U16 hci_opcode, TIFM_U8 len)
{
	return fm_transport_sendFmCommand(fm_transport_initCallback, hci_opcode, len);
}

FmStatus fm_transport_sendReadStatus(void)
{
	return fm_transport_sendFmCommand(fm_transport_readCallback, HCIPP_I2C_FM_READ, 3);
}

FmStatus fm_transport_sendFmReadRds(void)
{
	return fm_transport_sendFmCommand(fm_transport_readRdsCallback, HCIPP_I2C_FM_READ, 3);
}

/* This command is only for reading the flag when an interrupt is received.
   When we only want to clear the flag we will use the regular readStatus command */
FmStatus fm_transport_sendReadInt(TIFM_U8 fm_opcode)
{
	if(fm_opcode == FLAG_GET)
	{
		return fm_transport_sendFmCommand(fm_transport_readFlagCallback, HCIPP_I2C_FM_READ, 3);
	}
	else
	{
		return fm_transport_sendFmCommand(fm_transport_readMaskCallback, HCIPP_I2C_FM_READ, 3);
	}
}


static FmStatus fm_transport_sendFmCommand(BtCallBack callback, TIFM_U16 hci_opcode, TIFM_U8 len)
{
	/* For sending fm commands through the UART */
	static	MeCommandToken	fmCommandOp;
	BtStatus	status;
	
	OS_LockFmStack();

	fmCommandOp.callback = callback;
	fmCommandOp.p.general.in.hciCommand = hci_opcode;
	fmCommandOp.p.general.in.parmLen = len;
	fmCommandOp.p.general.in.parms = FMC(commandParams);
	fmCommandOp.p.general.in.event = HCE_COMMAND_COMPLETE;

	OS_UnlockFmStack();

	status = ME_SendHciCommandSync(&fmCommandOp);
	switch(status) {
	case BT_STATUS_PENDING:
		return	FM_STATUS_PENDING;
	case BT_STATUS_INVALID_TYPE:
		return	FM_STATUS_INVALID_TYPE;
	case BT_STATUS_INVALID_PARM:
		return	FM_STATUS_INVALID_PARM;
	case BT_STATUS_FAILED:
		return	FM_STATUS_FAILED;
	case BT_STATUS_HCI_INIT_ERR:
		return	FM_STATUS_HCI_INIT_ERR;
	default:
		return	FM_STATUS_FAILED;
	}
	
}

/* This function checks whether the event received is valid.
   It also fills the status, len and data parameters received in the event */
static TIFM_BOOL fm_transport_isGeneralParamsValid(const BtEvent *Event, TIFM_U8 *len, TIFM_U8 *status, TIFM_U8* *data)
{
    if ((Event->p.meToken->p.general.out.status != BT_STATUS_SUCCESS) || 
		(Event->p.meToken->p.general.out.event != HCE_COMMAND_COMPLETE))
	{
		return TIFM_FALSE;
	}
	else
	{
		*len = Event->p.meToken->p.general.out.parmLen;
		*status = (TIFM_U8)(Event->p.meToken->p.general.out.parms[3]);
		*data = &(Event->p.meToken->p.general.out.parms[4]);
		return TIFM_TRUE;
	}
}



FmStatus fm_transport_init(TiTransportFmNotificationCb callback)
{
	TiChipMngrStatus 	status;
	
	transportFmNotiCB = callback;
	status =  TI_CHIP_MNGR_Init();
	TI_CHIP_MNGR_RegisterForFMNotifications(fm_transportStatusCallback);
		
	return (TransportConvertErrorToFmError(status));
}


FmStatus fm_transport_on()
{
	TiChipMngrStatus 	status;

	status =  TI_CHIP_MNGR_FMOn();
	return (TransportConvertErrorToFmError(status));
}

FmStatus fm_transport_off()
{
	TiChipMngrStatus 	status;

	status =  TI_CHIP_MNGR_FMOff();
	return (TransportConvertErrorToFmError(status));
}

TiTrasportState fm_transport_getState()
{
	TiChipMngrState status;

	status = TI_CHIP_MNGR_GetState();
	switch(status)
	{
	case TI_CHIP_MNGR_STATE_CHIP_OFF:
		return TI_TRANSPORT_STATE_CHIP_OFF;
	case TI_CHIP_MNGR_STATE_BT_ON:
		return TI_TRANSPORT_STATE_BT_ON;
	case TI_CHIP_MNGR_STATE_FM_ON:
		return TI_TRANSPORT_STATE_FM_ON;
	case TI_CHIP_MNGR_STATE_ALL_ON:
		return TI_TRANSPORT_STATE_ALL_ON;
	default:
		return TI_TRANSPORT_STATE_CHIP_OFF;
	}
}

static void fm_transportStatusCallback(TiChipMngrFmNotificationType transNotification)
{
	if(transNotification == TI_CHIP_MNGR_FM_NOTIFICATION_FM_ON_COMPLETE)
	{
		transportFmNotiCB(TI_TRANSPORT_FM_NOTIFICATION_FM_ON_COMPLETE);
	}
	else if(transNotification == TI_CHIP_MNGR_FM_NOTIFICATION_FM_OFF_COMPLETE)
	{
		transportFmNotiCB(TI_TRANSPORT_FM_NOTIFICATION_FM_OFF_COMPLETE);
	}
	
}

static FmStatus	TransportConvertErrorToFmError(TiChipMngrStatus Status)
{
	switch(Status)
	{
	case TI_CHIP_MNGR_STATUS_SUCCESS:
		return FM_STATUS_SUCCESS;
	case TI_CHIP_MNGR_STATUS_FAILED:
		return FM_STATUS_FAILED;
	case TI_CHIP_MNGR_STATUS_PENDING:
		return FM_STATUS_PENDING;
	case TI_CHIP_MNGR_STATUS_IN_PROGRESS:
		return FM_STATUS_IN_PROGRESS;
	case TI_CHIP_MNGR_STATUS_INTERNAL_ERROR:
		return FM_STATUS_INTERNAL_ERROR;
	default:
		return FM_STATUS_FAILED;
	}
}



#endif /*BTL_CONFIG_FM_STACK == BTL_CONFIG_ENABLED*/




