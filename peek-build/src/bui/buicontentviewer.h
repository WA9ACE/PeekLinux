
#ifndef UICONTENTVIEWER_H
#define UICONTENTVIEWER_H 

#include "buivector.h"
#include "buicontentobject.h"
#include "buicontentcontainer.h"
#include "buicontentstatelistener.h"
#include "buidcc.h"

class ContentViewerC : public ContentStateListenerC
{
public:

  ContentViewerC(ContentContainerC * container);
  ~ContentViewerC(); 
  ContentObjectC * GetContentByPosition(uint16 absoluteX,uint16 absoluteY,uint16 * contentIndex,uint16 * glyphIndex);
  virtual void ContentStateNotify(uint16 index,bool updateView=TRUE);
  virtual void Draw(DCC *DcP);
  void ShowNotify();
  void HideNotify();
  void SetRect(RectC rect);
  void MoveWindow(int16 right,int16 down);
  bool SetAutoPlay(bool autoPlay);

  void SetTopMargin(uint16 topMargin) {mPageProperty.TopMargin = topMargin;}
  void SetBottomMargin(uint16 bottomMargin){mPageProperty.BottomMargin = bottomMargin;}
  void SetLeftMargin(uint16 leftMargin){mPageProperty.LeftMargin = leftMargin;}
  void SetRightMargin(uint16 rightMargin){mPageProperty.RightMargin = rightMargin;}
  void SetLineSpace(uint16 lineSpace){mPageProperty.LineSpace= lineSpace;}
  void SetGlyphSpace(uint16 glyphSpace){mPageProperty.GlyphSpace= glyphSpace;}

  uint16 MoveWindowToPreviousGlyph(uint16 glyphIndex);
  uint16 MoveWindowToNextGlyph(uint16 glyphIndex);
  uint16 MoveWindowToPreviousLine(uint16 glyphIndex,uint16 referenceGlyphIndex);
  uint16 MoveWindowToNextLine(uint16 glyphIndex,uint16 referenceGlyphIndex);
  void DrawCursor(DCC *DcP,uint16 glyphIndex);
  void GetCaretRect(RectC &Rect, uint16 CaretOffset);
  bool ToMoveOut(bool Down, uint16 CaretOffset);
  
private:
typedef struct 
{
  uint16  Y; /*y position of the top side */
  uint16  Height;   /* height of the line */
  uint16  StartGlyphIndex; /*start glyph index in this line */
  uint16  EndGlyphIndex; /* end glyph index in this line */
} LineInfoT;

typedef struct 
{
  uint16  TopMargin;   /* Margin from top side */
  uint16  BottomMargin; /* Margin from top side */
  uint16  LeftMargin; /* Margin from left side */
  uint16  RightMargin; /* Margin from right side */
  uint16  LineSpace; /* Space between two lines */
  uint16  GlyphSpace; /* Space between two glyphs */
} PagePropertyT;

private:
  bool mAutoPlay;
  ContentContainerC * mContainer; /* container which contains objects to be presented in the viewer */
  BVectorC mLinesInfo; /*height and index for contents located at each line */
  BVectorC mGlyphScopes; /* records scope of each glyph */
  RectC mViewerBound; /* the border of the viewer */
  bool mIsOnShow;/*  whether the viewer is on show or not */
  RectC mDisplayWindow; /* location of the current display window */
  uint16 mReadyGlyphCount; /* glyph count ready for display */
  PagePropertyT mPageProperty; /* page property */


private:
  bool RemoveScopes(uint16 startIndex);
  bool AppendGlyph(RectC glyphRect);
  void ForceLayout();
  int16 GetDistanceToDisplayWindow(uint16 glyphIndex);
  uint16 GetGlyphIndexInLine(uint16 lineIndex,uint16 referenceGlyphIndex);
  uint16 GetLineIndex(uint16 glyphIndex);
};



#endif

