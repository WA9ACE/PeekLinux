/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         $Workfile:: mfw_sat.c       $|
| $Author:: Xsp $ CONDAT GmbH           $Revision:: 12              $|
| CREATED: 07.05.99                     $Modtime:: 23/02/00 20:23   $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

   MODULE  : MFW_SAT

   PURPOSE : SIM Application Toolkit Handling

   EXPORT  :

   TO DO   :

   $History:: mfw_sat.c 
      Apr 18, 2006 Ref:OMAPS00116309 R.Prabakar (a0393213)
      CT-GCF[27.22.4.7.1]- MMI crashes on REFERSH seq 1.2
      Solution : Data type mismatch between ACI and MMI sorted out
      
	Nov 07, 2006 ER:OMAPS00070661 R.Prabakar(a0393213)
	R99 network compliancy : Implemented Language Notification and Provide Local Information (Language setting)
	SAT proactive commands and Language Selection SAT event

	10 Jul 2006, Ref OMAPS00077654, R.Prabakar(a0393213)
  	Description : Cyrillic character support in MMI to pass GCF/PTCRB  UCS2 TC (SATK)
  	Solution     : UCS2 Display added to terminal profile

       May 26, 2006 REF: OMAPS00076094, x0045876
       Description: MMI's Terminal Profile (TP) setup is not in line with the SAT class e implementation
       Solution: TP is updated based on the current support by MMI.

       March 29, 2006 REF: OMAPS00060199, x0045876
       Description: DISPLAY TEXT (Normal) SEQ 1.9 - ME got hanged
       Solution:  If there is no text string supplied in the function decDispText(), only SatResponse was sent saying that
       command data not understood by Me and return statement was missed out.

       Shashi Shekar B.S., a0876501, 16 Mar, 2006, OMAPS00061462
       Icon support for SetupMenu & Select item.
   
   	Mar 11, 2006   REF:DR:OMAPS00061467  x0035544
	Description:   27.22.4.13.3 SET UP CALL (display of icons) fails.
	Solution: SAT icon is supported for the SAT CALL SETUP.

	Jan 16, 2006 DR: OMAPS00061460 - Shashi Shekar B.S.
	Description: SAT Icon support
	Solution : SAT icon support added.

   	Aug 31, 2005  REF: CRR 33619  x0021334
       Description: RE: Notification of SATK File Change
       Solution:	  Added the stub function 'rAT_PercentSIMEF()'.

	July 26, 2005    REF: SPR 29520   x0018858
	Issue: 27.22.4.5 Proactive SIM Command: PLAY TONE fails 
	Solution: The window which used to come up "Please wait" was not getting destroyed.
	Care has been taken to check this.

	Jun 16, 2005    REF: CRR 31543   x0021334
	Issue: SATK: cannot get TR for send ussd command
	Solution: When the user presses "Hang up" key when the SAT USSD session is in progress,  
	              a call is made to function 'sAT_end_ussd()' to end the USSD session.
   
	Aug 16, 2004    REF: CRR 24323   Deepa M.D
	Bug:Clenup of sprintf used for tracing
	Fix:Replace the char buf[]; sprintf (buf, "...", ...); TRACE_EVENT (buf); statements by TRACE_EVENT_PX                                        $

    Jun 08,2006 x0047685
    Changed satChangeNotify() and satRefreshNotification(), added file_info instead of file_id
    due to Locosto REL99 SIM Interface changes.
 *
 * *****************  Version 12  *****************
 * User: Xsp          Date: 24/02/00   Time: 5:42
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * Debugged MmiSimToolkit...
 *
 * *****************  Version 11  *****************
 * User: Fk           Date: 23.02.00   Time: 18:33
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * Comprehension flags provided for default Terminal Response
 *
 * *****************  Version 10  *****************
 * User: Xsp          Date: 16/02/00   Time: 19:22
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * Fixed double allocation in satCreate()...
 *
 * *****************  Version 9  *****************
 * User: Xsp          Date: 14/02/00   Time: 18:11
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * Solved compiler errors...
 *
 * *****************  Version 8  *****************
 * User: Xsp          Date: 14/02/00   Time: 14:43
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * Added window pointers to sat structure...
 *
 * *****************  Version 7  *****************
 * User: Fk           Date: 11.02.00   Time: 15:40
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * New event 'SAT Session  End' derived from empty command
 *
 * *****************  Version 6  *****************
 * User: Fk           Date: 10.02.00   Time: 16:43
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * Filter for Terminal Profile
 *
 * *****************  Version 5  *****************
 * User: Vo           Date: 17.11.99   Time: 10:47
 * Updated in $/GSM/Condat/MS/SRC/MFW
|
| *****************  Version 4  *****************
| User: Le           Date: 1.10.99    Time: 15:28
| Updated in $/GSM/Condat/MS/SRC/MFW
 *
 * *****************  Version 3  *****************
 * User: Es           Date: 9.07.99    Time: 14:32
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 2  *****************
 * User: Es           Date: 8.07.99    Time: 12:04
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 1  *****************
 * User: Es           Date: 6.07.99    Time: 12:42
 * Created in $/GSM/DEV/MS/SRC/MFW
 * SIM application toolkit handler
 * Initial
*/
#define ENTITY_MFW

#include <stdio.h>
#include <string.h>

#if defined (NEW_FRAME)

#include "typedefs.h"
#include "vsi.h"
#include "custom.h"
#include "gsm.h"
#include "tok.h"

#else

#include "STDDEFS.H"
#include "custom.h"
#include "gsm.h"
#include "vsi.h"
#include "tok.h"

#endif

#include "message.h"
#include "prim.h"
#include "aci_cmh.h"
#include "ati_cmd.h"
//#include "aci_cmd.h"
#include "aci.h"
#include "prim.h"
#include "psa.h"
#include "psa_sim.h"
#include "psa_cc.h"
#include "psa_sat.h"

#include "mfw_mfw.h"
#include "mfw_sys.h"
#include "aci_cmh.h"
#include "mfw_win.h"
#include "mfw_sat.h"

#include "mfw_phb.h"
#include "mfw_nm.h"
#include "mfw_sima.h"
#include "mfw_nmi.h"
#include "mfw_sim.h"
#include "mfw_simi.h"
#include "mfw_cphs.h"
#include "mfw_cphsi.h"
#include "mfw_sms.h"
#include "mfw_smsi.h"
#ifdef FF_MMI_R99_SAT_LANG
#include "mfw_lng.h" 	/*OMAPS00070661 R.Prabakar(a0393213)*/
#endif

#include "cus_aci.h"

#ifndef PCM_2_FFS
#include "pcm.h"
#endif



extern char mfwSATLPRefresh;

#define TRM_RSP_LEN         (256)   /*SPR#2121 - DS - Length of Terminal Response buffer */
#define CMD_DETAILS_LEN     (5)    /*SPR#2121 - DS - Length of default command details buffer */

char mfwSatMoAccepted = 0;
static U8 lenResponse;                  /* len of terminal response */
static const  U8 defResponse [] =        /* default response values  */ /*a0393213 compiler warnings removal - storage class made first*/
{
    0x81, 0x03, 0x00, 0x00, 0x00,       /* command details          */
    0x82, 0x02, 0x82, 0x81,             /* device identities        */
    0x83, 0x01, 0x32                    /* result                   */
};
static U8 defCmdDetails [CMD_DETAILS_LEN]; /*SPR#2121 - DS - Default SAT command details */
static U8 satNoResponse = 0;            /* flag: no resp. from MFW  */

static U8 curCR;                        /* current TLV CR flag      */
static U8 curTag;                       /* current TLV tag          */
static U8 curLen;                       /* current TLV length       */
static U8 *curVal;                      /* current TLV value        */

SHORT curSatCallId;

static U8 *curCmd;                      /* current command pointer  */
static S16 curCmdLen;                   /* len of current command   */
static U8 *curData;                     /* data storage /end buffer */
static SatCmd cmd;                      /* temp storage for command */
SatCmd* sat_cmd_saved;              /* SPR#2321 - DS - Copy of SAT cmd details */

static U8 partialCompr;                 /* partial comprehension    */

static U16 eventList;

// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
static U32 curSATCmd = MfwSatCmdNone;
// Shashi Shekar B.S., a0876501, Feb 4, 2006, DR: OMAPS00061462
// Modified the below defn to accomodate changes for SetupMenu / Select Item.
static U8  isAlphaIdIconPresent = FALSE;
static U8  isItemIconPresent = FALSE;
static T_EF_IMG icon_image;
U8 read_image_instance_file = FALSE;
//x0035544 Mar 11, 2006. DR:OMAPS00061467
static U8 setupCall_Executed = FALSE;

#define BW_ICON	0x11
#define COLOR_ICON 0x21

#define MAX_SIM_EF_SIZE 0xFF

/* Assuming 8-bpp, RGB 332 format for destination */

#define RGB_TO_PIXEL(R, G, B, value)                                    \
do                                                                      \
{                                                                       \
    value = ((R) & 0xe0) | (((G) & 0xe0) >> 3) | (((B) & 0xc0) >> 6);   \
} while(0)
#endif

typedef  struct
{
  U8     count;
  USHORT fields[MFW_MAX_NT_SIZE];
} T_MFW_UPDATE_FIELDS;

static T_MFW_UPDATE_FIELDS udNotify;    /* update fields            */
static int                 fuRef;
static BOOL fu_aborted;
static int satCommand (U32 cmd, void *h);
static void satResponse (U8 res);

static const U8 satMfWSuppTPrf[] = {       /* Profile supported by MFW */ /*a0393213 compiler warnings removal - storage class made first and type 'U8' added*/
#ifdef SAT_TP1_PRF_DNL

    (SAT_TP1_PRF_DNL | SAT_TP1_MENU_SEL),

     /* 26-05-2006, x0045876 (OMAPS00076094 - Removed SAT_TP2_UCS2_ENTRY, SAT_TP2_UCS2_DSPL,
        SAT_TP2_DSPL_EXT) */
    /*10 Jul 2006, Ref OMAPS00077654, R.Prabakar(a0393213)
  	 Description : Cyrillic character support in MMI to pass GCF/PTCRB  UCS2 TC (SATK)
  	 Solution     : UCS2 Display added to terminal profile*/
        
    (SAT_TP2_CMD_RES | SAT_TP2_CC | SAT_TP2_MOSMC | SAT_TP2_ALPHA_ID| SAT_TP2_UCS2_DSPL),

    (SAT_TP3_DSPL_TXT | SAT_TP3_GET_INKEY | SAT_TP3_GET_INPUT|
     SAT_TP3_PLAY_TONE | SAT_TP3_REFRESH),

     /* 26-05-2006, x0045876 (OMAPS00076094 - Changed the order of bits) */
    (SAT_TP4_SEL_ITEM | SAT_TP4_SEND_SMS | SAT_TP4_SEND_SS 
    | SAT_TP4_SEND_USSD | SAT_TP4_SETUP_CALL | SAT_TP4_SETUP_MENU),

    (SAT_TP5_EVENT_LIST | SAT_TP5_USER_ACT | SAT_TP5_SCR_AVAIL),
#ifdef FF_MMI_R99_SAT_LANG
    (SAT_TP6_LANG_SEL),  /*OMAPS00070661 R.Prabakar(a0393213) Language Selection Enabled */
#else
    0x0,
#endif

    0x0,
    
    (SAT_TP8_BIN_GET_INKEY | SAT_TP8_IDLE_TXT | SAT_TP8_AI2_SETUP_CALL),

#if defined(FF_WAP)
#ifdef FF_MMI_R99_SAT_LANG
    /*OMAPS00070661 R.Prabakar(a0393213) Language Notification and Local Information(language setting)*/
    (SAT_TP9_SUST_DSPL_TXT|SAT_TP9_LAUNCH_BROWSER | SAT_TP9_PLI_LANG | SAT_TP9_LANG_NOTIFY),
#else
    (SAT_TP9_SUST_DSPL_TXT|SAT_TP9_LAUNCH_BROWSER), 
#endif
#else
#ifdef FF_MMI_R99_SAT_LANG
    (SAT_TP9_SUST_DSPL_TXT | SAT_TP9_PLI_LANG | SAT_TP9_LANG_NOTIFY) ,   
#else
    SAT_TP9_SUST_DSPL_TXT, 
#endif
#endif /*FF_WAP*/

     /* 26-05-2006, x0045876 (OMAPS00076094 - Added Softkeys) */
    0x03, 
    
    0x0,
    
    (SAT_TP12_OPEN_CHANNEL | SAT_TP12_CLOSE_CHANNEL | SAT_TP12_RECEIVE_DATA |
    SAT_TP12_SEND_DATA),
    
    (SAT_TP13_CSD_SUPP_BY_ME | SAT_TP13_GPRS_SUPP_BY_ME)
    
#else

#ifdef FF_2TO1_PS
/* START: 11-05-2006, x0045876 (OMAPS00076094) */
/* 0x09, 0x73, 0x17, 0x37       //       c032 rsa                 */
#ifdef FF_MMI_R99_SAT_LANG
    0x09, 0x7B, 0x97, 0x3F, 0x61, 0x01, 0x00, 0x58, 0x29, 0x03, 0x00, /*0x0F, 0x03*/0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
#else
      0x09, 0x7B, 0x97, 0x3F, 0x61, 0x00, 0x00, 0x58, 0x01, 0x03, 0x00, /*0x0F, 0x03*/0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
#endif
/* END: 11-05-2006, x0045876 (OMAPS00076094) */
#else
    /*0x09, 0x73, 0x17, 0x37          */    /* c032 rsa                 */
#endif

#endif
    };

extern MfwHdr *current_mfw_elem;
//x0035544 added fixes done for CQ33597 by x0021334 on 21-11-2005
// x0021334 : To test SAT session end - CQ33597
/* Handler for  cancel event sent by the MMI sim toolkit */
void Mfw_SAT_DTMF_EndSession(void)
{
	sAT_PercentSATT( CMD_SRC_LCL, SATT_CS_EndSession);
	return;
}



/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : satInit            |
+--------------------------------------------------------------------+

  PURPOSE : initialize SAT handler

*/

