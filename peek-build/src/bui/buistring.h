





#ifndef UISTRING_H
#define UISTRING_H

#include "restypedefines.h"
#include "resdatabase.h"


/************************************* functions for StringT begin ***************************************/
void StringInit(StringT *StringP, uint8 CodePage, bool IsStatic, uint16 StringLen);
StringT* StringCreate(uint8 CodePage, uint16 StringLen, char *ContentP);
StringT* StringCreate(const StringT *StringResP);
void StringFree(StringT *StringP);
/************************************* functions for StringT end ***************************************/

/************************************* functions for MsgStringT begin ***********************************/
//! message string
struct MsgStringT
{
  uint16 mLength;
  uint8 mCodepage;
  char mStringP[1];
};

uint16 MsgStringGetObjectSize(const MsgStringT *MsgStrP);
//MsgStringT* MsgStringCreate(uint16 );
void MsgStringFree(MsgStringT *MsgStrP);
/************************************* functions for MsgStringT end ************************************/

 
/************************************* functions for SidbStringT begin **********************************/
const int SIDB_STRING_MAX_LENGTH = 70;

//! sidb string
typedef PACKED struct 
{
uint8 CodePage;
uint8 StringLen;
uint8 String[SIDB_STRING_MAX_LENGTH];
}SidbStringT;

const int SIDB_STRING_VALUE_LEN	= sizeof(SidbStringT);
/************************************* functions for sidbStringT end ***********************************/


typedef unsigned int SizeT;

//global function begin

/*swap unicode byte, don't add 0x00 at the end*/
int SwapUnicodeByte(uint16 *strUnicodeOutP, int Buflen, const uint16 *strUnicodeInP);

/*swap unicode byte, don't add 0x00 at the end*/
int SwapUnicodeByte(uint16 *strUnicodeOutP, const uint16 *strUnicodeInP, SizeT NumOfChars);

//gloabl function end

//! string class 
class BStringC  
{
private:
  enum{
    MinLength=16
  };

private:
	StringT *mString;
  SizeT mMemLen;

/*! \cond private */
private:
  SizeT Size() const
  {
    return mMemLen;
  }
  SizeT CheckSize(SizeT);
  bool Init(SizeT);
  bool AddCharsMemory(SizeT NumOfChars);
   
  void SetDefaultValue();
  void Init(const StringT *strP);
  bool ToDynamic();
  BStringC& operator =(const BStringC &str); //In ADS1.1, not to use this function,please use operator +=
  BStringC& operator =(const char *strP); //In ADS1.1, not to use  this function, please use operator += 
/*! \endcond */ 

public:
  BStringC();
  BStringC(SizeT NumOfBytes);
  BStringC(const char *strP);
  BStringC(const uint16 *strP);
  BStringC(const StringT *strP);
  BStringC(const BStringC &str);
  BStringC(RESOURCE_ID(StringResT)  IdsResID, uint32 num_args,...);
  ~BStringC();

  SizeT GetLength() const;
  SizeT GetNumOfChars() const;

  void Empty();
  bool IsEmpty() const;

  void Append(char ch);
  void Append(char ch, SizeT NumOfChars);
  void Append(uint16 wch);
  void Append(uint16 wch, SizeT NumOfChars);
  void Append(const char *strP);
  void Append(const uint16 *wstrP);
  void Append(const char *strP, SizeT NumOfChars);
  void Append(const uint16 *wstrP, SizeT NumOfChars);
  void Append(const StringT *strP);
  void Append(const BStringC &str);

  void Insert(SizeT CharIndex,char ch);
  void Insert(SizeT CharIndex,uint16 wch);
  void Insert(SizeT CharIndex,char *strP);
  void Insert(SizeT CharIndex,uint16 *strP);
  void Insert(SizeT CharIndex,const StringT *strP);
  void Insert(SizeT CharIndex,const BStringC &str);
  
  uint16 GetAt(SizeT CharIndex) const;
  void SetAt(SizeT CharIndex,char ch);
  void SetAt(SizeT CharIndex,uint16 wch);
  SizeT SetAllCharsTo(char ch);
  SizeT SetAllCharsTo(uint16 wch);
  SizeT Delete(SizeT CharIndex, SizeT NumOfChars=1);
  
