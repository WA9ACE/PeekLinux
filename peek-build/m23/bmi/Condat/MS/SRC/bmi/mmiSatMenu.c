
/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		MMI
 $File:		    MmiSatMenu.c
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:
 
  Implementation of MMI SIM Application Toolkit (SAT)
                        
********************************************************************************

 $History: MmiSatMenu.c



 
    	Jun 06, 2006 DR: OMAPS00080543 - xreddymn
    	Description: Default item is not being highlighted for SELECT ITEM command.
    	Solution: Change list menu highlight index based on the default item provided
    	in the SELECT ITEM command.

    	June 05, 2006 REF:OMAPS00060424 x0045876
 	Description: Header Toggling
 	Solution: Implemented the toggling of header when displaying the length text in SELECT ITEM and SET UP MENU

  	Apr 24, 2006    REF: DRT OMAPS00075832  x0039928
	Description: SIM tool kit crash on plus 2.5 image
	Fix:	Memory freeing of data->list_menu_data.Attr->icon is put under if conditon
	setupMenuHeaderIconData.dst != NULL for setup menu and selectItemHeaderIconData.dst != NULL
	for select item.

       Shashi Shekar B.S., a0876501, 16 Mar, 2006, OMAPS00061462
       Icon support for SetupMenu & Select item.

       xrashmic 5 Oct, 2005 MMI-SPR-29356, MMI-SPR-29357
       Using the MenuSelect Key for requesting the help info in STK menu.
       
  	May 24, 2005    REF: CRR 29358  x0021334
	Description: CT_PTCRB 27.22.4.9.8 fails: wrong terminal response
	Fix:	The implementation is now based on timer. After the specified timeout period
	       the control comes back to SAT main menu, if there is no selection done by the user.
		
	Aug 16, 2004    REF: CRR 24323   Deepa M.D
	Bug:Clenup of sprintf used for tracing
	Fix:Replace the char buf[]; sprintf (buf, "...", ...); TRACE_EVENT (buf); statements by TRACE_EVENT_PX

	25/10/00			Original Condat(UK) BMI version.	
	   
 $End

*******************************************************************************/



/*******************************************************************************
                                                                              
                                Include Files
                                                                              
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
/* SPR#1428 - SH - New Editor changes */
#ifndef NEW_EDITOR
#include "mfw_edt.h"
#endif
#include "mfw_lng.h"
#include "mfw_icn.h"
#include "mfw_phb.h"
#include "mfw_sim.h"
#include "mfw_nm.h"
#include "mfw_sms.h"
#include "mfw_mnu.h"
#include "mfw_sat.h"
#include "mfw_tim.h"

#include "ksd.h"
#include "psa.h"
#include "dspl.h"


#include "MmiMmi.h"
#include "MmiDummy.h"
#include "MmiDialogs.h"
#include "MmiLists.h"

#include "MmiMain.h"
#include "MmiStart.h"
#include "MmiPins.h"
#include "MmiMenu.h"
#include "MmiSoftKeys.h"
#include "MmiSounds.h"
#include "mmiCall.h"

#include "mmiSat_i.h"

#include "cus_aci.h"

#include "prim.h"
#ifndef PCM_2_FFS
#include "pcm.h"
#endif


#include "mmiColours.h"

BOOL g_SATsession_is_active = FALSE;

// May 24, 2005    REF: CRR 29358  x0021334
static BOOL g_sat_scroll_status = FALSE;	// to determine if the user has scrolled up/down
BOOL getScrollStatus(void);	// function prototype

// Shashi Shekar B.S., a0876501, Feb 4, 2006, DR: OMAPS00061462
#ifdef FF_MMI_SAT_ICON
static T_SAT_IconInfo setupMenuItemIconData, setupMenuHeaderIconData;
static T_SAT_IconInfo selectItemIconData, selectItemHeaderIconData;
#endif


/*********************************************************************
**********************************************************************

                        setup_menu       DYNAMIC MENU WINDOW. DECLARATION

*********************************************************************
**********************************************************************/
typedef struct
{
    /* administrative data */

    T_MMI_CONTROL   mmi_control;
    T_MFW_HND       win;
    T_MFW_HND		kbd;  /* sbh - keyboard handler, so window can be destroyed by user */

    /* associated handlers */

    /* internal data */
    T_SAT_CMD     *sat_command; /* pointer to sat_command in parent */
    ListMenuData_t    list_menu_data;
} T_sat_setup_menu;

static T_MFW_HND sat_setup_menu_create (T_MFW_HND parent);
static void sat_setup_menu_destroy (T_MFW_HND window);
static void sat_setup_menu_exec (T_MFW_HND win, USHORT event, SHORT value, T_SAT_CMD * sat_command);
static void sat_setup_menu_listmnu_cb (T_MFW_HND win, ListMenuData_t * ListData);
static T_MFW sat_setup_menu_recreate(T_sat_setup_menu *data);

// May 24, 2005    REF: CRR 29358  x0021334
// Prototype declaratio for sat_select_TimerCb().
static int sat_select_TimerCb (T_MFW_EVENT event, T_MFW_TIM *timer);

static MfwMnuAttr sat_setup_menuAttrib =
{
    &sat_setup_menuArea,
    MNU_LEFT | MNU_LIST | MNU_CUR_LINE, 
    /* x0045876, 14-Aug-2006 (WR - integer conversion resulted in a change of sign) */
    (UBYTE) -1,                                 /* use default font         */
    NULL,                               /* with these items         */
    0 ,                                  /* number of items     */
    COLOUR_LIST_XX, TxtNull, NULL, MNUATTRSPARE,
    /* 05-June-2006, x0045876 (OMAPS00060424 - Header Toggle) */
    TRUE

};

static MfwMnuAttr sat_select_menuAttrib =
{
    &sat_select_menuArea,
    MNU_LEFT | MNU_LIST | MNU_CUR_LINE, 
    /* x0045876, 14-Aug-2006 (WR - integer conversion resulted in a change of sign) */
    (UBYTE) -1,                                 /* use default font         */
    NULL,                               /* with these items         */
    0,                                   /* number of items     */
    COLOUR_LIST_XX, TxtNull, NULL, MNUATTRSPARE,
    /* 05-June-2006, x0045876 (OMAPS00060424 - Header Toggle) */
    TRUE
};

/* SPR#2492 - DS - Dynamically allocated string storage for SAT menu header */
static char* menuHdr = NULL;


/*********************************************************************
**********************************************************************

                        setup_menu      DYNAMIC MENU WINDOW. IMPLEMENTATION

*********************************************************************
**********************************************************************/

T_MFW_HND sat_setup_menu_start(T_SAT_CMD * sat_command)
{
    T_MFW_HND win;

    TRACE_FUNCTION ("sat_setup_menu_start()");
	/***************************Go-lite Optimization changes Start***********************/
	//Aug 16, 2004    REF: CRR 24323   Deepa M.D
	TRACE_EVENT_P1("MFW Memory Left after starting SAT %d",mfwCheckMemoryLeft());
	/***************************Go-lite Optimization changes end***********************/
	
	win = sat_setup_menu_create (NULL);

    if (win NEQ NULL)
        {
            SEND_EVENT (win, SAT_SETUP_MENU, 0, sat_command); 
        }
    return win;
}

