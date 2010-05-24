

#ifndef STATICTEXTDATA_H
#define STATICTEXTDATA_H 

#include "buivector.h"
#include "baldispapi.h"
#include "buidcc.h"
#include "restypedefines.h"

/*! \cond private */
#define DISP_STYLE_3D_MASK 0x01
#define DISP_STYLE_REVERSE_MASK 0x02

typedef enum  
{
  CELL_DATA_TEXT_TYPE,  
  CELL_DATA_ICON_TYPE,
  CELL_DATA_INVALID_TYPE
} CellDataTypeT;


//! This data type is the vertiacl alignment used for the drawn text that is generated
typedef enum 
{ 
  DISP_VALIGN_UP,       //!< Aligns the point with the up side of the bounding rectangle
  DISP_VALIGN_CENTER,   //!< Aligns the point with the vertical center of the bounding rectangle
  DISP_VALIGN_DOWN      //!< Aligns the point with the down side of the bounding rectangle
} DispVAlignT;          

//! Text apperence
typedef PACKED struct
{
  BalDispHAlignT   HAlign;
  DispVAlignT   VAlign;
  uint8  DispStyle;  //!< bit0 3D, bit1 Reverse, bit2...7 Reserved  
} STPropertyT;

//! Text moving style
typedef enum  
{
  TEXT_STATIC,  //!< default mode, has none Move effect, will display ... if text can't fit in the rect
  TEXT_VERTICAL_SCROLL,
  TEXT_HORIZONTAL_ROLL,
  TEXT_FLASHING
}MoveModeT;

//! All moving property of the text control 
typedef PACKED struct
{
  MoveModeT TextMoveMode;
  uint32 MoveInterval;  //!< interval for text scrolling or rolling
  
  uint32 ShowTime;  //!< show time of flashing
  uint32 HideTime;  //!< Hide time of flashing
  uint32 Duration;  //!< duration of flashing
  bool   TextOn;    //!< flag to indicate if text is show or hide when flashing
} STMovePropertyT;

/*! \endcond */

//! pure virtual base class define of cell data, defined interfaces to support the management of RichStringC class
class CellC{
  public:
    CellC(CellDataTypeT CellType);
    virtual ~CellC() {}
    virtual CellC* Clone()=0;
    CellDataTypeT GetCellType() const { return mCellType;}
    
    //! if cell text is moving, return TRUE, else return FALSE
    bool IsCellMoving() const { return mIsMoveState;}
    //! set cell displayed rect
    void SetRect(const BalDispRectT &Rect) {mRect = Rect;}
    //! set cell apperence which will be drawed
    void SetProperty(const STPropertyT *Property) {mProperty = Property;}
    //! set cell moving property
    void SetMoveProperty(const STMovePropertyT *Property){mMoveProP = Property;}

    virtual int16 GetCellHeight(const int16 dx, const BalDispFontT font)=0;
    virtual int16 GetCellWidth(const BalDispFontT font)=0;
    virtual void StartFlashing()=0;
    virtual void StopFlashing()=0;
    virtual void ProcessFlashing()=0;
    
    virtual void ProcessMoving() {}
    virtual void StopMoving() {}
    
    virtual void Draw(DCC DC)=0;
    
  protected:
    //! set cell moving state
    void SetMoveState(bool curState) {mIsMoveState = curState;}
  protected:
    //! moving interval for the text
    const static int8 MOVING_STEP; 
    //! cell's drawn rect
    BalDispRectT mRect; 
    //! cell apperence pointer to the static text control
    const STPropertyT *mProperty; 
    //! cell moving property pointer to the static text control
    const STMovePropertyT *mMoveProP; 
  private: 
    bool mIsMoveState;
    CellDataTypeT mCellType;
};

//! Icon cell data processing class define
class IconCellC: public CellC
{
  public:
    IconCellC();
    IconCellC(const IconCellC &rhs);
    
    virtual ~IconCellC() {}
    virtual CellC* Clone();

    void SetIcon(RESOURCE_ID(ImageResT) IconId) { mResId = IconId;}
    //! Get resource ID of cell
    RESOURCE_ID(ImageResT) GetCellIcon() {return mResId;}
    
    virtual int16 GetCellHeight(const int16 dx, const BalDispFontT font);
    virtual int16 GetCellWidth(const BalDispFontT font);

    virtual void StartFlashing();
    virtual void StopFlashing();
    virtual void ProcessFlashing();
    
    virtual void Draw(DCC DC);
    
  private:
    IconCellC& operator=(const IconCellC &rhs);
    RESOURCE_ID(ImageResT)  mResId;
    RESOURCE_ID(ImageResT)  mBackupResId;
};

