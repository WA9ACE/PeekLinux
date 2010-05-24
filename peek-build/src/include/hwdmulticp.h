

#include "sysdefs.h"

/* define the CP2 flash device head strcuture */
typedef PACKED struct
{
    uint8 Name[4];  /*"PPM "*/
    uint32 Size;
    uint32 Status;
} HwdMultiCp2HeadT;

/* define the CP3 flash device head strcuture */
typedef PACKED struct
{
    uint8 Name[4];   /*"CPCP"*/
    uint32 Size;
    uint32 Status;
} HwdMultiCp3HeadT;

typedef PACKED struct
{
    uint32 StartAddr;
    uint32  BlockSize;
    uint16  BlockNum;
} HwdMultiCpUserFsmInfoT;

void HwdFlashInitCp3StartSector(void);
void HwdMultiCpGetCp2Info(uint16* StartAddr, HwdMultiCp2HeadT* pHeadData);
void HwdMultiCpGetCp3Info(uint16* StartAddr, HwdMultiCp3HeadT* pHeadData);
void HwdMultiCpGetUserFsmInfo(HwdMultiCpUserFsmInfoT* pInfo);


