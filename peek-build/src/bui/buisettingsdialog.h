

#ifndef UISETTINGSDIALOG_H
#define UISETTINGSDIALOG_H 

#include "buidialog.h"
#include "buiapp.h"
#include "buicontrol.h"
#include "buimenuctrl.h"
#include "buistring.h"
#include "./uiex/builistdialog.h"

/*! \cond private */
//#define SETTING_DEBUG    
#ifdef SETTING_DEBUG
#define SETTING_TRACE_STR(Str)       MonPrintf((char*)Str)
#else
#define SETTING_TRACE_STR(Str)
#endif

#define DOUBLE_LINE_SETTING_MENUITEM

#define SETTING_TO_IDLE                                 0xFFFFFFFE
#define UI_FIELD_OFFSET(type, field)                    ((uint32)&(((type*)0)->field))
#define UI_FIELD_OFFSET2(type, field1, field2)          ((uint32)&(((type*)0)->field1.field2))
#define UI_FIELD_OFFSET3(type, field1, field2, field3)  ((uint32)&(((type*)0)->field1.field2.field3))
#define UI_FIELD_SIZE(type, field)		(sizeof((((type *)0)->field)))

enum
{
  SETTING_SUCCESS = 0,
  SETTING_ERROR
};

/*! \endcond */

//setting type
typedef enum
{
  INVALID_SETTING = 0,
  MENU_SETTING,
  EDIT_SETTING,
  COMPOSITE_SETTING,
  MULTI_EDIT_SETTING,  /* must set the SettingInfoT and MultiSettingInfoT */
  OTHER_SETTING
} SettingTypeT;

//setting type which determines the value length in sidb.
typedef enum
{
  VALUE_INVALID = 0,
	INT8_TYPE = 1,
	INT16_TYPE = 2,
  INT32_TYPE = 4,
  STRING_TYPE = 72
}SettingValueTypeT;

/*! \cond private */
typedef enum
{
  STORAGE_INVALID = 0,
  STORAGE_SIDB,
  STORAGE_MAX
}SettingStorageTypeT;
/*! \endcond */

//!setting item information
typedef struct
{
  //!LogicID identifies a unique setting item.
  uint16 LogicID;                         
  //!menuitem id.If the menuitem is selected and "OK" KEY is pressed, the setting dialog of this setting item will be opend.
  RESOURCE_ID(MenuItemResT) MenuItemId; 
  //!setting dialog resource id.
  RESOURCE_ID(DialogResT) DlgResId;
  //!dialog title.If DlgTitle is not NULL, it will replace the dialog title defined in resouce.
  RESOURCE_ID(StringT) DlgTitle;
  //!setting type
  SettingTypeT SettingType;    

  //!sidb item Id
  uint16 SettingItem;
  //!sidb key Id
  uint16 SettingKey;
  //!offset
  uint16 Offset;
  //!store type
  SettingStorageTypeT StorageType; 
  //!value type
  SettingValueTypeT ValueType; 
}
SettingInfoT;

//!setting message.If the setting dialog status changes,it will send notify to application.
typedef struct
{
  uint32 ShowMsg;
	uint32 HideMsg;
  uint32 CloseMsg; 
  uint32 HoverMsg;
  uint32 CheckValueMsg;
  uint32 SaveMsg;
}
SettingMsgT;

//!multi setting item information
typedef struct
{
  uint16 LogicID;/*corresponds to LogicID in SettingInfo table.*/
  RESOURCE_ID(DialogResT) EditDlgResourceId; /*the dialog handle the setting   */
}
MultiSettingInfoT;

//!check information
typedef struct
{
	void* ValueP;
	bool CheckResult;		
}
CheckInfoT;

// 
typedef struct
{
  uint32 DlgId;

  uint16 wRangeBegin;
  //!sidb item Id
  uint16 SettingItem;
  //!sidb key Id
  uint16 SettingKey;
  //!offset
  uint16 Offset;
  //!store type
  SettingStorageTypeT StorageType; 
  //!value type
  SettingValueTypeT ValueType;
  uint32  dwMsgId;
  uint32  dwParam;
} SetInfoT;


typedef struct
{
  uint32 DlgId;         // id of dialog, is necessary
  DlgStyleT dlgStyle;   // style of dialog, is necessary
  ResourceIdT DataId;   // resource for dialog creating
  uint32 dwToDlgId;     // id of dialog which retrieved to when sub item selected
} SetDlgInfoT;

/*! \cond private */
#define UI_FIELD_TYPE(type, field)    ((SettingValueTypeT)(sizeof((((type *)0)->field))))
/*! \endcond */

/*! \cond private */
uint8 ReadIntSettingValue(SettingInfoT *SettingsInfoP, uint32& IntValue);
uint8 ReadStringSettingValue(SettingInfoT *SettingsInfoP, BStringC& StringValue);

uint8 WriteIntSettingValue(SettingInfoT *SettingsInfoP, uint32 IntValue);
uint8 WriteStringSettingValue(SettingInfoT *SettingsInfoP, BStringC StringValue);

