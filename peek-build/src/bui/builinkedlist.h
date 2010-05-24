

#ifndef UILINKEDLIST_H
#define UILINKEDLIST_H

#include "sysdefs.h"

#include "buidefs.h"

//! Iterator define
typedef void* IteratorT;

//! Linked List define
class BLinkedListC
{
public:
  BLinkedListC();
  virtual ~BLinkedListC();

  uint16 GetCount(void) const;  
  bool IsEmpty(void) const;

  void* GetHead(void) const;
  void* GetTail(void) const;

  IteratorT GetHeadIterator(void) const;
  IteratorT GetTailIterator(void) const;   
  void* GetNext(IteratorT& Iterator, bool IsCircular = FALSE) const;
  void* GetPrev(IteratorT& Iterator, bool IsCircular = FALSE) const;
  void* GetAt(IteratorT Iterator) const;

  IteratorT Find(void* DataP) const;
  IteratorT FindIndex(uint16 Index) const;

  IteratorT AddHead(void* DataP);    
  IteratorT AddTail(void* DataP);

  void AddHead(const BLinkedListC& NewList);
  void AddTail(const BLinkedListC& NewList);

  void* RemoveHead(void);
  void* RemoveTail(void);    
  void RemoveAt(IteratorT Iterator);
  void RemoveAll(void);  

  IteratorT InsertBefore(IteratorT Iterator, void* DataP);
  IteratorT InsertAfter(IteratorT Iterator, void* DataP);
  void SetAt(IteratorT Iterator, void* DataP);

protected:
  //! The node structure define
  struct Element
  {
    void* DataP; //!< the data of the current node
    Element* NextP; //!< point to the next node
    Element* PrevP; //!< point to the previous node
  };

#ifdef UI_DEBUG
private:
  bool IsValid(IteratorT Iterator) const;
#endif

private:
  BLinkedListC(const BLinkedListC& OtherList);
  BLinkedListC& operator=(const BLinkedListC& OtherList);

private:
  Element* mHeadP;
  Element* mTailP;
  uint16 mCount;
};


inline uint16 BLinkedListC::GetCount(void) const
{
  return mCount;
}

inline bool BLinkedListC::IsEmpty(void) const
{
  return (bool)(mCount==0);
}

inline void* BLinkedListC::GetHead(void) const
{
  return (IsNull(mHeadP) ? NULL : mHeadP->DataP);
}

inline void* BLinkedListC::GetTail(void) const
{
  return (IsNull(mTailP) ? NULL : mTailP->DataP);
}
    
inline IteratorT BLinkedListC::GetHeadIterator(void) const
{
  return (IteratorT)mHeadP;
}

inline IteratorT BLinkedListC::GetTailIterator(void) const
{
  return (IteratorT)mTailP;
}




#endif