  BStringC& operator =(char ch);
  BStringC& operator =(uint16 wch);
  BStringC& operator =(const uint16 *wstrP);
  BStringC& operator =(const StringT *strP);
//#ifdef BYD_USE_SIM

/*
#else
  //In ADS1.1, can't define BStringC& operator =(const BStringC& str) (a bug in ADS 1.1?).
  //For a temporary solution, define following friend function.
  //Since in standard C++, operator= can defined as member function only, 
  //this needs be tested detailedly in target platform.
  friend BStringC& operator=(BStringC &strLeft, const BStringC &strRight);
#endif
*/
  BStringC& operator +=(char ch);
  BStringC& operator +=(uint16 wch);
  BStringC& operator +=(const char *strP);
  BStringC& operator +=(const uint16 *wstrP);
  BStringC& operator +=(const StringT *strP);
  BStringC& operator +=(const BStringC &str);

  int Compare(const char *strP) const;
  int Compare(const uint16 *strP) const;
  int Compare(const StringT *strP) const;
  int Compare(const BStringC &str) const;

  int CompareNoCase(const char *strP) const;
  int CompareNoCase(const uint16 *strP) const;
  int CompareNoCase(const StringT *strP) const;
  int CompareNoCase(const BStringC &str) const;
  
  int32 Find(char ch, SizeT StartCharIndex=0) const;
  int32 Find(uint16 wch, SizeT StartCharIndex=0) const;
  int32 Find(const char *strP, SizeT StartCharIndex=0) const;
  int32 Find(const uint16 *strP, SizeT StartCharIndex=0) const;
  int32 Find(const StringT *strP, SizeT StartCharIndex=0) const;
  int32 Find(const BStringC &str, SizeT StartCharIndex=0) const;
  
  int32 ReverseFind(char ch) const;
  int32 ReverseFind(uint16 wch) const;
  
  void TrimLeft();
  void TrimLeft(char ch);
  void TrimRight();
  void TrimRight(char ch);
  
  
  void ToUpper();
  void ToLower();

  bool Load(RESOURCE_ID(StringResT) StringResourceID);
  BStringC SubString(SizeT StartCharIndex,SizeT NumOfChars) const;

  SizeT CopyFromBigUnicode(const uint16 *strP);
  SizeT CopyFromBigUnicodeStream(const uint16 *StreamP, SizeT NumOfChars);
  SizeT CopyFromUtf8(const char *strP);
  SizeT CopyFrom7BitCharStream(const uint8 *StreamP, int NumOfChars );
  
  bool IsAscii() const;
  const char* ConstStr() const;
  const uint16* ConstWstr() const;
  const StringT* ConstStringT() const;
  
  int32 ToAsciiStr(char *strP, int32 Buflen) const;
  int32 ToAsciiStream(char *strP, int32 Buflen) const;
  int32 ToUtf8Str(char *strP, int32 Buflen) const;
  int32 ToUnicodeStr(uint16 *strP, int32 Buflen) const;
  int32 ToUnicodeStream(uint16 *strP, int32 Buflen) const;
  int32 ToBigUnicodeStr(uint16 *strP, int32 Buflen) const;
  int32 ToBigUnicodeStream(uint16 *strP, int32 Buflen) const;
  int32 To7BitCharStream(uint8 *StreamP, int StreamLength, int *PadBitsP) const;

  bool ToUnicode();  
  bool ToAscii();  
  
  bool CanConvertToAscii() const;
  bool CanConvertTo7BitChar() const;

  void CopyFromMsgString(const MsgStringT *MsgStrP);
  MsgStringT* ToMsgString()  const;

  void CopyFromSidbString(const SidbStringT &SidbStr);
  bool ToSidbString(SidbStringT &SidbStr)  const;

