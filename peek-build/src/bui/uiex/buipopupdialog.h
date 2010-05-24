

#ifndef UIPOPUPDIALOG_H
#define UIPOPUPDIALOG_H 

#include "buidialog.h"
#include "buimsgtimer.h"
#include "buidialogutils.h"

//! Define the popup dialog id 
#define DLGID_POPUPEX 0xFFFF0001
#define NUM 13

typedef enum
{   
    DLG_BTN_NONE = 0,
    DLG_BTN_LEFT,
    DLG_BTN_CENTER,
    DLG_BTN_RIGHT,
}DlgPBtnT;

typedef enum
{
	ZONE_NONE,
		
	ZONE_TEXT1,
	ZONE_TEXT2,
  /*not in use*/	
	ZONE_TEXT3,
	ZONE_IMAGE1,
	ZONE_IMAGE2,
	ZONE_IMAGE3,
	ZONE_ANIMATION,
	ZONE_TITLE,			 //title text
	ZONE_LSK,			 //left softkey text
	ZONE_MSK,			 //middle softkey text
	ZONE_RSK,			 //right softkey text
	ZONE_ALLSK,		     //left and middle and right softkey
	ZONE_ALERT,			 //alert to play
	ZONE_BGIMAGE,		 //backgroud image
	ZONE_TIMEOUT,		 //timeout
	ZONE_TEXTLIMIT,		 //text limit of texteditor
	ZONE_IME,			 //input method of texteditor
	ZONE_PROGRESS,		 //progressbar
	ZONE_ALL
	/*not in use -- end*/
} DlgPZoneT;

typedef struct 
{
	uint16 wZoneID;
} DlgZoneDataT;

typedef struct
{
	DlgPZoneT dlgpzone;
	uint32 x;
   	uint32 y;
   	uint32 dx;
   	uint32 dy;
   
}SubCtrlInfoT;

typedef struct
{
	DlgPStyleT  dlgpstyle;
	uint32  NumCtrl;
	SubCtrlInfoT SubCtrlInfoArray[NUM];
}DlgPopupInfoT;

typedef struct
{
    DlgPBtnT BtnId;
    BalDispRectT BtnRect;
}BtnInfoT;

//! Popup class define
class PopupDialogC : public DialogC
{
public:
    PopupDialogC(DlgPStyleT dlgStyle = DLG_PSTYLE_NONE_BTN, bool bAutoClose = TRUE);
    virtual ~PopupDialogC();
    bool Open(void* ExtraDataP, uint32 CloseMailID = 0, uint32 WinId = DLGID_POPUPEX, uint32 Style = WINSTYLE_SKBAR,DispLCDTypeT LcdType = LCD_MAIN);    
    void SetTimeOut(uint32 nTimeout);//nTimeout = 0 will cancel the time out
    void SetPopupTitle(RESOURCE_ID(StringT) StringResId);
    void SetPopupTitle(const BStringC& Title); 
    void SetPopupContent(RESOURCE_ID(StringT) StringResId);
    void SetPopupContent(const BStringC& Content);
    /********************************************/
    /* 1 btn   -- Caption1: the only btn        */
    /* 2 btn   -- Caption1: left btn            */
    /*            Caption2: right btn           */
    /* 3 btn   -- Caption1: left btn            */
    /*            Caption2: center btn          */
    /*            Caption3: right btn           */
    /********************************************/
    void SetBtnCaption(RESOURCE_ID(StringT) StringResId1, RESOURCE_ID(StringT) StringResId2 = NULL, RESOURCE_ID(StringT) StringResId3 = NULL);
    void SetBtnCaption(const BStringC& Caption1, const BStringC& Caption2 = "",const BStringC& Caption3 = "");

	//add by maoshanshan for fix bug QWT00010323 20080625
	void SetFocusBtn(DlgPBtnT FocusBtn);

	//add by maoshanshan 20081021
	void SetAppAtchData(void *Data);
	void *GetAppAtchData(void) const;
	
private:
    BTimerC mClearTimer;
    DlgPStyleT mPopupStyle;
    ImageC mButtonImage;
    ImageC mButtonImageHigh;
    
    uint32  mBtnFocus;
    static uint32 mBtnFocusTable[];
    BalDispRectT mBtnRect;
        
    BStringC mBtnCaption1;
    BStringC mBtnCaption2;
    BStringC mBtnCaption3;
	
protected:
    virtual void Draw(DCC *DcP);
    bool OnKeyPress(uint32 KeyCode);
    void OnClosePopup();
    void SetCtrlPos(DlgPopupInfoT *Info);
    DlgPopupInfoT* GetDlgPopupinfoP(void);//1106
    
/*useless --  kept for compile, please not to use*/
public:
    bool CreateEx(void* ExtraDataP, RESOURCE_ID(StringT) StringResId = NULL,uint32 CloseMailID = 0, uint32 WinId = DLGID_POPUPEX, uint32 Style = WINSTYLE_SKBAR,DispLCDTypeT LcdType = LCD_MAIN);
    void SetAutoClose(bool bAutoClose = TRUE){mbAutoClose = bAutoClose;}
    void SetStyle(DlgPStyleT dlgStyle){mPopupStyle = dlgStyle;}
    void SetZoneValue(DlgPZoneT nZoneId,void **pValue);
    void SetZoneRes(DlgPZoneT nZoneId,ResourceIdT ResourceId);
    void SetCloseUpdate(bool bCloseUpdate);
    void SetZoomToView(bool bvalue){mZoomToView = bvalue;}
    bool GetZoomToView(void){return mZoomToView;}
private:
    bool mbAutoClose; //Popup Auto Close or not
    bool mbCloseUpdate;
    bool mZoomToView;
	//add by maoshanshan 20081021
	void *mAppAtchData;
    RESOURCE_ID(ImageResT) mNoteDefaultIamgeId;
 /* useless -- end*/

    DECLARE_MAIL_MAP()  
};

inline void PopupDialogC::SetCloseUpdate(bool bCloseUpdate)
{
  mbCloseUpdate = bCloseUpdate;
}

//add by maoshanshan 20081021
inline void PopupDialogC::SetAppAtchData(void *Data)
{
	mAppAtchData = Data;
}

//add by maoshanshan 20081021
inline void *PopupDialogC::GetAppAtchData(void) const
{
	return mAppAtchData;
}



#endif

