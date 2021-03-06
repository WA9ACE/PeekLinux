/*******************************************************************************\
*                                                                           	*
*  TEXAS INSTRUMENTS ISRAEL PROPRIETARY AND CONFIDENTIAL INFORMATION			*
*																				*
*  LEGAL NOTE: THE TRANSFER OF THE TECHNICAL INFORMATION IS BEING MADE			*
*	UNDER AN EXPORT LICENSE ISSUED BY THE ISRAELI GOVERNMENT AND THAT THE		*
*	APPLICABLE EXPORT LICENSE DOES NOT ALLOW THE TECHNICAL INFORMATION TO		*
*	BE USED FOR THE MODIFICATION OF THE BT ENCRYPTION OR THE DEVELOPMENT		*
*	OF ANY NEW ENCRYPTION. THE INFORMATION CAN BE USED FOR THE INTERNAL			*
*	DESIGN AND MANUFACTURE OF TI PRODUCTS THAT WILL CONTAIN THE BT IC.			*
*																				*
\*******************************************************************************/
/*******************************************************************************\
*
*   FILE NAME:      btl_configi.h
*
*   DESCRIPTION:    
*
*		Contains BTIPS INTERNAL configuration parameters that are specific to the BTL
*		layer (as opposed to ESI parameters).
*
*   AUTHOR:         Udi Ron
*
\*******************************************************************************/
#ifndef __BTL_CONFIGI_H
#define __BTL_CONFIGI_H


/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "btl_config.h"
#include "bthal_config.h"

/********************************************************************************
 *
 * Constants
 *
 *******************************************************************************/ 
 
/*
	BTL_CONFIGI_OBEX
*/
#if (	(BTL_CONFIG_BPPSND == BTL_CONFIG_ENABLED) ||	\
	(BTL_CONFIG_FTPC == BTL_CONFIG_ENABLED) ||		\
	(BTL_CONFIG_FTPS == BTL_CONFIG_ENABLED) ||		\
	(BTL_CONFIG_OPP == BTL_CONFIG_ENABLED) ||		\
	(BTL_CONFIG_BIP == BTL_CONFIG_ENABLED) ||		\
	(BTL_CONFIG_PBAPS == BTL_CONFIG_ENABLED))

#define BTL_CONFIGI_OBEX 	BTL_CONFIG_ENABLED
#define BTL_CONFIGI_GOEP 	BTL_CONFIG_ENABLED

#else

#define BTL_CONFIGI_OBEX 	BTL_CONFIG_DISABLED
#define BTL_CONFIGI_GOEP 	BTL_CONFIG_DISABLED

#endif

/*
	BTL_CONFIGI_MDG - Veirfy BTL_CONFIG_SPP ENABLED
*/
#if (	(BTL_CONFIG_SPP == BTL_CONFIG_DISABLED) &&(BTL_CONFIG_MDG == BTL_CONFIG_ENABLED))
#error "BTL_CONFIG_SPP  Must Be Enable To Work With MDG"
#endif






/*
	BTL_CONFIG_RFCOMM
*/

/*
#if ((BTL_CONFIG_BTL_RFCOMM == BTL_CONFIG_ENABLED) || \
	(BTL_CONFIGI_OBEX == BTL_CONFIG_ENABLED))
#endif


#define BTL_CONFIGI_RFCOMM		BTL_CONFIG_ENABLED

#else

#define BTL_CONFIGI_RFCOMM		BTL_CONFIG_DISABLED

#endif
*/

#define BTL_CONFIGI_RFCOMM		BTL_CONFIG_ENABLED


#if ((BTL_CONFIG_AVRCPTG == BTL_CONFIG_ENABLED) || (BTL_CONFIG_A2DP == BTL_CONFIG_ENABLED))

#define BTL_CONFIGI_AVRCP		BTL_CONFIG_ENABLED

#else

#define BTL_CONFIGI_AVRCP		BTL_CONFIG_DISABLED

#endif

#if (	(BTL_CONFIG_A2DP== BTL_CONFIG_ENABLED) ||	\
	(BTL_CONFIG_AVRCPTG == BTL_CONFIG_ENABLED) ||		\
	(BTL_CONFIG_BTL_RFCOMM == BTL_CONFIG_ENABLED) ||		\
	(BTL_CONFIG_AG == BTL_CONFIG_ENABLED) ||		\
	(BTL_CONFIG_SPP == BTL_CONFIG_ENABLED) ||	\
	(BTL_CONFIG_HIDH == BTL_CONFIG_ENABLED))


#define BTL_CONFIGI_CMGR		BTL_CONFIG_ENABLED

#else

#define BTL_CONFIGI_CMGR		BTL_CONFIG_DISABLED

#endif


#if (BTL_CONFIG_A2DP == BTL_CONFIG_ENABLED)

#define BTL_CONFIGI_AVDTP		BTL_CONFIG_ENABLED

