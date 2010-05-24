

#ifndef UIRECT_H
#define UIRECT_H 

#include "sysdefs.h"

#include "baldispapi.h"


//! long point type to BalDispRectT structure
typedef BalDispRectT* LPRectT;

//! const long point type to BalDispRectT structure
typedef const BalDispRectT* LPCRectT;

//! Rect class define
class RectC : public BalDispRectT
{
public:
  RectC();
  RectC(const BalDispRectT& Rect);
  ~RectC();

  operator LPRectT();
  operator LPCRectT() const;
  
  bool IsRectEmpty() const;
  
  void SetRect(int16 X1, int16 Y1, int16 Width, int16 Height);
  void SetRectEmpty();

  void OffsetRect(int16 DeltaX, int16 DeltaY);

  bool IntersectRect(LPCRectT Rect1P, LPCRectT Rect2P);
  bool UnionRect(LPCRectT Rect1P, LPCRectT Rect2P);

  void operator=(const BalDispRectT& Rect);
  bool operator==(const BalDispRectT& Rect) const;  
};




#endif
