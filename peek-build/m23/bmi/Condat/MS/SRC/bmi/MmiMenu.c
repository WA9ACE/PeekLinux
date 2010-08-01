/*******************************************************************************

          CONDAT (UK)

********************************************************************************

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.

********************************************************************************

 $Project name: Basic MMI
 $Project code: BMI (6349)
 $Module:   Menus
 $File:       MmiMenu.c
 $Revision:   1.0

 $Author:   Condat(UK)
 $Date:       25/10/00

********************************************************************************

 Description:

    This provides the implementation of the main menu handling for the basic
  MMI

********************************************************************************
 $History: MmiMenu.c

  25/10/00      Original Condat(UK) BMI version.

 $End

*******************************************************************************/


/*******************************************************************************

                                Include files

*******************************************************************************/

#define ENTITY_MFW

/* includes */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#if defined (NEW_FRAME)

#include "typedefs.h"
#include "vsi.h"
#include "pei.h"
#include "custom.h"
#include "gsm.h"

#else

#include "STDDEFS.H"
#include "custom.h"
#include "gsm.h"
#include "vsi.h"

#endif
#include "mfw_sys.h"

#include "prim.h"


#include "mfw_mfw.h"
#include "mfw_win.h"
#include "mfw_kbd.h"
#include "mfw_lng.h"
/* SPR#1428 - SH - New Editor changes */
#ifndef NEW_EDITOR
#include "mfw_edt.h"
#endif
#include "mfw_icn.h"
#include "mfw_mnu.h"
#include "mfw_phb.h"
#include "mfw_sim.h"
#include "mfw_nm.h"
#include "mfw_tim.h"
#include "mfw_sms.h"
#include "mfw_icn.h"
#include "mfw_sat.h"

#include "dspl.h"

#include "ksd.h"
#include "psa.h"


#include "MmiMmi.h"
#include "MmiMain.h"
#include "MmiStart.h"

#include "MmiDummy.h"
#include "MmiDialogs.h"
#include "MmiLists.h"
#include "mmiSmsBroadcast.h"

#include "MmiMenu.h"
#include "MmiSimToolkit.h"


#include "MmiSounds.h"
#include "MmiSettings.h"
#include "MmiServices.h"
#include "MmiPins.h"
#include "Mmiicons.h"
#include "MmiNetwork.h"
#include "MmiUserData.h"
#include "MmiSoftKeys.h"
#include "MmiTimeDate.h"
#include "MmiIdle.h"


#include "cus_aci.h"
#include "DataObject.h"
#include "Application.h"
#include "StaticApplications.h"
#include "ApplicationManager.h"

#include "prim.h"
#ifndef PCM_2_FFS
#include "pcm.h"
#endif

/*******************************************************************************

                                Private definitions

*******************************************************************************/


/* next define used for code that might be removed
*/
#define KEEP 0




/*******************************************************************************

                               Global Data Elements

*******************************************************************************/

/* Menus display area
*/



/*******************************************************************************

                               Module Global Data Elements

*******************************************************************************/

MfwHnd MenuWin = (MfwHnd) 0;

static MfwHnd win=0;                    /* our window               */

/* x0039928 -Lint warning removal 
static MfwHnd kbd;                     
static MfwHnd kbdLong;  */


static SimMenuFunc SimCallback;
static MfwHnd      SimWindow;

static MfwHnd	 emobiixWindow = (MfwHnd) 0;
static int emoWinCreated = 0;

static tEmoMenuData *tEmoMenuItem = NULL;

/* x0039928 - Lint warning removal
static MmiState nextState;              
static BOOL fromHotKey;
static BOOL flagHotKey;  

static const int OperatorIndex = 1;
*/

/*******************************************************************************

                               Local function prototypes

*******************************************************************************/

/*******************************************************************************

 $Function:     menuSimItemCallback

 $Description:  invokes the SimCallback function if it is allocated

 $Returns:    MfwResOk, always

 $Arguments:  m, menu being activated, i, item in menu

*******************************************************************************/

int menuSimItemCallback( MfwMnu* m, MfwMnuItem* i)
{
    if( SimCallback != NULL )
      SimCallback(SimWindow);

    return MfwResOk;
}

void updateScreen(void);
void lgui_set_dirty(void);
void manager_handleKey(int key);


