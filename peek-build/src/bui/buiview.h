

#ifndef UIVIEW_H_
#define UIVIEW_H_

#include "sysdefs.h"

#include "buidialog.h"

#include "restypedefines.h"

class BApplicationC;
class ModelC;

//! Base class for all view
class ViewC
{

public:

  ViewC();
  virtual ~ViewC();
  
  ModelC *GetModel() const;
  bool SetModel(ModelC *ModelP);
  void SetApp(BApplicationC *AppP);

  virtual void InitialUpdate();
  virtual void Update(ViewC *SenderP);    
  virtual void ActivateView(bool bActivate,ViewC *ActivateViewP,ViewC *DeactiveViewP);

  virtual DialogC *OpenDialog(ResourceIdT ResId,int DialogId,uint32 CloseMailId = 0,DispLCDTypeT LcdType = LCD_MAIN);
  virtual bool CloseDialog(int DialogId);
  virtual bool CancelDialog(int DialogId);
  virtual bool UpdateDialog(int DialogId);
  
/*! \cond private */
protected:

  ModelC *mModelP;
  bool mActive;
  BApplicationC *mAppP;
/*! \endcond */  
  
};

inline void ViewC::SetApp(BApplicationC *AppP)
{
  mAppP = AppP;
}

inline ModelC *ViewC::GetModel() const
{
  return mModelP;
}




#endif

