

#ifndef UIMAILCLIENT_H
#define UIMAILCLIENT_H 

#include "buidefs.h"


class MailClientC;

typedef void (MailClientC::*MailHandlerT)(void);

//! mail handler type defines, the name indicate the handler's return value and param type.
enum MailHandlerTypeT
{
  MHType_v_v,
  MHType_v_vp,
  MHType_v_b,
  MHType_v_w,
  MHType_v_wl,
  MHType_v_ii,
  MHType_v_W,
  MHType_b_v,
  MHType_b_vp,
  MHType_b_w,
  MHType_b_wl,
  MHType_l_v,
  MHType_v_wp,
  MHType_END
};

//! MailMapEntry struct define
struct MailMapEntryT
{
  uint32 MailMsgId; //!< the begin id of the mail message group
  uint32 LastMsgId; //!< the end id of the mail message group
  MailHandlerTypeT Type; //!< mail message handler type
  MailHandlerT MailHandlerP; //!< the point to the mail messsage handler
};



#if 0 //neil 
//! MailMap struct define
struct MailMapT
{
  const MailMapT* BaseMapP; //!< point to the MailMapT of its base class 
  const MailMapEntryT* MailEntryP; //!< point to the mail map table
};

/*!
\brief Begins the definition of the mail map

\param ThisClass -[in] specifies the name of the class whose mail map this is
\param BaseClass -[in] specifies the name of the base class of ThisClass
*/
#define BEGIN_MAIL_MAP(ThisClass, BaseClass) \
 const MailMapT* ThisClass::GetMailMap() const \
  { return &ThisClass::MailMap; } \
 const MailMapT ThisClass::MailMap = \
 { &BaseClass::MailMap, &ThisClass::MailEntries[0] }; \
   const MailMapEntryT ThisClass::MailEntries[] = \
 { \

//! Ends the definiton of the mail map
#define END_MAIL_MAP() \
  {0, 0, MHType_END, (MailHandlerT)0 } \
 };

//! Declares the mail map
#define DECLARE_MAIL_MAP() \
private: \
 static const MailMapEntryT MailEntries[]; \
protected: \
 static const MailMapT MailMap; \
 virtual const MailMapT* GetMailMap() const;

//! Terminates the mail map
#define DEFINE_TERMINATING_MAIL_MAP(ThisClass) \
const MailMapT ThisClass::MailMap = \
{ \
  NULL, \
  &ThisClass::MailEntries[0] \
}; \
const MailMapT* ThisClass::GetMailMap() const \
{ \
  return &ThisClass::MailMap; \
} \
const MailMapEntryT ThisClass::MailEntries[] = \
{ \
  {0, 0, MHType_END, (MailHandlerT)0 } \
};
#else

//! MailMap struct define
struct MailMapT
{
  MailMapT* BaseMapP; //!< point to the MailMapT of its base class 
  MailMapEntryT* MailEntryP; //!< point to the mail map table
};


/*!
\brief Begins the definition of the mail map

\param ThisClass -[in] specifies the name of the class whose mail map this is
\param BaseClass -[in] specifies the name of the base class of ThisClass
*/
#define BEGIN_MAIL_MAP(ThisClass, BaseClass) \
 MailMapT* ThisClass::GetMailMap()  \
  { return &ThisClass::MailMap; } \
  MailMapT ThisClass::MailMap = \
 { &BaseClass::MailMap, &ThisClass::MailEntries[0] }; \
    MailMapEntryT ThisClass::MailEntries[] = \
 { \

//! Ends the definiton of the mail map
#define END_MAIL_MAP() \
  {0, 0, MHType_END, (MailHandlerT)0 } \
 };

//! Declares the mail map
#define DECLARE_MAIL_MAP() \
private: \
 static  MailMapEntryT MailEntries[]; \
protected: \
 static  MailMapT MailMap; \
 virtual  MailMapT* GetMailMap() ;

