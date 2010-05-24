
#ifndef UIANNDATACENTER_H
#define UIANNDATACENTER_H 


#include "builinkedlist.h"
#include "buimsguser.h"
#include "buiannounciatorcust.h"
#include "buiannounciator.h"

/*!
\brief Announciator data center class

Description: 
\n This class is responsible from storage element state which announciator displayed
*/
class AnnDataCenterC: public MailClientC
{
public:
  virtual ~AnnDataCenterC();

  static AnnDataCenterC* GetInstance(void);

  void RegisterForMail(AnnounciatorC* AnnP);
  void UnregisterForMail(AnnounciatorC * AnnP);

  AnnStateIdT GetElementState(AnnElementIdT ElementId);

private:

  void OnStateNotify(uint32 ElementId, int32 StateId);
  static AnnDataCenterC* _instance;

  AnnDataCenterC();
  AnnStateIdT mElementState[ANN_ELEMENT_MAX];  
  BLinkedListC mRegisteredAnns;

  DECLARE_MAIL_MAP()
};

//! Get the unique instance of the announciator data center
inline AnnDataCenterC* UiGetAnnDataCenter(void)
{
  return AnnDataCenterC::GetInstance();
}

#endif