static T_MFW_HND sat_setup_menu_create(MfwHnd parent_window)
{
    T_sat_setup_menu      * data = (T_sat_setup_menu *)ALLOC_MEMORY (sizeof (T_sat_setup_menu));
    T_MFW_WIN  * win;

    TRACE_EVENT ("sat_setup_menu_create()");

    /*
     * Create window handler
     */

    data->win = win_create (parent_window, 0, E_WIN_VISIBLE, (T_MFW_CB)sat_win_cb);
    sat_set_setup_menu_win(data->win); // c030 rsa

    if (data->win EQ NULL)
        {
            return NULL;
        }

    /* 
     * connect the dialog data to the MFW-window
     */

    data->mmi_control.dialog = (T_DIALOG_FUNC)sat_setup_menu_exec;
    data->mmi_control.data   = data;
    win                      = ((T_MFW_HDR *)data->win)->data;
    win->user                = (MfwUserDataPtr)data;

	data->kbd      = kbdCreate( data->win, KEY_ALL, (T_MFW_CB) sat_kbd_cb); /* sbh - add keyboard handler */
	
    /*
     * return window handle
     */


	g_SATsession_is_active = TRUE;

    win_show(data->win);
    return data->win;
}

static void sat_setup_menu_destroy(MfwHnd own_window)
{
    T_MFW_WIN            * win_data;
    T_sat_setup_menu     * data;
    int i;

    TRACE_EVENT ("sat_setup_menu_destroy()");

    g_SATsession_is_active = FALSE;

    if (own_window)
        {
            win_data = ((T_MFW_HDR *)own_window)->data;
            data = (T_sat_setup_menu *)win_data->user;

            if (data)
                {
                    /*
                     * Delete WIN handler
                     */ 
                    win_delete (data->win);

                    /*     
                     * Free Memory
                     */

// Shashi Shekar B.S., a0876501, Feb 4, 2006, DR: OMAPS00061462
#ifdef FF_MMI_SAT_ICON
		  /* Free the memory allocated for Item icon data. */
		   if(setupMenuItemIconData.dst != NULL)
		   {
			FREE_MEMORY((U8 *)setupMenuItemIconData.dst, setupMenuItemIconData.width * 
								setupMenuItemIconData.height);
			setupMenuItemIconData.dst = NULL;
		   }

		  /* Free the memory allocated for Header icon data. */
		   if(setupMenuHeaderIconData.dst != NULL)
		   {
			FREE_MEMORY((U8 *)setupMenuHeaderIconData.dst, setupMenuHeaderIconData.width * 
								setupMenuHeaderIconData.height);
			setupMenuHeaderIconData.dst = NULL;

			//Apr 24, 2006    REF: DRT OMAPS00075832  x0039928
			// Shashi Shekar B.S., a0876501, Feb 4, 2006, DR: OMAPS00061462
			   if(data->list_menu_data.Attr->icon != NULL)
			  {
				FREE_MEMORY((U8 *)data->list_menu_data.Attr->icon, sizeof (MfwIcnAttr));
				data->list_menu_data.Attr->icon = NULL;
			  }
		   }		   
#endif	
					
                     for (i=0; i < data->list_menu_data.ListLength; i++)
                        {
                            sat_destroy_ITEM_ASCIIZ (data->list_menu_data.List[i].str);

// Shashi Shekar B.S., a0876501, Feb 4, 2006, DR: OMAPS00061462
#ifdef FF_MMI_SAT_ICON
				if(data->list_menu_data.List[i].icon != NULL)
				{
					FREE_MEMORY((U8 *)data->list_menu_data.List[i].icon, sizeof (MfwIcnAttr));
					data->list_menu_data.List[i].icon = NULL;
				}
#endif							
                        }
                    FREE_MEMORY ((U8 *)data->list_menu_data.List, 
                                        (data->list_menu_data.ListLength * sizeof (MfwMnuItem)));
                    data->list_menu_data.List = NULL;
                    FREE_MEMORY ((U8 *)data, sizeof (T_sat_setup_menu));

                    /* SPR#2492 - DS - Free SAT menu header if it exists */
                    if (menuHdr)
                    {
                      TRACE_EVENT_P2("Destroy menuHdr %d with size %d", menuHdr, *(menuHdr -(U16)sizeof(U16)) );
			   sat_destroy_TEXT_ASCIIZ(menuHdr);
			   menuHdr = NULL; /* dsm 01/12/03 - Added to explicitly set menuHdr to NULL */
		   
                    }
                    
                    sat_set_setup_menu_win(NULL); // c030 rsa
                }
            else
                {
                    TRACE_EVENT ("sat_setup_menu_destroy() called twice");
                }
        }
}

