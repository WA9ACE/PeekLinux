

#ifndef UIMENUEXCONTROL_H
#define UIMENUEXCONTROL_H


#include "buivector.h"
#include "buiwindow.h"
#include "buicontrol.h"

#include "buimenudefine.h"
#include "buimenumodel.h"
#include "buimenuview.h"
#include "buimenuimpl.h"
#include "buimenuctrl.h"
#include "buimenuexmodel.h"

//! BYD menu control define
class BYDHugeMenuCtrlC: public BYDMenuCtrlC
{
	public:
	BYDHugeMenuCtrlC();
	virtual ~BYDHugeMenuCtrlC(){};
	virtual void RefreshData();
	virtual void SetItemCount(uint32 nCount);
	void ChangeMenuView(MenuSizeT size,uint16 nCols = 1);
	void SetData(ItemDataFuncT Func);
	private:
	void PrepareData();
};





#endif

