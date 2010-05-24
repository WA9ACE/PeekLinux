

#ifndef UISCROLLBAR_H
#define UISCROLLBAR_H

#include "buicontrol.h"

#ifdef DISP_LCD_MONO

#else
//#define SCROLL_SHOW_ARROW
#endif

#define SCROLL_SHOW_ARROW

typedef struct
{
  //!Scroll Bars 
  BalDispColorT SBBorderCol;       //!<Scroll bar border color 
  BalDispColorT SBBkCol;            //!<Scroll bar background color 
  BalDispColorT SBThumbCol;         //!<Scroll bar thumb color 
  BalDispColorT SBArrowCol;        //!<Scroll bar arrow color 
}ScrollBarColorsT;

class ScrollBarControlC : public ControlC
{
private:
  uint16 mPos;
  uint16 mTopIndex;
  BalDispColorT mArrowColor;
  uint16 mDisplayCount;
  uint16 mTotalCount;
  float mMoveBarSize;

  BalDispRectT mThumbRect;

   // added by wanglili 20080506 3 lines
  uint16 TotalCount;	//how many counts 
  uint16 mScreenCount;	//how many counts in one screen
  bool isAlwaysDraw;	//check if scrollbar is draw in any case
  
public:
  ScrollBarControlC ( BalDispRectT Rect );

  virtual ~ ScrollBarControlC ();


public:
  uint16 GetScrollPos() const
  {
    return mPos;
  }

  void GetScrollRange( uint16 &DisplayCount, uint16 &TotalCount ) const
  {
    DisplayCount = mDisplayCount;
    TotalCount = mTotalCount;
  }

  void SetScrollPos( uint16 Pos, bool Redraw = TRUE );

  void SetScrollRange( uint16 DisplayCount, uint16 TotalCount, bool Redraw = TRUE );

  void ShowScrollBar( bool Show = TRUE );

  void SetScrollRect(BalDispRectT Rect);
/*! \cond private */        
  void SetScrollArrowColor(BalDispColorT SAColor){mArrowColor = SAColor;};

  /**  added by wanglili 20080506 to fix the bug QWERT00069318  **/
  void setTotalCountAndScreenCount(uint16 totalCount, uint16 screenCount);//set totalCount and screenCount
  void setIsAlwaysShow(bool isAlwaysShow);
  	
protected:
  virtual bool LoadResource(ResourceIdT ResourceId);
  virtual void Draw(DCC *DcP);
/*! \endcond */  

  DECLARE_MAIL_MAP()
};



#endif