static T_MFW sat_setup_menu_recreate(T_sat_setup_menu *data)
{
    int i;
    SatMenu  * menu  = &data->sat_command->c.menu;
    int Unicode_menu = FALSE; /*MC, SPR 940/2flag to lay-out as Unicode*/
	T_MFW	retVal;

// Shashi Shekar B.S., a0876501, Feb 4, 2006, DR: OMAPS00061462
#ifdef FF_MMI_SAT_ICON
	USHORT icon_length;
	USHORT fontHeight = 0, menuHeight = 0, titleHeight;
#endif

	TRACE_EVENT ("sat_setup_menu_recreate()");

#ifdef __COMPLETE_LIST_SUPPORT__ /* ??? rsa for future extension */
    if ((&data->sat_command.qual & SAT_M_SETUP_HELP_AVAIL) NEQ 0)
        {
            data->list_menu_data.AlternateLeftSoftKey  = TxtHelp; /* help available */
        }
    else
        {
            data->list_menu_data.AlternateLeftSoftKey  = TxtNull; /* no help available */
        }
#endif

     if (data->list_menu_data.List EQ NULL)
        {
            /* c030 rsa first time creation */
            /* allocate sufficient memory to hold the list of menu items */
            data->list_menu_data.ListLength = menu->nItems; /* actual number of entries in list menu.    */
            data->list_menu_data.List = (MfwMnuItem *)ALLOC_MEMORY (data->list_menu_data.ListLength * sizeof (MfwMnuItem));
            memset(data->list_menu_data.List, 0x00, data->list_menu_data.ListLength * sizeof (MfwMnuItem));

// Shashi Shekar B.S., a0876501, Feb 4, 2006, DR: OMAPS00061462
#ifdef FF_MMI_SAT_ICON
	     if(data->sat_command->c.menu.itemIconQual != 0xFF)
	     	{
	     	       /* We have icon to be displayed*/
			/* Copy the icon data to the editor attributes*/
			setupMenuItemIconData.width = data->sat_command->c.menu.items[0].iconInfo.width;  
			setupMenuItemIconData.height = data->sat_command->c.menu.items[0].iconInfo.height; 

			icon_length = data->sat_command->c.menu.items[0].iconInfo.width * data->sat_command->c.menu.items[0].iconInfo.height;
										
			setupMenuItemIconData.dst = (char *)ALLOC_MEMORY (icon_length);

			memcpy(setupMenuItemIconData.dst, data->sat_command->c.menu.items[0].iconInfo.dst, icon_length);
		
			/* Icon is self-explanatory. No need to display text for this case.*/
			     if(data->sat_command->c.menu.itemIconQual == 0x00)
			     	{
					/* Icon is self-explanatory. Do not display the text*/
					setupMenuItemIconData.selfExplanatory = TRUE;
			     	}
				else
				 	setupMenuItemIconData.selfExplanatory = FALSE;

				/* Get the height of Title & Line. This will be used to calculate the icon co-ordinates. */
				 fontHeight = dspl_GetFontHeight();
		 		 titleHeight = res_getTitleHeight();

				/* Set the initial menu height to the title height*/
				  menuHeight = titleHeight + ((fontHeight - 2) / 2) - (setupMenuItemIconData.height / 2);

				/* Free the memory of icon data that we got through the SATK command*/
				 mfwFree((U8 *)data->sat_command->c.menu.items[0].iconInfo.dst, icon_length);
				data->sat_command->c.menu.items[0].iconInfo.dst = NULL;
		 }
		 else
		 {
		 	setupMenuItemIconData.width = 0;
			setupMenuItemIconData.height = 0;
			setupMenuItemIconData.dst = NULL;
		  	setupMenuItemIconData.selfExplanatory = FALSE;	
		 }
#endif			

            for (i=0; i < data->list_menu_data.ListLength; i++) /* Fill Menu List */
                {
					mnuInitDataItem(&data->list_menu_data.List[i]);

// Shashi Shekar B.S., a0876501, Feb 4, 2006, DR: OMAPS00061462
#ifdef FF_MMI_SAT_ICON
	     if(data->sat_command->c.menu.itemIconQual != 0xFF)
	     	{
			/* Populate the Icon attributes. 
			We assume that we have the same icon for all elements in the item list. */
			data->list_menu_data.List[i].icon = (MfwIcnAttr *)ALLOC_MEMORY (sizeof (MfwIcnAttr));
			
			data->list_menu_data.List[i].icon->icons = setupMenuItemIconData.dst;
			data->list_menu_data.List[i].icon->nIcons = 1;
			data->list_menu_data.List[i].icon->icnType = BMP_FORMAT_256_COLOUR;
			data->list_menu_data.List[i].icon->area.px = 1;
			data->list_menu_data.List[i].icon->area.py = menuHeight + 1;
			data->list_menu_data.List[i].icon->area.sx = setupMenuItemIconData.width;
			data->list_menu_data.List[i].icon->area.sy = setupMenuItemIconData.height;
			data->list_menu_data.List[i].icon->selfExplanatory = setupMenuItemIconData.selfExplanatory;

			menuHeight += fontHeight;
	     	}

                   /* get the correct entry in list of setup_menuList */
                    data->list_menu_data.List[i].str = sat_create_ITEM_ASCIIZ (&menu->items[i]);
					if (data->list_menu_data.List[i].str[0] == (char)0x80)/*MC, SPR 940/2 check for unicode tag*/
						Unicode_menu = TRUE; 
#else
                    /* get the correct entry in list of setup_menuList */
                    data->list_menu_data.List[i].str = sat_create_ITEM_ASCIIZ (&menu->items[i]);
					if (data->list_menu_data.List[i].str[0] == 0x80)/*MC, SPR 940/2 check for unicode tag*/
						Unicode_menu = TRUE; 
#endif					
                    data->list_menu_data.List[i].flagFunc = (FlagFunc)item_flag_none;
                }
        }
	data->list_menu_data.autoDestroy    = TRUE;

	TRACE_EVENT_P1("menu->header.len %d", menu->header.len);

	/* SPR#2492 - DS - Setup menu header */
	if (menu->header.len > 0 && menu->header.len != 0xFF)
      {
            if (menuHdr) /* Free previously allocated header */
            {
               TRACE_EVENT_P2("Destroy menuHdr %d with size %d", menuHdr, *(menuHdr -(U16)sizeof(U16)) );

// Shashi Shekar B.S., a0876501, Feb 4, 2006, DR: OMAPS00061462
#ifdef FF_MMI_SAT_ICON
		  /* Free the memory allocated for Header icon data. */
		   if(setupMenuHeaderIconData.dst != NULL)
		   {
			FREE_MEMORY((U8 *)setupMenuHeaderIconData.dst, setupMenuHeaderIconData.width * 
								setupMenuHeaderIconData.height);
			setupMenuHeaderIconData.dst = NULL;
		   }
#endif	
		   sat_destroy_TEXT_ASCIIZ(menuHdr);
		   menuHdr = NULL; /* dsm 01/12/03 - Added to explicitly set menuHdr to NULL */
            }
			
// Shashi Shekar B.S., a0876501, Feb 4, 2006, DR: OMAPS00061462
#ifdef FF_MMI_SAT_ICON
	     if(data->sat_command->c.menu.header.icon.qual != 0xFF)
	     	{
	     	       /* We have icon to be displayed*/
			/* Copy the icon data to the editor attributes*/
			setupMenuHeaderIconData.width = data->sat_command->c.menu.header.iconInfo.width;  
			setupMenuHeaderIconData.height = data->sat_command->c.menu.header.iconInfo.height; 

			icon_length = data->sat_command->c.menu.header.iconInfo.width * data->sat_command->c.menu.header.iconInfo.height;
										
			setupMenuHeaderIconData.dst = (char *)ALLOC_MEMORY (icon_length);

			memcpy(setupMenuHeaderIconData.dst, data->sat_command->c.menu.header.iconInfo.dst, icon_length);
		
			/* Icon is self-explanatory. No need to display text for this case.*/
			     if(data->sat_command->c.menu.header.icon.qual == 0x00)
			     	{
					/* Icon is self-explanatory. Do not display the text*/
					setupMenuHeaderIconData.selfExplanatory = TRUE;
			     	}
				else
				 	setupMenuHeaderIconData.selfExplanatory = FALSE;

				/* Free the memory of icon data that we got through the SATK command*/
				 mfwFree((U8 *)data->sat_command->c.menu.header.iconInfo.dst, icon_length);
				data->sat_command->c.menu.header.iconInfo.dst = NULL;
		 }
		 else
		 {
		 	setupMenuHeaderIconData.width = 0;
			setupMenuHeaderIconData.height = 0;
			setupMenuHeaderIconData.dst = NULL;
		  	setupMenuHeaderIconData.selfExplanatory = FALSE;	
		 }
#endif

            menuHdr = sat_create_TEXT_ASCIIZ(&menu->header);
            TRACE_EVENT_P1("SAT menu header: %s", menuHdr);

            /* SPR#2492 - DS - Display menu header if one exists */
            if (menuHdr)
            {
                data->list_menu_data.Attr->hdrId = (int)menuHdr;

                TRACE_EVENT_P1("mode map before: %04x", data->list_menu_data.Attr->mode);

                /* SPR#2492 - DS - Use strings rather than text Ids */
                data->list_menu_data.Attr->mode |=  MNU_HDRFORMAT_STR;

                TRACE_EVENT_P1("mode map after: %04x", data->list_menu_data.Attr->mode);

// Shashi Shekar B.S., a0876501, Feb 4, 2006, DR: OMAPS00061462
#ifdef FF_MMI_SAT_ICON
/* We allow the Header text to be created. We send the icon data with self-explanatory status.*/ 
		 if(setupMenuHeaderIconData.dst != NULL)
		 {
		 	data->list_menu_data.Attr->icon = (MfwIcnAttr *)ALLOC_MEMORY (sizeof (MfwIcnAttr));
			
			data->list_menu_data.Attr->icon->icons = setupMenuHeaderIconData.dst;
			data->list_menu_data.Attr->icon->nIcons = 1;
			data->list_menu_data.Attr->icon->icnType = BMP_FORMAT_256_COLOUR;
			data->list_menu_data.Attr->icon->area.px = 1;
			data->list_menu_data.Attr->icon->area.py = 1;
			data->list_menu_data.Attr->icon->area.sx = setupMenuHeaderIconData.width;
			data->list_menu_data.Attr->icon->area.sy = setupMenuHeaderIconData.height;
			data->list_menu_data.Attr->icon->selfExplanatory = setupMenuHeaderIconData.selfExplanatory;
		 }
#endif				
            }
      }
	
      retVal = listDisplayListMenu(data->win, &data->list_menu_data, (ListCbFunc)sat_setup_menu_listmnu_cb,Unicode_menu/*MC*/);
        // xrashmic 5 Oct, 2005 MMI-SPR-29356, MMI-SPR-29357
        // Displaying '?' to indicate to the user that help is available for a menu
        if(data->sat_command->qual & SAT_M_SELECT_HELP_AVAIL)
        {
            displayHelpSymbol();
        }
        if (retVal != LISTS_FAIL)
	      sat_set_setup_menu_listmnu_win(data->list_menu_data.win);
	else
		sat_set_setup_menu_listmnu_win(NULL);

	  return retVal;
 }

