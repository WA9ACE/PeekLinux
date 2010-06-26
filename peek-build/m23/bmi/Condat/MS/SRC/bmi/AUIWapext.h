/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:                                                   
 $Project code:	                                                           
 $Module:		
 $File:		    AUIWapext.h
 $Revision:		                                                      
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		                                                          
                                                                               
********************************************************************************
                                                                              
 Description:
 	External prototypes for AUI WAP.
    
                        
********************************************************************************

 $History: AUIWapext.h

	15/05/2003 - SPR#1983 - SH - Updated to latest from 1.6.3 version.
	   
 $End

*******************************************************************************/
#ifdef FF_GPF_TCPIP
typedef enum
{
	WAP_BROWSER,

	WAP_PUSH,

#ifdef FF_MMI_MMS
	WAP_MMS,
#endif
	WAP_DOWNLOAD
}
T_WAP_APPLICATION;

typedef void (*T_WAP_CB) (UBYTE result);

void AUI_wap_init(void);
void AUI_wap_start(T_WAP_APPLICATION application, T_WAP_CB callback);
void AUI_wap_connect_URL (char *URL);  /* SPR#2086 - modified */
/* WAP TEST */
int AUI_download(MfwMnu* menu, MfwMnuItem* item);
#endif /* #ifdef FF_GPF_TCPIP */
int AUI_menu(MfwMnu* menu, MfwMnuItem* item);
int AUI_homepage(MfwMnu* menu, MfwMnuItem* item);
int AUI_goto_www(MfwMnu* menu, MfwMnuItem* item);
int AUI_goto_wap(MfwMnu* menu, MfwMnuItem* item);
int AUI_goto_other(MfwMnu* menu, MfwMnuItem* item);
int AUI_bookmarks_add(MfwMnu* menu, MfwMnuItem* item);
int AUI_bookmarks_add_www(MfwMnu* menu, MfwMnuItem* item);
int AUI_bookmarks_add_wap(MfwMnu* menu, MfwMnuItem* item);
int AUI_bookmarks_add_other(MfwMnu* menu, MfwMnuItem* item);
int AUI_bookmarks_list(MfwMnu* menu, MfwMnuItem* item);
int AUI_bookmarks_goto(MfwMnu* menu, MfwMnuItem* item);
int AUI_bookmarks_edit(MfwMnu* menu, MfwMnuItem* item);
int AUI_bookmarks_edit_url(MfwMnu* menu, MfwMnuItem* item);
int AUI_bookmarks_delete(MfwMnu* menu, MfwMnuItem* item);
int AUI_bookmarks_sendSMS(MfwMnu* menu, MfwMnuItem* item);
int AUI_history_list(MfwMnu* menu, MfwMnuItem* item);
int AUI_history_goto(MfwMnu* menu, MfwMnuItem* item);
int AUI_history_edit(MfwMnu* menu, MfwMnuItem* item);
int AUI_history_edit_url(MfwMnu* menu, MfwMnuItem* item);
int AUI_history_delete(MfwMnu* menu, MfwMnuItem* item);
int AUI_history_clear(MfwMnu* menu, MfwMnuItem* item);
int AUI_history_addbook(MfwMnu* menu, MfwMnuItem* item);
int AUI_history_sendSMS(MfwMnu* menu, MfwMnuItem* item);

#ifdef FF_GPF_TCPIP
int AUI_profiles_list(MfwMnu* menu, MfwMnuItem* item);
#else
/* SPR#2324 - SH - Removed AUI_profiles_list */
#endif
int AUI_profile_change(MfwMnu* menu, MfwMnuItem* item); /* SPR#2324 - SH - Added */
int AUI_profile_delete(MfwMnu* menu, MfwMnuItem* item); /* SPR#2324 - SH - Added */
int AUI_profile_new(MfwMnu* menu, MfwMnuItem* item); /* SPR#2324 - SH - Added */
int AUI_profile_reset(MfwMnu* menu, MfwMnuItem* item); /* SPR#2324 - SH - Added */
/* SPR#2324 - SH - Removed AUI_profiles_list */
int AUI_profiles_setup(MfwMnu* menu, MfwMnuItem* item);
int AUI_profile_name_edit(MfwMnu* menu, MfwMnuItem* item);
int AUI_profile_select(MfwMnu* menu, MfwMnuItem* item); /* SPR#2324 - SH - Added *///liuyu

