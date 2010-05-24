

#ifndef STATICTEXTCTRL_H
#define STATICTEXTCTRL_H 

#include "buicontrol.h"
#include "buistatictextdata.h"
#include "buimsgtimer.h"
//! Color configuration of Static text control
typedef struct
{
  BalDispColorT TextColor;      //!< Text Color
  BalDispColorT BorderColor;    //!< Border Color, just like PenColor in DC
  BalDispColorT BKColor;        //!< Background Color, just like BrushColor in DC
}StaticTextCtrlColorsT;

//! Class define of static text cortrol
class StaticTextCtrlC : public ControlC
{
  public:
    StaticTextCtrlC();
    virtual ~StaticTextCtrlC();
    virtual bool CanHaveFocus(){return FALSE;}    // virtual function of WindowC

    void SetText(const BStringC &String, uint16 CellIndex = 0);
    void SetText(RESOURCE_ID(StringT) StringResId, uint16 CellIndex = 0);
    void SetText(uint8 *TextP, BalDispFontCodepageT FontCodePage = BAL_DISP_FNT_CP_LATIN1, int16 StringLen = -1, uint16 CellIndex = 0);
    BStringC GetText(uint16 CellIndex = 0);

    void SetHorizontalAlignment(BalDispHAlignT HorizontalAlignment) {mProperty.HAlign = HorizontalAlignment;}
    BalDispHAlignT GetHorizontalAlignment() const {return mProperty.HAlign;}

    void SetVerticalAlignment(DispVAlignT VerticalAlignment) {mProperty.VAlign = VerticalAlignment;}
    DispVAlignT GetVerticalAlignment() const {return mProperty.VAlign;}

    FontC SetTextFont(BalDispFontT TextFont) {return mDc.SetFont(FontC(TextFont));}
    BalDispFontT GetTextFont() const {return mDc.GetFont().Type;}

    BalDispColorT SetTextColor(BalDispColorT TextColor) {return mDc.SetTextColor(TextColor);}
    BalDispColorT GetTextColor() const {return mDc.GetTextColor();}

    BalDispColorT SetBKColor(BalDispColorT BKColor){return mDc.SetBrush(BrushC(BKColor)).Color;}
    BalDispColorT GetBKColor() {return mDc.GetBrush().Color;}

    BalDispColorT SetBorderColor(BalDispColorT BorderColor) {return mDc.SetPen(PenC(BorderColor, BAL_DISP_PEN_1)).Color;}
    BalDispColorT GetBorderColor() const {return mDc.GetPen().Color;}

    BalDispModeT SetBKMode(BalDispModeT BKMode){return mDc.SetBackGroundMode(BackGroundModeC(BKMode)).Type;}
    BalDispModeT GetBKMode() {return mDc.GetBackGroundMode().Type;}

    int8 SetBorder(uint8 Border) {int8 OldBorder = mBorder; mBorder = Border; return OldBorder;}
    int8 GetBorder() const {return mBorder;}

    int8 SetDispStyle(int8 DispStyle) {int8 Oldstyle = mProperty.DispStyle; mProperty.DispStyle = DispStyle; return Oldstyle;}
    int8 GetDispStyle() const {return mProperty.DispStyle;}
    
    MoveModeT GetTextMoveMode( ) const {return mMoveProperty.TextMoveMode;}
    uint32 GetTextMoveInterval() const {return mMoveProperty.MoveInterval;}

    int16 GetTextHeight(const int16 dx, uint16 CellIndex = 0);
    int16 GetTextWidth(uint16 CellIndex = 0);

    
    void StartFlashing(uint32 ShowTime = 1000, uint32 HideTime = 500, uint32 Duration = 0); //Duration = 0, Will always flashing
    void SetMovePro(MoveModeT MoveMode = TEXT_HORIZONTAL_ROLL, uint32 MoveInterval = 500); // auto start the moving when cell text be drawn
    
    void Invalidate(bool Erase = TRUE);
	void SetFocusAble(bool bFocus){mFocusable = FALSE;}//static never ever can be focused   --alexander

  protected:
  /*! \cond private */

    virtual bool LoadResource(ResourceIdT ResourceId);
    virtual void Draw(DCC *DcP);

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
/*! \endcond */

  private:
    void ProcessFlashing();
    void ProcessMoving();
    
    BalDispFontCodepageT SetFontCodePage(const BalDispFontCodepageT FontCodePage){return mDc.SetFontCodepage(FontCodePage);}
    BalDispFontCodepageT GetFontCodePage() const {return mDc.GetFontCodepage();}
    void InitDataMembers(const StaticTextCtrlResT *StaticTextCtrlResP);
    void InitAppearance(void);
    
    void OnTimerCallBack();
    void StopFlashing();
    void LayoutCells();
  public:
    //following is for multi icons and text interface
    void SetRichString(const RichStringC &NewRichStr);

    /*!
    \brief Get the rich string data of control, maybe need to do some modify for the data

    \param none
    \retval pointer to the RichStringC data
    */
    RichStringC* GetRichString() const {return mCellArrayP;}

    void Append(const BStringC &String);
    void Append(RESOURCE_ID(StringT) StringResId);
    void Append(uint8 *TextP, BalDispFontCodepageT FontCodePage = BAL_DISP_FNT_CP_LATIN1, int16 StringLen = -1);
    void Append(RESOURCE_ID(ImageResT) IconId);
    
    void Insert(uint16 CellIndex, const BStringC &String);
    void Insert(uint16 CellIndex, RESOURCE_ID(StringT) StringResId);
    void Insert(uint16 CellIndex, uint8 *TextP, BalDispFontCodepageT FontCodePage = BAL_DISP_FNT_CP_LATIN1, int16 StringLen = -1);
    void Insert(uint16 CellIndex, RESOURCE_ID(ImageResT) IconId);
    
    void SetIcon(RESOURCE_ID(ImageResT) IconId, uint16 CellIndex);
    
    RESOURCE_ID(ImageResT) GetCellIcon(uint16 CellIndex);
    
    void Delete(uint16 CellIndex = 0);
  private:
    static const int MARGIN;
    STPropertyT mProperty;
    STMovePropertyT mMoveProperty;
    int8 mBorder;      //0 No Border, 1 Border
    
    const StaticTextCtrlResT *mStaticTextCtrlResP;      //Used in Create(ResourceIdT ResourceId) funciton only
    
    BTimerC mTimer;
    uint32 mFlashingCount; //remaining times for flashing
    bool mFlashing;
    
    //following is for multi icons and text interface
    RichStringC *mCellArrayP;
    
    DECLARE_MAIL_MAP()

};  





#endif