//! Terminates the mail map
#define DEFINE_TERMINATING_MAIL_MAP(ThisClass) \
 MailMapT ThisClass::MailMap = \
{ \
  NULL, \
  &ThisClass::MailEntries[0] \
}; \
 MailMapT* ThisClass::GetMailMap()  \
{ \
  return &ThisClass::MailMap; \
} \
 MailMapEntryT ThisClass::MailEntries[] = \
{ \
  {0, 0, MHType_END, (MailHandlerT)0 } \
};

#endif

#define ON_MSG_VOID_VOID(MailMsgId, MailFunc) \
 { MailMsgId, MailMsgId, MHType_v_v, \
    (MailHandlerT)(void (MailClientC::*)(void))&MailFunc },

#define ON_MSG_VOID_VOIDPTR(MailMsgId, MailFunc) \
 { MailMsgId, MailMsgId, MHType_v_vp, \
    (MailHandlerT)(void (MailClientC::*)(void*))&MailFunc },

#define ON_MSG_VOID_WORD(MailMsgId, MailFunc) \
 { MailMsgId, MailMsgId, MHType_v_w, \
    (MailHandlerT)(void (MailClientC::*)(uint32))&MailFunc },

#define ON_MSG_VOID_WORDLONG(MailMsgId, MailFunc) \
 { MailMsgId, MailMsgId, MHType_v_wl, \
    (MailHandlerT)(void (MailClientC::*)(uint32, int32))&MailFunc },

#define ON_MSG_BOOL_VOID(MailMsgId, MailFunc) \
 { MailMsgId, MailMsgId, MHType_b_v, \
    (MailHandlerT)(bool (MailClientC::*)(void))&MailFunc },

#define ON_MSG_BOOL_VOIDPTR(MailMsgId, MailFunc) \
 { MailMsgId, MailMsgId, MHType_b_vp, \
    (MailHandlerT)(bool (MailClientC::*)(void*))&MailFunc },

#define ON_MSG_BOOL_WORD(MailMsgId, MailFunc) \
 { MailMsgId, MailMsgId, MHType_b_w, \
    (MailHandlerT)(bool (MailClientC::*)(uint32))&MailFunc },

#define ON_MSG_BOOL_WORDLONG(MailMsgId, MailFunc) \
 { MailMsgId, MailMsgId, MHType_b_wl, \
    (MailHandlerT)(bool (MailClientC::*)(uint32, int32))&MailFunc },

#define ON_MSG_VOID_WORDPTR(MailMsgId, LastMailMsgId, MailFunc) \
  { MailMsgId, LastMailMsgId, MHType_v_wp, \
    (MailHandlerT)(void (MailClientC::*)(uint32,void*))&MailFunc },

//! Mail Client class define  
class MailClientC
{
public:
  MailClientC();
  virtual ~MailClientC();

  int32 SendMail(uint32 MailMsgId, WinHandleT WinHandle = WIN_HANDLE_NULL, uint32 ParamA = 0, int32 ParamB = 0);
  bool PostMail(uint32 MailMsgId, WinHandleT WinHandle = WIN_HANDLE_NULL, uint32 ParamA = 0, int32 ParamB = 0, ParamTypeT ParamType = BY_VALUE);

  virtual int32 ProcessMail(uint32 MailMsgId, WinHandleT WinHandle, uint32 ParamA, uint32 ParamB);

  uint32 GetMailMask(void) const;
  void SetMailMask(uint32 MailMask);

protected:
  virtual int32 DefMailProc(uint32 MailMsgId, WinHandleT WinHandle, uint32 ParamA, uint32 ParamB);

private:
  bool MailMapProc(uint32 MailMsgId, uint32 ParamA, uint32 ParamB, int32* ResultP);

  uint32 mMailMask;

  DECLARE_MAIL_MAP()
};

/*!
\brief Get the mail mask flags

\retval the mail mask double word
*/
inline uint32 MailClientC::GetMailMask(void) const
{
  return mMailMask;
}

/*!
\brief Set the mail mask flags

\param MailMask -[in] the mask to be set
*/ 
inline void MailClientC::SetMailMask(uint32 MailMask)
{
  mMailMask = MailMask;
}




#endif

