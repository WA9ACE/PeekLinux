

#ifndef VALUTITLS_H
#define VALUTITLS_H
#include "sysdefs.h"

/*****************************************************************************
 
  FUNCTION NAME: PhbChar2BCD - Revised by zhengyi 2004-9-9
  DESCRIPTION:
        Convert digital char string to BCD code.: example:convert '135' to 0x31 0xF5
  PARAMETERS:
        pBcd :where BCD code storaged.
        pCharArray: char string with '\0' end.
  RETURNED VALUES:

*****************************************************************************/
void PhbChar2BCD( uint8* pBcd,
                  uint8* pCharArray );

/*****************************************************************************
  FUNCTION NAME: PhbBCD2Char 
  DESCRIPTION:
        Convert BCD code to digital char string. example:convert 0x31 0xF5 to '135' 
        the max char string length is signed 10.
  PARAMETERS:
        pBcd : pointer to BCD code array, with 0xf*,or 0xff end.
        pCharArray: char string
  RETURNED VALUES:

*****************************************************************************/
void PhbBCD2Char( uint8* pCharArray,
                  uint8* pBcd );

uint8   getuint8(uint8* data, uint16 startBit, uint8 numBits);
uint16  getuint16(uint8* data, uint16 startBit, uint8 numBits);
void    putuint8(uint8* data, uint16 startBit, uint8 numBits, uint8 value);
void    putuint16(uint8* data, uint16 startBit, uint8 numBits, uint16 value);
uint16  crc16bit(uint8* msg, uint16 len);

#endif

