

#ifndef VSMS_FMT_CONV_H
#define VSMS_FMT_CONV_H

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "sysdefs.h"
/*
 *    Transport Layer Message Field Parameter Ids
 */
#define BAL_SMS_TL_TELESERVICE_ID	   0
#define BAL_SMS_TL_SERVICE_CAT	       1
#define BAL_SMS_TL_ORIG_ADDR	       2
#define BAL_SMS_TL_ORIG_SUBADDR	       3
#define BAL_SMS_TL_DEST_ADDR	       4
#define BAL_SMS_TL_DEST_SUBADDR	       5
#define BAL_SMS_TL_BEARER_RPLY_OPT	   6
#define BAL_SMS_TL_CAUSE_CODE	       7
#define BAL_SMS_TL_BEARER_DATA         8

/*
 *    Teleservice Layer Parameter Values
 */
#define BAL_SMS_ASE_MESSAGE_ID	       0
#define BAL_SMS_ASE_USER_DATA	       1
#define BAL_SMS_ASE_USER_RESP_CODE     2
#define BAL_SMS_ASE_TIMESTAMP	       3 
#define BAL_SMS_ASE_VPA		           4
#define BAL_SMS_ASE_VPR		           5
#define BAL_SMS_ASE_DDTA		       6
#define BAL_SMS_ASE_DDTR		       7
#define BAL_SMS_ASE_PRIORITY	       8
#define BAL_SMS_ASE_PRIVACY		       9
#define BAL_SMS_ASE_RPLY_OPT	      10
#define BAL_SMS_ASE_MSGNUM		      11
#define BAL_SMS_ASE_ALERT		      12
#define BAL_SMS_ASE_LANG		      13
#define BAL_SMS_ASE_CALLBACK	      14
#define BAL_SMS_ASE_DISPMODE	      15
#define BAL_SMS_ASE_MENC_DATA	      16
#define BAL_SMS_ASE_MSG_DEP_INDEX     17
#define BAL_SMS_ASE_SRV_CAT_DATA      18
#define BAL_SMS_ASE_SRV_CAT_RESULT    19
#define BAL_SMS_ASE_MSG_STATUS        20

#define NUM_BYTES(bitCount) \
      ((bitCount % 8) ? ((bitCount / 8) + 1):(bitCount / 8))
      
// Convert Mask and Flag to Bool
#define MASK_TO_BOOL(mask, flag) (bool)(((mask) & (flag)) ? TRUE : FALSE)

// Convert Bool to given Flag if TRUE
#define BOOL_TO_MASK(cond, flag) (((cond) == TRUE) ? (flag) : 0x00000000)

/*===========================================================================
                        FUNCTION DEFINITION AREA
============================================================================*/

void BalSmsPdu2Txt( uint8*          BalSmsPduP,
                    uint8           Length,
                    BalSmsMessageT* BalSmsMsgP );
void BalSmsTxt2Pdu( BalSmsMessageT* SmsMsg,
                    uint8*          PduP,
                    uint8*          nlength );

/*==========================================================================
FUNCTION ExtractAddress

DESCRIPTION
  extract the BalSmsAddressT type structure from the pdu
	
DEPENDENCIES
  

RETURN VALUE
  the number of bytes the address field covers in pdu
===========================================================================*/
uint8 BalSmsExtractAddress( uint8* data, BalSmsAddressT* Address );

uint8 BalSmsExtractSubaddress( uint8 *data, BalSmsSubaddressT* addr );

#endif

/*****************************************************************************
* $Log: valsmsfmtconv.h $
* Revision 1.1  2007/10/29 11:01:39  binye
* Initial revision
* Revision 1.1  2007/10/12 13:17:24  lwang
* Initial revision
* Revision 1.1  2007/09/24 14:01:19  binye
* Initial revision
* Revision 1.1  2006/11/26 22:16:11  yliu
* Initial revision
* Revision 1.1  2006/10/24 15:04:40  binye
* Initial revision
* Revision 1.1  2005/11/08 13:15:09  vnarayana
* Initial revision
* Revision 1.1  2005/11/07 15:33:37  wavis
* Initial revision
* Revision 1.1  2005/10/12 15:34:31  dorloff
* Initial revision
* Revision 1.1  2005/03/10 13:16:20  lwang
* Initial revision
* Revision 1.1  2004/08/02 15:52:56  xuhua
* Initial revision
* Revision 1.1  2004/05/26 13:14:41  javese
* Initial revision
* Revision 1.3  2003/11/12 17:47:58  shitong
* Revision 1.2  2003/11/12 15:36:54  york
* Revision 1.1  2003/11/12 10:11:44  shitong
* Initial revision
* Revision 1.2  2003/11/11 17:45:49  york
* merged in
*****************************************************************************/


