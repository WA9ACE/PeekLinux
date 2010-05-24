

#ifndef UIBIT_H_
#define UIBIT_H_

#include "sysdefs.h"

  /* Bitfield functions */
  uint8  UIBitGetByte( uint8 *Data, uint16 StartBit, uint8 NumBits );
  uint16 UIBitGetWord( uint8 *Data, uint16 StartBit, uint8 NumBits );
  uint32 UIBitGetDword( uint8 *Data, uint16 StartBit, uint8 NumBits );
  uint32 UIBitGetUnknown( uint8 *Data, uint16 StartBit, uint8 NumBits );
  void  UIBitPutByte( uint8 *Data, uint16 StartBit, uint8 NumBits, uint8 Value );
  void  UIBitPutWord( uint8 *Data, uint16 StartBit, uint8 NumBits, uint16 Value );
  void  UIBitPutDword( uint8 *Data, uint16 StartBit, uint8 NumBits, uint32 Value );
  void  UIBitPutUnknown( uint8 *Data, uint16 StartBit, uint8 NumBits, uint32 Value );




#endif  /* UIBIT_H_ */


