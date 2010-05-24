

#ifndef UIUSEDETAILDIALOG_H
#define UIUSEDETAILDIALOG_H 

#include "buidialog.h"
#include "buimsgtimer.h"
#include "buidialogutils.h"
#include "builistdialog.h"

typedef enum
{
    UD_TYPE_MAIN = 0x0,
    UD_TYPE_NUM = 0x01,
    UD_TYPE_MAIL = 0x10,
    UD_TYPE_URL = 0x100
}UseDetailT;
//! Popup class define
class UseDetailDialogC : public ListDialogC
{
public:
    UseDetailDialogC(DlgMStyleT dlgStyle = DLG_MSTYLE7_T);
    virtual ~UseDetailDialogC();
    
    bool CreateEx(void* ExtraDataP, RESOURCE_ID(StringT) StringResId = NULL, uint32 WinId = 0, uint32 Style = WINSTYLE_SKBAR,DispLCDTypeT LcdType = LCD_MAIN);
	
    void SetDetailText(RESOURCE_ID(StringT) StringResId);
    void SetDetailText(BStringC& strUseDetail);

    UseDetailT GetUseDetailType(void){return (UseDetailT)m_UDtype;}
    bool IsUseDeatilType(UseDetailT type){return (bool)((m_UDtype & type) == type);}
    bool OpenUseDetail(UseDetailT type = UD_TYPE_MAIN);

protected:
    bool UseDetail(BStringC& strUseDetail);
    void RemoveLinkList(BLinkedListC *pLinkList);

    void OnOptionSelected(uint32 dwParam);
    

private:
    BLinkedListC m_slNum;
    BLinkedListC m_slMAIL;
    BLinkedListC m_slURL;

    uint32 m_UDtype;
    
    DECLARE_MAIL_MAP()
};







#endif

