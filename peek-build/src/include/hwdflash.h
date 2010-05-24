#ifndef HWDFLASH_H
#define HWDFLASH_H


#include "sysdefs.h"

/* Fail/Okay return values */
#define FAIL 0
#define OKAY 1

/* Following commands are to reset INTEL and AMD FLASH */
#define HWD_FLASH_CMD_PID_EXIT      ((uint16)0xF0)  /* AMD style */
#define HWD_FLASH_CMD_READ_ARRAY    ((uint16)0xFF)  /* Intel style */

#define HWDADDR(addr)   ((addr) << 1)

enum {
    HWD_FLASH_ERR_SUSPD = -1,
    HWD_FLASH_ERR_NONE  = 0,
    HWD_FLASH_ERR_FAIL  = 1
};


/* define the MAN ID for flash */
typedef enum
{
    HWD_FLASH_MANID_NULL    = 0,
    
    HWD_FLASH_MANID_AMD     = 0x0001,
    HWD_FLASH_MANID_RENESAS = 0x001C,
    HWD_FLASH_MANID_ATMEL   = 0x001F,
    HWD_FLASH_MANID_ST      = 0x0020,
    HWD_FLASH_MANID_SILICON = 0x002C,
    HWD_FLASH_MANID_INTEL   = 0x0089,
    HWD_FLASH_MANID_TOSHIBA = 0x0098,
    HWD_FLASH_MANID_SHARP   = 0x00B0,
    HWD_FLASH_MANID_SST     = 0x00BF,
    HWD_FLASH_MANID_SAMSUNG = 0x00EC,

    HWD_FLASH_MANID_ERROR   = 0xFFFF
} HwdFlashManIdT;

/* the following definition is used to define the interfaces for drivers of flash */
typedef int     (*HwdFlashLowProgramT)(volatile uint16 *, const uint16 *, int);
typedef int     (*HwdFlashLowEraseT)(volatile uint16 *, int);
typedef int     (*HwdFlashLowClearstatT)(volatile uint16 *);
typedef uint16  (*HwdFlashLowReadstatT)(volatile uint16 *);

/* define the flash device strcuture */
typedef struct HwdFlashLowDev
{
    bool initialized;
    
    uint16                  man_id;     /* Manufacture ID */
    uint16                  dev_id;     /* Device ID */
    
    HwdFlashLowProgramT     program;
    HwdFlashLowEraseT       erase;
    HwdFlashLowReadstatT    read_stat;
    HwdFlashLowClearstatT   clear_stat;
} HwdFlashLowDevT;

/* Define logical flash section names */
typedef enum 
{
   HWD_FLASH_CP_BOOT_CODE_SECTION = 0,
   HWD_FLASH_CP_CODE_SECTION,
   HWD_FLASH_DSPM_CODE_SECTION,
   HWD_FLASH_DSPV_CODE_SECTION,
   HWD_FLASH_CP2_CODE_SECTION,
   HWD_FLASH_CP3_CODE_SECTION,
   HWD_FLASH_FSM_USER_SECTION,
   HWD_FLASH_FSM_DATA_SECTION,
   HWD_FLASH_ALL_SECTION,
   HWD_FLASH_SECTION_MAX
} HwdFlashSectionTypeT;

/*------------------------------------------------------------------------
*  Define function prototypes
*------------------------------------------------------------------------*/
/* the following interface is just for BOOT */
extern uint16   HwdFlashErase(HwdFlashSectionTypeT Section);
extern uint16   HwdFlashProgram16(HwdFlashSectionTypeT Section, uint32 Offset, uint16 Data);
extern void     HwdFlashRead(HwdFlashSectionTypeT Section, uint32 Offset, uint8 *Destination, uint16 ByteCount);
extern bool   HwdFlashSectionProtected(HwdFlashSectionTypeT Section);

/* the following interfaces is just for CP */
extern uint16 * HwdFlashSectionAddress(HwdFlashSectionTypeT Section);
extern uint32 HwdFlashSectionSize(HwdFlashSectionTypeT Section);
extern uint32 HwdFlashReadMemSize(void);

/************************************************************************
 *  the following interfaces is for CP and BOOT to probe type of flash  *
 *  this interface is in hwd/hwdflash_iram.c                            *
 ************************************************************************/
extern bool     HwdFlashProbe(HwdFlashLowDevT *dev, uint32 base_addr);
extern bool     HwdFlashInterruptPending(void);
extern int      HwdFlashInit(HwdFlashLowDevT *devP, uint32 baseAddr);
extern uint16   HwdFlashReadManId(void);
extern uint16   HwdFlashReadDevId(void);


#endif
