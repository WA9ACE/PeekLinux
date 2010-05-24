

#ifndef UIMODEL_H_
#define UIMODEL_H_

#include "sysdefs.h"

#include "buistring.h"
#include "buiapp.h"
#include "buiview.h"
#include "buivector.h"

//! Base class for all model
class ModelC    
{
public:

  ModelC();
  virtual ~ModelC();

  ViewC *GetActiveView() const;
  virtual ViewC *GetView(uint8 Index) const;
  uint8 GetViewCount(void);

  void AddView(ViewC *ViewP);
  void RemoveView(uint8 Index);
  void UpdateAllViews(ViewC *SenderP);

  bool IsModified();
  void SetModifiedFlag(bool Modified = TRUE );

  virtual void SetTitle(const BStringC &Title);
  void GetTitle(BStringC &Title);

  virtual void SetPathName(const BStringC &PathName);
  void GetPathName(BStringC &PathName);
  
/*! \cond private */
protected:

  BStringC mTitle;
  BStringC mPathName;
  bool mModified;
  BVectorC mViewArray; 
  ViewC *mActiveViewP;
/*! \endcond */  

};

inline ViewC *ModelC::GetActiveView() const
{
  return mActiveViewP;
}

inline bool ModelC::IsModified()
{
  return mModified;
}

inline void ModelC::SetModifiedFlag(bool Modified)
{
  mModified = Modified;
}

inline uint8 ModelC::GetViewCount(void)
{
  return mViewArray.GetSize();
}





#endif