#else

#define BTL_CONFIGI_AVDTP		BTL_CONFIG_DISABLED

#endif

/* 
	This is the minimal difference between the scan interval and the scan window 
	For Island 2 this is 6, for Island 3 this is 1. However, the type of the chip is not taken into account
	therefore, 6 is set as the value, to insure that it would work for both chips
*/
#define BTL_CONFIGI_BMG_MIN_DIFF_BETWEEN_INTERVAL_AND_WINDOW				(6)

#if 0
#define BT_DEFAULT_INQ_SCAN_INTERVAL					(BTL_CONFIG_BMG_DFLT_INQ_SCAN_INTERVAL_NC)
#define BT_DEFAULT_INQ_SCAN_WINDOW					(BTL_CONFIG_BMG_DFLT_INQ_SCAN_WINDOW_NC)
#define BT_DEFAULT_PAGE_SCAN_INTERVAL				(BTL_CONFIG_BMG_DFLT_PAGE_SCAN_INTERVAL_NC)
#define BT_DEFAULT_PAGE_SCAN_WINDOW					(BTL_CONFIG_BMG_DFLT_PAGE_SCAN_WINDOW_NC)
#endif

/*
	Setting ESI's default values to 0, (BTL defaults must not be 0) to make sure that when the radio is turned on, 
	the values will be written to the chip (they must be different than ESI's defaults to be written to the chip)
*/
#define BT_DEFAULT_INQ_SCAN_INTERVAL					(BTL_CONFIG_BMG_DFLT_INQ_SCAN_INTERVAL_NC)
#define BT_DEFAULT_INQ_SCAN_WINDOW					(BTL_CONFIG_BMG_DFLT_INQ_SCAN_WINDOW_NC)
#define BT_DEFAULT_PAGE_SCAN_INTERVAL				(BTL_CONFIG_BMG_DFLT_PAGE_SCAN_INTERVAL_NC)
#define BT_DEFAULT_PAGE_SCAN_WINDOW					(BTL_CONFIG_BMG_DFLT_PAGE_SCAN_WINDOW_NC)

/*
 *	Default Link Policy Values
 */
/*
*/#define BTL_CONFIG_BMG_DFLT_IN_ALLOW_MASTER_SLAVE_SWITCH					BTL_CONFIG_ENABLED

/*
*/
#define BTL_CONFIG_BMG_DFLT_IN_ALLOW_HOLD									BTL_CONFIG_ENABLED

/*
*/
#define BTL_CONFIG_BMG_DFLT_IN_ALLOW_PARK									BTL_CONFIG_ENABLED

/*
*/
#define BTL_CONFIG_BMG_DFLT_IN_ALLOW_SNIFF									BTL_CONFIG_ENABLED


/*
*/
#define BTL_CONFIG_BMG_DFLT_OUT_ALLOW_MASTER_SLAVE_SWITCH				BTL_CONFIG_ENABLED

/*
*/
#define BTL_CONFIG_BMG_DFLT_OUT_ALLOW_HOLD									BTL_CONFIG_ENABLED

/*
*/
#define BTL_CONFIG_BMG_DFLT_OUT_ALLOW_PARK									BTL_CONFIG_ENABLED

/*
*/
#define BTL_CONFIG_BMG_DFLT_OUT_ALLOW_SNIFF								BTL_CONFIG_ENABLED

/*
	
*/
#define BTL_CONFIG_BMG_DEFAULT_PREFERRED_CONNECTION_ROLE								(BCR_ANY)


/*
*/
#define BTL_CONFIG_BMG_MAX_NUM_OF_REQUESTS_IN_PARALLEL_IN_ADDITION_TO_SEARCH		(20)


/* Number of HCI commands that could queued per active connection */
#define BTL_CONFIG_HID_MAX_HCI_COMMAND_PER_CHANNEL_ID								(2)


#define BTL_CONFIGI_INIT_MODULE_DECLARATION(a) extern BOOL a##_Init(void); 

#define BTL_CONFIGI_INIT_BTL_MODULE_DECLARATION(a)	\
			extern BtStatus BTL_##a##_Init(void);			\
			extern BtStatus BTL_##a##_Deinit(void);

#define BTL_CONFIGI_INIT_MODULE(a) a##_Init() &&