int AUI_homepage_edit(MfwMnu* menu, MfwMnuItem* item);
int AUI_connectionType(MfwMnu* menu, MfwMnuItem* item);
int AUI_connectionType_set(MfwMnu* menu, MfwMnuItem* item);
int AUI_connectionSpeed(MfwMnu* menu, MfwMnuItem* item);
int AUI_connectionSpeed_set(MfwMnu* menu, MfwMnuItem* item);
//xpradipg - Aug 4:changes for WAP2.0 Menu
#ifdef FF_GPF_TCPIP
int 	AUI_WirelessProfiledHTTP_set(MfwMnu* menu, MfwMnuItem* item);
int 	AUI_PPGAuthentication_set(MfwMnu* menu, MfwMnuItem* item);
int 	AUI_WirelessProfiledHTTP(MfwMnu* menu, MfwMnuItem* item);
int 	AUI_PPGAuthentication(MfwMnu* menu, MfwMnuItem* item);
int 	AUI_NameServer1(MfwMnu* menu, MfwMnuItem* item);
int 	AUI_NameServer2(MfwMnu* menu, MfwMnuItem* item);
EXTERN BOOL HTTP_Selected();
#endif
int AUI_security(MfwMnu* menu, MfwMnuItem* item);
int AUI_security_set(MfwMnu* menu, MfwMnuItem* item);
int AUI_access(MfwMnu* menu, MfwMnuItem* item);
int AUI_access_set(MfwMnu* menu, MfwMnuItem* item);
int AUI_dialupNumber(MfwMnu* menu, MfwMnuItem* item);
int AUI_IPAddress1(MfwMnu* menu, MfwMnuItem* item);
int AUI_IPAddress2(MfwMnu* menu, MfwMnuItem* item);
int AUI_Port1(MfwMnu* menu, MfwMnuItem* item);
int AUI_Port2(MfwMnu* menu, MfwMnuItem* item);
int AUI_APN(MfwMnu* menu, MfwMnuItem* item);
int AUI_username(MfwMnu* menu, MfwMnuItem* item);
int AUI_password(MfwMnu* menu, MfwMnuItem* item);
int AUI_response_timer(MfwMnu* menu, MfwMnuItem* item);
int AUI_savehist(MfwMnu* menu, MfwMnuItem* item);
int AUI_savehist_set(MfwMnu* menu, MfwMnuItem* item);
int AUI_scaleimages(MfwMnu* menu, MfwMnuItem* item);
int AUI_scaleimages_set(MfwMnu* menu, MfwMnuItem* item);
#ifdef FF_GPF_TCPIP
T_MFW_HND AUI_wap_launch_browser(char *URL,UBYTE* bearer_list,UBYTE bearer_list_len);
#endif
void AUI_wap_extDestroy(void);
void AUI_wap_gprs_status(UBYTE status);
void AUI_wap_gprs_cb(UBYTE status);//xrashmic 13 Feb, 2006 MMI-SPR-58532

void AUI_wap_call_connect(SHORT cId);
void AUI_wap_call_disconnect(SHORT cId);
void AUI_end_call(SHORT cId);
UBYTE AUI_wap_in_call(void);
void AUI_wap_browser_disconnect(void);  // RAVI
#ifdef FF_GPF_TCPIP
void AUI_wap_content_get(UBYTE id, char *Url, BOOL reload, char *acceptHeader);

int AUI_wap_push_list(MfwMnu* menu, MfwMnuItem* item);
int AUI_wap_push_message_read(MfwMnu* menu, MfwMnuItem* item);
int AUI_wap_push_message_delete(MfwMnu* menu, MfwMnuItem* item);
int AUI_wap_push_showmessages(MfwMnu* menu, MfwMnuItem* item);
int AUI_wap_push_showmessages_set(MfwMnu* menu, MfwMnuItem* item);
#endif
