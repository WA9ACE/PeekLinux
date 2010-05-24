#ifndef __UIEDITPOPMENU_H__
#define __UIEDITPOPMENU_H__

#include "buidialog.h"
#include "buimenudrawer.h"

class EditPopMenuTextCellDataC: public TextCellDataC
{
public:
	EditPopMenuTextCellDataC(WinHandleT WinHandle);
	virtual ~EditPopMenuTextCellDataC();

protected:
	virtual void Draw(DCC *DC,CellStatusT &CellStatus, CellPropertiesT &CellProperties);
	void DrawText(bool bRealFont = FALSE);
};

class EditPopMenuItemDrawerC: public ItemDrawerC
{
protected:
    virtual void DrawBack(DCC *DC,ItemStatusT &Status,MenuPropertiesT &MenuProperties);
    virtual void DrawCells(DCC *DC,ItemStatusT &Status,MenuPropertiesT &MenuProperties);

};

class EditPopMenuDrawerC: public InnerMenuDrawerC
{
public:  
	virtual ItemDrawerC* GetItemDrawer(uint16 ItemIndex);
	virtual void DoDrawBack(DCC *DC,MenuPropertiesT &MenuProperties);

};

class EditPopMenuViewC: public MenuViewC
{
public:
	virtual BalDispRectT GetItemRect(uint16 ItemIndex);
};

class EditPopMenuInputC: public MenuInputC
{
public:
	virtual bool OnKeyPress(uint32 KeyCode);
	virtual bool OnKeyRelease(uint32 KeyCode);
	
//protected:
    //virtual void ScrollItem(CursorMoveT MoveType);
	//virtual bool IsShortcutKey(uint32 KeyCode, uint8 ShortcutKey);
};

#endif /* __UIPOPMENU_H__ */

