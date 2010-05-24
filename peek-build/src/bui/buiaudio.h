

#ifndef UIAUDIO_H
#define UIAUDIO_H


#include "balsndapi.h"

#include "buiappids.h"
#include "builinkedlist.h"

/* Define app priorities */
#define APP_MAX_PRI                 0xFFFFFFFF
enum
{
    SMS_APP_PRI= 1,
    SETTING_APP_PRI,
    MYMEDIA_APP_PRI,
    MP3_APP_PRI,
    FM_APP_PRI,
    RECORDER_APP_PRI,
    SCHEDULE_APP_PRI,
    CALL_APP_PRI = APP_MAX_PRI
};
/*
#define SMS_APP_PRI                 1
#define SETTING_APP_PRI             2
#define MYMEDIA_APP_PRI             3
#define MP3_APP_PRI                 4
#define SCHEDULE_APP_PRI            5
#define CALL_APP_PRI                APP_MAX_PRI
*/

//! Structure for registering sound event
typedef struct
{
  bool   IsUse;                  /* if this entry is in use */
  uint32 Priority;               /* priority of the corresponding application */
  uint32 AppSoundEventMsgId;     /* sound event message to app when sound is suspend or resume */
  uint32 AppEarphoneEventMsgId;  /* earphone event message to app when earphone is connected or removed */
} UiSndRegTableT;

//! Identifier of device type
typedef enum
{
  DEV_CURRENT,
  DEV_DEFAULT
} DeviceTypeT;

//! audio class
class AudioC
{ 
public:
  static AudioC* GetInstance(void);
  static void DelInstance(void);

  ~AudioC();

  void RegisterSndApp(AppIdT AppId, uint32 Priority, uint32 AppSoundEventMsgId, uint32 AppEarphoneEventMsgId);
  void UnRegisterSndApp(AppIdT AppId);

    int32 PlayFM(AppIdT AppId, bool PutInActQueue);

  int32 PlayMp3File(AppIdT AppId, const uint8 *filename, uint8 len, uint8 Iteration, 
                    BalSoundVolumeT BalSoundVolume = BAL_SOUND_VOLUME_CURRENT,
                    bool PutInActQueue = TRUE, DeviceTypeT DeviceType = DEV_DEFAULT);
  int32 PlayMidiFile(AppIdT AppId, const uint8 *filename, uint8 len, uint8 Iteration, uint32 Duration,  
                     BalSoundVolumeT BalSoundVolume = BAL_SOUND_VOLUME_CURRENT,
                     bool PutInActQueue = FALSE, DeviceTypeT DeviceType = DEV_DEFAULT);
  int32 PlayMidiBuf(AppIdT AppId, void *Buffer, uint16 BufferLen, uint8 Iteration, uint32 Duration,
                    BalSoundVolumeT BalSoundVolume = BAL_SOUND_VOLUME_CURRENT,
                    bool PutInActQueue = FALSE, DeviceTypeT DeviceType = DEV_DEFAULT);
  int32 PlayVirtualMusic(AppIdT AppId);
  int32 PlayRecord(AppIdT AppId, bool PutInActQueue);

  bool StopMusic(AppIdT AppId, bool DelFromActQueue = TRUE);
  bool StopVirtualMusic(void);

  inline bool GetMp3StopFlag(void) const;
  inline bool SetMp3StopFlag(bool flag);
  void Reset(void);

private:
  static AudioC *_instance;
  AudioC();

  int32 RemoveAppFromQueue(uint32 AppId);
  bool InsertAppInQueue(uint32 AppId);
  bool SuspendMusicInQueue(uint32 AppId);
  void ResumeMusicInQueue(void);

  uint16 PlayMusicFile(const uint8 *filename, uint8 len, BalSoundFormatT BalSoundFormat, uint8 Iteration, 
                       uint32 Duration = SOUND_DURATION_FOREVER);
  void SwitchDevice(AppIdT AppId, BalSoundFormatT BalSoundFormat, DeviceTypeT DeviceType);
  int32 PirCompareToHead(uint32 AppId);

private:
  UiSndRegTableT mSndRegTable[APPID_MAX];
  BLinkedListC mActSndAppList;
  bool mMp3StopFlag; //!< Flag to check current mp3 soundtrack is playend or stopped compulsively
  AppIdT mCurrentAppId;
};

bool AudioC::GetMp3StopFlag(void) const
{
  return mMp3StopFlag;
}

bool AudioC::SetMp3StopFlag(bool flag)
{
  bool Temp = mMp3StopFlag;
  mMp3StopFlag = flag;
  return Temp;
}





#endif
