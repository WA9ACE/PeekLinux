#ifndef __UIPOPMENU_H__
#define __UIPOPMENU_H__

#include "buidialog.h"
#include "buimenudrawer.h"

class PopMenuTextCellDataC: public TextCellDataC
{
public:
	PopMenuTextCellDataC(WinHandleT WinHandle);
	virtual ~PopMenuTextCellDataC();
	/*------optimize code lyf------*/
	 void SetBitMap(BalDispBitmapT Bmp1, BalDispBitmapT Bmp2, BalDispBitmapT Bmp3, BalDispBitmapT Bmp4) ;
    inline void SetShortcut(uint8 Shortcut) {mShortcut = Shortcut;};
	inline void SetShortcutPos(int32 Pos) {mShortcutPos = Pos;};
	inline void SetSplitLine(bool SplitLine) {mIsSplitLine = SplitLine;};
	inline void SetActivate(bool Activate) {mIsActivate = Activate;};
protected:
	virtual void Draw(DCC *DC,CellStatusT &CellStatus, CellPropertiesT &CellProperties);
	void DrawText(bool bRealFont = FALSE);
	void OnAnimationText(void *MailMsgP);
	void DrawUnHighlightText(BalDispRectT Rect);
	void ResetAnimation();
	void DeliverMail(uint32 MsgId, void *MsgBufferP);
protected:
    BVectorC mBgVector;//!<StringT*
    uint8 mShortcut;
    int32 mShortcutPos;
	bool mIsSplitLine;
	bool mIsActivate;
	/*------optimize code lyf begin------*/
	BalDispBitmapT mBmp_highlight_line;
	BalDispBitmapT mBmp_highlight;
	BalDispBitmapT mBmp_bg_line;
	BalDispBitmapT mBmp_bg;
};

class PopMenuItemDrawerC: public ItemDrawerC
{
protected:
    virtual void DrawBack(DCC *DC,ItemStatusT &Status,MenuPropertiesT &MenuProperties);
    virtual void DrawCells(DCC *DC,ItemStatusT &Status,MenuPropertiesT &MenuProperties);

};

class PopMenuDrawerC: public InnerMenuDrawerC
{
public:  
	PopMenuDrawerC();//optimize code lyf
	~PopMenuDrawerC();
	virtual ItemDrawerC* GetItemDrawer(uint16 ItemIndex);
	virtual void DoDrawBack(DCC *DC,MenuPropertiesT &MenuProperties);
/*------optimize code lyf begin------*/
private:
	BalDispBitmapT mBmp_highlight_line;
	BalDispBitmapT mBmp_highlight;
	BalDispBitmapT mBmp_bg_line;
	BalDispBitmapT mBmp_bg;

	ImageC *SelectedItemImageHLl ;
	ImageC *SelectedItemImageHL;
	ImageC *SelectedItemImageBgl ;
	ImageC *SelectedItemImageBg ;
/*------optimize code lyf end------*/
};

class PopMenuViewC: public MenuViewC
{
public:
	virtual BalDispRectT GetItemRect(uint16 ItemIndex);
};

class PopMenuInputC: public MenuInputC
{
public:
	virtual bool OnKeyPress(uint32 KeyCode);
	virtual bool OnKeyRelease(uint32 KeyCode);
	
protected:
    virtual void ScrollItem(CursorMoveT MoveType);
	virtual bool IsShortcutKey(uint32 KeyCode, uint8 ShortcutKey);
};

#endif /* __UIPOPMENU_H__ */

