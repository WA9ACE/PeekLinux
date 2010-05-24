

#ifndef UIMAILSERVER_H
#define UIMAILSERVER_H 

#include "exeapi.h"
#include "sysdefs.h"

#include "buidefs.h"
#include "builinkedlist.h"
#include "buimsguser.h"


#define UI_EXTERNAL_MAIL_TYPE  EXE_MAILBOX_1
#define UI_INTERNAL_MAIL_TYPE  EXE_MAILBOX_2 
#define UI_PAINT_MAIL_TYPE  EXE_MAILBOX_3 
#define UI_LIGHT_MAIL_TYPE  EXE_MAILBOX_4 

#define UI_MAIL_TYPE (UI_EXTERNAL_MAIL_TYPE | UI_INTERNAL_MAIL_TYPE | UI_PAINT_MAIL_TYPE | UI_LIGHT_MAIL_TYPE)

#define UI_EXTERNAL_MAILBOX_ID   EXE_MAILBOX_1_ID
#define UI_INTERNAL_MAILBOX_ID   EXE_MAILBOX_2_ID
#define UI_PAINT_MAILBOX_ID   EXE_MAILBOX_3_ID
#define UI_LIGHT_MAILBOX_ID   EXE_MAILBOX_4_ID

//! Mail Server class define 
class MailServerC
{
public:
  virtual ~MailServerC();

  static MailServerC* GetInstance(void);

  void RegisterForMail(MailClientC* MailClientP);
  void UnregisterForMail(MailClientC* MailClientP);

  int32 SendMail(uint32 MailMsgId, WinHandleT WinHandle = WIN_HANDLE_NULL, uint32 ParamA = 0, int32 ParamB = 0);
  bool PostMail(uint32 MailMsgId, WinHandleT WinHandle = WIN_HANDLE_NULL, uint32 ParamA = 0, int32 ParamB = 0, ParamTypeT ParamType = BY_VALUE);

  void ProcessMail(void);
  void EmptyMailbox(ExeMailboxIdT MailboxID);

private:
  MailServerC();
  
  bool GetMail(MailMsgT* MailMsgP);
  int32 DispatchMail(const MailMsgT* MailMsgP);
  void FreeMail(MailMsgT* MailMsgP);

private:
  static MailServerC* _instance;
  BLinkedListC mRegisteredClients;
  void* mMailMsgP;
};

//! Get the unique instance of the MailServerC
inline MailServerC* UiGetMailServer(void)
{
  return MailServerC::GetInstance();
}




#endif
