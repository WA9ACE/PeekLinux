/********************************************************************************
 * btc functions -- Enhanced TestMode (ETM)
 * @file	etm_btc.h (Support for TMM CORE commands)
 *
 * @author	Xi Peng
 * @version 0.1
 *

 *
 * History:
 *
 * 	Date       	Modification
 *  ------------------------------------
 *  06/05/2008	Creation
 *
 *********************************************************************************/

#ifndef _ETM_BTC_H_
#define _ETM_BTC_H_

#define ETM_BTC_VERSION						0
#define ETM_BTC_CHK_SIM_STS					1
#define ETM_BTC_START_CHARGE				2
#define ETM_BTC_STOP_CHARGE					3
#define ETM_BTC_CAL_BACKUP					4
#define ETM_BTC_CAL_RESTORE					5
#define ETM_BTC_CHK_GPRS_STS				6//maoshanshan add 20090206
#define ETM_BTC_RESTART_DEVICE  			7//maoshanshan add 20090212
#define ETM_BTC_GETSERVERIP     			8//maoshanshan add 20090310
#define ETM_BTC_GETPAYMODE 					9//maoshanshan add 20090310
#define ETM_ETC_GETPAYMODE					10//maoshanshan add 20090310
#define ETM_ETC_FLUSHBUFFER					11//maoshanshan add 20090312
#define ETM_ETC_GETALLINTO					12//maoshanshan add 20090320
#define ETM_BTC_SETTOOL_GETVERSION			13//maoshanshan add 20090525

#endif // _ETM_BTC_H_