static void sat_setup_menu_exec (T_MFW_HND win, USHORT event, SHORT value, T_SAT_CMD * sat_command)
    /* callback handler for events sent from parents or childs to to trigger some execution */
{
    T_MFW_WIN      * win_data = ((T_MFW_HDR *) win)->data;
    T_sat_setup_menu          * data = (T_sat_setup_menu *)win_data->user;
    T_SAT_RES sat_res;



    TRACE_FUNCTION ("sat_setup_menu_exec()");

    switch (event)
        {
        case SAT_SETUP_MENU:
            
            TRACE_EVENT("sat_setup_menu_exec() SAT_SETUP_MENU");
            
            /* initialization of administrative data */

            data->sat_command = sat_command; /* save a pointer to the parameter for later use in callbacks */

            /* initialization of the dialog data */

            data->list_menu_data.ListPosition   = 1;/* True cursor position in list menu. */
            data->list_menu_data.Font           = 0;
            data->list_menu_data.LeftSoftKey    = TxtSoftSelect;
            data->list_menu_data.RightSoftKey   = TxtSoftBack;
                        // xrashmic 5 Oct, 2005 MMI-SPR-29356, MMI-SPR-29357
            //Adding the support for MenuSelect in this list view
            data->list_menu_data.KeyEvents      = KEY_CLEAR | KEY_RIGHT | KEY_LEFT | KEY_MNUSELECT| KEY_MNULEFT| KEY_MNUUP | KEY_MNUDOWN |KEY_HUP;
            data->list_menu_data.Reason         = 0;
            data->list_menu_data.Strings        = TRUE;
            data->list_menu_data.Attr           = &sat_setup_menuAttrib;
            data->list_menu_data.List           = NULL; /* c030 rsa mark as first time creation */
		    data->list_menu_data.autoDestroy    = TRUE;

// Shashi Shekar B.S., a0876501, Feb 4, 2006, DR: OMAPS00061462
#ifdef FF_MMI_SAT_ICON
/*Initialize the data members of Icon attributes */
	 	setupMenuItemIconData.width = 0;
		setupMenuItemIconData.height = 0;
		setupMenuItemIconData.dst = NULL;
	  	setupMenuItemIconData.selfExplanatory = FALSE;	
	 	setupMenuHeaderIconData.width = 0;
		setupMenuHeaderIconData.height = 0;
		setupMenuHeaderIconData.dst = NULL;
	  	setupMenuHeaderIconData.selfExplanatory = FALSE;	
#endif

            // c030 rsa
            /* NOBREAK */
        case SAT_RETURN:

            if (event == SAT_RETURN)
                TRACE_EVENT("sat_setup_menu_exec() SAT_RETURN");
            
            // end c030 rsa
            if (sat_get_setup_menu_listmnu_win() == NULL)
           {
	            /* (re)create the dialog handler */
	            if (sat_setup_menu_recreate(data) == LISTS_FAIL)
	                {
			    sat_res[SAT_ERR_INDEX] = SAT_RES_IMPOSSIBLE;
			    sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
			    sat_done (sat_command, sat_res);
	                    sat_setup_menu_destroy (data->win);
	                }
           }
           break;

			/* sbh - all window types are being provided with this event to destroy the window */
		case SAT_DESTROY_WINDOW:
		/* ...sbh */
		
        case SAT_EXIT:
            sat_setup_menu_destroy (data->win);
            break;

        default:
            TRACE_EVENT ("sat_setup_menu_exec() unexpected event");
            break;
        }
}

