
#ifndef __UITITLEBARCTRL_H_DEF__
#define __UITITLEBARCTRL_H_DEF__ 

#include "buiwindow.h"
#include "buicontrol.h"
#include "buimsgtimer.h"
#include "buiimage.h"

#include "rescommon.h"
#include "restypedefines.h"
#include "buipopupdialog.h"
#include "resexcommonctrl.h"
#include "resimages.h"


#define TITLEBARCTRL_MAX_BATTERY_IMGNUM	5
#define TITLEBARCTRL_MAX_SIGNAL_IMGNUM	6 //5  lihaibing 20081013 
#define TITLEBARCTRL_MAX_INDICATOR_IMGNUM	8

#define TITLEBARCTRL_WINID 0xFFFF00F1
#define TITLEBARCTRL_BGIMGID IDI_TITLEBAR_BG_PNG

#define TITLEBARCTRL_INDICATOR_TIMEOUT 20000
#define TITLEBARCTRL_INDICATOR_INTERVAL 100
#define TITLEBARCTRL_ELECTRICIZE_TIMEOUT 500//1000 change to 500 liuhaibing 2008.06.16
#define TITLEBARCTRL_LOCALTIME_TIMEOUT 30000



class TitleBarCtrlC : public ControlC
{
protected:
  virtual bool LoadResource(ResourceIdT ResourceId);
  virtual void Draw(DCC *DcP);
  virtual bool Create(ResourceIdT ResourceId, WindowC* ParentWinP, uint32 WinId = TITLEBARCTRL_WINID);
  void OnBatteryChangeNotify(uint32 ParamA, int32 ParamB);
  void OnElectricizeBeginNotify();
  void OnElectricizeStopNotify(uint32 ParamA, int32 ParamB);
  void OnSignalChangeNotify(uint32 ParamA, int32 ParamB);
  int32 OnCreate(void);
  void OnDestroy(void);
  void OnShowNotify(void);
  void OnHideNotify(void);
  void OnShow(bool Show);


  void OnLocalTimer();
  void OnIndicatorTimer();
  void OnElectricizeTimer();
  bool OnEraseBkgnd(void);
  void OnIndicatorStop();

public:
  TitleBarCtrlC();
  virtual ~TitleBarCtrlC();

  
  void SetRightTitle(RESOURCE_ID(StringResT) ResourceId,RESOURCE_ID(StringResT) ResourceId2=NULL);
  void SetRightTitle(BStringC& str,BStringC& str2);
  void SetRightTitle(BStringC& str);
  void SetRightTitle(char* str,char* str2=NULL);
  void StartIndicator(uint32 nTimeout=TITLEBARCTRL_INDICATOR_TIMEOUT , uint32 nInterval=TITLEBARCTRL_INDICATOR_INTERVAL); //nSpeed equal nTimeout
  void StopIndicator();
  bool UpdateBattery();
  

// Data information
private:
  bool m_isInit;
  
  BStringC m_sRightTitle;
  BStringC m_sRightTitle_ex;

static   uint32 m_nBatteryImgLst[TITLEBARCTRL_MAX_BATTERY_IMGNUM];
static   uint32 m_nSignalImgLst[TITLEBARCTRL_MAX_SIGNAL_IMGNUM];  
static   uint32 m_nIndicatorImgLst[TITLEBARCTRL_MAX_INDICATOR_IMGNUM]; 
#ifdef BAL_SELECT_PLMN 
static   uint32 m_nSignalRoamingImgLst[TITLEBARCTRL_MAX_SIGNAL_IMGNUM];
#endif
  

static   BTimerC m_LocalTimer;
static   BTimerC m_IndicatorTimer;  
static   BTimerC m_ElectricizeTimer;  
static   BTimerC m_IndicatorTimeout;  
static   uint32 m_nBatteryIndex;
static   uint32 m_nSignalIndex;



static  uint32 m_nBatteryTimeInt;

static   ImageC* m_pBatteryLogo;
static   ImageC* m_pSignalLogo;
static   ImageC* m_pIndictLogo;  

static   uint32 m_nIndicatorIndex;
static   uint32 m_nIndicatorTimeInt;
static   uint32 m_nIndicatorTimeStart;//liuhaibing
static   int32 m_nIndicatorTimeRemain;//liuhaibing

static   uint8 m_nMonth;
static   uint8 m_nDay;
static    uint8 m_nHour;
static    uint8 m_nMinite;
static    bool m_nPMflag;
private:
  void SetDateTime(uint8 nMonth, uint8 nDay, uint8 nHour, uint8 nMinite,uint8 PMflag);
  void SetBatteryImageID(int index);
  void SetSignalmageID(int index);
  void SetIndicatorImageID(int index);
  bool UpdateDatetime();
  bool UpdateSignal();
  
  

  DECLARE_MAIL_MAP()
};



#endif //__UITITLEBARCTRL_H_DEF__