static void menuEmobiixDestroy(MfwHnd own_window)
{
    T_MFW_WIN 	     * win_data;

        emo_printf("menuEmobiixDestroy");

        if(own_window) {
           	win_data = ((T_MFW_HDR *)own_window)->data;
            	tEmoMenuItem = (tEmoMenuData *)win_data->user;

                win_delete(tEmoMenuItem->win);
        }
}

static int winEmobiixCB( MfwEvt e, MfwWin *w )
{
	emo_printf("winEmobiixCB() event %d", e);

	switch(e)
	{
		case MfwWinVisible:
                        //dspl_Clear( 0,0, SCREEN_SIZE_X,SCREEN_SIZE_Y);
			lgui_set_dirty();
			updateScreen();		
			dspl_Enable(1);
			break;
		default:
			return MFW_EVENT_PASSED;
	}

	return MFW_EVENT_CONSUMED;
}

static int kbdEmobiixCB(MfwEvt e, MfwKbd *k)
{
    	T_MFW_HND     win  = mfwParent(mfw_header());
    	T_MFW_WIN     *win_data = ((T_MFW_HDR *)win)->data;
    	
	tEmoMenuItem = (tEmoMenuData *)win_data->user;

	emo_printf("kbdEmobiixCB() in callback");

	switch (k->code)
	{
		case KCD_RIGHT:
		case KCD_HUP:
			menuEmobiixDestroy(tEmoMenuItem->win);
			free(tEmoMenuItem);
			break;

		case KCD_MNUSELECT:
			manager_handleKey(13);
			break;

		case KCD_MNUUP:
			manager_handleKey(87);
			break;
		
		case KCD_MNUDOWN:
			manager_handleKey(86);
			break;
	}

	return MFW_EVENT_CONSUMED;
}

#define URL_ALL                 0x3FF
typedef struct URL_t URL;
typedef struct ConnectionContext_t ConnectionContext;
extern int connectionContext_syncRequest(ConnectionContext *, URL *);
extern int connectionContext_loopIteration(ConnectionContext *ctx);
extern URL *url_parse(const char *URL, unsigned int parts);
extern ConnectionContext *connectionContext;

int menuEmobiixItemCallback(MfwMnu* m, MfwMnuItem* i) {
	URL *purl = url_parse("tcp://10.150.9.6:12345/calc", URL_ALL);
        emo_printf("menuEmobiixItemCallback()");
        return menuEmobiixItemCB(purl, m, i);
}

int menuEmobiixItemCallback2(MfwMnu* m, MfwMnuItem* i) {
        URL *purl = url_parse("tcp://10.150.9.6:12345/whereami", URL_ALL);
        emo_printf("menuEmobiixItemCallback2()");
        return menuEmobiixItemCB(purl, m, i);
}

int menuEmobiixItemCallback3(MfwMnu* m, MfwMnuItem* i) { 
        URL *purl = url_parse("tcp://10.150.9.6:12345/mail", URL_ALL);
        emo_printf("menuEmobiixItemCallback3()");
        return menuEmobiixItemCB(purl, m, i);
}

int menuEmobiixItemCallback4(MfwMnu* m, MfwMnuItem* i) { 
        URL *purl = url_parse("tcp://10.150.9.6:12345/helloworld", URL_ALL);
        emo_printf("menuEmobiixItemCallback4()");
        return menuEmobiixItemCB(purl, m, i);
}

int menuEmobiixItemCallback5(MfwMnu* m, MfwMnuItem* i) { 
        URL *purl = url_parse("tcp://10.150.9.6:12345/sample", URL_ALL);
	emo_printf("menuEmobiixItemCallback5()");
        return menuEmobiixItemCB(purl, m, i);
}

void menuEmobiixDialogCB (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
    U8 * key_code;
    T_MFW_WIN      * win_data = ((T_MFW_HDR *) win)->data;
    
    emo_printf("menuEmobiixDialogCB()");

    switch(event) {
		case SCREEN_UPDATE:
			emo_printf("menuEmobiixDialogCB() Got Screen Update\n");
			lgui_set_dirty();
			updateScreen();
			dspl_Enable(1);
			break;
    }
}