static void sat_setup_menu_listmnu_cb (T_MFW_HND win, ListMenuData_t * ListData)
    /* sat_setup_menu menu event handler */
{
    T_MFW_WIN  * win_data = ((T_MFW_HDR *)win)->data;
    T_sat_setup_menu      * data = (T_sat_setup_menu *)win_data->user;
    SatItem  * item;
    int index;
//    T_SAT_RES sat_res;  // RAVI

    TRACE_FUNCTION ("sat_setup_menu_listmnu_cb()");

	if ((win EQ NULL) || (win_data EQ NULL) || (data EQ NULL))
		return;
		
    switch (ListData->Reason)
        {
        case E_MNU_SELECT:
		case LISTS_REASON_SELECT:

   	    index = ListData->ListPosition; /* index of selected item */
            item = &data->sat_command->c.menu.items[index]; /* selected item */
           // xrashmic 5 Oct, 2005 MMI-SPR-29356, MMI-SPR-29357
           // Using the MenuSelect Key for requesting the help info in STK menu.
	    if(ListData->selectKey && ((data->sat_command->qual & SAT_M_SELECT_HELP_AVAIL) != 0))
	    {
              satMenuItem(item->id, 1);
              ListData->selectKey = FALSE;
	    }
           else
              satMenuItem(item->id, 0);  /* issue the answering envelope */

            /*
            ** The SAT Menu is automatically destroyed when an item is selected
            */
            sat_set_setup_menu_listmnu_win(NULL);

            /* sat_setup_menu_destroy(data->win); c030 rsa do not destroy in order to keep the menu on top */
            break;
        case LISTS_REASON_TIMEOUT: // c015 rsa
        case LISTS_REASON_BACK: /* back to previous menu */
        	case LISTS_REASON_HANGUP:	// sbh - added so hangup key exits
	case LISTS_REASON_CLEAR:
		 
            /* 
             * we don't need to signal <SAT_RES_USER_BACK> here, since the session itself 
             * has been finished immediately after sat_setup_menu_proc().
             * Furthermore the entering of the menu has not been signalled to the SIM...
             */

            /*
            ** The SAT Menu is automatically destroyed and recreated when we move.
            */
            sat_set_setup_menu_listmnu_win(NULL);

            sat_setup_menu_destroy(data->win);
            break;
        default:
            return;
        }
    return;
}

/*********************************************************************
**********************************************************************

                        select_item      DYNAMIC MENU WINDOW. DECLARATION

*********************************************************************
**********************************************************************/
typedef struct
{
    /* administrative data */

    T_MMI_CONTROL   mmi_control;
    T_MFW_HND       win;

    /* associated handlers */

    T_MFW_HND		kbd; /* sbh - keyboard handler, so window can be destroyed by user */
    
    /* internal data */
    T_SAT_CMD     *sat_command; /* pointer to sat_command in parent */
    ListMenuData_t    list_menu_data;
    T_MFW_HND	sat_timer;	// May 24, 2005    REF: CRR 29358  x0021334
    ULONG	sat_timeout;		// May 24, 2005    REF: CRR 29358  x0021334
} T_sat_select_item;

static void sat_select_item_destroy (T_MFW_HND window);
static void sat_select_item_exec (T_MFW_HND win, USHORT event, SHORT value, T_SAT_CMD * sat_command);
static void sat_select_item_listmnu_cb (T_MFW_HND win, ListMenuData_t * ListData);
static T_MFW sat_select_item_recreate(T_sat_select_item *data);

/*********************************************************************
**********************************************************************

                        select_item     DYNAMIC MENU WINDOW. IMPLEMENTATION

*********************************************************************
**********************************************************************/

T_MFW_HND sat_select_item_create(MfwHnd parent_window)
{
    T_sat_select_item      * data = (T_sat_select_item *)ALLOC_MEMORY (sizeof (T_sat_select_item));
    T_MFW_WIN  * win;

    TRACE_FUNCTION ("sat_select_item_create()");

    /*
     * Create window handler
     */

    data->win = win_create (parent_window, 0, E_WIN_VISIBLE, (T_MFW_CB)sat_win_cb);
    if (data->win EQ NULL)
        {
            return NULL;
        }

    /* 
     * connect the dialog data to the MFW-window
     */

    data->mmi_control.dialog = (T_DIALOG_FUNC)sat_select_item_exec;
    data->mmi_control.data   = data;
    win                      = ((T_MFW_HDR *)data->win)->data;
    win->user                = (MfwUserDataPtr)data;

	data->kbd      = kbdCreate( data->win, KEY_ALL, (T_MFW_CB) sat_kbd_cb); /* sbh - add keyboard handler */

    /*
     * return window handle
     */

 
    win_show(data->win);
    return data->win;
}

static void sat_select_item_destroy(MfwHnd own_window)
{
    T_MFW_WIN             * win_data;
    T_sat_select_item     * data;
    int i;

    TRACE_FUNCTION ("sat_select_item_destroy()");

    if (own_window)
        {
            win_data = ((T_MFW_HDR *)own_window)->data;
            data = (T_sat_select_item *)win_data->user;

            if (data)
                {
                    /*
                     * Delete WIN handler
                     */ 
                    win_delete (data->win);

                    /*     
                     * Free Memory
                     */

// Shashi Shekar B.S., a0876501, Feb 4, 2006, DR: OMAPS00061462
#ifdef FF_MMI_SAT_ICON
		  /* Free the memory allocated for Item icon data. */
		   if(selectItemIconData.dst != NULL)
		   {
			FREE_MEMORY((U8 *)selectItemIconData.dst, selectItemIconData.width * 
								selectItemIconData.height);
			selectItemIconData.dst = NULL;
		   }

		  /* Free the memory allocated for Header icon data. */
		   if(selectItemHeaderIconData.dst != NULL)
		   {
			FREE_MEMORY((U8 *)selectItemHeaderIconData.dst, selectItemHeaderIconData.width * 
								selectItemHeaderIconData.height);
			selectItemHeaderIconData.dst = NULL;

			// Apr 24, 2006    REF: DRT OMAPS00075832  x0039928
			// Shashi Shekar B.S., a0876501, Feb 4, 2006, DR: OMAPS00061462
			   if(data->list_menu_data.Attr->icon != NULL)
			  {
				FREE_MEMORY((U8 *)data->list_menu_data.Attr->icon, sizeof (MfwIcnAttr));
				data->list_menu_data.Attr->icon = NULL;
			  }

		   }
#endif

                     for (i=0; i < data->list_menu_data.ListLength; i++)
                        {
                            sat_destroy_ITEM_ASCIIZ (data->list_menu_data.List[i].str);

// Shashi Shekar B.S., a0876501, Feb 4, 2006, DR: OMAPS00061462
#ifdef FF_MMI_SAT_ICON
				if(data->list_menu_data.List[i].icon != NULL)
				{
					FREE_MEMORY((U8 *)data->list_menu_data.List[i].icon, sizeof (MfwIcnAttr));
					data->list_menu_data.List[i].icon = NULL;
				}
#endif
                        }
                    FREE_MEMORY ((U8 *)data->list_menu_data.List, 
                                 (data->list_menu_data.ListLength * sizeof (MfwMnuItem)));
                    data->list_menu_data.List = NULL;
                    FREE_MEMORY ((U8 *)data, sizeof (T_sat_select_item));

                    /* SPR#2492 - DS - Free SAT menu header if it exists */
                    if (menuHdr)
                    {
                      TRACE_EVENT_P2("Destroy menuHdr %d with size %d", menuHdr, *(menuHdr -(U16)sizeof(U16)) );
			   sat_destroy_TEXT_ASCIIZ(menuHdr);
			   menuHdr = NULL; /* dsm 01/12/03 - Added to explicitly set menuHdr to NULL */
			   	
                    }
                 }
            else
                {
                    TRACE_EVENT ("sat_select_item_destroy() called twice");
                }
        }
}

