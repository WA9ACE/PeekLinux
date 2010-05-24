

#ifndef ASLMIDIDEF_H_
#define ASLMIDIDEF_H_

#include "sysdefs.h"
#include "restypedefines.h"
#include "resexsettings.h"
#include "resen.h"

#ifdef  __cplusplus
extern "C" {
#endif

enum
{
  E_RINGTONE_BEGIN,
  E_RINGTONE_AGENCY     = E_RINGTONE_BEGIN,
  E_RINGTONE_AIRES,
  E_RINGTONE_BALLROOM,
  E_RINGTONE_BROOK,
  E_RINGTONE_DANCE_FLOOR,
  E_RINGTONE_DEER,
  E_RINGTONE_DESK_PHONE,
  E_RINGTONE_GREAMY_RAINY_DAY,
  E_RINGTONE_FLY_HIGH,
  E_RINGTONE_HOT_SWING,
  E_RINGTONE_LUMINOSITY,
  E_RINGTONE_MASQUERADE,
  E_RINGTONE_MORE_MAMBO,
  E_RINGTONE_NOCTURNAL,
  E_RINGTONE_NOKIA_TUNE,
  E_RINGTONE_ONE_TWO_THREE,
  E_RINGTONE_PARTY,
  E_RINGTONE_PATTERN,
  E_RINGTONE_POLITE,
  E_RINGTONE_TIMID,
  E_RINGTONE_END,
  E_ALERTTONE_BEGIN     = E_RINGTONE_END,
  E_ALERTTONE_CALENDAR1 = E_ALERTTONE_BEGIN,
  E_ALERTTONE_CALENDAR2,
  E_ALERTTONE_CALENDAR3,
  E_ALERTTONE_CLOCK1,
  E_ALERTTONE_CLOCK2,
  E_ALERTTONE_CLOCK3,
  E_ALERTTONE_MESSAGE1,
  E_ALERTTONE_MESSAGE2,
  E_ALERTTONE_MESSAGE3,
  E_ALERTTONE_MESSAGE4,
  E_ALERTTONE_MESSAGE5,
  E_ALERTTONE_END,
  E_TONES_MAX
};

/*according to position in gStaticBufferRinger Array*/
#define STANDARD_ALERT_ID   E_RINGTONE_NOKIA_TUNE
#define SPECIAL_ALERT_ID    5
#define LONGLOUD_ALERT_ID   6
#define BEEP_ONCE_ALERT_ID  7
#define POWERON_ID  (E_ALERTTONE_END+8-2)
#define POWEROFF_ID  (E_ALERTTONE_END+9-2)

//!midi content structure
typedef struct
{
  const unsigned char *Buffer;    //!<midi buffer
  uint16 BufferLen;               //!<midi buffer lenth
  uint8  ucEnumIndex;
  const char* pszName;
}StaticBufferMidiT;

//!predefine midi infomation structure
typedef struct
{
  uint32 RealSoundId;                   //!<midi id
  RESOURCE_ID(StringT) RingerNameText;  //!<midi name
}PredefineMidiInfoT;

extern const uint16 AslGetMidiBufReservedNum();
extern const uint16 AslGetMidiBufReservedMaxNum();
extern const StaticBufferMidiT* AslGetMidiBufData(uint16 index);
extern const StaticBufferMidiT* AslGetPredefMidiBufByName(const char* pszName);

extern uint32 AslGetPredefMidiEntrySize();
extern uint32 AslGetPDMidiInfoIndexById(uint32 Id); 
extern const PredefineMidiInfoT* AslGetPredefMidiInfoByIndex(uint32 Index); 
#ifdef  __cplusplus
}
#endif



#endif