//! Text cell data processing class define
class TextCellC: public CellC
{
  public:
    TextCellC();
    TextCellC(const TextCellC &rhs);
    virtual ~TextCellC();
    virtual CellC* Clone();
    
    virtual int16 GetCellHeight(const int16 dx, const BalDispFontT font);
    virtual int16 GetCellWidth(const BalDispFontT font);
    
    void SetText(const BStringC &String);
    void SetText(RESOURCE_ID(StringT) StringResId);
    void SetText(uint8 *TextP, BalDispFontCodepageT FontCodePage = BAL_DISP_FNT_CP_LATIN1, int16 StringLen = -1);
    BStringC GetText(){ return BStringC(mStringP);}
    
    
    virtual void StartFlashing();
    virtual void StopFlashing();
    
    virtual void ProcessFlashing();
    virtual void ProcessMoving();
    virtual void StopMoving();
  
    
    virtual void Draw(DCC DC);
  private:
    void DrawMultiLineText(DCC &DC, uint8 *TextP, const uint16 TotalStrLen, const uint16 LineNum, const uint16 LineHeight, 
                const BalDispHAlignT HAlign, BalDispRectT LineRect, BalDispRectT AddedLineRect, bool DefaultDisplay = FALSE);
  
    BalDispFontCodepageT GetCodePage() const { return (BalDispFontCodepageT)mStringP->CodePage;}
    bool GetBytesDrawn(uint8 *TextP, int16 *BytesFitP, int16 *BytesDrawnP);
    uint8* CreateDefaultString(uint8 *TextP, uint16 len, BalDispFontCodepageT codepage);
    
    int16 mMovingOffset;
    bool mMovingToLeft;
  private:
    TextCellC& operator=(const TextCellC &rhs);
    StringT *mStringP;
    StringT *mBackupStrP; //for flash text replace
};

class StaticTextCtrlC;

//! Rich string class define, manage all of the cell draw, append, insert, remove operations
class RichStringC{
  friend class StaticTextCtrlC;
  public:
    RichStringC(){};
    RichStringC(const RichStringC &rhs);
    virtual ~RichStringC();
  public:
    //following is for multi icons and text interface
    void Append(const BStringC &String);
    void Append(RESOURCE_ID(StringT) StringResId);
    void Append(uint8 *TextP, BalDispFontCodepageT FontCodePage = BAL_DISP_FNT_CP_LATIN1, int16 StringLen = -1);
    void Append(RESOURCE_ID(ImageResT) IconId);
    
    void InsertAt(uint16 CellIndex, const BStringC &String);
    void InsertAt(uint16 CellIndex, RESOURCE_ID(StringT) StringResId);
    void InsertAt(uint16 CellIndex, uint8 *TextP, BalDispFontCodepageT FontCodePage = BAL_DISP_FNT_CP_LATIN1, int16 StringLen = -1);
    void InsertAt(uint16 CellIndex, RESOURCE_ID(ImageResT) IconId);
    
    void SetAt(const BStringC &String, uint16 CellIndex = 0);
    void SetAt(RESOURCE_ID(StringT) StringResId, uint16 CellIndex = 0);
    void SetAt(uint8 *TextP, BalDispFontCodepageT FontCodePage = BAL_DISP_FNT_CP_LATIN1, int16 StringLen = -1, uint16 CellIndex = 0);
    void SetAt(RESOURCE_ID(ImageResT) IconId, uint16 CellIndex = 0);
    
    void RemoveAt(uint16 CellIndex);
    
    CellC * GetAt(uint16 CellIndex = 0);
    BStringC GetTextAt(uint16 CellIndex = 0);
    RESOURCE_ID(ImageResT) GetIconAt(uint16 CellIndex = 0);
    //! return the count of all the cells
    int GetSize() const { return mVector.GetSize();}
    
  protected:
  
  private:    
    int16 GetHeightAt(const int16 dx, const BalDispFontT font, uint16 CellIndex);
    int16 GetWidthAt(const BalDispFontT font, uint16 CellIndex);
    int16 GetTotalWidth(const BalDispFontT font);
    void SetRectAt(const BalDispRectT &Rect, uint16 CellIndex = 0);
    
    void SetAllProperty(const STPropertyT *Property);
    void SetAllMoveProperty(const STMovePropertyT *Property);
    void DrawAll(DCC *DcP);
    
    void StartAllFlashing();
    void StopAllFlashing();
    void ProcessAllFlashing();
    void ProcessAllMoving();
    void StopAllMoving();
    bool StartTimerNeeded();
    
  private:    
    RichStringC& operator=(const RichStringC &rhs);
    void RemoveAll();
    BVectorC mVector;
};





#endif