static T_MFW sat_select_item_recreate(T_sat_select_item *data)
{
    	int i;
	int Unicode_menu = FALSE; /*MC, SPR 940/2 flag to lay-out as Unicode*/
	//May 24, 2005    REF: CRR 29358  x0021334
	// Added this variable to store return value from listDisplayListMenu
	T_MFW sat_select_item_status;	

// Shashi Shekar B.S., a0876501, Feb 4, 2006, DR: OMAPS00061462
#ifdef FF_MMI_SAT_ICON
	USHORT icon_length;
	USHORT fontHeight = 0, menuHeight = 0, titleHeight;
#endif	

    SatMenu  * menu  = &data->sat_command->c.menu;
    TRACE_FUNCTION ("sat_select_item_recreate()");

#ifdef __COMPLETE_LIST_SUPPORT__ /* ??? rsa for future extension */
    if ((&data->sat_command.qual & SAT_M_SELECT_HELP_AVAIL) NEQ 0)
        {
            data->list_menu_data.AlternateLeftSoftKey  = TxtHelp; /* help available */
        }
    else
        {
            data->list_menu_data.AlternateLeftSoftKey  = TxtNull; /* no help available */
        }
#endif

     /* allocate sufficient memory to hold the list of menu items */
    data->list_menu_data.ListLength = menu->nItems; /* actual number of entries in list menu.    */
    data->list_menu_data.List = (MfwMnuItem *)ALLOC_MEMORY (data->list_menu_data.ListLength * sizeof (MfwMnuItem));
    memset(data->list_menu_data.List, 0x00, data->list_menu_data.ListLength * sizeof (MfwMnuItem));

// Shashi Shekar B.S., a0876501, Feb 4, 2006, DR: OMAPS00061462
#ifdef FF_MMI_SAT_ICON
	     if(menu->itemIconQual != 0xFF)
	     	{
	     	       /* We have icon to be displayed*/
			/* Copy the icon data to the editor attributes*/
			selectItemIconData.width = menu->items[0].iconInfo.width;  
			selectItemIconData.height = menu->items[0].iconInfo.height; 

			icon_length = menu->items[0].iconInfo.width * menu->items[0].iconInfo.height;
										
			selectItemIconData.dst = (char *)ALLOC_MEMORY (icon_length);

			memcpy(selectItemIconData.dst, menu->items[0].iconInfo.dst, icon_length);
		
			/* Icon is self-explanatory. No need to display text for this case.*/
			     if(menu->itemIconQual == 0x00)
			     	{
					/* Icon is self-explanatory. Do not display the text*/
					selectItemIconData.selfExplanatory = TRUE;
			     	}
				else
				 	selectItemIconData.selfExplanatory = FALSE;

				/* Get the height of Title & Line. This will be used to calculate the icon co-ordinates. */
				 fontHeight = dspl_GetFontHeight();
		 		 titleHeight = res_getTitleHeight();

				/* Set the initial menu height to the title height*/
				 menuHeight = titleHeight + ((fontHeight - 2) / 2) - (selectItemIconData.height / 2);

				/* Free the memory of icon data that we got through the SATK command*/
				 mfwFree((U8 *)menu->items[0].iconInfo.dst, icon_length);
				menu->items[0].iconInfo.dst = NULL;
		 }
		 else
		 {
		 	selectItemIconData.width = 0;
			selectItemIconData.height = 0;
			selectItemIconData.dst = NULL;
		  	selectItemIconData.selfExplanatory = FALSE;	
		 }
#endif

    for (i=0; i < data->list_menu_data.ListLength; i++) /* Fill Menu List */
        {
			mnuInitDataItem(&data->list_menu_data.List[i]);

// Shashi Shekar B.S., a0876501, Feb 4, 2006, DR: OMAPS00061462
#ifdef FF_MMI_SAT_ICON
	     if(data->sat_command->c.menu.itemIconQual != 0xFF)
	     	{
			/* Populate the Icon attributes. 
			We assume that we have the same icon for all elements in the item list. */
			data->list_menu_data.List[i].icon = (MfwIcnAttr *)ALLOC_MEMORY (sizeof (MfwIcnAttr));
			
			data->list_menu_data.List[i].icon->icons =  selectItemIconData.dst;
			data->list_menu_data.List[i].icon->nIcons = 1;
			data->list_menu_data.List[i].icon->icnType = BMP_FORMAT_256_COLOUR;
			data->list_menu_data.List[i].icon->area.px = 1;
			data->list_menu_data.List[i].icon->area.py = menuHeight + 1;
			data->list_menu_data.List[i].icon->area.sx = selectItemIconData.width;
			data->list_menu_data.List[i].icon->area.sy = selectItemIconData.height;
			data->list_menu_data.List[i].icon->selfExplanatory = selectItemIconData.selfExplanatory;

			menuHeight += fontHeight;
	     	}
                   /* get the correct entry in list of select_itemList */
            data->list_menu_data.List[i].str = sat_create_ITEM_ASCIIZ (&menu->items[i]);
			if (data->list_menu_data.List[i].str[0] == (char)0x80)/*MC, SPR 940/2 check for unicode tag*/
				Unicode_menu = TRUE; 
#else
                    /* get the correct entry in list of select_itemList */
                    data->list_menu_data.List[i].str = sat_create_ITEM_ASCIIZ (&menu->items[i]);
					if (data->list_menu_data.List[i].str[0] == 0x80)/*MC, SPR 940/2 check for unicode tag*/
						Unicode_menu = TRUE; 
#endif	

            data->list_menu_data.List[i].flagFunc = (FlagFunc)item_flag_none;

            /* xreddymn OMAPS00080543 Jun-05-2006
             * A flag bit in item.action is set to TRUE for default item in decDefItem.
             */
            if(menu->items[i].action & 0x40)
                data->list_menu_data.ListPosition = i + 1;

        }

	data->list_menu_data.autoDestroy    = TRUE;

	TRACE_EVENT_P1("menu->header.len %d", menu->header.len);

	/* SPR#2492 - DS - Setup menu header */
	if (menu->header.len > 0 && menu->header.len != 0xFF)
      {
            if (menuHdr) /* Free previously allocated header */
            {
               TRACE_EVENT_P2("Destroy menuHdr %d with size %d", menuHdr, *(menuHdr -(U16)sizeof(U16)) );
// Shashi Shekar B.S., a0876501, Feb 4, 2006, DR: OMAPS00061462
#ifdef FF_MMI_SAT_ICON
		  /* Free the memory allocated for Header icon data. */
		   if(selectItemHeaderIconData.dst != NULL)
		   {
			FREE_MEMORY((U8 *)selectItemHeaderIconData.dst, selectItemHeaderIconData.width * 
								selectItemHeaderIconData.height);
			selectItemHeaderIconData.dst = NULL;
		   }
#endif
		   sat_destroy_TEXT_ASCIIZ(menuHdr);
		   menuHdr = NULL; /* dsm 01/12/03 - Added to explicitly set menuHdr to NULL */
            }

// Shashi Shekar B.S., a0876501, Feb 4, 2006, DR: OMAPS00061462
#ifdef FF_MMI_SAT_ICON
	     if(data->sat_command->c.menu.header.icon.qual != 0xFF)
	     	{
	     	       /* We have icon to be displayed*/
			/* Copy the icon data to the editor attributes*/
			selectItemHeaderIconData.width = data->sat_command->c.menu.header.iconInfo.width;  
			selectItemHeaderIconData.height = data->sat_command->c.menu.header.iconInfo.height; 

			icon_length = data->sat_command->c.menu.header.iconInfo.width * data->sat_command->c.menu.header.iconInfo.height;
										
			selectItemHeaderIconData.dst = (char *)ALLOC_MEMORY (icon_length);

			memcpy(selectItemHeaderIconData.dst, data->sat_command->c.menu.header.iconInfo.dst, icon_length);
		
			/* Icon is self-explanatory. No need to display text for this case.*/
			     if(data->sat_command->c.menu.header.icon.qual == 0x00)
			     	{
					/* Icon is self-explanatory. Do not display the text*/
					selectItemHeaderIconData.selfExplanatory = TRUE;
			     	}
				else
				 	selectItemHeaderIconData.selfExplanatory = FALSE;

				/* Free the memory of icon data that we got through the SATK command*/
				 mfwFree((U8 *)data->sat_command->c.menu.header.iconInfo.dst, icon_length);
				data->sat_command->c.menu.header.iconInfo.dst = NULL;
		 }
		 else
		 {
		 	selectItemHeaderIconData.width = 0;
			selectItemHeaderIconData.height = 0;
			selectItemHeaderIconData.dst = NULL;
		  	selectItemHeaderIconData.selfExplanatory = FALSE;	
		 }
#endif


            menuHdr = sat_create_TEXT_ASCIIZ(&menu->header);
            TRACE_EVENT_P1("SAT menu header: %s", menuHdr);

            /* SPR#2492 - DS - Display menu header if one exists */
            if (menuHdr)
            {
                data->list_menu_data.Attr->hdrId = (int)menuHdr;

                TRACE_EVENT_P1("mode map before: %04x", data->list_menu_data.Attr->mode);

                /* SPR#2492 - DS - Use strings rather than text Ids */
                data->list_menu_data.Attr->mode |=  MNU_HDRFORMAT_STR;

                TRACE_EVENT_P1("mode map after: %04x", data->list_menu_data.Attr->mode);

// Shashi Shekar B.S., a0876501, Feb 4, 2006, DR: OMAPS00061462
#ifdef FF_MMI_SAT_ICON
/* We allow the Header text to be created. We send the icon data with self-explanatory status.*/ 
		 if(selectItemHeaderIconData.dst != NULL)
		 {
		 	data->list_menu_data.Attr->icon = (MfwIcnAttr *)ALLOC_MEMORY (sizeof (MfwIcnAttr));
			
			data->list_menu_data.Attr->icon->icons = selectItemHeaderIconData.dst;
			data->list_menu_data.Attr->icon->nIcons = 1;
			data->list_menu_data.Attr->icon->icnType = BMP_FORMAT_256_COLOUR;
			data->list_menu_data.Attr->icon->area.px = 1;
			data->list_menu_data.Attr->icon->area.py = 1;
			data->list_menu_data.Attr->icon->area.sx = selectItemHeaderIconData.width;
			data->list_menu_data.Attr->icon->area.sy = selectItemHeaderIconData.height;
			data->list_menu_data.Attr->icon->selfExplanatory = selectItemHeaderIconData.selfExplanatory;
		 }
#endif
            }
      }

//	May 24, 2005    REF: CRR 29358  x0021334
//	Description: CT_PTCRB 27.22.4.9.8 fails: wrong terminal response
//	Now the list will be displayed, so start the timer.	     
	sat_select_item_status = listDisplayListMenu(data->win, &data->list_menu_data, (ListCbFunc)sat_select_item_listmnu_cb,Unicode_menu/*MC*/);
        // xrashmic 5 Oct, 2005 MMI-SPR-29356, MMI-SPR-29357
        // Displaying '?' to indicate to the user that help is available for a menu
        if(data->sat_command->qual & SAT_M_SELECT_HELP_AVAIL)
        {
            displayHelpSymbol();    
        }
	tim_start(data->sat_timer);
	return sat_select_item_status; 
 }