  friend BStringC operator +(const BStringC &str, char ch);
  friend BStringC operator +(const BStringC &str, uint16 wch);
  friend BStringC operator +(const BStringC &strLeft, const char *strRightP);
  friend BStringC operator +(const BStringC &strLeft, const uint16 *strRightP);
  friend BStringC operator +(const BStringC &strLeft, const StringT *strRightP);
  friend BStringC operator +(const BStringC &strLeft, const BStringC &strRight);
  friend BStringC operator +(char ch, const BStringC &str);
  friend BStringC operator +(uint16 wch, const BStringC &str);
  friend BStringC operator +(const char *strLeftP, const BStringC &strRight);
  friend BStringC operator +(const uint16 *strLeftP, const BStringC &strRight);
  friend BStringC operator +(const StringT *strLeftP, const BStringC &strRight);
  
  friend bool operator ==(const BStringC &strLeft, const char *strRightP);
  friend bool operator ==(const BStringC &strLeft, const uint16 *strRightP);
  friend bool operator ==(const BStringC &strLeft, const StringT *strRightP);
  friend bool operator ==(const BStringC &strLeft, const BStringC &strRight);
  friend bool operator ==(const char *strLeftP, const BStringC &strRight);
  friend bool operator ==(const uint16 *strLeftP, const BStringC &strRight);
  friend bool operator ==(const StringT *strLeftP, const BStringC &strRight);

  
  friend bool operator !=(const BStringC &strLeft, const char *strRightP);
  friend bool operator !=(const BStringC &strLeft, const uint16 *strRightP);
  friend bool operator !=(const BStringC &strLeft, const StringT *strRightP);
  friend bool operator !=(const BStringC &strLeft, const BStringC &strRight);
  friend bool operator !=(const char *strLeftP, const BStringC &strRight);
  friend bool operator !=(const uint16 *strLeftP, const BStringC &strRight);
  friend bool operator !=(const StringT *strLeftP, const BStringC &strRight);
  
  friend bool operator <(const BStringC &strLeft, const char *strRightP);
  friend bool operator <(const BStringC &strLeft, const uint16 *strRightP);
  friend bool operator <(const BStringC &strLeft, const StringT * strRightP);
  friend bool operator <(const BStringC &strLeft, const BStringC &strRight);
  friend bool operator <(const char *strLeftP, const BStringC &strRight);
  friend bool operator <(const uint16 *strLeftP, const BStringC &strRight);
  friend bool operator <(const StringT *strLeftP, const BStringC &strRight);
  
  friend bool operator <=(const BStringC &strLeft, const char *strRightP);
  friend bool operator <=(const BStringC &strLeft, const uint16 *strRightP);
  friend bool operator <=(const BStringC &strLeft, const StringT * strRightP);
  friend bool operator <=(const BStringC &strLeft, const BStringC &strRight);
  friend bool operator <=(const char *strLeftP, const BStringC &strRight);
  friend bool operator <=(const uint16 *strLeftP, const BStringC &strRight);
  friend bool operator <=(const StringT *strLeftP, const BStringC &strRight);
  
  friend bool operator >(const BStringC &strLeft, const char *strRightP);
  friend bool operator >(const BStringC &strLeft, const uint16 *strRightP);
  friend bool operator >(const BStringC &strLeft, const StringT *strRightP);
  friend bool operator >(const BStringC &strLeft, const BStringC &strRight);
  friend bool operator >(const char *strLeftP, const BStringC &strRight);
  friend bool operator >(const uint16 *strLeftP, const BStringC &strRight);
  friend bool operator >(const StringT *strLeftP, const BStringC &strRight);
  
  friend bool operator >=(const BStringC &strLeft, const char *strRightP);
  friend bool operator >=(const BStringC &strLeft, const uint16 *strRightP);
  friend bool operator >=(const BStringC &strLeft, const StringT *strRightP);
  friend bool operator >=(const BStringC &strLeft, const BStringC &strRight);
  friend bool operator >=(const char *strLeftP, const BStringC& strRight);
  friend bool operator >=(const uint16 *strLeftP, const BStringC &strRight);
  friend bool operator >=(const StringT *strLeftP, const BStringC &strRight);

  /*NOTE: only Format 1024 bytes*/
  BStringC& Format(const char* pszFormat, ...);  // Format itself if pszFormat is NULL
};





#endif // UISTRING_H