void SidbStringToStringC(SidbStringT *SidbStrP, BStringC &String);
void StringCToSidbString(BStringC &String, SidbStringT *SidbStrP);

int32 GetValueLength(SettingValueTypeT Type);
bool IsIntType(SettingValueTypeT Type);
BStringC NumToString(uint32 Num);
/*! \endcond */

#ifdef DOUBLE_LINE_SETTING_MENUITEM
class SettingsMenuDrawerC: public InnerMenuDrawerC
{
public:
	SettingsMenuDrawerC(); 
  virtual ItemDrawerC* GetItemDrawer(uint16 ItemIndex);
  /*! \cond private */
	void SetExtraData(void* ExtraDataP);
  /*! \endcond */
private:
  BStringC GetItemSettingValue(MenuItemC *MenuItemP);
	void* mExtraDataP;
};
inline SettingsMenuDrawerC::SettingsMenuDrawerC()
{
  mExtraDataP = NULL;
}
#endif

class SettingsDialogC : public ListDialogC
{
public:
  SettingsDialogC(uint32 dlgId, DlgMStyleT etStyle = DLG_MSTYLE7_T);
  virtual ~SettingsDialogC();
  bool CreateEx(void* ExtraDataP);
  void SetSetInfo(SetInfoT *pSetInfoList){mSetInfoListP = pSetInfoList;}
  void SetDlgInfo(SetDlgInfoT *pDlgInfoList){mDlgInfoListP = pDlgInfoList;}

  SetInfoT* GetSettingsInfo(uint32 dlgId);
  void UpdateWindow(bool Synchronous = TRUE);

public:
	SettingsDialogC(SettingInfoT *SettingInfoP, SettingMsgT *SettingMsgP);
  void LoadSettingsTables(SettingInfoT *SettingInfoTableP,MultiSettingInfoT *MultiSettingInfoTableP);
/*! \cond private */
  int32 OnCreate(); 
  virtual void SaveString(uint32 dwDlgId, BStringC& strText);


private:
  void OnShowNotify();
  void OnHideNotify();
  void OnDestroy(void);
  bool OnKeyPress(uint32 KeyCode);
  void OnOpenSettingDialog();
  void OnCancel();
  void OnMenuItemHovered(uint32 Msg);
  void DrawStatus();

private:
  uint16 GetSubIndexByStyle();
  SetDlgInfoT* GetDlgInfo(uint32 dlgId);
  bool CreateNext(uint32 dwDlgId = 0);
  void ReturnToPrev(SetDlgInfoT* pstDlgInfo);

  SetInfoT* mSetInfoListP;
  SetDlgInfoT* mDlgInfoListP;
  uint32 m_nDlgId;

private:
  SettingInfoT *mSettingsInfoP;
  MultiSettingInfoT *mMultiSettingsInfoP;
  SettingInfoT *mCurrSettingInfoP;
	SettingMsgT *mSettingMsgP;
/*! \endcond */
		
  DECLARE_MAIL_MAP()
};


SettingsDialogC* OpenSettingDialog(uint32 dwDlgId, BStringC& strTitle, BApplicationC* pApp,  SetDlgInfoT* pstSetDlgInfo, SetInfoT* pstSetInfo = NULL);

inline SettingsDialogC* OpenSettingDialog(uint32 dwDlgId, RESOURCE_ID(StringT) strTitleRes, BApplicationC* pApp,  SetDlgInfoT* pstSetDlgInfo, SetInfoT* pstSetInfo = NULL)
{
  BStringC strTitle;
  if (strTitleRes)
    strTitle.Load(strTitleRes);
  return OpenSettingDialog(dwDlgId, strTitle, pApp, pstSetDlgInfo, pstSetInfo);
}


/*!
\brief  constructor function

Initialize the variables.
\param SettingInfoP - SettingInfo pointer.It should not be null.
\param SettingMsgP - SettingMsg pointer.
\retval none
*/
inline SettingsDialogC::SettingsDialogC(SettingInfoT *SettingInfoP, SettingMsgT *SettingMsgP)
: ListDialogC(DLG_MSTYLE7_T)
{
  mSettingsInfoP = NULL;
	mMultiSettingsInfoP = NULL;
	mCurrSettingInfoP = SettingInfoP;
	mSettingMsgP = SettingMsgP;
  m_nDlgId = 0;
  mSetInfoListP = NULL;
  mDlgInfoListP = NULL;
}


/*!
\brief  initialize SettingInfo and MultiSettingInfo table

\param SettingInfoTableP - setting information table.
\param MultiSettingInfoTableP - multisetting information table.
\retval none
*/
inline void SettingsDialogC::LoadSettingsTables(SettingInfoT *SettingInfoTableP, MultiSettingInfoT *MultiSettingInfoTableP)
{
  mSettingsInfoP = SettingInfoTableP;
  mMultiSettingsInfoP = MultiSettingInfoTableP;
}




#endif