static void sat_select_item_exec (T_MFW_HND win, USHORT event, SHORT value, T_SAT_CMD * sat_command)
    /* callback handler for events sent from parents or childs to to trigger some execution */
{
    T_MFW_WIN                  * win_data = ((T_MFW_HDR *) win)->data;
    T_sat_select_item          * data = (T_sat_select_item *)win_data->user;
    T_SAT_RES sat_res;

    TRACE_FUNCTION ("sat_select_item_exec()");

    switch (event)
        {
        case SAT_SELECT_ITEM:
            /* initialization of administrative data */

            data->sat_command = sat_command; /* save a pointer to the parameter for later use in callbacks */

            /* initialization of the dialog data */
             data->list_menu_data.ListPosition   = 1;/* True cursor position in list menu. */
            data->list_menu_data.Font           = 0;
            data->list_menu_data.LeftSoftKey    = TxtSoftSelect;
            data->list_menu_data.RightSoftKey   = TxtSoftBack;
            // xrashmic 5 Oct, 2005 MMI-SPR-29356, MMI-SPR-29357
            //Adding the support for MenuSelect in this list view
            data->list_menu_data.KeyEvents      = KEY_CLEAR | KEY_RIGHT | KEY_LEFT | KEY_MNUSELECT| KEY_MNULEFT| KEY_MNUUP | KEY_MNUDOWN|KEY_HUP;
            data->list_menu_data.Reason         = 0;
            data->list_menu_data.Strings        = TRUE;
            data->list_menu_data.Attr           = &sat_select_menuAttrib;
	     data->list_menu_data.autoDestroy    = TRUE;

	      // May 24, 2005    REF: CRR 29358  x0021334
	      // Description: CT_PTCRB 27.22.4.9.8 fails: wrong terminal response
	      // Fix: Set Timeout period to 20 seconds and create the timer.	 
		data->sat_timeout = TWENTY_SECS;
       	data->sat_timer =  tim_create(data->win, data->sat_timeout, (T_MFW_CB)sat_select_TimerCb);
              g_sat_scroll_status = TRUE; 
 
// Shashi Shekar B.S., a0876501, Feb 4, 2006, DR: OMAPS00061462
#ifdef FF_MMI_SAT_ICON
/*Initialize the data members of Icon attributes */
	 	selectItemIconData.width = 0;
		selectItemIconData.height = 0;
		selectItemIconData.dst = NULL;
	  	selectItemIconData.selfExplanatory = FALSE;	
	 	selectItemHeaderIconData.width = 0;
		selectItemHeaderIconData.height = 0;
		selectItemHeaderIconData.dst = NULL;
	  	selectItemHeaderIconData.selfExplanatory = FALSE;	
#endif

            /* (re)create the dialog handler */
            if (sat_select_item_recreate(data) == LISTS_FAIL)
                {
		    sat_res[SAT_ERR_INDEX] = SAT_RES_IMPOSSIBLE;
		    sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
		    sat_done (sat_command, sat_res);
		    // May 24, 2005    REF: CRR 29358  x0021334
		    // Delete the timer and set g_sat_scroll_status to FALSE.
    		    tim_delete (data->sat_timer);
		    g_sat_scroll_status = FALSE;
	           sat_select_item_destroy (data->win);
                }
            break;

		/* sbh - all window types are being provided with this event to destroy the window */
		case SAT_DESTROY_WINDOW:
		    // May 24, 2005    REF: CRR 29358  x0021334
		    // Delete the timer and set g_sat_scroll_status to FALSE.
    		    tim_delete (data->sat_timer);
		    g_sat_scroll_status = FALSE;
		    sat_select_item_destroy (data->win);
		    break;
		/* ...sbh */
		
	      // May 24, 2005    REF: CRR 29358  x0021334
	      // Description: CT_PTCRB 27.22.4.9.8 fails: wrong terminal response
	      // Fix: Timeout has occured. 	Used existing LISTS_REASON_TIMEOUT event for the same
		case LISTS_REASON_TIMEOUT:  
	    	    sat_res[SAT_ERR_INDEX] = SAT_RES_USER_NO_RESP; // set no response from user
	    	    sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
            	    sat_done (data->sat_command, sat_res);
            	    tim_stop(data->sat_timer);	// stop the timer
	           tim_delete(data->sat_timer);	// delete the timer
	     	    g_sat_scroll_status = FALSE;	// set scrolling status to FALSE
            	    sat_select_item_destroy(data->win);
                  break;

 	      // May 24, 2005    REF: CRR 29358  x0021334
	      // Description: CT_PTCRB 27.22.4.9.8 fails: wrong terminal response
	      // Fix: User has scrolled up/down. Used existing LISTS_REASON_SCROLL event for the same
		case LISTS_REASON_SCROLL: 
	     	    tim_stop(data->sat_timer);	// stop the timer
	     	    tim_start(data->sat_timer);	// start the timer again for 20 seconds
	     	    break;
        	default:
            	    TRACE_EVENT ("sim_select_item_exec() unexpected event");
            	    break;
        }
}

