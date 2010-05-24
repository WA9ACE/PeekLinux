#ifndef _HWD_ADC_API_H_
#define _HWD_ADC_API_H_


/*----------------------------------------------------------------------------
 Global Definitions
----------------------------------------------------------------------------*/

#define HWD_ADC_TX_OFF         0
#define HWD_ADC_TX_ON          1
#define HWD_ADC_TIMEOUT_ERROR  0xff

/*don't change it*/
#define HWD_ADC_MAX_CHANNELS   16
/*----------------------------------------------------------------------------
 Global Typedefs
----------------------------------------------------------------------------*/
typedef enum
{
    HWD_ADC_0 = 0, 
    HWD_ADC_1,
    HWD_ADC_2,
    HWD_ADC_3,
    HWD_ADC_4,
    HWD_ADC_5,    
    HWD_ADC_6,
    /*Don't change the following channel ID, internal used*/
    HWD_ADC_7,
    HWD_ADC_INT_VBG,
    HWD_ADC_INT_VCM,
    HWD_ADC_TX_VCM,
    HWD_ADC_TX_I_P,
    HWD_ADC_TX_I_N,
    HWD_ADC_TX_Q_P,
    HWD_ADC_TX_Q_N,
    HWD_ADC_MIC_BIAS
} HwdAdcChanIdT;

/* Typedef for CDMA and AMPS meas */
typedef PACKED struct
{
    HwdAdcChanIdT   ChanId;
    uint16          MeasResult;
    uint8           Status;
} HwdAdcMeasResponseMsgT;


typedef PACKED struct
{
    ExeRspMsgT      RspInfo;
    HwdAdcChanIdT   ChanId;
    bool            TxSync;
} HwdAdcMeasRequestMsgT;


/* for AMPS application */
typedef enum
{ 
	GET_RSSI_CHNL = 0,
	GET_TX_PWR_CHNL,
	GET_TEMP_CHNL,
	GET_INT_VBG_CHNL,
	GET_INT_VCM_CHNL,
	GET_INT_MIC_BIAS_CHNL
} HwdAdcGetChnlT;

/*----------------------------------------------------------------------------
 Global API Function Prototypes
----------------------------------------------------------------------------*/

/*****************************************************************************
 
  FUNCTION NAME:    HwdAdcMeasRequest

  DESCRIPTION:      This function is the API for ADC measurement requests 
                    in CDMA mode
                    
  
  PARAMETERS:       Msg -  Pointer to request data

  RETURNED VALUES:  None

*****************************************************************************/
extern void HwdAdcMeasRequest (HwdAdcMeasRequestMsgT *Msg);

/*****************************************************************************
* $Id: hwdadcapi.h 1.1 2007/10/29 10:52:43 binye Exp $

*****************************************************************************/

#endif

