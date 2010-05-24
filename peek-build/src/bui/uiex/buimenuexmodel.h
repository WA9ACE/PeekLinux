

#ifndef UIMENUEXMODEL_H
#define UIMENUEXMODEL_H


#include "buivector.h"
#include "buiwindow.h"
#include "buicontrol.h"

#include "buimenudefine.h"

#include "buimenuctrl.h"
#include "buimenudrawer.h"
#include "buimenuview.h"
#include "buimenumodel.h"

#include "buiimage.h"
class BYDMenuItemC:public MenuItemC
{
private:
  BStringC mText2;
  BStringC mText3;
  BStringC mText4; // add by zhanglanlan
  BStringC mText5;// add by zhanglanlan
  RESOURCE_ID(ImageResT) mIconId3;
  uint32 mdwFalgs;
  RESOURCE_ID(StringT) mHelpTextContext;
  protected:
  ImageC *mGridImage;//add
public:
  BYDMenuItemC();
  virtual ~BYDMenuItemC();
  void Create(RESOURCE_ID(BYDMenuItemResT) ResourceId);
  void GetExText(BStringC& Str,int index=0);
  void SetExText(BStringC &Text,int index=0);  	 
  void SetExText(RESOURCE_ID(StringT) Text,int index=0);
  void GetExIcon(ResourceIdT& IconId,int index);
  void SetExIcon(RESOURCE_ID(ImageResT) Icon,int index=0);
  uint32  GetFlags(){return mdwFalgs;}
  void SetFlags(uint32 flags){mdwFalgs=flags;}
  bool GetCheck(){return mIsChecked;}
  void SetCheck(bool bCheck){mIsChecked = bCheck;}
  void SetExHelpText(RESOURCE_ID(StringT) Text);
  void GetExHelpText(RESOURCE_ID(StringT)& Text){Text = mHelpTextContext;};
  BYDMenuItemC* CloneItem(void);
//----add 1218
  ImageC* GetImage(){return mGridImage;}
  void ReleaseImage();
  //void releaseImage	


  virtual ItemDrawerC* GetItemDrawer(uint16 ItemIndex);
  
};
typedef BYDMenuItemC* (*ItemDataFuncT)(BYDMenuItemC*ItemData,  uint16  index);

class BYDHugeMenuModelC:public BYDMenuModelC//MenuModelC 
{
public:
	BYDHugeMenuModelC();
	virtual ~BYDHugeMenuModelC();
	virtual MenuItemC* GetItem(uint16 ItemIndex);
	virtual void PrepareScreenData(PrepareTypeT PrepareType); 
	virtual uint16 GetItemCount(){return m_nItemCount;}
	virtual void SetItemCount(uint32 nCount){m_nItemCount = nCount;}
	void SetCallBack(ItemDataFuncT  func){m_cbFunc =func;};

	MenuItemC* GetMenuItem(uint16 ItemIndex);
	protected:
	ItemDataFuncT m_cbFunc;

private:
	BVectorC mTempItems;
	uint16 m_nItemCount;

private:
	inline void InsertItem(uint16 ItemIndex, MenuItemC *MenuItem)
	{
	  ItemIndex = ItemIndex;
	  MenuItem = MenuItem;
	};
	inline void AppendItem(MenuItemC *MenuItem)
	{
	  MenuItem = MenuItem;
	};
	inline uint16 DeleteItem(MenuItemC *ItemP)
	{
	  ItemP = ItemP;
	  return 0; 
	};

};
//Add by BYD-zouwq 2007.09.12
/*****************************************************************************
* Initial revision
*****************************************************************************/



#endif