static void sat_select_item_listmnu_cb (T_MFW_HND win, ListMenuData_t * ListData)
    /* sat_select_item menu event handler */
{
    T_MFW_WIN  * win_data = ((T_MFW_HDR *)win)->data;
    T_sat_select_item      * data = (T_sat_select_item *)win_data->user;
    SatItem  * item;
    int index;
    T_SAT_RES sat_res;

	if ((win EQ NULL) || (win_data EQ NULL) || (data EQ NULL))
		return;

    TRACE_FUNCTION ("sat_select_item_listmnu_cb()");

    switch (ListData->Reason)
        {
        case E_MNU_SELECT:
		case LISTS_REASON_SELECT:           
            index = ListData->ListPosition; /* index of selected item */
            item = &data->sat_command->c.menu.items[index]; /* selected item */
            /* CQ 16307 - Start */
            if (data->sat_command->c.menu.itemIconQual == 0xFF)
	         sat_res[SAT_ERR_INDEX] = SatResSuccess;
	     else
	     	{
// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
		 sat_res[SAT_ERR_INDEX] = SatResSuccess;
#else	     	
	         sat_res[SAT_ERR_INDEX] = SatResNoIcon;
            /* CQ 16307 - End */
#endif			
	     	}


           // xrashmic 5 Oct, 2005 MMI-SPR-29356, MMI-SPR-29357
           // Using the MenuSelect Key for requesting the help info in STK menu.
    	    if(ListData->selectKey && ((data->sat_command->qual & SAT_M_SELECT_HELP_AVAIL) != 0))
    	    {
              sat_res[SAT_ERR_INDEX]  = SatResUserHelp;
              ListData->selectKey = FALSE;
     	    }
             sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
             satItem (data->sat_command, item->id, sat_res, sizeof(T_SAT_RES)); /* issue the answering envelope */
            

	     // May 24, 2005    REF: CRR 29358  x0021334
	     // Stop and delete the timer. Also, set scrolling status to FALSE
	     tim_stop(data->sat_timer);
	     tim_delete(data->sat_timer);
	     g_sat_scroll_status =  FALSE;
	     sat_select_item_destroy(data->win);
            break;
        // c015 rsa
        case LISTS_REASON_TIMEOUT: 
	    sat_res[SAT_ERR_INDEX] = SAT_RES_USER_NO_RESP;
	    sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
            sat_done (data->sat_command, sat_res);
            sat_select_item_destroy(data->win);
            break;
        // end c015 rsa
	case LISTS_REASON_CLEAR: /* abort */
	case LISTS_REASON_HANGUP: /* abort */
            sat_res[SAT_ERR_INDEX] = SAT_RES_USER_ABORT;
            sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
            sat_done (data->sat_command, sat_res);
            sat_select_item_destroy(data->win);
            break;
        case LISTS_REASON_BACK: /* back to previous menu */
	    sat_res[SAT_ERR_INDEX] = SAT_RES_USER_BACK;
	    sat_res[SAT_AI_INDEX]  = SatResAiNoCause;
            sat_done (data->sat_command, sat_res);

	     // May 24, 2005    REF: CRR 29358  x0021334
	     // Stop and delete the timer. Also, set scrolling status to FALSE
	     tim_stop(data->sat_timer);
	     tim_delete(data->sat_timer);
	     g_sat_scroll_status =  FALSE;
	     sat_select_item_destroy(data->win);
            break;
        default:
            return;
        }
    return;
}
// end c016 rsa

// May 24, 2005    REF: CRR 29358  x0021334
// Description: CT_PTCRB 27.22.4.9.8 fails: wrong terminal response
// Fix: This is the call back function for sat timer
static int sat_select_TimerCb (T_MFW_EVENT event, T_MFW_TIM *timer)
{
	T_MFW_HND			win			= mfw_parent (mfw_header());
    	T_MFW_WIN			* win_data	= ((T_MFW_HDR *)win)->data;
    	T_sat_select_item 	*data		= (T_sat_select_item *)win_data->user;

	TRACE_FUNCTION("sat_select_TimerCb()");

	// Destroy the list before going back to the sat menu
	listsDestroy(data->list_menu_data.win);
	// Send timeout event.
	SEND_EVENT (data->win, LISTS_REASON_TIMEOUT, NULL, NULL);
       return MFW_EVENT_CONSUMED;
}

// May 24, 2005    REF: CRR 29358  x0021334
// Description: CT_PTCRB 27.22.4.9.8 fails: wrong terminal response
// Fix: This function returns the status of scrolling, which is used for restarting the timer
BOOL getScrollStatus()
{
 	return g_sat_scroll_status;
}
