#ifndef UIRADIOBOX_H
#define UIRADIOBOX_H

#include "buiwindow.h"
#include "buicontrol.h"
#include "buimsgids.h"

#include "restypedefines.h"
#include "buicheckboxctrl.h"
#include "buivector.h"

class RadioBoxCtrlC : public ControlC
{
public:
	RadioBoxCtrlC(void);
	virtual ~RadioBoxCtrlC(void);

private:
	BVectorC *mRadioButtons;
	int16 mNumOfBts;
	int16 mChecked;
    uint16 mSelectedItem;
	ResourceIdT mFocusImageId;
	ResourceIdT mBackImageId;


protected:
	virtual bool LoadResource(ResourceIdT ResourceId);
	virtual void Draw(DCC * DcP);
/*! \cond private */

  bool OnNotify(uint32 ParamA, int32 ParamB);
  void OnParentNotify(uint32 Message, int32 Param);  
  void OnShowNotify(void);
  void OnHideNotify(void);
  int32 OnCreate(void);
  void OnDestroy(void);
  void OnMove(int16 X, int16 Y);
  void OnSize(int16 Width, int16 Height);
  void OnEnable(bool Enable);
  void OnShow(bool Show);  
  bool OnEraseBkgnd(void);
  void OnSetFocus(WindowC* OldWinP);  
  void OnKillFocus(WindowC* NewWinP);
  bool OnKeyPress(uint32 KeyCode);
  bool OnKeyRelease(uint32 KeyCode);

  bool HandleUpKey( bool IsUpKey );

public:
	void SetCheckedItem(uint16 Item);
	uint16 GetCheckedItem(void){return mChecked;};
	void SetFocusImageId(ResourceIdT ImageResId);
	ResourceIdT GetFocusImageId(void){return mFocusImageId;};
	void SetBackImageId(ResourceIdT ImageResId);
	ResourceIdT GetBackImageId(void){return mBackImageId;};
  
/*! \endcond */  
private:
	DECLARE_MAIL_MAP()
};

#endif
