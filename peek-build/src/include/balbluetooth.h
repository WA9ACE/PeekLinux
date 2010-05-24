
#ifndef  _BAL_BLUETOOTH_H_
#define  _BAL_BLUETOOTH_H_

#include "sysdefs.h"
#include "balsndapi.h"
#include "balapi.h"

#define BAL_BT_NAME_LEN    32
#define BAL_BT_ADDR_LEN    12
#define BAL_BT_LINKKEY_LEN 32
#define BAL_BT_PIN_LEN     16


#ifdef  __cplusplus
extern "C" {
#endif

/*class of device, currently we only support Audio and phone Major Class
  value is set according to BLUE TOOTH STANDARD*/
typedef enum
{
  BAL_BT_COD_CELLULAR = 0X0204,
  BAL_BT_COD_CORDLESS = 0X0208,
  BAL_BT_COD_SMARTPHONE = 0X020c,
  BAL_BT_COD_WIREDMODEM_VOICEGATEWAY = 0X0210,
  BAL_BT_COD_WEARABLE_HEADSET =0x0404,
  BAL_BT_COD_HANDFREE = 0x0408,
  BAL_BT_COD_MICROPHONE = 0x0410,
  BAL_BT_COD_LOUDSPEAKER = 0x0414,
  BAL_BT_COD_HEDPHONE = 0x0418,
  BAL_BT_COD_PORTALBE_AUDIO = 0x041c,
  BAL_BT_COD_CAR_AUDIO = 0x0420,
  BAL_BT_COD_HIFI_AUDIO = 0x0428,

  BAL_BT_COD_UNKNOWN = 0
}BalBTCODT;

typedef struct
{
  uint8          nameLen;                  /*length of device name*/
  char           devName[BAL_BT_NAME_LEN];
  char           devAddr[BAL_BT_ADDR_LEN];
  BalBTCODT      devType;  

  /*is linkKey valid, UI may use it to justify whether the linkKey can be used to register a device*/
  bool           isValid;                  
  char           linkKey[BAL_BT_LINKKEY_LEN];  
}BalBTDevInfoT;

typedef enum
{
  BAL_BT_STATE_INVISIBLE = 0,
  BAL_BT_STATE_VISIBLE,
  BAL_BT_STATE_MAX
}BalBTDevStateT;

typedef enum
{
  BAL_BT_INIT_RESULT,
  BAL_BT_REGISTER_RESULT,
  BAL_BT_INQUIRY_RESULT,
  BAL_BT_PAIRING_RESULT,
  BAL_BT_CONNECT_RESULT,
  BAL_BT_DISCONNECT_RESULT,
  BAL_BT_VISIBLE_SET_RESULT,
  BAL_BT_VOLUME_SET_RESULT,
  BAL_BT_IDLE_REQUEST_RESULT,
  BAL_BT_RESET_RESULT,
  BAL_BT_VOLUME_LEVEL,
  
  BAL_BT_ABNORMAL_EVENT_RESULT
}BalBTEventT;

typedef enum
{
  BAL_BT_OK = 0,
  BAL_BT_ERR_PARA,
  BAL_BT_ERR_LINKKEY,
  BAL_BT_ERR_BUSY,
  BAL_BT_ERR_PIN,
  BAL_BT_ERR_TIMEUP,
  BAL_BT_ERR_UNKNOWN,

  BAL_BT_ERR_UNSUPPORT_FUNCTION
}BalBTResultT;

/*used by val internal layer , NOT for UI*/
typedef enum
{
  BAL_BT_DEVICE_OK,
  BAL_BT_DEVICE_PIN_WRONG,
  BAL_BT_DEVICE_LINKKEY_WRONG,
  BAL_BT_DEVICE_TIME_UP,
  BAL_BT_DEVICE_UNKNOWN,
  BAL_BT_DEVICE_DISCONNECT_UNKNOWN,
  BAL_BT_DEVICE_UNSUPPORT
}BalBTDevResultT;

/****************************************************************************
*Name
   valBTInit
*Description
   Initialize the resource used by Bluetooth module
*Parameters
   name  -- name of blue-core we want to set
   addr  -- address of blue-core we want to set
*Return value
   BAL_BT_OK: initiation request is ok
   BAL_BT_ERR_BUSY: initiation is not valid
*****************************************************************************/
BalBTResultT  BalBTInit(char *name, char *addr );

/****************************************************************************
*Name
   BalBTInquiry
*Description
   Inquiry all of Bluetooth device
*Parameters
   devNum: num of device ui want to inquiry.
*Return value
   BAL_BT_OK:       ok to inquiry
   BAL_BT_ERR_PARA: para is invalid. Max num we currently support is 8
   BAL_BT_ERR_BUSY: inquiry is not valid in currect state
*****************************************************************************/
BalBTResultT  BalBTInquiry(uint8 devNum);

/****************************************************************************
*Name
   BalBTPairing
*Description
   Pairing with the selected device
*Parameters
   devAddr -- remote device address
   pin     -- pin of the remote device
   length  -- length of pin
*Return value
   BAL_BT_OK:         request is ok, 
   BAL_BT_ERR_BUSY:   in current state, paring request is invalid
   BAL_BT_ERROR_PARA: para is wrong
*****************************************************************************/
BalBTResultT  BalBTPairing(char *devAddr, char *pin, uint8 length);

/****************************************************************************
*Name
   valBTConnect
*Description
   make blue-core gateway connect to remote device
*Parameters
   devAddr -- remote device address
*Return value
   BAL_BT_OK:         request is ok, 
   BAL_BT_ERR_BUSY:   in current state, paring request is invalid
   BAL_BT_ERROR_PARA: para is wrong
*****************************************************************************/
BalBTResultT  BalBTConnect(char *devAddr);

/****************************************************************************
*Name
   valBTDisconnect
*Description
   make blue-core gateway disconnect to remote device
*Parameters
   devAddr -- remote device address
*Return value
   BAL_BT_OK:         request is ok, 
   BAL_BT_ERR_BUSY:   in current state, disconnect request is invalid
   BAL_BT_ERROR_PARA: para is wrong
*****************************************************************************/
BalBTResultT  BalBTDisconnect(char *devAddr);

/****************************************************************************
*Name
   BalBTTrustDevRegister
*Description
   register trust device, so it is not need to Authentication 
*Parameters
   addr -- remote device address
   linkKey -- link key of the remote device
*Return value
   BAL_BT_OK:         request is ok, 
   BAL_BT_ERR_BUSY:   in current state, register request is invalid
   BAL_BT_ERROR_PARA: para is wrong
*****************************************************************************/
BalBTResultT BalBTTrustDevRegister( char *addr, char *linkKey);

/****************************************************************************
*Name
   BalBTFirstDeviceInfoGet
*Description
   get first device information in the link table
*Parameters
   pBuffer -- buffer to accommodate the device information
*Return value
   True - get info successfully
   False - the link table is null
*****************************************************************************/
bool BalBTFirstDeviceInfoGet( BalBTDevInfoT *pBuffer);

/****************************************************************************
*Name
   BalBTNextDeviceInfoGet
*Description
   get next device information in the link table 
*Parameters
   pBuffer -- buffer to accommodate the device information
*Return value
   True - get info successfully
   False - the link table is null or has reached the end of the table
*****************************************************************************/
bool BalBTNextDeviceInfoGet( BalBTDevInfoT *pBuffer);


/****************************************************************************
*Name
   BalBTCtrl
*Description
   Turn on/off the device
*Parameters
   TRUE to turn on the device, FALSE to off
*Return value
   NULL
*****************************************************************************/
void  BalBTCtrl(bool);

/****************************************************************************
*Name
   BalBTVisibleSet
*Description
   Set the cell phone to the visible or invisible state
*Parameters
   state of BT device
*Return value
   BAL_BT_OK:       ok to set
   BAL_BT_ERR_BUSY: state change is not allow in current state
*****************************************************************************/
BalBTResultT BalBTVisibleSet(BalBTDevStateT state);

/*****************************************************************************
*NAME:
   BalBTRegister
*DESCRIPTION:
   Register for bluetooth events. Mostly it is function call result
*PARAMETERS:
   EventFunc - the event handler
*RETURNED VALUES:
   The ID of the registered callback.
*****************************************************************************/
RegIdT BalBTRegister(BalEventFunc EventFunc);
 
/*****************************************************************************
*NAME: 
   BalBTUnRegister
*DESCRIPTION:
 	 Unregister the bluetooth callback.
*PARAMETERS:
   RegId - the ID of the callback to unregister.
*RETURNED VALUES:
   None.
*****************************************************************************/
void BalBTUnRegister(RegIdT RegId);

/****************************************************************************
*Name
   BalBTLinkkeyAdd
*Description
   Store the link key in the val layer, called by CSR bluetooth layer.Not for UI
*Parameters
   devAddr -- remote device address
   linkKey -- link key of the device
*Return value
   True  -- operate successfully
   False -- operate failure
*****************************************************************************/
bool  BalBTLinkkeyAdd( char *devAdd, char *linkKey);

/*****************************************************************************
*NAME: 
   BalBTDeviceInfoAdd
*DESCRIPTION:
 	 call this function to add a ney device to val. it is NOT for UI;
*PARAMETERS:
   addr:    device's address.
   name:    device's name
   devType: type of device
*RETURNED VALUES:
   TRUE:  operate successfully
   FALSE: operate failure.
*****************************************************************************/
bool BalBTDeviceInfoAdd(char *addr, char *name, char *devType);

/*****************************************************************************
*NAME: 
   BalBTResultNotify
*DESCRIPTION:
 	 call this function to notify val that the previous operation result. 
 	 it is NOT for UI;
*PARAMETERS:
   result: CSR result of the last operation
*RETURNED VALUES:
   None
*****************************************************************************/
void BalBTResultNotify(BalBTDevResultT result );

/*****************************************************************************
*NAME: 
   BalBTIdleRequest
*DESCRIPTION:
 	 user can use this funtion to reset blue-core, cancel last operation; blue-core
 	 will return to idle state.
*PARAMETERS:
   None
*RETURNED VALUES:
   None
*****************************************************************************/
BalBTResultT BalBTIdleRequest(void);

/*****************************************************************************
*NAME: 
   BalBTReset
*DESCRIPTION:
 	 reset blue-core, when fatal error occurs, user may use this function to reset
 	 blue-core, it is equal to cold boot
*PARAMETERS:
   None
*RETURNED VALUES:
   None
*****************************************************************************/
void BalBTReset(void);

/*****************************************************************************
*NAME: 
   BalBTDevInfoDelete
*DESCRIPTION:
 	 Delete a device whose address is ADDR in the device table.
*PARAMETERS:
   addr -- address of a device to be deleted
*RETURNED VALUES:
   BAL_BT_OK       -- operate successfully
   BAL_BT_ERR_PARA -- operate failure( addr is null )
*****************************************************************************/
BalBTResultT  BalBTDevInfoDelete( void );

/*****************************************************************************
*NAME: 
   BalBTDevVolumeSet
*DESCRIPTION:
 	 Set remote device volume.
*PARAMETERS:
   VolMode: only BAL_SOUND_MIC_VOLUME and BAL_SOUND_VOICE_VOLUME is valid
   Volume:  self-evident
*RETURNED VALUES:
   BAL_BT_OK       -- operate successfully
   BAL_BT_ERR_PARA -- para is unvalid   
   BAL_BT_ERR_BUSY -- operate failure( invalid request in current state )
*****************************************************************************/
BalBTResultT BalBTDevVolumeSet(BalSoundVolumeModesT VolMode, BalSoundVolumeT Volume);


/****************************************************************************
*Name
   BalBTIncomingCall
*Description
   Notify bluecore call in
*Parameters
   N/A
*Return value
   BAL_BT_OK:         request is ok, 
   BAL_BT_ERR_BUSY:   Connect has not been established
*****************************************************************************/
BalBTResultT  BalBTIncomingCall(void);

/*****************************************************************************
*NAME: 
   BalBTMsgHandler
*DESCRIPTION:
 	 handle some message that isnot send  via call back function
*PARAMETERS:
   None
*RETURNED VALUES:
   None
*****************************************************************************/
void  BalBTMsgHandler( uint32 MsgId, void* MsgDataP, uint32 MsgSize);

#ifdef  __cplusplus
}
#endif

#endif


