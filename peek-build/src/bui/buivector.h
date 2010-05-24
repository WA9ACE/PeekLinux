

#ifndef UIVECTOR_H
#define UIVECTOR_H

#include "sysdefs.h"

class BVectorC
{
public:
  BVectorC();
  BVectorC(BVectorC& vector);
  BVectorC(int Size);

  ~BVectorC();
  
  void  ReSet();
  int GetSize()const;
  int GetCapacity();
  void* GetElement(int Index);

  void* operator[](int Index) const;
  void* &operator[](int Index);

  int PushBack(void* ElementP);
  int PushFront(void* ElementP);

  int Insert(int Index,void* ElementP);
  int Delete(int Index);
  int Delete(void* ElementP);
  int Find(void* ElementP);

private:
  uint32* mStartP;
  uint32* mFinishP;
  uint32* mEndOfStorageP;
  
};




#endif
