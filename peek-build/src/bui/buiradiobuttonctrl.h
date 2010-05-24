#ifndef UIRADIOBUTTON_H
#define UIRADIOBUTTON_H 

#include "buiwindow.h"
#include "buicheckboxctrl.h"

class RadioButtonC : public CheckboxCtrlC
{

public:
	RadioButtonC();
	~RadioButtonC();
protected:
	bool virtual LoadResource(ResourceIdT ResourceId);
	void virtual Draw(DCC * DcP);
private:
	RESOURCE_ID(ImageResT)  mIcon;
	uint8                   mDisplayMode;
	bool                    mIconIsDynamic;
	int32                   mBGColor;
	int32                   mFGColor;
	bool                    mIsPressed;
	BalDispFontT               mFont;
	BalDispHAlignT      mHAlignMode;
	bool			mShowCheckState;
	RESOURCE_ID(ImageResT)		mCheckedIcon;
	RESOURCE_ID(ImageResT)		mUncheckedIcon;
	BalDispHAlignT	mIconAlignMode;
	ImageC* mCheckImage;

	void DrawIcon(DCC *DcP,BalDispRectT Rect,BalDispBitmapT Bmp);
	DECLARE_MAIL_MAP()

/*! \cond private */
protected:

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
/*! \endcond */  

	
};

#endif
