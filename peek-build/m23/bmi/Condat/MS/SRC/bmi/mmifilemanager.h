#ifndef _MMI_FILEMANGER_H_
#define _MMI_FILEMANGER_H_

#include "rfs/rfs_api.h"
#include "mfw_fm.h"


/*******************************************************************************
                                                                              
                                Interface constants
                                                                              
*******************************************************************************/
typedef enum
{
	FM_INIT,
	FM_DRAW,
	FM_SUSPEND,
	FM_RESUME,
	FM_DESTROY,
	FM_IMGDRAW_INIT,
	FM_IMGDRAW_DRAW,
	FM_IMGDRAW_SUSPEND,
	FM_IMGDRAW_RESUME,
	FM_IMGDRAW_DESTROY,
	FM_EDITOR_INIT,
	FM_EDITOR_CANCEL,
	FM_EDITOR_SELECT,
	FM_EDITOR_ERROR,
	FM_COPY_INIT,
	FM_COPY_DRAW,
	FM_COPY_SUSPEND,
	FM_COPY_RESUME,
	FM_COPY_DESTROY
}T_FM_WIN_EVNETS;
typedef enum
{
	FM_LIST_CREATE = -1,
	FM_LIST_RECREATE,
	FM_LIST_UPDATE
}T_FM_LIST_STATE;


/* ====================================================== */
/*
* FielManager window structure
*/
/* ====================================================== */
typedef struct
{
	T_MFW_FM_COPYMOVE_STRUCT *data;
	T_MFW_HND            win;
	T_MFW_HND  		timer; 
}T_FM_COPYMOVE;
typedef struct
{
	T_MMI_CONTROL   mmi_control;
	T_MFW_HND             parent_win;

	T_MFW_HND             dest_drive_win; 
	T_MFW_HND             dest_root_win;
	T_MFW_HND             dest_rootfolderlist_win; 
	T_MFW_HND             dest_list_win;
	ListMenuData            *dest_menu_list_data;
	T_FM_DEVICE_TYPE  destination;//Destination drive for copy or move scenario
	T_FM_DESTDIR       *dest_dir; 
	T_FM_APP_TYPE  app;//Destination drive for copy or move scenario
}T_MMI_FM_DEST_STRUCT;
typedef struct
{
	T_MMI_CONTROL   mmi_control;
	
	T_MFW_HND             parent_win;//Hanlder for the parent window

	T_MFW_HND             root_win;//Handler for the Root dummy window
	T_MFW_HND             mfw_win; //handler for the mfw component to the root window 
	
	T_MFW_HND             list_win;//Hanlder for the list
	ListMenuData            *menu_list_data;//List menu data
	int lineheight;

	T_MFW_HND             opt_win;  //Handler for the options window
	
	T_MFW_HND             img_win;  //Hanlder for the image view window
	
	T_FM_OPERATION_TYPE opt_type;//Holds the operation being excecuted
	
	T_FM_LIST_STATE 		list_state;//Indicates whether list should be recreated on entering the list window
	
	T_FM_DEVICE_TYPE  source;//Source drive
	T_FM_CURDIR       *cur_dir;//Current directory
	T_FM_APP_TYPE app;
	T_FM_COPYMOVE	  copymove;//Hold the copy or move related details
	T_MMI_FM_DEST_STRUCT *dest_data;//Holds the destination drirectory details


	int nFreeBytes;
	int nUsedBytes;
	char * edt_buf;
} T_MMI_FM_STRUCT;

typedef struct
{
	T_MMI_CONTROL   mmi_control;
	
	T_MFW_HND             parent_win;
	T_MFW_HND             img_win;	
	T_MFW_HND             kbd;
	int zoom;
	T_FM_IMG_ROTATE rotate;
} T_MMI_FM_IMGDRAW_STRUCT;
typedef struct
{
	T_MMI_CONTROL   mmi_control;
	
	T_MFW_HND             parent_win;
	T_MFW_HND             win;	
	T_MFW_HND             kbd;
} T_MMI_FM_COPY_STRUCT;

#define FM_MAX_DETAILS_BUFFER 100
#define FM_COPYMOVE_TIMER   500
/* ====================================================== */
/*
* List Attribute
*/
/* ====================================================== */
static const MfwMnuAttr FMList_Attrib =
{
    &FileType_menuArea,
    MNU_LEFT| MNU_LIST_ICONS| MNU_CUR_LINE, /* centered page menu       */
    (U8)-1,                                 /* use default font         */
    NULL,                               /* with these items         */
    0,                                   /* number of items     */
    COLOUR_LIST_XX,
    TxtFileMgmt,
    NULL,
    MNUATTRSPARE
};
/*******************************************************************************
                                                                              
                                Public methods
                                                                              
*******************************************************************************/
GLOBAL int  mmi_fm_display(MfwMnu* mnu, MfwMnuItem* item);
GLOBAL int  mmi_fm_root_folders(MfwMnu* mnu, MfwMnuItem* item);

GLOBAL int mmi_fm_rename (MfwMnu* m, MfwMnuItem* i);
GLOBAL int mmi_fm_newdir(MfwMnu * m, MfwMnuItem * i);

GLOBAL int mmi_fm_copy (MfwMnu* m, MfwMnuItem* i);
GLOBAL int mmi_fm_move (MfwMnu* m, MfwMnuItem* i);
GLOBAL int mmi_fm_remove(MfwMnu * m, MfwMnuItem * i);
GLOBAL int mmi_fm_properties(MfwMnu* m, MfwMnuItem* i);
GLOBAL int  mmi_fm_drives(MfwMnu* mnu, MfwMnuItem* item);
GLOBAL int mmi_fm_memStat(MfwMnu* m, MfwMnuItem* i);
GLOBAL int mmi_fm_format(MfwMnu* m, MfwMnuItem* i);
GLOBAL int  mmi_fm_dest_display(MfwMnu* mnu, MfwMnuItem* item);
GLOBAL int  mmi_fm_dest_root_folders(MfwMnu* mnu, MfwMnuItem* item);
USHORT showCopyMove( struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi );
USHORT showCreate( struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi );
USHORT check_dev( struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi );
USHORT format_check_dev( struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi );	
USHORT check_source_image( struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi );
USHORT check_source_audio( struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi );
USHORT check_source_setting( struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi );
GLOBAL int mmi_pb_on_off(MfwMnu* m, MfwMnuItem* i);
#endif

