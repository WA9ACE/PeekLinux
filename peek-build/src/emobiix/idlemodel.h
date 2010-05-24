

#ifndef IDLEMODEL_H_
#define IDLEMODEL_H_

#include "buikeymap.h"
#include "buimodel.h"
#include "buimsgtimer.h"

#include "uilifecycleutils.h"
#include "idleview.h"
#include "conflictmodel.h"

/*! 
\brief Idle model class define

Description:
\n This class is responsible for idle model functions which including:
\n 1. Work with idle view to display idle dialog(announciator, background, time...)
\n 2. Process dialing function
\n 3. Process emergency call function
\n 4. Enter mainmenu or shortcut
\n 5. Process key lock and phone lock function
*/
class IdleModelC: public ModelC
{
public:
  IdleModelC(ConflictModelC* ModelP);
  virtual ~IdleModelC(void);

  bool HandleKey(uint32 KeyMsgId = KEYMSG_PRESS, uint32 KeyCode = KP_NO_KEY);   //enter or return idle state
  bool ProcessMail(uint32 MsgId, void *MsgP = NULL);
  
private:
  ConflictModelC* m_pConflictModel;
  bool        m_bSleep;
};

#endif