#define BTL_CONFIGI_INIT_DECLARATIONS		\
								BTL_CONFIGI_INIT_MODULE_DECLARATION(CMGR) \
								BTL_CONFIGI_INIT_MODULE_DECLARATION(OBEX) \
								BTL_CONFIGI_INIT_MODULE_DECLARATION(GOEP) \
								BTL_CONFIGI_INIT_MODULE_DECLARATION(SPP) \
								BTL_CONFIGI_INIT_MODULE_DECLARATION(OPUSH) \
								BTL_CONFIGI_INIT_MODULE_DECLARATION(BPP) \
								BTL_CONFIGI_INIT_MODULE_DECLARATION(BIP) \
								BTL_CONFIGI_INIT_MODULE_DECLARATION(PBAP) \
								BTL_CONFIGI_INIT_MODULE_DECLARATION(FTP) \
								BTL_CONFIGI_INIT_MODULE_DECLARATION(HFG) \
								BTL_CONFIGI_INIT_MODULE_DECLARATION(HSG) \
								BTL_CONFIGI_INIT_MODULE_DECLARATION(AVRCP) \
								BTL_CONFIGI_INIT_MODULE_DECLARATION(AVDTP) \
								BTL_CONFIGI_INIT_MODULE_DECLARATION(A2DP) \
								BTL_CONFIGI_INIT_MODULE_DECLARATION(HID) \
								BTL_CONFIGI_INIT_MODULE_DECLARATION(SIM)


#define BTL_CONFIGI_BTL_INIT_DECLARATIONS		\
								BTL_CONFIGI_INIT_BTL_MODULE_DECLARATION(BMG) \
								BTL_CONFIGI_INIT_BTL_MODULE_DECLARATION(SPP) \
								BTL_CONFIGI_INIT_BTL_MODULE_DECLARATION(OPPC) \
								BTL_CONFIGI_INIT_BTL_MODULE_DECLARATION(OPPS) \
								BTL_CONFIGI_INIT_BTL_MODULE_DECLARATION(BPPSND) \
								BTL_CONFIGI_INIT_BTL_MODULE_DECLARATION(BIPINT) \
								BTL_CONFIGI_INIT_BTL_MODULE_DECLARATION(BIPRSP) \
								BTL_CONFIGI_INIT_BTL_MODULE_DECLARATION(PBAPS) \
								BTL_CONFIGI_INIT_BTL_MODULE_DECLARATION(AVRCPTG) \
								BTL_CONFIGI_INIT_BTL_MODULE_DECLARATION(FTPS) \
								BTL_CONFIGI_INIT_BTL_MODULE_DECLARATION(FTPC) \
								BTL_CONFIGI_INIT_BTL_MODULE_DECLARATION(AG) \
								BTL_CONFIGI_INIT_BTL_MODULE_DECLARATION(RFCOMM) \
								BTL_CONFIGI_INIT_BTL_MODULE_DECLARATION(L2CAP) \
								BTL_CONFIGI_INIT_BTL_MODULE_DECLARATION(A2DP) \
								BTL_CONFIGI_INIT_BTL_MODULE_DECLARATION(HIDH) \
								BTL_CONFIGI_INIT_BTL_MODULE_DECLARATION(SAPS) \
								BTL_CONFIGI_INIT_BTL_MODULE_DECLARATION(MDG) \
								BTL_CONFIGI_INIT_BTL_MODULE_DECLARATION(VG) \
								BTL_CONFIGI_INIT_BTL_MODULE_DECLARATION(BSC)


#define BTL_CONFIGI_INIT_LIST	\
								BTHAL_TRUE &&					\
								BTL_CONFIGI_INIT_MODULE(CMGR) \
								BTL_CONFIGI_INIT_MODULE(OBEX) \
								BTL_CONFIGI_INIT_MODULE(GOEP) \
								BTL_CONFIGI_INIT_MODULE(SPP) \
								BTL_CONFIGI_INIT_MODULE(OPUSH) \
								BTL_CONFIGI_INIT_MODULE(BPP) \
								BTL_CONFIGI_INIT_MODULE(PBAP) \
								BTL_CONFIGI_INIT_MODULE(FTP) \
								BTL_CONFIGI_INIT_MODULE(HFG) \
								BTL_CONFIGI_INIT_MODULE(HSG) \
								BTL_CONFIGI_INIT_MODULE(AVDTP) \
								BTL_CONFIGI_INIT_MODULE(A2DP) \
								BTL_CONFIGI_INIT_MODULE(AVRCP) \
								BTL_CONFIGI_INIT_MODULE(HID) \
								BTL_CONFIGI_INIT_MODULE(SIM) \
								BTHAL_TRUE

#define BTL_CONFIGI_UNLOAD_LIST

#if 0
									BTL_CONFIGI_SPP_DEINIT \
									BTL_CONFIGI_OPP_DEINIT \
									BTL_CONFIGI_BPPSND_DEINIT \
									BTL_CONFIGI_PBAP_DEINIT \
									BTL_CONFIGI_AVRCP_DEINIT \
									BTL_CONFIGI_FTP_DEINIT \
									BTL_CONFIGI_HF_DEINIT \
									BTL_CONFIGI_HS_DEINIT \
									BTL_CONFIGI_GOEP_DEINIT \
									BTL_CONFIGI_OBEX_DEINIT \
									BTL_CONFIGI_RFCOMM_DEINIT
#endif			


#endif /* _BTL_CONFIGI_H */


