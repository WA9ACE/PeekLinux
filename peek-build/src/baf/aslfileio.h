

#ifndef ASLFILEIO_H_
#define ASLFILEIO_H_

#include "balfsiapi.h"

#ifdef  __cplusplus
extern "C" {
#endif

//! File system operation result
typedef enum
{
  ASLFS_SUCCESS, //!< Success
  ASLFS_ERROR    //!< Failed
}AslFSResultT;

AslFSResultT AslSidbReadNvItemsEx(uint16 ItemID, uint16 KeyID, uint16 Offset, void*DataP, uint32 DataLength);
AslFSResultT AslSidbWriteNvItemsEx(uint16 ItemID, uint16 KeyID, uint16 Offset, void *DataP, uint32 DataLength);
AslFSResultT AslSidbWriteNvItems(uint16 ItemID, uint16 KeyID, void*DataP, uint32 DataLength);
AslFSResultT AslSidbReadNvItems(uint16 ItemID, uint16 KeyID, void*DataP, uint32 DataLength);

#ifdef  __cplusplus
}
#endif




#endif