MfwRes satInit (U8 *profile, U8 plen)
{
    U8 index;
#ifdef FF_2TO1_PS
    U8 combProfile [SIM_MAX_STK_PRF];
#else
    U8 combProfile [MAX_STK_PRF];
#endif

    TRACE_FUNCTION("satInit()");

    mfwCommand[MfwTypSat] = (MfwCb) satCommand;
    satNoResponse = 0;
    mfwSatMoAccepted = 0;
	mfwSATLPRefresh = 0;	
	eventList= 0;
	fu_aborted = FALSE;
#ifdef FF_2TO1_PS
    plen = MINIMUM(plen, SIM_MAX_STK_PRF);
#else
    plen = MINIMUM(plen, MAX_STK_PRF);
#endif
	sat_cmd_saved = NULL; /* SPR#2321 - DS - Initialise ptr to saved cmd  */
    for (index = 0; index < plen; index++)     // check profile against
    {                                   // MFW capability
        if (index < sizeof(satMfWSuppTPrf))
            combProfile[index] = profile[index] & satMfWSuppTPrf[index];
        else
            combProfile[index] = 0;
    }
    sAT_PercentSATC(CMD_SRC_LCL,plen,combProfile);

    /* install the call back for file change notification */
    fuRef = -1;
	psaSAT_FUNotifyRegister(satRefreshNotification);
    psaSAT_FURegister (satChangeNotify);

    return MfwResOk;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : satExit            |
+--------------------------------------------------------------------+

  PURPOSE : finalize SAT handler

*/

MfwRes satExit (void)
{
    mfwCommand[MfwTypSat] = 0;

    return MfwResOk;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : satCreate          |
+--------------------------------------------------------------------+

  PURPOSE : create SAT control

*/

MfwHnd satCreate (MfwHnd w, MfwEvt e, MfwCb f, SatCmd *c)
{
    MfwSat *sat = (MfwSat*) mfwAlloc(sizeof(MfwSat));
    MfwHdr *hdr = (MfwHdr *)mfwAlloc(sizeof(MfwHdr));
	MfwHdr *insert_status =0;
	
    if (!hdr || !sat)
    {
    	TRACE_ERROR("ERROR: satCreate() Mem Alloc Failed.");
			
	   	if(hdr)
   			mfwFree((U8*)hdr,sizeof(MfwHdr));
   		if(sat)
   			mfwFree((U8*)sat,sizeof(MfwSat));	
   		
	   	return 0;
  	}

    sat->map           = e;
    sat->evt           = 0;
    sat->handler       = f;
    sat->cmd           = c;
    sat->parentWindow  = w;
    sat->currentWindow = NULL;

    hdr->data = sat;
    hdr->type = MfwTypSat;

    insert_status =  mfwInsert(NULL,hdr);
    if(!insert_status)
	{
  		TRACE_ERROR("ERROR: satCreate() Failed to Install Handler. ");
   		mfwFree((U8*)hdr,sizeof(MfwHdr));
   		mfwFree((U8*)sat ,sizeof(MfwSat));
		return 0;
  	}
    return insert_status;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : satDelete          |
+--------------------------------------------------------------------+

  PURPOSE : delete SAT control

*/

MfwRes satDelete (MfwHnd h)
{
    MfwRes res;

    if (!h)
        return MfwResIllHnd;

    res = (mfwRemove(h)) ? MfwResOk : MfwResIllHnd;

    mfwFree(((MfwHdr *) h)->data,sizeof(MfwSat));
    mfwFree(h,sizeof(MfwHdr));

    return res;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : satString          |
+--------------------------------------------------------------------+

  PURPOSE : user string to SIM

  SPR#2121 - DS - Dynamically allocate memory for Terminal Response.

*/

MfwRes satString (SatCmd *cmd, SatTxt *txt, U8 *res, int rlen)
{
    S16 ti;
    U8* trmResponse = NULL;

    TRACE_FUNCTION("satString()");

    /* Allocate memory for Terminal Response buffer */
    trmResponse = (U8*)mfwAlloc(TRM_RSP_LEN);

    if (trmResponse == NULL)
    {
        /* Memory allocation failed */
        TRACE_ERROR("ERROR: Failed to allocate memory for TR buffer");
        return MfwResNoMem;
    }

    /*SPR#2121 - DS - Set up terminal response with default values */
    lenResponse = sizeof(defResponse);  /* prep. terminal response  */
    memcpy(trmResponse,defResponse,lenResponse);

    trmResponse[2] = cmd->number;       /* command number           */
    trmResponse[3] = cmd->type;         /* command type             */
    trmResponse[4] = cmd->qual;         /* command qualifier        */

    ti = sizeof(defResponse) - 2;       /* result length            */
    if(res[1] != SatResAiNoCause)       /* only attach additional information if existent*/
    {
      trmResponse[ti++] = rlen;         /* dito                     */
      while (rlen--)
          trmResponse[ti++] = *res++;   /* result data              */
    }
    else                                /* result ok no additional resultcode attached*/
    {
      trmResponse[ti++] = 0x01;
      trmResponse[ti++] = *res;
    }
    trmResponse[ti++] = 0x8D;           /* text string tag + CR     */
    if (txt->len + 1 > 161)             /* incl. coding scheme byte */
    {
        TRACE_ERROR("Text too long!!!");
        return MfwResErr;
    }
    if (txt->len + 1 > 127)
        trmResponse[ti++] = 0x81;
    trmResponse[ti++] = txt->len + 1;
    trmResponse[ti++] = txt->code;      /* data coding scheme       */
    memcpy(trmResponse+ti,(U8*)txt+txt->text,txt->len);

    sAT_PercentSATR(CMD_SRC_LCL,(S16)(ti+txt->len),trmResponse);

    /* Deallocate memory used for Terminal Response buffer */
    if (trmResponse != NULL)
    {
        mfwFree(trmResponse, TRM_RSP_LEN);
        trmResponse = NULL;
    }

    return MfwResOk;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : satItem            |
+--------------------------------------------------------------------+

  PURPOSE : user selected popup item to SIM

  SPR#2121 - DS - Dynamically allocate memory for Terminal Response.

*/

MfwRes satItem (SatCmd *cmd, U8 itemId, U8 *res, int rlen)
{
    S16 ti;
    U8* trmResponse;

    TRACE_FUNCTION("satItem()");

    /* Allocate memory for Terminal Response buffer */
    trmResponse = (U8*)mfwAlloc(TRM_RSP_LEN);

    if (trmResponse == NULL)
    {
        /* Memory allocation failed */
        TRACE_ERROR("ERROR: Failed to allocate memory for TR buffer");
        return MfwResNoMem;
    }

    /*SPR#2121 - DS - Set up terminal response with default values */
    lenResponse = sizeof(defResponse);  /* prep. terminal response  */
    memcpy(trmResponse,defResponse,lenResponse);

    trmResponse[2] = cmd->number;       /* command number           */
    trmResponse[3] = cmd->type;         /* command type             */
    trmResponse[4] = cmd->qual;         /* command qualifier        */

    ti = sizeof(defResponse) - 2;       /* result length            */
    if(res[1] != SatResAiNoCause)       /* only attach additional information if existent*/
    {
      trmResponse[ti++] = rlen;           /* dito                     */
      while (rlen--)
          trmResponse[ti++] = *res++;     /* result data              */
    }
    else                                  /* result ok no additional resultcode attached*/
    {
      trmResponse[ti++] = 0x01;
      trmResponse[ti++] = *res;
    }
    trmResponse[ti++] = 0x90;           /* item identifier tag + CR */
    trmResponse[ti++] = 1;              /* length                   */
    trmResponse[ti++] = itemId;         /* selected item id         */
    TRACE_EVENT("SATR:satItem");
    sAT_PercentSATR(CMD_SRC_LCL,ti,trmResponse);

    /* Deallocate memory used for Terminal Response buffer */
    if (trmResponse != NULL)
    {
        mfwFree(trmResponse, TRM_RSP_LEN);
        trmResponse = NULL;
    }

    return MfwResOk;
}

#ifdef FF_MMI_R99_SAT_LANG
/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : satLocalInfo            |
+--------------------------------------------------------------------+

  PURPOSE : send terminal response for local info 

  SPR#2121 - DS - Dynamically allocate memory for Terminal Response.

*/

MfwRes satLocalInfo (SatCmd *cmd, U8 *res, int rlen, U8* info, int info_len)
{
    S16 ti;
    U8* trmResponse;

    TRACE_FUNCTION("satLocalInfo()");

    /* Allocate memory for Terminal Response buffer */
    trmResponse = (U8*)mfwAlloc(TRM_RSP_LEN);

    if (trmResponse == NULL)
    {
        /* Memory allocation failed */
        TRACE_ERROR("ERROR: Failed to allocate memory for TR buffer");
        return MfwResNoMem;
    }

    /*SPR#2121 - DS - Set up terminal response with default values */
    lenResponse = sizeof(defResponse);  /* prep. terminal response  */
    memcpy(trmResponse,defResponse,lenResponse);

    trmResponse[2] = cmd->number;       /* command number           */
    trmResponse[3] = cmd->type;         /* command type             */
    trmResponse[4] = cmd->qual;         /* command qualifier        */

    ti = sizeof(defResponse) - 2;       /* result length            */
    if(res[1] != SatResAiNoCause)       /* only attach additional information if existent*/
    {
      trmResponse[ti++] = rlen;           /* dito                     */
      while (rlen--)
          trmResponse[ti++] = *res++;     /* result data              */
    }
    else                                  /* result ok no additional resultcode attached*/
    {
      trmResponse[ti++] = 0x01;
      trmResponse[ti++] = *res;
    }

    if(cmd->qual==0x04) /*local info - lang setting*/
    	{
    		trmResponse[ti++] = 0xAD;           /* item identifier tag + CR */
		trmResponse[ti++] = info_len;              /* length*/
	      while (info_len--)
	          trmResponse[ti++] = *info++;     /* result data              */

    	}
    
   sAT_PercentSATR(CMD_SRC_LCL,ti,trmResponse);

    /* Deallocate memory used for Terminal Response buffer */
    if (trmResponse != NULL)
    {
        mfwFree(trmResponse, TRM_RSP_LEN);
        trmResponse = NULL;
    }

    return MfwResOk;
}
#endif

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : satMenuItem        |
+--------------------------------------------------------------------+

  PURPOSE : user selected menu item to SIM

  SPR#2121 - DS - Dynamically allocate memory for Terminal Response.

*/

MfwRes satMenuItem (U8 itemId, U8 help)
{
    S16 ti = 0;
    U8* trmResponse = NULL;

    TRACE_FUNCTION("satMenuItem()");

    /* Allocate memory for Terminal Response buffer */
    trmResponse = (U8*)mfwAlloc(TRM_RSP_LEN);

    if (trmResponse == NULL)
    {
        /* Memory allocation failed */
        TRACE_ERROR("ERROR: Failed to allocate memory for TR buffer");
        return MfwResNoMem;
    }

    trmResponse[ti++] = 0xD3;           /* menu selection tag       */
    trmResponse[ti++] = 0;              /* length (filled later)    */
    trmResponse[ti++] = 0x82;           /* --device identities--    */
    trmResponse[ti++] = 0x02;           /* length                   */
    trmResponse[ti++] = 0x01;           /* source: Keypad           */
    trmResponse[ti++] = 0x81;           /* destination: SIM         */
    trmResponse[ti++] = 0x90;           /* --item identifier--      */
    trmResponse[ti++] = 0x01;           /* length                   */
    trmResponse[ti++] = itemId;         /* item id                  */
    if (help)
    {
        trmResponse[ti++] = 0x95;       /* --help request--         */
        trmResponse[ti++] = 0x00;       /* length                   */
    }
    trmResponse[1] = ti - 2;            /* length of menu selection */

    sAT_PercentSATE(CMD_SRC_LCL,ti,trmResponse);

    /* Deallocate memory used for Terminal Response buffer */
    if (trmResponse != NULL)
    {
        mfwFree(trmResponse, TRM_RSP_LEN);
        trmResponse = NULL;
    }

    return MfwResOk;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : satEvtDownload     |
+--------------------------------------------------------------------+

  PURPOSE : download actual event to SAT (User activity and Idle Screen available)

  SPR#2121 - DS - Dynamically allocate memory for Terminal Response.

*/

MfwRes satEvtDownload (U8 evt)
{
    S16 ti = 0;
    U8 src;
    U8 evtFlags=0;
    U8* trmResponse = NULL;

    TRACE_FUNCTION("satEvtDownload()");

    /* Allocate memory for Terminal Response buffer */
    trmResponse = (U8*)mfwAlloc(TRM_RSP_LEN);

    if (trmResponse == NULL)
    {
        /* Memory allocation failed */
        TRACE_ERROR("ERROR: Failed to allocate memory for TR buffer");
        return MfwResNoMem;
    }

    switch (evt)
    {
        case SatEvtUserAction:          /* user activity            */
            evtFlags = SatEvtUserActionActivated;       /* source of event          */
            break;
        case SatEvtIdleScreen:          /* idle screen available    */
            evtFlags = SatEvtIdleScreenActivated;        /* source of event          */
            break;
        default:
            TRACE_ERROR("ERROR: Unrecognised evt");
            /* Deallocate memory used for Terminal Response buffer */
            if (trmResponse != NULL)
            {
                mfwFree(trmResponse, TRM_RSP_LEN);
                trmResponse = NULL;
            }
            return MfwResErr;           /* illegal event            */
    }

      	if (!(eventList&evtFlags))	/*Only send registered events */
      {
        TRACE_ERROR("ERROR: evtFlags not registered in eventList");
        
         /* Deallocate memory used for Terminal Response buffer */
        if (trmResponse != NULL)
        {
            mfwFree(trmResponse, TRM_RSP_LEN);
            trmResponse = NULL;
        }
        return MfwResErr;
      }

    switch (evt)
    {
        case SatEvtUserAction:          /* user activity            */
            src = SatDevME;             /* source of event          */
            break;
        case SatEvtIdleScreen:          /* idle screen available    */
            src = SatDevDisplay;        /* source of event          */
            break;
        default:
            TRACE_ERROR("ERROR: Unrecognised evt");
            /* Deallocate memory used for Terminal Response buffer */
            if (trmResponse != NULL)
            {
                mfwFree(trmResponse, TRM_RSP_LEN);
                trmResponse = NULL;
            }
            return MfwResErr;           /* illegal event            */
    }

	eventList&=(~evtFlags);				  /* Deregister event	   */

     

    trmResponse[ti++] = 0xD6;           /* event download tag       */
    trmResponse[ti++] = 0;              /* length (filled later)    */
    trmResponse[ti++] = 0x99;           /* --event list--           */
    trmResponse[ti++] = 0x01;           /* length                   */
    trmResponse[ti++] = evt;            /* actual event             */
    trmResponse[ti++] = 0x82;           /* --device identities--    */
    trmResponse[ti++] = 0x02;           /* length                   */
    trmResponse[ti++] = src;            /* source                   */
    trmResponse[ti++] = 0x81;           /* destination: SIM         */
    trmResponse[1] = ti - 2;            /* length of event download */

    sAT_PercentSATE(CMD_SRC_LCL,ti,trmResponse);

    /* Deallocate memory used for Terminal Response buffer */
    if (trmResponse != NULL)
    {
        mfwFree(trmResponse, TRM_RSP_LEN);
        trmResponse = NULL;
    }
    return MfwResOk;
}

#ifdef FF_MMI_R99_SAT_LANG
/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : satLanguageSelected     |
+--------------------------------------------------------------------+

  PURPOSE : download actual event to SAT (Language Selected)

  SPR#2121 - DS - Dynamically allocate memory for Terminal Response.

*/

MfwRes satLanguageSelected(U8 language)
{
    S16 ti = 0;
    U8* trmResponse = NULL;

    TRACE_FUNCTION("satLanguageSelected");
	
    if (!(eventList&SatEvtLangSelectActivated))	/*Only send registered events */
    {
        TRACE_ERROR("ERROR: language selection not registered in eventList");
        return MfwResErr;
    }
	
    /* Allocate memory for Terminal Response buffer */
    trmResponse = (U8*)mfwAlloc(TRM_RSP_LEN);

    if (trmResponse == NULL)
    {
        /* Memory allocation failed */
        TRACE_ERROR("ERROR: Failed to allocate memory for TR buffer");
        return MfwResNoMem;
    }
    /*Fill up envelope*/  
    trmResponse[ti++] = 0xD6;           /* event download tag       */
    trmResponse[ti++] = 0;              /* length (filled later)    */
    trmResponse[ti++] = 0x19;           /* --event list--           */
    trmResponse[ti++] = 0x01;           /* length                   */
    trmResponse[ti++] = SatEvtLangSelection;    /* actual event             */
    trmResponse[ti++] = 0x82;           /* --device identities--    */
    trmResponse[ti++] = 0x02;           /* length                   */
    trmResponse[ti++] = SatDevME;       /* source                   */
    trmResponse[ti++] = SatDevSIM;           /* destination: SIM         */
    trmResponse[ti++] = 0x2D;		  /* Language tag */	
    trmResponse[ti++] = 0x02;   	  /* Language length*/
    switch(language)
    	{
    	case ENGLISH_LANGUAGE:
		trmResponse[ti++]=0x65; /*'e'*/
		trmResponse[ti++]=0x6E; /*'n'*/
		break;
	case CHINESE_LANGUAGE:
		trmResponse[ti++]=0x7A; /*'z'*/
		trmResponse[ti++]=0x68; /*'h'*/
		break;
	case GERMAN_LANGUAGE:
		trmResponse[ti++]=0x64; /*'d'*/
		trmResponse[ti++]=0x65; /*'e'*/
		break;
	default:
       	    /* Deallocate memory used for Terminal Response buffer */
       	    if (trmResponse != NULL)
       	    {
       	        mfwFree(trmResponse, TRM_RSP_LEN);
       	        trmResponse = NULL;
       	    }
		    return MfwResErr;

		
    	}
    trmResponse[1] = ti - 2;            /* length of event download */

    sAT_PercentSATE(CMD_SRC_LCL,ti,trmResponse);

    /* Deallocate memory used for Terminal Response buffer */
    if (trmResponse != NULL)
    {
        mfwFree(trmResponse, TRM_RSP_LEN);
        trmResponse = NULL;
    }

    return MfwResOk;

}
#endif

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : satEvtRegistered     |
+--------------------------------------------------------------------+

  PURPOSE : Check if evt (User activity and Idle Screen available) has been
  registered by previous SAT Setup Event List command.

  SPR#2121 - DS - Created function.

*/

int satEvtRegistered (int evt)
{
    TRACE_FUNCTION("satEvtRegistered()");
    
    if (eventList & evt)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : satDone            |
+--------------------------------------------------------------------+

  PURPOSE : SIM toolkit command confirmation

  SPR#2121 - DS - Dynamically allocate memory for Terminal Response.

*/

MfwRes satDone (SatCmd *cmd, U8 *res, int rlen)
{
    S16 ti;
    U8 response = *res;
    U8* trmResponse = NULL;   

    if (satNoResponse)                  /* no response, if ...      */
    {
    	    TRACE_EVENT("satDone: no response. Handled by ACI");
	    return MfwResOk;                /* ... notified by ACI      */
    }

    /* Allocate memory for Terminal Response buffer */
    trmResponse = (U8*)mfwAlloc(TRM_RSP_LEN);

    if (trmResponse == NULL)
    {
        /* Memory allocation failed */
        TRACE_ERROR("ERROR: Failed to allocate memory for TR buffer");
        return MfwResNoMem;
    }

    /*SPR#2121 - DS - Set up terminal response with default values */
    lenResponse = sizeof(defResponse);  /* prep. terminal response  */
    memcpy(trmResponse,defResponse,lenResponse);

    trmResponse[2] = cmd->number;       /* command number           */
    trmResponse[3] = cmd->type;         /* command type             */
    trmResponse[4] = cmd->qual;         /* command qualifier        */

    ti = sizeof(defResponse) - 2;       /* result length            */
    if(res[1] != SatResAiNoCause)       /* only attach additional information if existent*/
    {
      trmResponse[ti++] = rlen;           /* dito                     */
      while (rlen--)
          trmResponse[ti++] = *res++;     /* result data              */
    }
    else                                  /* result ok no additional resultcode attached*/
    {
      trmResponse[ti++] = 0x01;
      trmResponse[ti++] = *res;
    }

    switch (response)
    {
        case SatResSuccess:             /* performed successsfully  */
        case SatResPartial:             /* ok,partial comprehension */
        case SatResMissing:             /* ok,missing information   */
        case SatResAdditional:          /* ok,additional EFs read   */
        case SatResNoIcon:              /* ok,no icon displayed     */
        case SatResModified:            /* ok,modified by SIM       */
        case SatResUserAbort:           /* terminated by user       */
        case SatResUserBack:            /* backward move by user    */
        case SatResUserNoResp:          /* no response from user    */
        case SatResUserHelp:            /* help request from user   */
        case SatResUserAbortUSSD:       /* USSD terminated by user  */
        case SatResBusyME:              /* ME temporary failure     */
        case SatResBusyNet:             /* Net temporary failure    */
        case SatResReject:              /* user reject call setup   */
        case SatResCleared:             /* call cleared (user/net)  */
        case SatResTimerConflict:       /* conflict on timer state  */
        case SatResCCTemp:              /* CC by SIM: temp failure  */
        case SatResImpossible:          /* ME cannot do it          */
        case SatResUnknownType:         /* type not understood      */
        case SatResUnknownData:         /* data not understood      */
        case SatResUnknownNumber:       /* number not understood    */
        case SatResSSerror:             /* SS return error          */
        case SatResSMSerror:            /* SMS-RP error             */
        case SatResNoValues:            /* missing required values  */
        case SatResUSSDerror:           /* USSD return error        */
        case SatResMultiCard:           /* Multiple card error      */
        case SatResCCPermanent:         /* CC by SIM: perm. failure */
            break;
        default:
            TRACE_ERROR("ill response code");
            break;
    }

    TRACE_EVENT_P1("SATR:satdone. Response: %x", response);

    sAT_PercentSATR(CMD_SRC_LCL,ti,trmResponse);

    /* Deallocate memory used for Terminal Response buffer */
    if (trmResponse != NULL)
    {
        mfwFree(trmResponse, TRM_RSP_LEN);
        trmResponse = NULL;
    }

    return MfwResOk;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : sigExec            |
+--------------------------------------------------------------------+

  PURPOSE : execute SAT signal

*/

static int sigExec (MfwHdr *curElem, U32 evt, void *data)
{
    MfwSat *sc;

    while (curElem)
    {
        if (curElem->type == MfwTypSat)
        {
            sc = curElem->data;
            if (sc->map & evt)
            {                           /* events match             */
                if (sc->handler)        /* handler valid            */
                {
                    current_mfw_elem = curElem; /* LE 06.06.00      */
                    sc->evt = evt;
                    if (sc->cmd)
                        memcpy(sc->cmd,(U8 *)data,sizeof(SatCmd));
                    if ((*(sc->handler))(evt,sc))
                        return 1;       /* event consumed           */
                }
            }
        }
        curElem = curElem->next;
    }

    return 0;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : satSignal          |
+--------------------------------------------------------------------+

  PURPOSE : SAT event

*/

void satSignal (U32 evt, void *data)
{
 UBYTE temp = dspl_Enable(0);

	TRACE_FUNCTION("satSignal event");
	TRACE_EVENT_P1("satSignal event %d",evt);

    if (mfwSignallingMethod EQ 0)
  	{
        if (mfwFocus)
            if (sigExec(mfwFocus,evt,data))
			{
				dspl_Enable(temp);            
				return;
			}
        if (mfwRoot)
            sigExec(mfwRoot,evt,data);
	}
    else
    {
        MfwHdr* h = 0;

        if (mfwFocus)
            h = mfwFocus;
        if (!h)
            h = mfwRoot;
        while (h)
        {
            if (sigExec(h,evt,data))
			{
				dspl_Enable(temp);            
				return;
			}
            if (h == mfwRoot)
			{
				dspl_Enable(temp);            
				return;
			}
            h = mfwParent(mfwParent(h));
		    if (h)
			    h = ((MfwWin *)(h->data))->elems;
        }
        sigExec (mfwRoot,evt,data);
    }

	dspl_Enable(temp);            
	return;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : satCommand         |
+--------------------------------------------------------------------+

  PURPOSE : handle mfw windows command

*/

static int satCommand (U32 cmd, void *h)
{
    switch (cmd)
    {
        case MfwCmdDelete:              /* delete me                */
            if (!h)
                return 0;
            satDelete(h);
            return 1;
        default:
            break;
    }

    return 0;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : satResponse        |
+--------------------------------------------------------------------+

  PURPOSE : setup Terminal Response for SIM

  SPR#2121 - DS - Dynamically allocate memory for Terminal Response.

*/

static void satResponse (U8 res)
{
    U8* trmResponse = NULL;
    
    TRACE_FUNCTION("satResponse()");

    if (satNoResponse)                  /* no response, if ...      */
        return;                         /* ... notified by ACI      */

    /* Allocate memory for Terminal Response buffer */
    trmResponse = (U8*)mfwAlloc(TRM_RSP_LEN);

    if (trmResponse == NULL)
    {
        /* Memory allocation failed */
        TRACE_ERROR("ERROR: Failed to allocate memory for TR buffer");
        return;
    }

    /*SPR#2121 - DS - Set up terminal response with default values */
    lenResponse = sizeof(defResponse);  /* prep. terminal response  */
    memcpy(trmResponse,defResponse,lenResponse);

    /*SPR#2121 - DS - Set up command specific details. defCmdDetails set up in rAT_PercentSATI */
    memcpy(trmResponse, defCmdDetails, CMD_DETAILS_LEN);

    trmResponse[10] = 1;
    trmResponse[11] = res;
    sAT_PercentSATR(CMD_SRC_LCL,sizeof(defResponse),trmResponse);

    /* Deallocate memory used for Terminal Response buffer */
    if (trmResponse != NULL)
    {
        mfwFree(trmResponse, TRM_RSP_LEN);
        trmResponse = NULL;
    }

    return;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : getTLValue         |
+--------------------------------------------------------------------+

  PURPOSE : check TLV:
            setup curCR, curTag, curLen
            return size of TL part

*/

static U8 getTLValue (U8 *sc)
{
    TRACE_FUNCTION("getTLValue()");

    curCR = *(sc+0) & 0x80;             /* comprehension required ? */
    curTag = *(sc+0) & 0x7f;            /* TLV id                   */
    curLen = *(sc+1);                   /* length of TLV value part */

    if (curLen < 0x80)
        return 2;
    if (curLen == 0x81)
    {
        curLen = *(sc+2);               /* long length              */
        return 3;
    }
    TRACE_ERROR("invalid length coding");
    satResponse(SatResUnknownData);

    return 0;                           /* invalid TLV object       */
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : decTLV             |
+--------------------------------------------------------------------+

  PURPOSE : decode TLV

*/

static U8 decTLV (U8 tagWanted, U8 mandatory, char *id)
{
    U8 *sc = curCmd;
    S16 len = curCmdLen;
    U8 tl, partial = 0;

    TRACE_EVENT_P3("decTLV(%02x,%d,%s)",tagWanted,mandatory,id);
	
    while (len > 0)
    {
        tl = getTLValue(sc);
        if (!tl)
            return 0;
        if (curTag == tagWanted)
        {
            sc += tl;
            curVal = sc;
            curCmd = sc + curLen;
            curCmdLen = len - (tl + curLen);
	     TRACE_EVENT_P1("decTLV(): %d",partialCompr);
            partialCompr += partial;
            return 2;
        }
        TRACE_EVENT_P1("no %s TLV",id);

        if (curCR)
        {
            if (mandatory)
                satResponse(SatResUnknownData);
            return !mandatory;
        }
        sc += tl + curLen;
        len -= tl + curLen;
        partial = 1;
    }
    if(!strcmp(id,"Tone"))
    {
      /*
       *  according to GSM 11.14 if no tone is included play general beep
       */
       //July 26, 2005    REF: SPR 29520   x0018858
       //Value modified from integer to hex.
		 *curVal = 0x10;
	  
      return 2;
    }
    if(!strcmp(id,"Duration"))
    {
      /*
       *  according to GSM 11.14 if no duration is included set to a default value
       */
      *(curVal) = 1;  /* Units = sec  */
      *(curVal+1) = 5;  /* play 5 seconds ??*/
      return 2;
    }

    TRACE_EVENT_P1("not found: %s TLV",id);

    if (mandatory)
        satResponse(SatResNoValues);

    return !mandatory;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : decByte            |
+--------------------------------------------------------------------+

  PURPOSE : decode TLV: miscelleaneous TLVs (single byte value)

*/

static U8 decByte (U8 *t, U8 tagWanted, U8 mandatory)
{
    U8 res;

    TRACE_FUNCTION("decByte()");

    res = decTLV(tagWanted,mandatory,"byte");

    if (res > 1)
    {
        *t = *curVal;
        return 1;
    }

    return res;
}
#ifdef FF_MMI_R99_SAT_LANG
/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : decLang            |
+--------------------------------------------------------------------+

  PURPOSE : decode TLV: miscelleaneous TLVs (language)

*/

static U8 decLang (SatLanguageNotification*t, U8 tagWanted, U8 mandatory)
{
    U8 res;

    TRACE_FUNCTION("decLang()");

    res = decTLV(tagWanted,mandatory,"language");

    if (res > 1)
    {
       t->lang[0] = curVal[0];
	 t->lang[1]=curVal[1];
       return 1;
    }
    return res;
}
#endif
/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : decData            |
+--------------------------------------------------------------------+

  PURPOSE : decode TLV: miscelleaneous TLVs (length, data)

*/

static U8 decData (SatData *t, U8 tagWanted, U8 mandatory)
{
    U8 res;

    TRACE_FUNCTION("decData()");

    res = decTLV(tagWanted,mandatory,"data");

    if (res > 1)
    {
        t->len = curLen;                /* data len                 */
        curData -= curLen;
        memcpy(curData,curVal,curLen);  /* data itself              */
        t->data = (U16) (curData - (U8*) t); /* save the offset     */
        return 1;
    }

    return res;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : decText            |
+--------------------------------------------------------------------+

  PURPOSE : decode TLV: text string

*/

static U8 decText (SatTxt *t, U8 tagWanted, U8 mandatory)
{
    U8 res;

    TRACE_FUNCTION("decText()");

    res = decTLV(tagWanted,mandatory,"text");

    TRACE_EVENT_P1("res %d", res);

    if (res > 1)
    {
        if (curLen == 0x00) /* SPR#2340 - DS - Text len zero */
            t->len = 0;
        else
            t->len = curLen - 1;            /* text len (not dcs)       */
        
        if (t->len)
            t->code = *curVal;          /* data coding scheme       */
        else
            t->code = 0;
        curData -= t->len;
        memcpy(curData,curVal+1,t->len); /* text itself             */
        t->text = (U16) (curData - (U8*) t); /* save the offset     */

        TRACE_EVENT_P2("len %d, code(dcs) %d", t->len, t->code);
        
        return 1;      
    }

    return res;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : decAddrSS          |
+--------------------------------------------------------------------+

  PURPOSE : decode TLV: address or SS string

*/

static U8 decAddrSS (SatAddress *t, U8 tagWanted, U8 mandatory)
{
    U8 res;

    TRACE_FUNCTION("decAddr-SS()");

    res = decTLV(tagWanted,mandatory,"Addr-SS");

    if (res > 1)                        /* address or ss string     */
    {
        t->len = curLen - 1;            /* number len wo ton/npi    */
        curData -= t->len;
        t->ton = *curVal;               /* type of number, npi      */
        memcpy(curData,curVal+1,t->len); /* number itself           */
        t->number = (U16) (curData - (U8*) t); /* offset            */
        return 1;
    }

    return res;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : decAlpha           |
+--------------------------------------------------------------------+

  PURPOSE : decode TLV: alpha identifier

*/

static U8 decAlpha (SatTxt *t, U8 mandatory)
{
    U8 res;

    TRACE_FUNCTION("decAlpha()");

    res = decTLV(0x05,mandatory,"Alpha");

    /* SPR#2340 - DS - Added check of val returned from decTLV */
    if (res > 1)
    {
        t->len = curLen;                /* text len                 */
        if (t->len)
            t->code = 0x04;             /* data coding scheme       */
        else
            t->code = 0;
        curData -= t->len;
        memcpy(curData,curVal,t->len);  /* text itself              */
        t->text = (U16) (curData - (U8*) t); /* save the offset     */
        return 1;
    }
    else
    {
        /* Alpha tag not present */
        t->len = 0x00;
        t->code = 0x00;
        t->text = 0x00;
    }

    return res;
}



/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : decSMS             |
+--------------------------------------------------------------------+

  PURPOSE : decode TLV: SMS TPDU

*/

static U8 decSMS (SatSmsPdu *t, U8 mandatory)
{
    U8 res;

    TRACE_FUNCTION("decSMS()");

    res = decTLV(0x0B,mandatory,"SMS TPDU");

    if (res > 1)
    {
        t->len = curLen;                /* SMS len                  */
        curData -= t->len;
        memcpy(curData,curVal,t->len);  /* SMS itself               */
        t->data = (U16) (curData - (U8*) t); /* save the offset     */
        return 1;
    }

    return res;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : decItem            |
+--------------------------------------------------------------------+

  PURPOSE : decode TLV: item

*/

static U8 decItem (SatItem *t, U8 mandatory)
{
    U8 res;

    TRACE_FUNCTION("decItem()");

    res = decTLV(0x0F,mandatory,"Item");

    if (res > 1)
    {
        if (!curLen)                    /* no item: remove menu     */
        {
            t->id = 0;                  /* no id                    */
            t->len = 0;                 /* no length                */
        }
        else
        {
            t->id = *curVal;
            t->len = curLen - 1;        /* without item id          */
        }
        t->action = 0;                  /* filled in later          */
        t->icon = 0;                    /* filled in later          */
        curData -= t->len;
        memcpy(curData,curVal+1,t->len); /* item data               */
        t->text = (U16) (curData - (U8*) t); /* save the offset     */
        return 1;
    }

    return res;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : decNextAction      |
+--------------------------------------------------------------------+

  PURPOSE : decode TLV: next action

*/

static U8 decNextAction (SatItem *t, U8 nItems, U8 mandatory)
{
    U8 res;

    TRACE_FUNCTION("decNextAction()");

    res = decTLV(0x18,mandatory,"Next Action");

    if (res > 1)
    {
        if (nItems > curLen)
            nItems = curLen;
        while (nItems--)
        {
            t->action = *curVal++;      /* next action identifier   */
            t++;
        }
        return 1;
    }

    return res;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : decItemIcons       |
+--------------------------------------------------------------------+

  PURPOSE : decode TLV: icon identifier list

*/

static U8 decItemIcons (SatMenu *m, SatItem *t, U8 nItems, U8 mandatory)
{
    U8 res;

    TRACE_FUNCTION("decItemIcons()");

    res = decTLV(0x1F,mandatory,"Item Icon List");

    m->itemIconQual = 0xFF;             /* default: not present     */
    if (res > 1)
    {
        m->itemIconQual = *curVal++;
        curLen--;
        if (nItems > curLen)
            nItems = curLen;
        while (nItems--)
        {
            t->icon = *curVal++;        /* next action identifier   */
            t++;
        }
        return 1;
    }

    return res;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : decDefItem         |
+--------------------------------------------------------------------+

  PURPOSE : decode TLV: default item

*/

static U8 decDefItem (SatItem *t, U8 nItems, U8 mandatory)
{
    U8 res;

    TRACE_FUNCTION("decDefItem()");

    res = decTLV(0x10,mandatory,"Default Item");

    if (res > 1)
    {
        if (curLen != 1)
            return res;                 /* ES!! error               */
        while (nItems--)
        {
            if (t->id == *curVal)
                t->action |= 0x40;      /* flag as default          */
            t++;
        }
        return 1;
    }

    return res;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : decTone            |
+--------------------------------------------------------------------+

  PURPOSE : decode TLV: tone

*/

static U8 decTone (U8 mandatory)
{
    U8 res;

    TRACE_FUNCTION("decTone()");

    res = decTLV(0x0E,mandatory,"Tone");

    if (res > 1)
    {
    	switch (*curVal)
    	{
		case 0x01:	/* Dial Tone */
		case 0x02:	/* Called Subscriber Busy */
		case 0x03:	/* Congestion */
		case 0x04:	/* Radio Path Ack */
		case 0x05:	/* Radio Path Not Avail */
		case 0x06:	/* Error / Special Info */
		case 0x07:	/* Call Waiting */
		case 0x08:	/* Ringing */
		case 0x10:	/* General Beep */
		case 0x11:	/* Positive Ack */
		case 0x12:	/* Negative Ack or Error */
        		cmd.c.tone.tone = *curVal;      /* tone type                */
        		return 1;

		default :
			satResponse(SatResUnknownData);
			return 0;
    	}
    }

    return res;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : decDuration        |
+--------------------------------------------------------------------+

  PURPOSE : decode TLV: duration

*/
//Sandip CQ-16299
//Reason : This function does not take care of reserved Time Interval 0x00
#if 0
static U8 decDuration (U8 *dur, U8 mandatory)
{
    U8 res;

    TRACE_FUNCTION("decDuration()");

    res = decTLV(0x04,mandatory,"Duration");

    if (res > 1)
    {
        *dur++ = *curVal++;             /* duration unit            */
        *dur++ = *curVal;               /* duration value           */
        return 1;
    }

    return res;
}

#endif

static U8 decDuration (U8 *dur, U8 mandatory)
{
    U8 res;

    TRACE_FUNCTION("decDuration()");

    res = decTLV(0x04,mandatory,"Duration");

    if (res > 1)
    {
	 *dur++ = *curVal++;             /* duration unit            */
	  
        *dur++ = *curVal;               /* duration value           */
	 if(*curVal ==0x00)
	 {
	 	TRACE_EVENT_P1("Duration value is %d",*curVal);
		
		satResponse(SatResUnknownData);
		return 0;
	 }
        return 1;
    }
    return res;
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : decDevId           |
+--------------------------------------------------------------------+

  PURPOSE : decode TLV: device identities

*/

static U8 decDevId (U8 mandatory)
{
    U8 res;

    TRACE_FUNCTION("decDevId()");

    res = decTLV(0x02,mandatory,"Devices");

    if (res > 1)
    {
        cmd.source = *curVal++;         /* source identifier        */
        cmd.dest = *curVal;             /* destination identifier   */
        return 1;
    }

    return res;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : decRespLen         |
+--------------------------------------------------------------------+

  PURPOSE : decode TLV: response length

*/

static U8 decRespLen (U8 mandatory)
{
    U8 res;

    TRACE_FUNCTION("decRespLen()");

    res = decTLV(0x11,mandatory,"Response Length");

    if (res > 1)
    {
        cmd.c.inp.rspMin = *curVal++;   /* minimal length           */
        cmd.c.inp.rspMax = *curVal;     /* maximal length           */
        /* MMI-SPR-16296: [mdf] verify the parameters as expected by GSM 11.14 */
        TRACE_EVENT_P2("decRespLen: Min=%d Max=%d",cmd.c.inp.rspMin,cmd.c.inp.rspMax);
        if ( cmd.c.inp.rspMin > cmd.c.inp.rspMax )
        {
          satResponse(SatResUnknownData); /* send TR(32), prms not understood  */ 
          return 0; 
        }
        /* end of MMI-SPR-16296: [mdf] verify the parameters as expected by GSM 11.14 */
        
        return 1;
    }

    return res;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : decIconId          |
+--------------------------------------------------------------------+

  PURPOSE : decode TLV: icon identifier

*/

static U8 decIconId (SatIconId *id, U8 mandatory)
{
    U8 res;

    TRACE_FUNCTION("decIconId()");

    res = decTLV(0x1E,mandatory,"IconID");

    if (res > 1)
    {
        id->qual = *curVal++;           /* icon qualifier           */
        id->id = *curVal;               /* icon identifier          */
// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
		isAlphaIdIconPresent = TRUE;
#endif
        return 1;
    }
    else
    {
        id->qual = 0xFF;                /* not present              */
        id->id = 0xFF;
// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
		isAlphaIdIconPresent = FALSE;
#endif
     }

    return res;
}

// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : simReadIconData        |
+--------------------------------------------------------------------+

  PURPOSE : Read SIM EF(IMG).

*/
void simReadIconData (U8 record_number)
{
	T_SIM_ACCESS_PARAM sim_read_param;

       TRACE_FUNCTION("simReadIconData()");

	/* Check if the SIM card supports EF(IMG) & read the contents. 
	   Ideally we should be using SRV_RFU3 which corresponds to number 39
	   but, the calls from MMI are using the number directly */
	if (sim_serv_table_check(39)
		== ALLOCATED_AND_ACTIVATED)
	{
		sim_read_param.data_id = SIM_IMG;
		sim_read_param.offset  = 0;
		sim_read_param.record_number = record_number;
		sim_read_param.len = MAX_SIM_EF_SIZE;
		sim_read_param.max_length = MAX_SIM_EF_SIZE;

		sim_read_sim_icon(&sim_read_param);
	}
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : sim_img_cnf        |
+--------------------------------------------------------------------+

  PURPOSE : SIM EF(IMG) read confirm.

*/

void sim_img_cnf (USHORT error, UBYTE record_number, UBYTE *efimg)
{
    USHORT simErr; /*a0393213 compiler warnings removal - variable,sim_read_param count removed*/



    TRACE_FUNCTION("sim_img_cnf()");

#ifdef FF_2TO1_PS
    simErr = CAUSE_SIM_NO_ERROR;
#else
    simErr = SIM_NO_ERROR;
#endif

    if (error == simErr)
	{
		if (*efimg >= 1)
		{
			/* Read the contents of the image file. */
			icon_image.number_of_image_instance = *efimg;
			efimg++;

			icon_image.image_instance.image_width  = *efimg;
			efimg++;
			icon_image.image_instance.image_height = *efimg;
			efimg++;
			icon_image.image_instance.image_coding = *efimg;
			efimg++;

			/* Image instance file identifier*/
			icon_image.image_instance.image_instance_file_id = *efimg << 8;
			efimg++;
			icon_image.image_instance.image_instance_file_id |= *efimg;
			efimg++;	

			/* Offset to Image instance file identifier*/
			icon_image.image_instance.image_offset = *efimg << 8;
			efimg++;
			icon_image.image_instance.image_offset |= *efimg;
			efimg++;

			/* Lenght of the Image instance file identifier. 
			Do not completely depend on this length as this doesn't include the Palette length, 
			so Color image decoding goes wrong!!!*/
			icon_image.image_instance.image_instance_length = *efimg << 8;
			efimg++;
			icon_image.image_instance.image_instance_length |= *efimg;
			efimg++;
			
			read_image_instance_file = TRUE;

			sim_read_sim(icon_image.image_instance.image_instance_file_id, MAX_SIM_EF_SIZE, MAX_SIM_EF_SIZE);

		}

	}
}

/*
+-----------------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT					  |
| STATE   : code                        ROUTINE : sim_img_instance_data_cnf   |
+-----------------------------------------------------------------------------+

  PURPOSE : SIM EF(ImageInst) read confirm & image converter.

*/

void sim_img_instance_data_cnf (USHORT error, UBYTE *data_instance)
{
    USHORT simErr, CLUT_offset;
	char *dst = NULL;
	char *palette;
	UBYTE  depth, width=0, height=0; /*a0393213 compiler warning removal- variable n_CLUT_entries removed*/
	UBYTE *data = (UBYTE *)data_instance;

    TRACE_FUNCTION("sim_img_instance_data_cnf()");

#ifdef FF_2TO1_PS
    simErr = CAUSE_SIM_NO_ERROR;
#else
    simErr = SIM_NO_ERROR;
#endif

    if (error == simErr)
	{
		/* Decode the Black & white image*/
		if(icon_image.image_instance.image_coding == BW_ICON)
		{
			width = *data;
			data++;
			height = *data;
			data++;
			dst = (char*)mfwAlloc(width * height);

			if (dst == NULL)
			{
				/* Memory allocation failed, clean up & return */
				TRACE_ERROR("ERROR: Failed to allocate memory for sim_img_instance_data_cnf");
				satResponse(SatResImpossible);
				read_image_instance_file = FALSE;
				isAlphaIdIconPresent = FALSE;
				return; 
			}
			/* B&W Converter function*/
			convert_11_image(width, height, (char *)data, dst);
		}
		/* Decode the Color image*/
		else if(icon_image.image_instance.image_coding == COLOR_ICON)
		{
			width = *data;
			data++;
			height = *data;
			data++;
			depth = *data;
			data++;
			
			data++;
			CLUT_offset = (*data) << 8; /* HIGH: offset of CLUT */
			data++;
			CLUT_offset |= (*data); /* LOW: offset of CLUT */
			data++;
			dst = (char*)mfwAlloc(width * height);

			if (dst == NULL)
			{
				/* Memory allocation failed, clean up & return */
				TRACE_ERROR("ERROR: Failed to allocate memory for sim_img_instance_data_cnf");
				satResponse(SatResImpossible);
				read_image_instance_file = FALSE;
				isAlphaIdIconPresent = FALSE;
				return;
			}

			palette = (char *)(data + CLUT_offset - 6);
			/* Color Converter function*/
			convert_21_image(width, height, depth, data, (UBYTE *)palette, (UBYTE *)dst);
		}

	    switch (cmd.type)
	    {
	        case SatCmdDispText:  iconDataUpdate(&cmd.c.text, width, height, dst);    break;
	        case SatCmdGetKey:    iconDataUpdate(&cmd.c.text, width, height, dst);    break;
	        case SatCmdGetString: iconDataUpdate(&cmd.c.inp.prompt, width, height, dst); break;
	        case SatCmdSetMenu: 				
	        case SatCmdSelItem:   
				{
					if(isAlphaIdIconPresent)
					{
					/* copy the header icon data & set the isAlphaIdIconPresent flag FALSE temporarily*/
						iconDataUpdate(&cmd.c.menu.header, width, height, dst);
						isAlphaIdIconPresent = FALSE;
					}

					/* We have to read the icons present for each menu item now.
					    We safely assume that the icons for each menu item is the same.
					    Since the SAT Icon display is only for demo, we are not checking
					    if separate icons has to be read for each item. 
					*/
						if(isItemIconPresent)
						{
							/* Copy the data into the cmd structure to be passed to BMI. */
							cmd.c.menu.items[0].iconInfo.dst = (char *) dst;
							cmd.c.menu.items[0].iconInfo.height = height;
							cmd.c.menu.items[0].iconInfo.width = width;
							isAlphaIdIconPresent = TRUE;
							isItemIconPresent = FALSE;
						}
						
					    if (!isAlphaIdIconPresent)
					    {
					       /* If the icon is present for menu items also, go ahead & read it. */
					    	if(cmd.c.menu.itemIconQual != 0xFF)
					    	{
						   	 isItemIconPresent = TRUE;
							 /* Set the Image instance flag to FALSE, so that we read the icons for the menu items. */
							 read_image_instance_file = FALSE;
						      /* Read the icon data present for Item menu list*/
						    	simReadIconData(cmd.c.menu.items[0].icon);
					    	}
						else
							isAlphaIdIconPresent = TRUE;
					    }
					}
			break;
	        case SatCmdSndSMS:    iconDataUpdate(&cmd.c.sms.info, width, height, dst);  break;
	        case SatCmdSndSS:     iconDataUpdate(&cmd.c.ss.info, width, height, dst);   break;
	        case SatCmdSndUSSD:   iconDataUpdate(&cmd.c.ss.info, width, height, dst);  break;
	        case SatCmdCall:             //x0035544 Feb 06, 2006 DR:OMAPS00061467
				                        //check if icon data is present for the first alpha identifier then update it. also check if icon data is present for alpha id2
				                        if(cmd.c.call.info.icon.qual != 0xFF)
				                        	{				                        				
				                                iconDataUpdate(&cmd.c.call.info, width, height, dst);							           
					              	}		 
			                               if(cmd.c.call.info2.icon.qual != 0xFF)
			                               	{
			                               	// check if icon id for both the alpha identifiers is same or not. if so then get the same icon data.
			                               	if(cmd.c.call.info2.icon.id == cmd.c.call.info.icon.id) 
						                iconDataUpdate(&cmd.c.call.info2, width, height, dst);								  
							      //else part is not implemented as it is just for demonstration
			                               	}  
				                         break;
	        case SatCmdIdleText:  iconDataUpdate(&cmd.c.text, width, height, dst);  break;
	        case SatCmdExecAT:          break;
	        case SatCmdSndDTMF:  iconDataUpdate(&cmd.c.cmd.info, width, height, dst);  break;
	        default:
	            TRACE_ERROR("unknown command");
	            satResponse(SatResUnknownType);
	            return;
	    }	
		
	}

    if (curSATCmd && isAlphaIdIconPresent)
	{
        	satSignal(curSATCmd,&cmd);
		read_image_instance_file = FALSE;
		isAlphaIdIconPresent = FALSE;
		
	}
//x0035544 Mar 11, 2006 DR:OMAPS00061467	
//Aafter SAT_SETUP_CALL event is executed then only send MfwSatCcAlert event
	if(setupCall_Executed == TRUE)
	{
	        satSignal(MfwSatCcAlert,&cmd);
		setupCall_Executed = FALSE;
	}

}

/*
+-----------------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT					           |
| STATE   : code                        		     ROUTINE : iconDataUpdate					    |
+-----------------------------------------------------------------------------+

  PURPOSE : To copy the image data details to be passed to BMI.

*/
void iconDataUpdate(SatTxt *t, UBYTE width, UBYTE height, char  *dst)
{
	TRACE_FUNCTION("iconDataUpdate()");

	/* Copy the data into the cmd structure to be passed to BMI. */
	t->iconInfo.dst = (char *) dst;
	t->iconInfo.height = height;
	t->iconInfo.width = width;

}


/*
+-----------------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT					  |
| STATE   : code                        ROUTINE : convert_21_image            |
+-----------------------------------------------------------------------------+

  PURPOSE : Image converter for Color icon.

*/
/* src = image data */
/* dst = converted image data */
void convert_21_image( UBYTE width, UBYTE height, UBYTE depth, UBYTE *src, UBYTE *palette, UBYTE *dst)
{
    UBYTE         i, j, k;
    USHORT      src_offset, dst_offset;
    USHORT      color_index;
    UBYTE        src_mask = 0x80;
    UBYTE        dst_mask;
    UBYTE   	value;
    USHORT     R, G, B;

    TRACE_FUNCTION("convert_21_image()");
	
    src_offset = 0;
    dst_offset = 0;

    for(j=0; j<height; j++)
    {
        for(i=0; i<width; i++)
        {
            color_index = 0;
            dst_mask = 1 << depth;
            for(k=0; k<depth; k++)
            {
                dst_mask = dst_mask >> 1;
                if(src[src_offset] & src_mask) color_index |= dst_mask;

                if(src_mask == 0x01)
                {
                    src_mask = 0x80;
                    src_offset ++;
                }
                else src_mask = src_mask >> 1;
            }
            color_index *= 3;
            R = palette[color_index + 0];
            G = palette[color_index + 1];
            B = palette[color_index + 2];
            RGB_TO_PIXEL(R, G, B, value);
            dst[dst_offset] = value;
            dst_offset++;
        }
    }
    
}

/*
+-----------------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT					  |
| STATE   : code                        ROUTINE : convert_11_image            |
+-----------------------------------------------------------------------------+

  PURPOSE : Image converter for B/W icon.

*/
/* src = image data */
/* dst = converted image data */
void convert_11_image(UBYTE width, UBYTE height, char *src, char *dst)
{
    UBYTE       i, j;
    USHORT    src_offset, dst_offset;
    USHORT    mask = 0x80;
    UBYTE       value;

    TRACE_FUNCTION("convert_11_image()");

    src_offset = 0;
    dst_offset = 0;

    for(j=0; j<height; j++)
    {
        for(i=0; i<width; i++)
        {
            if(src[src_offset] & mask)
                value = 0x00;
            else
                value = 0xFF;

            dst[dst_offset] = value;
            dst_offset++;

            if(mask == 0x01)
            {
                mask = 0x80;
                src_offset ++;
            }
            else mask = mask >> 1;
        }
    }
}
#endif

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : decRespFlag        |
+--------------------------------------------------------------------+

  PURPOSE : decode TLV: immediate response

*/

static U8 decRespFlag (U8 *flag, U8 mandatory)
{
    U8 res;

    TRACE_FUNCTION("decRespFlag()");

    res = decTLV(0x2B,mandatory,"ResponseFlag");

    if (res > 1)
    {
        *flag = 1;                      /* immediate response       */
        return 1;
    }
    else
    {
        *flag = 0;                      /* no immediate response    */
    }

    return res;
}

#ifdef FF_MMI_R99_SAT_LANG
/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : decLangNotify        |
+--------------------------------------------------------------------+

  PURPOSE : decode command: Language Notification

*/

static U32 decLangNotify(void)
{

     
     /* Need to decode and get the language selected in SIM */
     TRACE_FUNCTION("decLangNotify ()");

    while (curCmdLen > 0)
    {
        if (!decDevId(1))
            return 0;

        if (cmd.source == SatDevSIM && cmd.dest == SatDevME)
            break;                      /* success                  */

        TRACE_ERROR("illegal device combination");
        if (curCR)
        {
            satResponse(SatResUnknownData);
            return 0;
        }
        partialCompr = 1;
    }
	
     if (!decLang(&cmd.c.language,0x2D,0))
        return 0;
    
     return MfwSatLangNotify;
}
#endif
/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : decDispText        |
+--------------------------------------------------------------------+

  PURPOSE : decode command: display text

*/

static U32 decDispText (void)
{
    TRACE_FUNCTION("decDispText()");

    while (curCmdLen > 0)
    {
        if (!decDevId(1))
            return 0;

        if (cmd.source == SatDevSIM && cmd.dest == SatDevDisplay)
            break;                      /* success                  */

        TRACE_ERROR("illegal device combination");
        if (curCR)
        {
            satResponse(SatResUnknownData);
            return 0;
        }
        partialCompr = 1;
    }
    if (!decText(&cmd.c.text,0x0D,1))
        return 0;

    if (!decIconId(&cmd.c.text.icon,0))
        return 0;                       /* fatal error              */
  
    if (!decRespFlag(&cmd.c.text.responseFlag,0))
        return 0;                       /* fatal error              */

    /* SPR#2340 - DS - If len is zero, string is empty */
    if (cmd.c.text.len == 0x00)
    {
        /* If no text string is supplied send result "Command data not understood by ME" */
        satResponse(SatResUnknownData);
	 /* 29-Mar-2006, x0045876 (OMAPS00060199) */
	 return 0;
    }

// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
	/* Check if the icon data is present & needs to be read*/
    if (cmd.c.text.icon.qual != 0xFF)
    {
      /* Initialise the icon data members*/
	cmd.c.text.iconInfo.width = 0;
	cmd.c.text.iconInfo.height = 0; 
	cmd.c.text.iconInfo.dst = NULL;
    	simReadIconData(cmd.c.text.icon.id);
    }
#endif
    return MfwSatTextOut;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : decGetKey          |
+--------------------------------------------------------------------+

  PURPOSE : decode command: get inkey

*/

static U32 decGetKey (void)
{
    TRACE_FUNCTION("decGetKey()");

    while (curCmdLen > 0)
    {
        if (!decDevId(1))
            return 0;

        if (cmd.source == SatDevSIM && cmd.dest == SatDevME)
            break;                      /* success                  */

        TRACE_ERROR("illegal device combination");
        if (curCR)
        {
            satResponse(SatResUnknownData);
            return 0;
        }
        partialCompr = 1;
    }
    if (!decText(&cmd.c.text,0x0D,1))
        return 0;

    if (!decIconId(&cmd.c.text.icon,0))
        return 0;                       /* fatal error              */

// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
	/* Check if the icon data is present & needs to be read*/
    if (cmd.c.text.icon.qual != 0xFF)
    {
      /* Initialise the icon data members*/
	cmd.c.text.iconInfo.width = 0;
	cmd.c.text.iconInfo.height = 0; 
	cmd.c.text.iconInfo.dst = NULL;
    	simReadIconData(cmd.c.text.icon.id);
    }
#endif	
    return MfwSatGetKey;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : decGetString       |
+--------------------------------------------------------------------+

  PURPOSE : decode command: get input

*/

static U32 decGetString (void)
{
    TRACE_FUNCTION("decGetString()");

    while (curCmdLen > 0)
    {
        if (!decDevId(1))
            return 0;

        if (cmd.source == SatDevSIM && cmd.dest == SatDevME)
            break;                      /* success                  */

        TRACE_ERROR("illegal device combination");
        if (curCR)
        {
            satResponse(SatResUnknownData);
            return 0;
        }
        partialCompr = 1;
    }
    if (!decText(&cmd.c.inp.prompt,0x0D,1))
        return 0;
    if (!decRespLen(1))
        return 0;

    cmd.c.inp.defRsp.len = 0;           /* default settings         */
    cmd.c.inp.defRsp.code = 0;
    cmd.c.inp.defRsp.text = 0;

    if (!decText(&cmd.c.inp.defRsp,0x17,0))
        return 0;                       /* fatal error              */

    if (!decIconId(&cmd.c.inp.prompt.icon,0))
        return 0;                       /* fatal error              */

// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
	/* Check if the icon data is present & needs to be read*/
    if (cmd.c.inp.prompt.icon.qual != 0xFF)
    {
      /* Initialise the icon data members*/
	cmd.c.inp.prompt.iconInfo.width = 0;
	cmd.c.inp.prompt.iconInfo.height = 0; 
	cmd.c.inp.prompt.iconInfo.dst = NULL;
    	simReadIconData(cmd.c.inp.prompt.icon.id);
    }
#endif	
    return MfwSatGetString;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : decPlayTone        |
+--------------------------------------------------------------------+

  PURPOSE : decode command: play tone

*/

static U32 decPlayTone (void)
{
    TRACE_FUNCTION("decPlayTone()");

    while (curCmdLen > 0)
    {
        if (!decDevId(1))
            return 0;

        if (cmd.source == SatDevSIM && cmd.dest == SatDevSpeaker)
            break;                      /* success                  */

        TRACE_ERROR("illegal device combination");
        if (curCR)
        {
            satResponse(SatResUnknownData);
            return 0;
        }
        partialCompr = 1;
    }

    cmd.c.tone.alpha.len = 0;           /* default settings         */
    cmd.c.tone.alpha.code = 0;
    cmd.c.tone.alpha.text = 0;
    cmd.c.tone.tone = 0;
    cmd.c.tone.durUnit = 0;
    cmd.c.tone.durValue = 0;

    if (!decAlpha(&cmd.c.tone.alpha,0))
        return 0;
    if (!decTone(0))
        return 0;
    if (!decDuration(&cmd.c.tone.durUnit,0))
        return 0;

    return MfwSatPlayTone;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : decSetMenu         |
+--------------------------------------------------------------------+

  PURPOSE : decode command: setup menu

*/

static U32 decSetMenu (void)
{
    SatItem *ip;
    S16 oldLen;
    U8 nItems;

    TRACE_FUNCTION("decSetMenu()");

    while (curCmdLen > 0)
    {
        if (!decDevId(1))
            return 0;

        if (cmd.source == SatDevSIM && cmd.dest == SatDevME)
            break;                      /* success                  */

        TRACE_ERROR("illegal device combination");
        if (curCR)
        {
            satResponse(SatResUnknownData);
            return 0;
        }
        partialCompr = 1;
    }
    cmd.c.menu.nItems = 0;
    if (!decAlpha(&cmd.c.menu.header,1))
        return 0;

    oldLen = curCmdLen;
    ip = cmd.c.menu.items;
    if (!decItem(ip,1))
        return 0;
    nItems = 0;                         /* number of menu items     */

    while (oldLen != curCmdLen)         /* really decoded something */
    {
        nItems++;
        oldLen = curCmdLen;
        ip++;
        if (!decItem(ip,0))
            return 0;
    }

    if (!decNextAction(cmd.c.menu.items,nItems,0))
        return 0;

    if (!decIconId(&cmd.c.menu.header.icon,0))
        return 0;

    if (!decItemIcons(&cmd.c.menu,cmd.c.menu.items,nItems,0))
        return 0;

    cmd.c.menu.nItems = nItems;

// Shashi Shekar B.S., a0876501, Feb 4, 2006, DR: OMAPS00061462
#ifdef FF_MMI_SAT_ICON
	/* Check if the icon data is present  in the Alpha id & needs to be read*/
    if (cmd.c.menu.header.icon.qual != 0xFF)
    {
      /* Initialise the icon data members*/
	cmd.c.menu.header.iconInfo.width = 0;
	cmd.c.menu.header.iconInfo.height = 0; 
	cmd.c.menu.header.iconInfo.dst = NULL;
    	simReadIconData(cmd.c.menu.header.icon.id);
    }
	
/* We have to read theicons present for each menu item now.
    We safely assume that the icons for each menu item is the same.
    Since the SAT Icon display is only for demo, we are not checking
    if separate icons has to be read for each item. 
*/
    if (cmd.c.menu.itemIconQual != 0xFF)
    {
      /* Initialise the icon data members*/
	cmd.c.menu.items[0].iconInfo.width = 0;
	cmd.c.menu.items[0].iconInfo.height = 0; 
	cmd.c.menu.items[0].iconInfo.dst = NULL;
    }
#endif

    return MfwSatSetupMenu;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : decSelItem         |
+--------------------------------------------------------------------+

  PURPOSE : decode command: select item

*/

static U32 decSelItem (void)
{
    SatItem *ip;
    S16 oldLen;
    U8 nItems;

    TRACE_FUNCTION("decSelItem()");

    while (curCmdLen > 0)
    {
        if (!decDevId(1))
            return 0;

        if (cmd.source == SatDevSIM && cmd.dest == SatDevME)
            break;                      /* success                  */

        TRACE_ERROR("illegal device combination");
        if (curCR)
        {
            satResponse(SatResUnknownData);
            return 0;
        }
        partialCompr = 1;
    }
    cmd.c.menu.nItems = 0;

    cmd.c.menu.header.code = 0;         /* set default values       */
    cmd.c.menu.header.len = 0;
    cmd.c.menu.header.text = 0;
    if (!decAlpha(&cmd.c.menu.header,0))
        return 0;

    oldLen = curCmdLen;
    ip = cmd.c.menu.items;
    if (!decItem(ip,1))
        return 0;
    nItems = 0;                         /* number of menu items     */

    while (oldLen != curCmdLen)         /* really decoded something */
    {
        nItems++;
        oldLen = curCmdLen;
        ip++;
        if (!decItem(ip,0))
            return 0;
    }

    if (!decNextAction(cmd.c.menu.items,nItems,0))
        return 0;

    if (!decDefItem(cmd.c.menu.items,nItems,0))
        return 0;

    if (!decIconId(&cmd.c.menu.header.icon,0))
        return 0;

    if (!decItemIcons(&cmd.c.menu,cmd.c.menu.items,nItems,0))
        return 0;

    cmd.c.menu.nItems = nItems;

// Shashi Shekar B.S., a0876501, Feb 4, 2006, DR: OMAPS00061462
#ifdef FF_MMI_SAT_ICON
	/* Check if the icon data is present  in the Alpha id & needs to be read*/
    if (cmd.c.menu.header.icon.qual != 0xFF)
    {
      /* Initialise the icon data members*/
	cmd.c.menu.header.iconInfo.width = 0;
	cmd.c.menu.header.iconInfo.height = 0; 
	cmd.c.menu.header.iconInfo.dst = NULL;
    	simReadIconData(cmd.c.menu.header.icon.id);
    }
	
/* We have to read theicons present for each menu item now.
    We safely assume that the icons for each menu item is the same.
    Since the SAT Icon display is only for demo, we are not checking
    if separate icons has to be read for each item. 
*/
    if (cmd.c.menu.itemIconQual != 0xFF)
    {
      /* Initialise the icon data members*/
	cmd.c.menu.items[0].iconInfo.width = 0;
	cmd.c.menu.items[0].iconInfo.height = 0; 
	cmd.c.menu.items[0].iconInfo.dst = NULL;
    }
#endif

    return MfwSatSelectItem;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : decSndSMS          |
+--------------------------------------------------------------------+

  PURPOSE : decode command: send SMS

*/

static U32 decSndSMS (void)
{
    TRACE_FUNCTION("decSndSMS()");

    while (curCmdLen > 0)
    {
        if (!decDevId(1))
            return 0;

        if (cmd.source == SatDevSIM && cmd.dest == SatDevNet)
            break;                      /* success                  */

        TRACE_ERROR("illegal device combination");
        if (curCR)
        {
            satResponse(SatResUnknownData);
            return 0;
        }
        partialCompr = 1;
    }

    cmd.c.sms.info.code = 0;            /* set default values       */
    cmd.c.sms.info.len = 0;            /* optional info if absent  *//*a0393213 warnings removal - -1 changed to 0*/
    cmd.c.sms.info.text = 0;
    if (!decAlpha(&cmd.c.sms.info,0))
        return 0;

    cmd.c.sms.addr.ton = 0;             /* set default values       */
    cmd.c.sms.addr.len = 0;
    cmd.c.sms.addr.number = 0;
    if (!decAddrSS(&cmd.c.sms.addr,0x06,0)) /* address (SS string)  */
        return 0;

    if (!decSMS(&cmd.c.sms.sms,1))
        return 0;

    if (!decIconId(&cmd.c.sms.info.icon,0))
        return 0;

// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
	/* Check if the icon data is present & needs to be read*/
    if (cmd.c.sms.info.icon.qual != 0xFF)
    {
      /* Initialise the icon data members*/
	cmd.c.sms.info.iconInfo.width = 0;
	cmd.c.sms.info.iconInfo.height = 0; 
	cmd.c.sms.info.iconInfo.dst = NULL;
    	simReadIconData(cmd.c.sms.info.icon.id);
    }
#endif
    return MfwSatSendSMS;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : decSndSS           |
+--------------------------------------------------------------------+

  PURPOSE : decode command: send SS

*/

static U32 decSndSS (void)
{
    TRACE_FUNCTION("decSndSS()");

    while (curCmdLen > 0)
    {
        if (!decDevId(1))
            return 0;

        if (cmd.source == SatDevSIM && cmd.dest == SatDevNet)
            break;                      /* success                  */

        TRACE_ERROR("illegal device combination");
        if (curCR)
        {
            satResponse(SatResUnknownData);
            return 0;
        }
        partialCompr = 1;
    }

    cmd.c.ss.info.code = 0;             /* set default values       */
    cmd.c.ss.info.len = 0;  /* optional info if absent  *//*a0393213 warnings removal - -1 changed to 0*/
    cmd.c.ss.info.text = 0;
    if (!decAlpha(&cmd.c.ss.info,0))
        return 0;

    if (!decAddrSS(&cmd.c.ss.ss,0x09,1)) /* SS string (== address)  */
        return 0;

    if (!decIconId(&cmd.c.ss.info.icon,0))
        return 0;

// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
	/* Check if the icon data is present & needs to be read*/
    if (cmd.c.ss.info.icon.qual != 0xFF)
    {
      /* Initialise the icon data members*/
	cmd.c.ss.info.iconInfo.width = 0;
	cmd.c.ss.info.iconInfo.height = 0; 
	cmd.c.ss.info.iconInfo.dst = NULL;
    	simReadIconData(cmd.c.ss.info.icon.id);
    }
#endif
    return MfwSatSendSS;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : decSndUSSD         |
+--------------------------------------------------------------------+

  PURPOSE : decode command: send USSD

*/

static U32 decSndUSSD (void)
{
    TRACE_FUNCTION("decSndUSSD()");

    while (curCmdLen > 0)
    {
        if (!decDevId(1))
            return 0;

        if (cmd.source == SatDevSIM && cmd.dest == SatDevNet)
            break;                      /* success                  */

        TRACE_ERROR("illegal device combination");
        if (curCR)
        {
            satResponse(SatResUnknownData);
            return 0;
        }
        partialCompr = 1;
    }

    cmd.c.ss.info.code = 0;             /* set default values       */
    cmd.c.ss.info.len = 0;      /* optional info if absent  *//*a0393213 warnings removal - -1 changed to 0*/
    cmd.c.ss.info.text = 0;
    if (!decAlpha(&cmd.c.ss.info,0))
        return 0;

    if (!decAddrSS(&cmd.c.ss.ss,0x0A,1)) /* USSD string             */
        return 0;

    if (!decIconId(&cmd.c.ss.info.icon,0))
        return 0;

// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
	/* Check if the icon data is present & needs to be read*/
    if (cmd.c.ss.info.icon.qual != 0xFF)
    {
      /* Initialise the icon data members*/
	cmd.c.ss.info.iconInfo.width = 0;
	cmd.c.ss.info.iconInfo.height = 0; 
	cmd.c.ss.info.iconInfo.dst = NULL;
    	simReadIconData(cmd.c.ss.info.icon.id);
    }
#endif
    return MfwSatSendUSSD;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : decCall            |
+--------------------------------------------------------------------+

  PURPOSE : decode command: setup call

*/

static U32 decCall (void)
{
    TRACE_FUNCTION("decCall()");

    while (curCmdLen > 0)
    {
        if (!decDevId(1))
            return 0;

        if (cmd.source == SatDevSIM && cmd.dest == SatDevNet)
            break;                      /* success                  */

        TRACE_ERROR("illegal device combination");
        if (curCR)
        {
            satResponse(SatResUnknownData);
            return 0;
        }
        partialCompr = 1;
    }

    cmd.c.call.info.code = 0;           /* set default values       */
    cmd.c.call.info.len = 0;   /* optional info if absent  *//*a0393213 warnings removal - -1 changed to 0*/
    cmd.c.call.info.text = 0;

    cmd.c.call.info2.code = 0;          /* set default values       */
    cmd.c.call.info2.len = 0; /* optional info if absent  *//*a0393213 warnings removal - -1 changed to 0*/
    cmd.c.call.info2.text = 0;

    if (!decAlpha(&cmd.c.call.info,0))
        return 0;

    if (!decAddrSS(&cmd.c.call.addr,0x06,1)) /* address (SS string)  */
        return 0;

    cmd.c.call.bc.len = 0;              /* set default values       */
    cmd.c.call.bc.data = 0;
    if (!decData(&cmd.c.call.bc,0x07,0)) /* bearer capabilities     */
        return 0;

    cmd.c.call.sa.len = 0;              /* set default values       */
    cmd.c.call.sa.data = 0;
    if (!decData(&cmd.c.call.sa,0x08,0)) /* subaddress              */
        return 0;

    cmd.c.call.durUnit = 0;             /* set default values       */
    cmd.c.call.durValue = 0;
    if (!decDuration(&cmd.c.call.durUnit,0)) /* redial timing       */
        return 0;

    if (!decIconId(&cmd.c.call.info.icon,0)) /* icon for info 1     */
        return 0;

    if (!decAlpha(&cmd.c.call.info2,0)) /* call setup info (2)      */
        return 0;

    if (!decIconId(&cmd.c.call.info2.icon,0)) /* icon for info 2    */
        return 0;
    //Sudha.V x0035544, feb 06, 2006 DR:OMAPS00061467 	
   #ifdef FF_MMI_SAT_ICON   
	/* Check if the icon data is present for alpha identifier 1 & needs to be read*/
    if (cmd.c.call.info.icon.qual != 0xFF)
    {
		/* Since we have two icons present, if second Alpha id is present, 
		set the isAlphaIdIconPresent to TRUE explicitly since the second Alpha icon will
		reset this flag if not present & we wont enter the icon decoder*/
	isAlphaIdIconPresent = TRUE;
      /* Initialise the icon data members*/
	cmd.c.call.info.iconInfo.width = 0;
	cmd.c.call.info.iconInfo.height = 0; 
	cmd.c.call.info.iconInfo.dst = NULL;
    	simReadIconData(cmd.c.call.info.icon.id);
    }
	/* Check if the icon data is present for alpha identifier 2 & needs to be read*/
	//No need to read icon data for alpha id2 from sim again as it is same as that of alpha id1
   if (cmd.c.call.info2.icon.qual != 0xFF)
    {
      // Initialise the icon data members
	cmd.c.call.info2.iconInfo.width = 0;
	cmd.c.call.info2.iconInfo.height = 0; 
	cmd.c.call.info2.iconInfo.dst = NULL; 
    }    	
#endif

    return MfwSatCall;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : decExecAT          |
+--------------------------------------------------------------------+

  PURPOSE : decode command: run AT command

*/

static U32 decExecAT (void)
{
    TRACE_FUNCTION("decExecAT()");

    while (curCmdLen > 0)
    {
        if (!decDevId(1))
            return 0;

        if (cmd.source == SatDevSIM && cmd.dest == SatDevME)
            break;                      /* success                  */

        TRACE_ERROR("illegal device combination");
        if (curCR)
        {
            satResponse(SatResUnknownData);
            return 0;
        }
        partialCompr = 1;
    }

    cmd.c.cmd.info.code = 0;            /* set default values       */
    cmd.c.cmd.info.len = 0;    /* optional info if absent  *//*a0393213 warnings removal - -1 changed to 0*/
    cmd.c.cmd.info.text = 0;
    if (!decAlpha(&cmd.c.cmd.info,0))
        return 0;

    cmd.c.cmd.cmd.len = 0;              /* set default values       */
    cmd.c.cmd.cmd.data = 0;
    if (!decData(&cmd.c.cmd.cmd,0x28,1)) /* AT command string       */
        return 0;

    if (!decIconId(&cmd.c.cmd.info.icon,0)) /* icon for info        */
        return 0;

    return MfwSatExecAT;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : decSndDTMF         |
+--------------------------------------------------------------------+

  PURPOSE : decode command: send DTMF

*/

static U32 decSndDTMF (void)
{
    TRACE_FUNCTION("decSndDTMF()");

    while (curCmdLen > 0)
    {
        if (!decDevId(1))
            return 0;

        if (cmd.source == SatDevSIM && cmd.dest == SatDevNet)
            break;                      /* success                  */

        TRACE_ERROR("illegal device combination");
        if (curCR)
        {
            satResponse(SatResUnknownData);
            return 0;
        }
        partialCompr = 1;
    }

    cmd.c.cmd.info.code = 0;            /* set default values       */
    cmd.c.cmd.info.len = 0;    /* optional info if absent  *//*a0393213 warnings removal - -1 changed to 0*/
    cmd.c.cmd.info.text = 0;
    if (!decAlpha(&cmd.c.cmd.info,0))
        return 0;

    cmd.c.cmd.cmd.len = 0;              /* set default values       */
    cmd.c.cmd.cmd.data = 0;
    if (!decData(&cmd.c.cmd.cmd,0x2C,1)) /* DTMF string             */
        return 0;

    if (!decIconId(&cmd.c.cmd.info.icon,0)) /* icon for info        */
        return 0;

// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
	/* Check if the icon data is present & needs to be read*/
    if (cmd.c.cmd.info.icon.qual != 0xFF)
    {
      /* Initialise the icon data members*/
	cmd.c.cmd.info.iconInfo.width = 0;
	cmd.c.cmd.info.iconInfo.height = 0; 
	cmd.c.cmd.info.iconInfo.dst = NULL;
    	simReadIconData(cmd.c.cmd.info.icon.id);
    }
#endif
    return MfwSatSendDTMF;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : decCcRes           |
+--------------------------------------------------------------------+

  PURPOSE : decode Call Control result

*/

static U32 decCcRes (void)
{
    TRACE_FUNCTION("decCcRes()");

    cmd.c.ccres.addr.ton = 0;  /* set default values       *//*a0393213 warnings removal - -1 changed to 0 for all items*/
    cmd.c.ccres.addr.len = 0; 
    cmd.c.ccres.addr.number = 0;
    cmd.c.ccres.addr2.ton = 0;         /* set on SMS result        */
    cmd.c.ccres.addr2.len = 0;
    cmd.c.ccres.addr2.number = 0;

    if (!decAddrSS(&cmd.c.ccres.addr,0x06,0)) /* address            */
        return 0;

    if (curTag == 0x06)                 /* was an address           */
    {
        if (!decAddrSS(&cmd.c.ccres.addr2,0x06,0)) /* try another   */
            return 0;
        if (curTag == 0x06)             /* hit: have SMS result     */
        {
            cmd.c.ccres.info.code = 0; /* get alpha id *//*a0393213 warnings removal - -1 changed to 0 for all items*/
            cmd.c.ccres.info.len = 0;
            cmd.c.ccres.info.text = 0;
            if (!decAlpha(&cmd.c.ccres.info,0))
                return 0;

            return MfwSatCcRes;
        }
    }
    else
    {
        if (curTag == 0x09)             /* not an address           */
            if (!decAddrSS(&cmd.c.ccres.addr,0x09,0)) /* try SS     */
                return 0;
        if (curTag == 0x0a)             /* not an SS string         */
            if (!decAddrSS(&cmd.c.ccres.addr,0x0a,0)) /* try USSD   */
                return 0;
    }

    cmd.c.ccres.bc.len = 0; /* set default values       */ /*a0393213 warnings removal - -1 changed to 0 for all items*/
    cmd.c.ccres.bc.data = 0;
    if (!decData(&cmd.c.ccres.bc,0x07,0)) /* bearer capabilities    */
        return 0;

    cmd.c.ccres.sa.len = 0;            /* set default values  *//*a0393213 warnings removal - -1 changed to 0 for all items*/
    cmd.c.ccres.sa.data = 0;
    if (!decData(&cmd.c.ccres.sa,0x08,0)) /* subaddress             */
        return 0;

    cmd.c.ccres.info.code = 0; /* set default values       *//*a0393213 warnings removal - -1 changed to 0 for all items*/
    cmd.c.ccres.info.len = 0;
    cmd.c.ccres.info.text = 0;
    if (!decAlpha(&cmd.c.ccres.info,0)) /* alpha information        */
        return 0;

    cmd.c.ccres.bcRepInd = 0;           /* set default values       */
    if (decByte(&cmd.c.ccres.bcRepInd,0x2a,0)<2) /* BC repeat ind.  */
        return MfwSatCcRes;

    cmd.c.ccres.bc2.len = 0; /* set default values *//*a0393213 warnings removal - -1 changed to 0 for all items*/
    cmd.c.ccres.bc2.data = 0;
    if (!decData(&cmd.c.ccres.bc2,0x07,0)) /* bearer 2 capabilities */
        return 0;

    return MfwSatCcRes;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : decRefresh         |
+--------------------------------------------------------------------+

  PURPOSE : decode command: refresh

*/

static U32 decRefresh (void)
{
    TRACE_FUNCTION("decRefresh()");

    while (curCmdLen > 0)
    {
        if (!decDevId(1))
            return 0;

        if (cmd.source == SatDevSIM && cmd.dest == SatDevME)
            break;                      /* success                  */

        TRACE_ERROR("illegal device combination");
        if (curCR)
        {
            satResponse(SatResUnknownData);
            return 0;
        }
        partialCompr = 1;
    }

    cmd.c.files.len = 0;                /* set default values       */
    cmd.c.files.data = 0;
    if (!decData(&cmd.c.files,0x12,0))  /* refresh file list        */
        return 0;

    //ES!! do real refresh here...

    return MfwSatRefresh;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : decSetEvents       |
+--------------------------------------------------------------------+

  PURPOSE : decode command: setup event list

*/

static U32 decSetEvents (void)
{
    U8 *ep;
    U8 res;
	U8 sat_res[2];

    TRACE_FUNCTION("decSetEvents()");

    while (curCmdLen > 0)
    {
        if (!decDevId(1))
            return 0;

        if (cmd.source == SatDevSIM && cmd.dest == SatDevME)
            break;                      /* success                  */

        TRACE_ERROR("illegal device combination");
        if (curCR)
        {
            satResponse(SatResUnknownData);
            return 0;
        }
        partialCompr = 1;
    }

    cmd.c.events.len = 0;               /* default: remove list     */
	cmd.c.events.elem[0] = 0;
	eventList =0;
    res = decTLV(0x19,1,"Event List");

    if (res > 1)
    {
        ep = &cmd.c.events.elem[0];
        while (curLen--)
        {            
            switch (*curVal)
            {
				case SatEvtUserAction:
					if (eventList & SatEvtUserActionActivated)
					{
						/*
						** CQ 16435   : This can only have happend if this event is listed more than one
						**                     which is invalid and causes a reject!
						*/
	 					eventList = 0;
	 					satResponse(SatResImpossible);
						return 0;
					}
					else
					{
						eventList|= SatEvtUserActionActivated;
						*ep |= *curVal;          /* event list element       */
						cmd.c.events.len++;     /* number of events         */
					}
					break;
					
 				case SatEvtIdleScreen:
					if (eventList & SatEvtIdleScreenActivated)
					{
						/*
						** CQ 16435   : This can only have happend if this event is listed more than one
						**                     which is invalid and causes a reject!
						*/
	 					eventList = 0;
	 					satResponse(SatResImpossible);
						return 0;
					}
					else
					{
						eventList|= SatEvtIdleScreenActivated;
						*ep |= *curVal;          /* event list element       */
						cmd.c.events.len++;     /* number of events         */
					}
					break;
					
/* ACI-SPR-18200: - this event is not completely implemented 
                  "Not supported" stated in TERMINAL PROFILE */
#if 0 
 				case SatEvtBrowserTermination:
					if (eventList & SatEvtBrowserTerminateActivated)
					{
						/*
						** CQ 16435   : This can only have happend if this event is listed more than one
						**                     which is invalid and causes a reject!
						*/
	 					eventList = 0;
	 					satResponse(SatResImpossible);
						return 0;
					}
					else
					{
						eventList|= SatEvtBrowserTerminateActivated;
						*ep |= *curVal;          /* event list element       */
						cmd.c.events.len++;     /* number of events         */
					}
					break;
#endif
#ifdef FF_MMI_R99_SAT_LANG
 				case SatEvtLangSelection: /*OMAPS00070661 (SAT-Lang sel) R.Prabakar(a0393213)*/
					if (eventList & SatEvtLangSelectActivated)
					{
						/*
						** CQ 16435   : This can only have happend if this event is listed more than one
						**                     which is invalid and causes a reject!
						*/
	 					eventList = 0;
	 					satResponse(SatResImpossible);
						return 0;
					}
					else
					{
						eventList|= SatEvtLangSelectActivated;
						*ep |= *curVal;          /* event list element       */
						cmd.c.events.len++;     /* number of events         */
					}
					break;
#endif					
 				default:
 					/* CQ16435 : If there is an unsupported event, then the entire list fails,
 					**                 reset the eventList to zero and return the response "Beyond ME Capabilities"
 					*/
 					eventList = 0;
 					satResponse(SatResImpossible);
					return 0;
			}
            curVal++;
        }
									
      sat_res[0] = SatResSuccess;	/*Terminal response always sent if there are elements */
	  sat_res[1]  = SatResAiNoCause;  /* even if they are not MMI 				 */
      TRACE_EVENT_P1("MFW event list %x",eventList);
	  satDone(&cmd, (U8*)&sat_res, sizeof(sat_res));
    }
    else
	{
		satResponse(SatResUnknownData);
        return 0;                       /* no event list found      */
	}
    return MfwSatSetEvents;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : decIdleText        |
+--------------------------------------------------------------------+

  PURPOSE : decode command: setup idle mode text

*/

static U32 decIdleText (void)
{
    TRACE_FUNCTION("decIdleText()");

    while (curCmdLen > 0)
    {
        if (!decDevId(1))
            return 0;

        if (cmd.source == SatDevSIM && cmd.dest == SatDevME)
            break;                      /* success                  */

        TRACE_ERROR("illegal device combination");
        if (curCR)
        {
            satResponse(SatResUnknownData);
            return 0;
        }
        partialCompr = 1;
    }
    if (!decText(&cmd.c.text,0x0D,1))
        return 0;

    if (!decIconId(&cmd.c.text.icon,0))
        return 0;

// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
	/* Check if the icon data is present & needs to be read*/
    if (cmd.c.text.icon.qual != 0xFF)
    {
      /* Initialise the icon data members*/
	cmd.c.text.iconInfo.width = 0;
	cmd.c.text.iconInfo.height = 0; 
	cmd.c.text.iconInfo.dst = NULL;
    	simReadIconData(cmd.c.text.icon.id);
    }
#endif
    return MfwSatIdleText;
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : decOpenChannel       |
+--------------------------------------------------------------------+

  PURPOSE : decode command: open channel

*/

static U32 decOpenChannel (void)
{
    SatAddress addr;
    SatData subaddr;
    
    TRACE_FUNCTION("decOpenChannel()");

    while (curCmdLen > 0)
    {
        if (!decDevId(1))
            return 0;

        if (cmd.source == SatDevSIM && cmd.dest == SatDevME)
            break;                      /* success                  */

        TRACE_ERROR("illegal device combination");
        if (curCR)
        {
            satResponse(SatResUnknownData);
            return 0;
        }
        partialCompr = 1;
    }

    /* Decode alpha idenitifier if present */
    cmd.c.open.alpha_id.code = 0;            /* set default values       */
    cmd.c.open.alpha_id.len = 0;  /* optional info if absent  *//*a0393213 warnings removal - -1 changed to 0*/
    cmd.c.open.alpha_id.text = 0;  
    if (!decAlpha(&cmd.c.open.alpha_id,0))
        return 0;

    /* Decode icon identifier if present */
    if (!decIconId(&cmd.c.open.alpha_id.icon,0))
        return 0;

    /* Decode address if present */
    /* If address is present then channel type is CSD */
    addr.ton = 0;             /* set default values       */
    addr.len = 0;
    addr.number = 0;  
    if (!decAddrSS(&addr,0x06,0))
        return 0;

    /* Decode subaddress if present */
    subaddr.len = 0;
    subaddr.data = 0;
    if (!decData(&subaddr,0x08,0))
        return 0;

    /* Decode duration 1 if present */
    cmd.c.open.dur1Unit = 0;             /* set default values       */
    cmd.c.open.dur1Value = 0;
    if (!decDuration(&cmd.c.open.dur1Unit,0)) /* redial timing       */
        return 0;

    /* Decode duration 2 if present */
    cmd.c.open.dur2Unit = 0;             /* set default values       */
    cmd.c.open.dur2Value = 0;
    if (!decDuration(&cmd.c.open.dur2Unit,0)) /* redial timing       */
        return 0;

    return MfwSatOpenChannel;

}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : decCloseChannel        |
+--------------------------------------------------------------------+

  PURPOSE : decode command: close channel

*/

static U32 decCloseChannel (void)
{

    /*a0393213 compiler warnings removal - removed alpha_res*/
    
    TRACE_FUNCTION("decCloseChannel()");

    while (curCmdLen > 0)
    {
        if (!decDevId(1))
            return 0;

        /* SPR#2321 - DS - Compare with correct SAT Class E device identities */
        if ( (cmd.source == SatDevSIM && cmd.dest == SatDevCh1) ||
		(cmd.source == SatDevSIM && cmd.dest == SatDevCh2) ||
		(cmd.source == SatDevSIM && cmd.dest == SatDevCh3) ||
		(cmd.source == SatDevSIM && cmd.dest == SatDevCh4) ||
		(cmd.source == SatDevSIM && cmd.dest == SatDevCh5) ||
		(cmd.source == SatDevSIM && cmd.dest == SatDevCh6) ||
		(cmd.source == SatDevSIM && cmd.dest == SatDevCh7) )
            break;                      /* success                  */

        TRACE_ERROR("illegal device combination");
        if (curCR)
        {
            satResponse(SatResUnknownData);
            return 0;
        }
        partialCompr = 1;
    }

    /* Decode alpha identifier if present */
    cmd.c.close.alpha_id.code = 0;            /* set default values       */
    cmd.c.close.alpha_id.len = 0; /* optional info if absent  *//*a0393213 warnings removal - -1 changed to 0*/
    cmd.c.close.alpha_id.text = 0;
    
    if (!decAlpha(&cmd.c.close.alpha_id,0))
        return 0;

    /* Decode icon identifier if present */
    if (!decIconId(&cmd.c.close.alpha_id.icon,0))
        return 0;

    return MfwSatCloseChannel;

}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : decSendData       |
+--------------------------------------------------------------------+

  PURPOSE : decode command: send data

*/

static U32 decSendData (void)
{
    TRACE_FUNCTION("decSendData()");

    while (curCmdLen > 0)
    {
        if (!decDevId(1))
            return 0;

        /* SPR#2321 - DS - Compare with correct SAT Class E device identities */
        if ( (cmd.source == SatDevSIM && cmd.dest == SatDevCh1) ||
		(cmd.source == SatDevSIM && cmd.dest == SatDevCh2) ||
		(cmd.source == SatDevSIM && cmd.dest == SatDevCh3) ||
		(cmd.source == SatDevSIM && cmd.dest == SatDevCh4) ||
		(cmd.source == SatDevSIM && cmd.dest == SatDevCh5) ||
		(cmd.source == SatDevSIM && cmd.dest == SatDevCh6) ||
		(cmd.source == SatDevSIM && cmd.dest == SatDevCh7) )
            break;                      /* success                  */

        TRACE_ERROR("illegal device combination");
        if (curCR)
        {
            satResponse(SatResUnknownData);
            return 0;
        }
        partialCompr = 1;
    }

    /* Decode alpha identifier if present */
    cmd.c.send.alpha_id.code = 0;            /* set default values       */
    cmd.c.send.alpha_id.len = 0;   /* optional info if absent  *//*a0393213 warnings removal - -1 changed to 0*/
    cmd.c.send.alpha_id.text = 0;
    
    if (!decAlpha(&cmd.c.send.alpha_id,0))
        return 0;

    /* Decode icon identifier if present */
    if (!decIconId(&cmd.c.send.alpha_id.icon,0))
        return 0;

    return MfwSatSendData;

}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : decRecvData       |
+--------------------------------------------------------------------+

  PURPOSE : decode command: receive data

*/

static U32 decRecvData (void)
{
    TRACE_FUNCTION("decRecvData()");

    while (curCmdLen > 0)
    {
        if (!decDevId(1))
            return 0;

        /* SPR#2321 - DS - Compare with correct SAT Class E device identities */
        if ( (cmd.source == SatDevSIM && cmd.dest == SatDevCh1) ||
		(cmd.source == SatDevSIM && cmd.dest == SatDevCh2) ||
		(cmd.source == SatDevSIM && cmd.dest == SatDevCh3) ||
		(cmd.source == SatDevSIM && cmd.dest == SatDevCh4) ||
		(cmd.source == SatDevSIM && cmd.dest == SatDevCh5) ||
		(cmd.source == SatDevSIM && cmd.dest == SatDevCh6) ||
		(cmd.source == SatDevSIM && cmd.dest == SatDevCh7) )
            break;                      /* success                  */

        TRACE_ERROR("illegal device combination");
        if (curCR)
        {
            satResponse(SatResUnknownData);
            return 0;
        }
        partialCompr = 1;
    }

     /* Decode alpha identifier if present */
    cmd.c.receive.alpha_id.code = 0;            /* set default values       */
    cmd.c.receive.alpha_id.len = 0; /* optional info if absent  *//*a0393213 warnings removal - -1 changed to 0*/
    cmd.c.receive.alpha_id.text = 0;
    
    if (!decAlpha(&cmd.c.send.alpha_id,0))
        return 0;

    /* Decode icon identifier if present */
    if (!decIconId(&cmd.c.receive.alpha_id.icon,0))
        return 0;

    return MfwSatReceiveData;

}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : cmdDecode          |
+--------------------------------------------------------------------+

  PURPOSE : command specific decoding

*/

static void cmdDecode (void)
{
    U32 evt = 0;
//	T_SIM_ACCESS_PARAM sim_read_param;

    TRACE_FUNCTION("cmdDecode()");

    switch (cmd.type)
    {
        case SatCmdDispText:  evt = decDispText();    break;
        case SatCmdGetKey:    evt = decGetKey();      break;
        case SatCmdGetString: evt = decGetString();   break;
        case SatCmdPlayTone:  evt = decPlayTone();    break;
        case SatCmdSetMenu:   evt = decSetMenu();     break;
        case SatCmdSelItem:   evt = decSelItem();     break;
        case SatCmdSndSMS:    evt = decSndSMS();      break;
        case SatCmdSndSS:     evt = decSndSS();       break;
        case SatCmdSndUSSD:   evt = decSndUSSD();     break;
        case SatCmdCall:      evt = decCall();        break;
        case SatCmdRefresh:   evt = decRefresh();     break;
        case SatCmdEvtList:   evt = decSetEvents();   break;
        case SatCmdIdleText:  evt = decIdleText();    break;
        case SatCmdExecAT:    evt = decExecAT();      break;
        case SatCmdSndDTMF:   evt = decSndDTMF();     break;
#if defined(FF_WAP)
        case SatCmdLaunchBrowser:  break;
#endif

        /* SPR#1786 - DS - SAT Class E */
        case SatCmdOpenChannel:     evt = decOpenChannel(); break;
        case SatCmdCloseChannel:    evt = decCloseChannel(); break;
        case SatCmdSendData:        evt = decSendData(); break;
        case SatCmdReceiveData:     evt = decRecvData(); break;

        case SatCmdEOS:       evt = MfwSatSessionEnd; break;
#ifdef FF_MMI_R99_SAT_LANG	
        case SatCmdLangNotify: evt = decLangNotify(); break; /*OMAPS00070661 (SAT-Lang notify & Local info) R.Prabakar(a0393213)*/
        case SatCmdLocalInf: evt = MfwSatLocalInfo; break;
#endif		
        default:
            TRACE_ERROR("unknown command");
            satResponse(SatResUnknownType);
            return;
    }
	
    if (evt == MfwSatOpenChannel)
    {
    	/* SPR#2321 - DS - Store command details so rAT_PercentSATA
    	 * can send the correct command details for Open Channel.
    	 */
    	 sat_cmd_saved = (SatCmd*)mfwAlloc(sizeof(SatCmd));

	if (sat_cmd_saved != NULL)
	{
		TRACE_EVENT("Save Open Channel command details");
		memcpy(sat_cmd_saved, &cmd, sizeof(cmd));
	}
	
        /* Event is sent from rAT_PercentSATA */
	
        return;
    }
// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
    curSATCmd = evt; /* Keep a current copy of the command */

    if (evt && !isAlphaIdIconPresent)
        satSignal(evt,&cmd);
#else
    if (evt)
        satSignal(evt,&cmd);
#endif

    return;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : rAT_PercentSATI    |
+--------------------------------------------------------------------+

  PURPOSE : called by ACI for SIM Toolkit command

*/

void rAT_PercentSATI (S16 len, U8 *sc)
{
    U8 tl;

    TRACE_FUNCTION("rAT_PercentSATI()");
    if (len <= 0)                       /* empty command?           */
    {
        cmd.number = 0;                 /* virtual command          */
        cmd.type = SatCmdEOS;           /* terminate session        */
        cmd.qual = 0;
        cmd.source = SatDevSIM;
        cmd.dest = SatDevME;
        cmdDecode();
        return;
    }

    if (!sc || *sc != 0xD0)
    {                                   /* no proactive SIM command */
        TRACE_ERROR("no SAT command");  /* ignore message           */
        return;
    }

    curCmd = sc;
    curCmdLen = len;
    curData = (U8 *) &cmd + sizeof(SatCmd);/* end of command buffer */
    *(curData - 1) = 0x07;
   
    tl = getTLValue(sc);
    if (!tl)                            /* length error             */
        return;                         /* ignore message           */

    curCmd += tl;
    curCmdLen -= tl;
    if (curCmdLen < curLen)             /* not enough data          */
    {
        TRACE_ERROR("not enough data");
        satResponse(SatResUnknownData); /* not understood by ME     */
        return;
    }
    if (curCmdLen > curLen)             /* more data than needed:   */
        curCmdLen = curLen;             /* ignore the rest          */

    partialCompr = 0;                   /* partial comprehen. flag  */
    while (curCmdLen > 0)
    {
        tl = getTLValue(curCmd);
        if (!tl)
            return;
        curCmd += tl;
        curCmdLen -= tl;
        if (curTag == 0x01)             /* command details          */
        {
            cmd.number = *(curCmd+0);   /* transaction number       */
            cmd.type = *(curCmd+1);     /* command ID               */
            cmd.qual = *(curCmd+2);     /* command qualifier        */
            if (cmd.number == 0x00 || cmd.number == 0xFF)
            {
                TRACE_ERROR("invalid command number");
                satResponse(SatResUnknownData);
            }
            /*SPR#2121 - DS - Copy command specific details into global buffer defCmdDetails */
            memcpy(defCmdDetails, curCmd-2, 5);
            
            curCmd += curLen;
            curCmdLen -= curLen;        /* length of value bytes    */
            cmdDecode();                /* do specific decoding     */
            return;
        }
        TRACE_ERROR("no command details");
        if (curCR)
        {
            satResponse(SatResUnknownData);
            return;
        }
        curCmd += curLen;
        curCmdLen -= curLen;
        partialCompr = 1;
    }
    satResponse(SatResNoValues);
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : rAT_PercentSATN    |
+--------------------------------------------------------------------+

  PURPOSE : Notification by ACI on notifications for SAT commands

*/

/* BEGIN ADD:  : Req ID: : 31-Mar-2005*/
#ifndef NEPTUNE_BOARD
/* END ADD: : Req ID: : 31-Mar-2005*/
void rAT_PercentSATN (S16 len, U8 *sc, T_ACI_SATN_CNTRL_TYPE  cntrl_type)
/* BEGIN ADD:  : Req ID: : 31-Mar-2005*/
#else
void rAT_PercentSATN  ( SHORT           len,UBYTE          *sc)
#endif
/* END ADD:  : Req ID: : 31-Mar-2005*/
{
    U8 tl;
    U32 evt = 0;

    TRACE_FUNCTION("rAT_PercentSATN()");

    if (len <= 0 || !sc)                /* empty command?           */
    {
        TRACE_ERROR("empty SATN!");
#if 0
        // ES!! simulate CC result: not modified ??
        sc = (U8*) &evt;                /* temporary misuse...      */
        len = 2;
#else
        // ES!! or ignore it ??
        return;
#endif
    }
    if (*sc == 0xD0)
    {
        satNoResponse = 1;
        rAT_PercentSATI(len,sc);        /* normal processing        */
        satNoResponse = 0;
        return;
    }

    curCmd = sc;
    curCmdLen = len;
    curData = (U8 *) &cmd + sizeof(SatCmd);/* end of command buffer */

    switch (*sc)
    {
        case 0x00:                      /* CC: allowed, not mod.    */
        case 0x01:                      /* CC: not allowed          */
        case 0x02:                      /* CC: allowed, modified    */
            partialCompr = 0;           /* partial comprehen. flag  */
            cmd.c.ccres.result = *sc;
            tl = getTLValue(sc);
            if (!tl)                    /* length error             */
                return;                 /* ignore message           */
            curCmd += tl;
            curCmdLen -= tl;
            if (curCmdLen < curLen)     /* not enough data          */
            {
                TRACE_ERROR("SATN: not enough data (ignored).");
                return;
            }
            if (curCmdLen > curLen)     /* more data than needed:   */
                curCmdLen = curLen;     /* ignore the rest          */
            evt = decCcRes();
            if (evt)
                satSignal(evt,&cmd);
            break;
        case 0x81:
            TRACE_EVENT("SATN: ACI terminal response (ignored).");
            break;
        default:
            TRACE_EVENT("SATN: unknown header (ignored).");
            break;
    }
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : rAT_PercentSATA    |
+--------------------------------------------------------------------+

  PURPOSE : Alert indication by ACI for SAT commands

*/
#if defined (TI_PS_FF_SATE) || defined (FF_SAT_E) 
void rAT_PercentSATA (S16 cId, S32 rdlTimeout_ms,T_ACI_SATA_ADD *addParm)
#else
void rAT_PercentSATA (S16 cId, S32 rdlTimeout_ms)
#endif /* TI_PS_FF_SATE */
{
    TRACE_FUNCTION("rAT_PercentSATA()");

    TRACE_EVENT_P1("Redial Time %d",rdlTimeout_ms); 

    /* Store call details */
    cmd.c.ccres.redialTime = rdlTimeout_ms;
    cmd.c.ccres.callId = cId;

    if (rdlTimeout_ms > 0) /* Check timeout value is valid */
    {
	cm_set_redial(CM_REDIAL_AUTO);
    }
#if defined (TI_PS_FF_SATE) || defined (FF_SAT_E) 
    /* No Channel information available, therefore treat as normal Voice call */
    if (addParm == NULL)
    {
#endif /* TI_PS_FF_SATE */ 
//x0035544 Mar 11, 2006. DR:OMAPS00061467
//unless SAT_SETUP_CALL event is executed, dont send MfwSatCcAlert event to BMI
    #ifdef FF_MMI_SAT_ICON
      if(isAlphaIdIconPresent )
      	{
      		setupCall_Executed = TRUE;
      	}
	else
	  	satSignal(MfwSatCcAlert,&cmd);
#else
	satSignal(MfwSatCcAlert,&cmd);
#endif 	
        return;
		
#if defined (TI_PS_FF_SATE) || defined (FF_SAT_E) 
    }
    else /* Store channel information in command structure */
    {
        cmd.c.open.channel_type = (T_MFW_SAT_CHN)addParm->chnType;
        cmd.c.open.establishment_type = (T_MFW_SAT_EST)addParm->chnEst;
    }

    /* Determine Channel type and send approriate event to BMI */
    switch ((T_MFW_SAT_CHN)addParm->chnType)/*a0393213 lint warnings removal - type casting done*/
    {
        case    MFW_SAT_CHN_NONE:
        case    MFW_SAT_CHN_VOICE:
            satSignal(MfwSatCcAlert,&cmd);
            break;
        case    MFW_SAT_CHN_CSD:
        case    MFW_SAT_CHN_GPRS: 

	     /* SPR#2321 - DS - Copy the saved Open Channel details to cmd in case a Call
	      * Control result command was received (therefore overwriting the Open Channel 
	      * data) between the original MfwSatOpenChannel command and this alert indication
	      */
	     if (sat_cmd_saved != NULL)
	     {
	     	  TRACE_EVENT("Copy the saved Open Channel details to the cmd structure");
	         memcpy(&cmd, sat_cmd_saved, sizeof(SatCmd)); /* SPR#2340 - DS - Copy full structure */

		  /* Deallocate saved Open Channel command details */
		  if (sat_cmd_saved)
		  {
		      TRACE_EVENT("Deallocate sat_cmd_saved");
		      mfwFree((U8*)sat_cmd_saved, sizeof(SatCmd));
		      sat_cmd_saved = NULL;
		  }
	     }
	     TRACE_EVENT("satSignal(MfwSatOpenChannel, &cmd)");
	     satSignal(MfwSatOpenChannel, &cmd);
            break;
	  default:
	  	TRACE_ERROR("Error with SATA channle type!");
		break;
    }
#endif /* TI_PS_FF_SATE */    
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : rAT_PercentSATE    |
+--------------------------------------------------------------------+

  PURPOSE : called by ACI on response for Envelope commands

*/

void rAT_PercentSATE (S16 len, U8 *res)
{
    TRACE_FUNCTION("rAT_PercentSATE()"); //ES!! FOR WHAT ??
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT           |
| STATE   : code                        ROUTINE : rAT_PercentSATBROW |
+--------------------------------------------------------------------+

  PURPOSE : called by ACI when the SAT Browser command arrives.

*/

#if defined(FF_WAP)

void rAT_PercentSATBROW (T_LAUNCH_BROWSER *browser_command) //To be change to T_LAUNCH_BROWSER
{
   int i;
   T_prov_file_ref* current_prov_file; 
  
   TRACE_FUNCTION("rAT_PercentSATBROW()"); 

   cmd.c.browser.identity = browser_command->browser_id.browser;
   cmd.c.browser.url = (U8*) browser_command->url.url_value;
   cmd.c.browser.n_bearer = browser_command->c_bearer;
   cmd.c.browser.bearer = (U8*)browser_command->bearer;
   cmd.c.browser.n_prov_file_ref = browser_command->c_prov_file_ref;
   for (i=0;(i<browser_command->c_prov_file_ref) && (i<MFW_MAX_PFP_ELEM);i++)
   {
      current_prov_file = browser_command->prov_file_ref;
      cmd.c.browser.prov_file_ref[i].prov_file_id_len = current_prov_file->c_prov_file_id;
      cmd.c.browser.prov_file_ref[i].prov_file_id = (U8*)current_prov_file->prov_file_id;
      current_prov_file++;
   }     
   cmd.c.browser.text_string.icon.qual = browser_command->icon.icon_qu;
   cmd.c.browser.text_string.icon.id = browser_command->icon.icon_id;
/* Copy the text string into the cmd buffer */
   {
     U8 *provPointer;
    provPointer = (U8 *) &cmd + sizeof(SatCmd);/* end of command buffer */
    provPointer -= browser_command->text.c_text_str;  /*Substract length of the text */
    memcpy(provPointer,browser_command->text.text_str,browser_command->text.c_text_str);/*Copies text*/
    cmd.c.browser.text_string.text = (U16) (provPointer - (U8*)&cmd.c.browser.text_string);/*Stores the offset */
    cmd.c.browser.text_string.len = browser_command->text.c_text_str;
    cmd.c.browser.text_string.code = browser_command->text.dcs;
   }
/* Copy the alpha id */
   cmd.c.browser.alpha_len = browser_command->c_alpha_id;
   cmd.c.browser.alpha_id = (U8*)browser_command->alpha_id;
   

   satSignal(MfwSatLaunchBrowser, &cmd); /*Send the event to MMI */
	

}

#endif

// Aug 31, 2005  REF: CRR 33619  x0021334
// Description: RE: Notification of SATK File Change
// Solution: Added stub function 'rAT_PercentSIMEF()'
/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework             MODULE  : MFW_SAT                                    |
| STATE     : code                             ROUTINE  : rAT_PercentSIMEF                       |
+--------------------------------------------------------------------+

  PURPOSE : handles rAT_PercentSIMEF call back
*/
void rAT_PercentSIMEF (T_SIM_FILE_UPDATE_IND *sim_file_update_ind)
{
	TRACE_FUNCTION ("rAT_PercentSIMEF()");
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : satAccept          |
+--------------------------------------------------------------------+

  PURPOSE : accept SIM initiated connection

*/

void satAccept (void)
{
    T_ACI_RETURN res;

    TRACE_FUNCTION("satAccept()");

    mfwSatMoAccepted = 1;
    res = sAT_A(CMD_SRC_LCL);
    if (res != AT_EXCT && res != AT_CMPL)
    {
        TRACE_ERROR("satAccept() error");
        mfwSatMoAccepted = 0;
    }

    return;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : satReject          |
+--------------------------------------------------------------------+

  PURPOSE : reject SIM initiated connection

*/

void satReject (void)
{
    T_ACI_RETURN res;

    TRACE_FUNCTION("satReject()");

    res = sAT_H(CMD_SRC_LCL);
    if (res != AT_EXCT && res != AT_CMPL)
        TRACE_ERROR("satReject() error");

    return;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : satDisc            |
+--------------------------------------------------------------------+

  PURPOSE : disconnect SIM initiated connection

*/

void satDisc (void)
{
    T_ACI_RETURN res;

    TRACE_FUNCTION("satDisc()");

    res = sAT_Abort(CMD_SRC_LCL,AT_CMD_A);
    if (res != AT_EXCT && res != AT_CMPL)
        TRACE_ERROR("satDisc() error");

    return;
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT          |
| STATE   : code                        ROUTINE : satChangeNotify     |
+--------------------------------------------------------------------+

  PURPOSE : File change notification

*/
#ifdef FF_2TO1_PS
BOOL satChangeNotify ( int ref, T_SIM_FILE_UPDATE_IND *updateInd)
{
  int i;
  int cnt;
  UBYTE f = 0;
  U32 evt = 0;

  TRACE_FUNCTION ("satChangeNotify ()");


	//send a notification to the MMI as well
#ifdef OLD_MFW_SAT_REFRESH
	evt = decRefresh();  /* JVJ. This is not sending the proper information!!!!*/
	satSignal(evt, &cmd);
#endif



  memset ( &udNotify, 0, sizeof(udNotify));
  /***************************Go-lite Optimization changes Start***********************/
  //Aug 16, 2004    REF: CRR 24323   Deepa M.D
  TRACE_EVENT_P5("file_num: %d, files: %x %x %x %x", updateInd->val_nr,
	  updateInd->file_info[0].datafield, updateInd->file_info[1].datafield,
	  updateInd->file_info[2].datafield, updateInd->file_info[3].datafield);
  /***************************Go-lite Optimization changes END***********************/
  fuRef = ref;
  cnt = 0;
  for (i = 0; i < (int)updateInd->val_nr; i++)
  {
    if (updateInd->file_info[i].datafield EQ SIM_GID1 OR
        updateInd->file_info[i].datafield EQ SIM_GID2 OR
        updateInd->file_info[i].datafield EQ SIM_CPHS_CINF OR
        updateInd->file_info[i].datafield EQ SIM_SST OR
        updateInd->file_info[i].datafield EQ SIM_SMS OR
        updateInd->file_info[i].datafield EQ SIM_SPN OR
        updateInd->file_info[i].datafield EQ SIM_LP )
    {
      if (cnt < MFW_MAX_NT_SIZE)
      {
        udNotify.fields[cnt] = updateInd->file_info[i].datafield;
        cnt++;
        if (updateInd->file_info[i].datafield EQ SIM_SMS) // bugfix for SIM refresh
          sms_busy_ind(); /* When SMS fields will be updated, MFW sends SMS BUSY event to MMI */
      }
    }
  }
  udNotify.count = cnt;

  if (!cnt)
  {
    if (fuRef >= 0)
        fuRef = -1;
    return TRUE;
  }

  for ( i=0; i < (int)udNotify.count; i++)
  {
    if ( udNotify.fields[i] EQ SIM_SST )
      f = 1;
  }

  if (udNotify.count)
  {
    if (f)
    {
      f = 0;
      sim_sat_file_update(SIM_SST);
    }
    else
      satUpdateFiles(FALSE, 0);
  }

  return FALSE;
}
#else
BOOL satChangeNotify ( int ref, T_SIM_FILE_UPDATE_IND *updateInd)
{
  int i;
  int cnt;
  UBYTE f = 0;
#if defined(OLD_MFW_SAT_REFRESH)
  U32 evt = 0;
#endif

  TRACE_FUNCTION ("satChangeNotify ()");


	//send a notification to the MMI as well
#ifdef OLD_MFW_SAT_REFRESH
	evt = decRefresh();  /* JVJ. This is not sending the proper information!!!!*/
	satSignal(evt, &cmd);
#endif



  memset ( &udNotify, 0, sizeof(udNotify));
  /***************************Go-lite Optimization changes Start***********************/
  //Aug 16, 2004    REF: CRR 24323   Deepa M.D

  // x0047685 Due to R99 SIM Interface changes, file_id[i] has been
  // replaced by file_info[i].datafield
  TRACE_EVENT_P5("file_num: %d, files: %x %x %x %x", updateInd->val_nr,
    updateInd->file_info[0].datafield, updateInd->file_info[1].datafield,
    updateInd->file_info[2].datafield, updateInd->file_info[3].datafield);
  /***************************Go-lite Optimization changes END***********************/



  fuRef = ref;
  cnt = 0;
  for (i = 0; i < (int)updateInd->val_nr; i++)
  {
    if (updateInd->file_info[i].datafield EQ SIM_GID1 OR
        updateInd->file_info[i].datafield EQ SIM_GID2 OR
        updateInd->file_info[i].datafield EQ SIM_CPHS_CINF OR
        updateInd->file_info[i].datafield EQ SIM_SST OR
        updateInd->file_info[i].datafield EQ SIM_SMS OR
        updateInd->file_info[i].datafield EQ SIM_SPN OR
        updateInd->file_info[i].datafield EQ SIM_LP
		         )
    {
      if (cnt < MFW_MAX_NT_SIZE)
      {
        udNotify.fields[cnt] = updateInd->file_info[i].datafield;
        cnt++;
        if (updateInd->file_info[i].datafield EQ SIM_SMS) // bugfix for SIM refresh
          sms_busy_ind(); /* When SMS fields will be updated, MFW sends SMS BUSY event to MMI */
      }
    }
  }

  udNotify.count = cnt;

  if (!cnt)
  {
    if (fuRef >= 0)
        fuRef = -1;
    return TRUE;
  }

  for ( i=0; i < (int)udNotify.count; i++)
  {
    if ( udNotify.fields[i] EQ SIM_SST )
      f = 1;
  }

  if (udNotify.count)
  {
    if (f)
    {
      f = 0;
      sim_sat_file_update(SIM_SST);
    }
    else
      satUpdateFiles(FALSE, 0);
  }

  return FALSE;
}


#endif

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT            |
| STATE   : code                        ROUTINE : satUpdateFiles     |
+--------------------------------------------------------------------+

  PURPOSE : Update changed files
            updated = TRUE, dataId field is updated
            updated = FALSE, no field is updated

*/

void satUpdateFiles ( U8 updated, USHORT dataId )
{
  int i;

  TRACE_FUNCTION("satUpdateFiles()");

  /* remove the updated file from list */
  if (updated)
  {
    for (i=0; i < MFW_MAX_NT_SIZE; i++)
    {
      if (udNotify.fields[i] EQ dataId)
      {
        udNotify.count--;
        udNotify.fields[i] = 0;
      }
    }
  }

  if (!udNotify.count)
  {
    if (fuRef >= 0)
    {
        psaSAT_FUConfirm (fuRef, SIM_FU_SUCCESS);
        fuRef = -1;
    }
    return;
  }

  /* update the next file */
  for ( i=0; i < MFW_MAX_NT_SIZE; i++)
  {
    if ( udNotify.fields[i] )
    {
      switch ( udNotify.fields[i] )
      {
        case SIM_GID1:
          sim_sat_file_update( SIM_GID1 );
          return;
        case SIM_GID2:
          sim_sat_file_update( SIM_GID2 );
          return;
        case SIM_CPHS_CINF:
          cphs_sat_file_update( SIM_CPHS_CINF );
          return;
        case SIM_SMS:
          sms_sat_file_update( SIM_SMS );
          return;
        case SIM_SPN:
          nm_sat_file_update( SIM_SPN );
          return;
        case SIM_LP:
		  mfwSATLPRefresh = TRUE;
          sim_sat_file_update( SIM_LP );
          return;
        default:
          break;
      }
    }
  }
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT           |
| STATE   : code                        ROUTINE : satRefreshNotification |
+--------------------------------------------------------------------+

  PURPOSE : File change notification start and refresh confirmatio

*/

BOOL satRefreshNotification ( int ref, T_SIM_FILE_UPDATE_IND *updateInd)
{
  U32 evt=0;

  TRACE_FUNCTION("satRefreshNotification");

	if (ref<0)					/*File update indication arrives from ACI */
	{							/* MMI has to be notified once	  */

  /*
   * x0047685 Due to R99 SIM Interface changes, file_id[i] has been
   * replaced by file_info[i].datafield.
   * Also removed FF_2TO1_PS flag, since in both cases we need to 
   * access EF using file_info and not file_id.
   */
    /*a0393213 OMAPS00116309 Data type of 'files' changed from U16* to T_file_info*
	because of R99 change in ACI*/
    cmd.c.refFiles.files = updateInd->file_info;
		cmd.c.refFiles.files_nr = updateInd->val_nr;
		cmd.c.refFiles.status = ref;
		if (updateInd->val_nr)
			evt = MfwSatRefresh;
		if (evt)
			satSignal(evt, &cmd);
		if (fu_aborted)
		{
			fu_aborted = FALSE;
			return FALSE;
		}
	}
	else
	{
    /*a0393213 OMAPS00116309 Data type of 'files' changed from U16* to T_file_info*
	because of R99 change in ACI*/	
    cmd.c.refFiles.files = updateInd->file_info;
		cmd.c.refFiles.files_nr = updateInd->val_nr;
		cmd.c.refFiles.status = ref;
		if (updateInd->val_nr)
			evt = MfwSatDataRefreshed;
		if (evt)
			satSignal(evt, &cmd);
	}
	return TRUE;
}




/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT           |
| STATE   : code                        ROUTINE : satRefreshAbort     |
+--------------------------------------------------------------------+

  PURPOSE : MMI aborts the file update notification. It should answer the event MfwSatRefresh
*/

void satRefreshAbort()
{
	fu_aborted = TRUE;
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT           |
| STATE   : code                        ROUTINE : satBrowserTerminated |
+--------------------------------------------------------------------+

  PURPOSE : downloads the Browser termination event to the SAT.

  Cause can be, 

		 SatBrowserTerminCauseUser 0x00
		 SatBrowserTerminCauseError 0x01

SPR#2121 - DS - Dynamically allocate memory for Terminal Response.

*/

#if defined(FF_WAP)

MfwRes satBrowserTerminated(U8 cause)
{
    S16 ti = 0;
    U8* trmResponse = NULL;

    TRACE_FUNCTION("satBrowserTerminated");

    /* Allocate memory for Terminal Response buffer */
    trmResponse = (U8*)mfwAlloc(TRM_RSP_LEN);

    if (trmResponse == NULL)
    {
        /* Memory allocation failed */
        TRACE_ERROR("ERROR: Failed to allocate memory for TR buffer");
        return MfwResNoMem;
    }

    trmResponse[ti++] = 0xD6;           /* event download tag       */
    trmResponse[ti++] = 0;              /* length (filled later)    */
    trmResponse[ti++] = 0x99;           /* --event list--           */
    trmResponse[ti++] = 0x01;           /* length                   */
    trmResponse[ti++] = SatEvtBrowserTermination;    /* actual event             */
    trmResponse[ti++] = 0x82;           /* --device identities--    */
    trmResponse[ti++] = 0x02;           /* length                   */
    trmResponse[ti++] = SatDevME;       /* source                   */
    trmResponse[ti++] = 0x81;           /* destination: SIM         */
    trmResponse[ti++] = 0x34;		  /* Browser Termination Cause tag */	
    trmResponse[ti++] = 0x01;   	  /* Browser Termination length*/
    trmResponse[ti++] = cause;         /* termination cause   */
    trmResponse[1] = ti - 2;            /* length of event download */

    sAT_PercentSATE(CMD_SRC_LCL,ti,trmResponse);

    /* Deallocate memory used for Terminal Response buffer */
    if (trmResponse != NULL)
    {
        mfwFree(trmResponse, TRM_RSP_LEN);
        trmResponse = NULL;
    }

    return MfwResOk;

}

#endif

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_SAT           |
| STATE   : code                        ROUTINE : sate_error |
+--------------------------------------------------------------------+

  PURPOSE : Handles errors returned from SAT.

 SPR#2321 - DS - Function created.

*/
void sate_error(T_ACI_CME_ERR error)
{
	TRACE_FUNCTION("sate_error()");

	TRACE_EVENT_P1("SATE Error: %d", error);

	if (error == CME_ERR_SimSatBusy)
	{
		/* Close any open connections */
		satDisc();

		/* Send "temporary failure" terminal response so SIM can restart the busy SAT Application */
		satResponse(SatResBusyME);

		/* Send event to BMI so user can be informed of error */
		satSignal(MfwSatErrBusy, (void*)error);
	}
}

// Jun 16, 2005    REF: CRR 31543   x0021334

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework     MODULE  : MFW_SAT                                            |
| STATE   : code                        ROUTINE : satHangup                                         |
+--------------------------------------------------------------------+

  PURPOSE : Handles Hang up key event during USSD session.

*/
T_MFW_SS_RETURN satHangup (T_ACI_CMD_SRC sId)
{
	T_ACI_RETURN result;

	TRACE_FUNCTION("satHangup()");
	
	result = sAT_end_ussd(sId); // end the USSD session
	return ((result EQ AT_CMPL) ? MFW_SS_OK : MFW_SS_FAIL);
}