int menuEmobiixItemCB(URL *purl, MfwMnu* m, MfwMnuItem* i)
{
	T_MFW_HND  parent = mfwParent(mfw_header());
        DataObject *dobj;
        T_MFW_WIN  * win;
	Application *App;

	if(!tEmoMenuItem) {
		tEmoMenuItem = (tEmoMenuData *)ALLOC_MEMORY (sizeof (tEmoMenuData ));
	}

        emo_printf("menuEmobiixItemCB()");

        tEmoMenuItem->win = win_create(mfwParent(mfw_header()), 0, E_WIN_VISIBLE, (T_MFW_CB)winEmobiixCB);
        kbdCreate(tEmoMenuItem->win, KEY_ALL, (MfwCb)kbdEmobiixCB);
        //connectionContext_syncRequest(connectionContext, url);
        //connectionContext_loopIteration(connectionContext);

    	tEmoMenuItem->mmi_control.dialog    = (T_DIALOG_FUNC)menuEmobiixDialogCB;
    	tEmoMenuItem->mmi_control.data      = tEmoMenuItem;
    	win                         = ((T_MFW_HDR *)tEmoMenuItem->win)->data;
    	win->user                   = (void *)tEmoMenuItem;

        dobj = dataobject_locate(purl);
        if (dobj == NULL)
        {
		App = application_load(LoadingApplication());
		manager_launchApplication(App);
		manager_focusApplication(App);
	        lgui_set_dirty();
		updateScreen();
        	dspl_Enable(1);

		emo_printf("menuEmobiixItemCB() calling connectionContext_syncRequest()");
                connectionContext_syncRequest(connectionContext, purl);
                dobj = dataobject_locate(purl);
        } else {
		App = manager_applicationForDataObject(dobj);
		manager_focusApplication(App);
	}

        connectionContext_loopIteration(connectionContext);
        winShow(tEmoMenuItem->win);

        lgui_set_dirty();
        updateScreen();
	dspl_Enable(1);

        return MfwResOk;
}

MfwHnd * emoMenu_get_window (void)
{
	if(tEmoMenuItem)
        	return tEmoMenuItem->win;
	return NULL;
}


/*******************************************************************************

              Public Routines

*******************************************************************************/

/*******************************************************************************

 $Function:     item_flag_none

 $Description:  Attribut function for menu entries. This function shall
                be used for all cases of no specific attributes (default)

 $Returns:    zero always

 $Arguments:  m, ma, mi - not used

*******************************************************************************/

USHORT item_flag_none( struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi )
{
  return 0;
}

//xpradipg - Aug 4:changes for WAP2.0 Menu
#if defined (FF_WAP) && defined (FF_GPF_TCPIP)
USHORT item_flag_WAP_2_0( struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi )
{
	return (HTTP_Selected()?0:1);
}
#endif
/*******************************************************************************

 $Function:     item_flag_hide

 $Description:  Attribut function for menu entries. This function shall
              be used for all cases of hiding menu entries.

 $Returns:    MNU_ITEM_HIDE always

 $Arguments:  m, ma, mi - not used

*******************************************************************************/

USHORT item_flag_hide( T_MFW_MNU *m, T_MFW_MNU_ATTR *ma, T_MFW_MNU_ITEM *mi )
{
  return MNU_ITEM_HIDE;
}







/*******************************************************************************

 $Function:     menuIsFocussed

 $Description:  Returns 1 if the menu window has the focus, else returns 0.

 $Returns:    Returns 1 if the menu window has the focus, else returns 0.

 $Arguments:  None

*******************************************************************************/

int menuIsFocussed( void )
{
    return winIsFocussed( win );
}









/*******************************************************************************

 $Function:     menuEnableSimMenu

 $Description:  Unhides the SIM toolkit menu item, using <label> as the label.
        If <label> is 0, the default label is used. If the SIM toolkit
        menu item is invoked by the user, <simCallback> gets called
        with argument <window>, unless <simCallback> is 0.

 $Returns:    None

 $Arguments:  label, simCallBack, simWindow as indicated above

*******************************************************************************/

void menuEnableSimMenu( char *label, SimMenuFunc simCallback, MfwHnd simWindow )
{

  TRACE_FUNCTION("menuEnableSimMenu");

    SimCallback = simCallback;

  /* parameter to callback
  */
    SimWindow   = simWindow;
}






/*******************************************************************************

 $Function:     menuDisableSimMenu

 $Description:  Hides the SIM toolkit menu item

 $Returns:    None

 $Arguments:  None

*******************************************************************************/

void menuDisableSimMenu(void)
{
  TRACE_FUNCTION("menuDisableSimMenu");
}

/*******************************************************************************

              End Of File

*******************************************************************************/

